#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <algorithm>
#include <random>
#include <string>
#include <string_view>
#include <vector>

using namespace geode::prelude;

namespace {
    std::mt19937& randomEngine() {
        static std::mt19937 engine{std::random_device{}()};
        return engine;
    }

    // A conservative pool of common solid/block object IDs. Do not put triggers,
    // portals, pads, orbs, or other objects with special gameplay data in this pool.
    constexpr int kBlockIDs[] = { 1, 2, 3, 4, 5, 6, 7 };

    int randomBlockID(int originalID) {
        std::vector<int> choices;
        for (int id : kBlockIDs) {
            if (id != originalID) choices.push_back(id);
        }
        if (choices.empty()) return originalID;
        std::uniform_int_distribution<size_t> dist(0, choices.size() - 1);
        return choices[dist(randomEngine())];
    }

    bool isBlockID(int id) {
        return std::find(std::begin(kBlockIDs), std::end(kBlockIDs), id) != std::end(kBlockIDs);
    }

    // GD's object setup format is comma-separated key/value pairs and semicolon-
    // separated objects. Key "1" is the object ID. Transform the setup BEFORE
    // the game creates objects so this changes the actual object type, not just
    // the rotation/scale of an already-created object.
    std::string randomizeBlockIDs(std::string_view setup, int& changed) {
        std::string output;
        output.reserve(setup.size());
        size_t objectStart = 0;

        while (objectStart < setup.size()) {
            size_t objectEnd = setup.find(';', objectStart);
            if (objectEnd == std::string_view::npos) objectEnd = setup.size();

            std::string object(setup.substr(objectStart, objectEnd - objectStart));
            std::vector<std::string> fields;
            size_t fieldStart = 0;
            while (fieldStart <= object.size()) {
                size_t fieldEnd = object.find(',', fieldStart);
                if (fieldEnd == std::string::npos) fieldEnd = object.size();
                fields.emplace_back(object.substr(fieldStart, fieldEnd - fieldStart));
                if (fieldEnd == object.size()) break;
                fieldStart = fieldEnd + 1;
            }

            // Only change known common block IDs. Other objects—including triggers,
            // portals and orbs—are preserved to avoid corrupting their special data.
            for (size_t i = 0; i + 1 < fields.size(); i += 2) {
                if (fields[i] != "1") continue;

                try {
                    int id = std::stoi(fields[i + 1]);
                    if (isBlockID(id)) {
                        fields[i + 1] = std::to_string(randomBlockID(id));
                        ++changed;
                    }
                } catch (...) {
                    // Leave malformed/unrecognized object data unchanged.
                }
                break;
            }

            for (size_t i = 0; i < fields.size(); ++i) {
                if (i) output.push_back(',');
                output += fields[i];
            }
            if (objectEnd < setup.size()) output.push_back(';');
            objectStart = objectEnd + 1;
        }

        return output;
    }
}

class $modify(RandomEverythingPlayLayer, PlayLayer) {
    void createObjectsFromSetup(gd::string const& setup, bool p1, bool p2) {
        int changed = 0;
        auto randomized = randomizeBlockIDs(std::string_view(setup.c_str()), changed);

        log::info("Random Everything: randomized {} block IDs before object creation", changed);
        PlayLayer::createObjectsFromSetup(gd::string(randomized), p1, p2);
    }
};
