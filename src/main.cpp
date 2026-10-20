#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace {
    const char* messageForPercent(int percent) {
        if (percent < 25) return "SKILL ISSUE";
        if (percent < 50) return "SO CLOSE...";
        if (percent < 75) return "HALF WAY THERE!";
        if (percent < 90) return "PAIN.";
        return "ARE YOU KIDDING ME?!";
    }
}

class $modify(DeathQuotePlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Show a single message for player 1, including in dual mode.
        bool shouldShow = player == m_player1;
        int percent = shouldShow ? getCurrentPercent() : 0;

        PlayLayer::destroyPlayer(player, object);

        if (!shouldShow || !m_uiLayer) return;

        auto label = CCLabelBMFont::create(messageForPercent(percent), "bigFont.fnt");
        if (!label) return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        label->setPosition({winSize.width / 2.0f, winSize.height / 2.0f});
        label->setScale(0.8f);
        label->setZOrder(10000);
        m_uiLayer->addChild(label);

        auto fadeAndRemove = CCSequence::create(
            CCDelayTime::create(1.1f),
            CCFadeOut::create(0.35f),
            CCRemoveSelf::create(),
            nullptr
        );
        label->runAction(fadeAndRemove);
    }
};
