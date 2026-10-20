#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace {
    const char* imageForPercent(int percent) {
        if (percent <= 10) return "death_0_10.png";
        if (percent <= 30) return "death_11_30.png";
        if (percent <= 50) return "death_31_50.png";
        if (percent <= 80) return "death_51_80.png";
        if (percent <= 90) return "death_81_90.png";
        return "death_91_99.png";
    }

    void showImage(CCNode* parent, const char* filename) {
        if (!parent) return;

        auto path = Mod::get()->getResourcesDir() / "Resources" / filename;
        auto sprite = CCSprite::create(path.string().c_str());
        if (!sprite) {
            log::error("Death Quote: failed to load image at '{}'", path.string());
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
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        bool shouldShow = player == m_player1;
        int percent = shouldShow ? getCurrentPercent() : 0;

        PlayLayer::destroyPlayer(player, object);

        if (!shouldShow || !m_uiLayer) return;
        showImage(m_uiLayer, imageForPercent(percent));
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        showImage(m_uiLayer, "level_complete_gg.png");
    }
};
