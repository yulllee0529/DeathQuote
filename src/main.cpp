#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace {
    CCSprite* imageForPercent(int percent) {
        if (percent <= 10) return CCSprite::create("death_0_10.png"_spr);
        if (percent <= 30) return CCSprite::create("death_11_30.png"_spr);
        if (percent <= 50) return CCSprite::create("death_31_50.png"_spr);
        if (percent <= 80) return CCSprite::create("death_51_80.png"_spr);
        if (percent <= 90) return CCSprite::create("death_81_90.png"_spr);
        return CCSprite::create("death_91_99.png"_spr);
    }

    void showImage(CCNode* parent, int percent, bool completed = false) {
        if (!parent) {
            log::error("Death Quote: no scene available to display message");
            return;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        CCSprite* sprite = completed
            ? CCSprite::create("level_complete_gg.png"_spr)
            : imageForPercent(percent);

        if (sprite) {
            auto size = sprite->getContentSize();
            if (size.width > 0.f && size.height > 0.f) {
                float scale = std::min((winSize.width * 0.78f) / size.width,
                                       (winSize.height * 0.30f) / size.height);
                sprite->setScale(scale);
                sprite->setPosition({winSize.width / 2.f, winSize.height / 2.f});
                sprite->setZOrder(10000);
                parent->addChild(sprite, 10000);
                sprite->runAction(CCSequence::create(
                    CCDelayTime::create(1.1f),
                    CCFadeOut::create(0.35f),
                    CCRemoveSelf::create(),
                    nullptr
                ));
                log::info("Death Quote: displayed image for {}%", percent);
                return;
            }
            log::error("Death Quote: image loaded with invalid size");
        } else {
            log::error("Death Quote: image asset failed to load");
        }

        // Visible text fallback: this also confirms the death hook and scene placement work.
        auto label = CCLabelTTF::create(
            completed ? "LEVEL COMPLETE!" : fmt::format("YOU DIED AT {}%", percent).c_str(),
            "Arial", 30.f
        );
        if (label) {
            label->setPosition({winSize.width / 2.f, winSize.height / 2.f});
            label->setZOrder(10001);
            parent->addChild(label, 10001);
            label->runAction(CCSequence::create(
                CCDelayTime::create(1.1f),
                CCFadeOut::create(0.35f),
                CCRemoveSelf::create(),
                nullptr
            ));
        }
    }
}

class $modify(DeathQuotePlayLayer, PlayLayer) {
    struct Fields {
        bool gameplayHasStarted = false;
    };

    void update(float dt) {
        PlayLayer::update(dt);
        if (m_gameState.m_levelTime > 0.f) {
            m_fields->gameplayHasStarted = true;
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        bool show = player == m_player1 && m_fields->gameplayHasStarted;
        int percent = show ? getCurrentPercent() : 0;

        PlayLayer::destroyPlayer(player, object);

        if (show) {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            showImage(scene, percent);
        }
    }

    void levelComplete() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        showImage(scene, getCurrentPercent(), true);
        PlayLayer::levelComplete();
    }
};
