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
        bool gameplayHasStarted = false;
    };

    void update(float dt) {
        PlayLayer::update(dt);
        // Only arm death quotes after gameplay time advances.
        if (m_gameState.m_levelTime > 0.f) {
            m_fields->gameplayHasStarted = true;
        }
    }

    void resetLevel() {
        // Suppress destroyPlayer calls made while the attempt is being reset.
        m_fields->gameplayHasStarted = false;
        PlayLayer::resetLevel();
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (player == m_player1 && m_uiLayer && m_fields->gameplayHasStarted) {
            showImage(m_uiLayer, imageForPercent(getCurrentPercent()));
        }
        PlayLayer::destroyPlayer(player, object);
    }

    void levelComplete() {
        if (m_uiLayer) showImage(m_uiLayer, CCSprite::create("level_complete_gg.png"_spr));
        PlayLayer::levelComplete();
    }
};
