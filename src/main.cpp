#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <random>

using namespace geode::prelude;

namespace {
    std::mt19937& randomEngine() {
        static std::mt19937 engine{std::random_device{}()};
        return engine;
    }

    int randomInt(int low, int high) {
        return std::uniform_int_distribution<int>(low, high)(randomEngine());
    }

    float randomFloat(float low, float high) {
        return std::uniform_real_distribution<float>(low, high)(randomEngine());
    }

    void randomizeLoadedObjects(PlayLayer* layer) {
        if (!layer || !layer->m_objects) return;

        int count = 0;
        for (auto* object : CCArrayExt<GameObject*>(layer->m_objects)) {
            if (!object) continue;

            // Prototype pass: randomize common visual properties on loaded objects.
            // Type conversion and trigger-data mutation need a serialized-level
            // transformation pass before the game creates these objects.
            object->setRotation(static_cast<float>(randomInt(0, 359)));
            object->setScale(randomFloat(0.35f, 1.8f));
            object->setOpacity(static_cast<GLubyte>(randomInt(35, 255)));

            // An invisible object can still collide, which is part of the chaos.
            if (randomInt(0, 1) == 0) object->setVisible(false);

            ++count;
        }

        log::info("Random Everything: randomized visual properties on {} loaded objects", count);
        log::warn("Random Everything prototype: object-type and trigger conversion is not implemented yet.");
    }
}

class $modify(RandomEverythingPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        // A fresh random roll occurs every time a new PlayLayer is initialized.
        randomizeLoadedObjects(this);
        return true;
    }
};
