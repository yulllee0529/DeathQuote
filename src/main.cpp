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

    void showImage(CCNode* parent, CCSprite* sprite) {
        if (!parent || !sprite) {
            log::error("Death Quote: could not create quote sprite");
            return;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto size = sprite->getContentSize();
        if (size.width <= 0.f || size.height <= 0.f) return;

        float scale = std::min((winSize.width * 0.78f) / size.width,
                               (winSize.height * 0.30f) / size.height);
        sprite->setScale(scale);
        sprite->setPosition({winSize.width / 2.0f, winSize.height / 2.0f});
        sprite->setZOrder(10000);
        parent->addChild(sprite);

        sprite->runAction(CCSequence::create(
            CCDelayTime::create(1.1f),
            CCFadeOut::create(0.35f),
            CCRemoveSelf::create(),
            nullptr
        ));
    }
}

class $modify(DeathQuotePlayLayer, PlayLayer) {
    struct Fields {
        float deathQuoteElapsed = 0.f;
    };

    void update(float dt) {
        PlayLayer::update(dt);
        m_fields->deathQuoteElapsed += dt;
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (player != m_player1) {
            PlayLayer::destroyPlayer(player, object);
            return;
        }

        int percent = getCurrentPercent();
        bool pastStartDelay = m_fields->deathQuoteElapsed >= 0.1f;
        PlayLayer::destroyPlayer(player, object);

        // Suppress only callbacks during the first 0.1 seconds after this
        // PlayLayer is created. Do not reset this timer on each level reset,
        // because resetLevel can also be called as part of death handling.
        // Do not gate on integer percent: real deaths at 0.1% can round to 0%.
        if (!pastStartDelay) return;

        if (m_uiLayer) showImage(m_uiLayer, imageForPercent(percent));
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        if (m_uiLayer) showImage(m_uiLayer, CCSprite::create("level_complete_gg.png"_spr));
    }
};
