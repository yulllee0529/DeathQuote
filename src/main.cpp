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
            log::error("Death Quote: display parent is null");
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
                parent->addChild(sprite, 10000);
                sprite->runAction(CCSequence::create(
                    CCDelayTime::create(1.1f),
                    CCFadeOut::create(0.35f),
                    CCRemoveSelf::create(),
                    nullptr
                ));
                log::info("Death Quote: image displayed, percent={}, completed={}", percent, completed);
                return;
            }
            log::error("Death Quote: loaded image has invalid dimensions");
        } else {
            log::error("Death Quote: image asset failed to load");
        }

        auto label = CCLabelTTF::create(
            completed ? "LEVEL COMPLETE!" : fmt::format("YOU DIED AT {}%", percent).c_str(),
            "Arial", 30.f
        );
        if (label) {
            label->setPosition({winSize.width / 2.f, winSize.height / 2.f});
            parent->addChild(label, 10001);
            label->runAction(CCSequence::create(
                CCDelayTime::create(1.1f),
                CCFadeOut::create(0.35f),
                CCRemoveSelf::create(),
                nullptr
            ));
            log::info("Death Quote: fallback text displayed");
        } else {
            log::error("Death Quote: failed to create fallback label");
        }
    }
}

class $modify(DeathQuotePlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Show before the original handler can change the active scene or player state.
        int percent = getCurrentPercent();
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        log::info("Death Quote: destroyPlayer hook called at {}%", percent);
        showImage(scene ? scene : this, percent);
        PlayLayer::destroyPlayer(player, object);
    }

    void levelComplete() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        showImage(scene ? scene : this, getCurrentPercent(), true);
        PlayLayer::levelComplete();
    }
};
