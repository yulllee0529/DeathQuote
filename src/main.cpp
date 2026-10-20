#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace {
    const char* messageForPercent(int percent) {
        if (percent <= 10) return "\\uac1c\\ubabb\\ud558\\ub124";
        if (percent <= 30) return "\\uac70\\uae30\\uc11c \\uc8fd\\ub0d0";
        if (percent <= 50) return "\\uc5f0\\uc2b5 \\ub354 \\ud558\\uc138\\uc694";
        if (percent <= 80) return "\\uc880\\ub9cc \\ub354 \\ud558\\uba74 \\ub420 \\uac83 \\uac19\\uc740\\ub370";
        if (percent <= 90) return "\\uc548\\ud0c0\\uae5d\\ub124\\uc694";
        return "\\uc18c\\uace0\\uae30";
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

