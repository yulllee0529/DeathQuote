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
            log::error("Death Quote: failed to create image or find scene");
            return;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto size = sprite->getContentSize();
        if (size.width <= 0.f || size.height <= 0.f) {
            sprite->release();
            return;
        }

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
        // True only after the current attempt has actually started advancing.
        bool gameplayHasStarted = false;
        // Prevent duplicate messages during the same death/reset sequence.
        bool quoteShownThisAttempt = false;
    };

    void update(float dt) {
        PlayLayer::update(dt);

        if (m_gameState.m_levelTime > 0.f) {
            // Reset the per-attempt guard once, when a new attempt starts.
            if (!m_fields->gameplayHasStarted) {
                m_fields->gameplayHasStarted = true;
                m_fields->quoteShownThisAttempt = false;
            }
        } else {
            // During reset/retry, suppress destroyPlayer callbacks until play resumes.
            m_fields->gameplayHasStarted = false;
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        bool shouldShowQuote =
            player == m_player1 &&
            m_fields->gameplayHasStarted &&
            !m_fields->quoteShownThisAttempt;

        if (shouldShowQuote) {
            // Mark first so any extra callbacks from the same death cannot duplicate it.
            m_fields->quoteShownThisAttempt = true;
        }

        PlayLayer::destroyPlayer(player, object);

        if (shouldShowQuote) {
            // Use the running scene rather than m_uiLayer: the UI layer may be cleared
            // during the death/reset flow, which can make the image disappear instantly.
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            showImage(scene, imageForPercent(getCurrentPercent()));
        }
    }

    void levelComplete() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene) {
            showImage(scene, CCSprite::create("level_complete_gg.png"_spr));
        }
        PlayLayer::levelComplete();
    }
};
