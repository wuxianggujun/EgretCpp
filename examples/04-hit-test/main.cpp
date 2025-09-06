// 命中测试示例：空心（粗描边）/ 重叠优先级

#include "utils/Logger.hpp"
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "player/Player.hpp"
#include "sys/Screen.hpp"
#include "events/TouchEvent.hpp"

#include <memory>

using namespace egret;

static void bindHitLog(DisplayObject* obj, const char* name) {
    obj->addEventListener(TouchEvent::TOUCH_BEGIN, [name](Event& e){
        auto& te = static_cast<TouchEvent&>(e);
        EGRET_INFOF("HIT %s at local(%.1f, %.1f) phase=%d", name, te.getLocalX(), te.getLocalY(), static_cast<int>(e.getEventPhase()));
    }, nullptr, false, 0);
}

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    Logger::setConsoleEnabled(true);
    Logger::setLogFile("hit-test.log", true);
    EGRET_INFO("=== Example 04 - Hit Test Demo ===");

    auto screen = std::make_shared<sys::Screen>();
    auto player = std::make_shared<sys::Player>(800, 600, "Hit Test Demo", "");
    auto stage = player->getStage();
    stage->setScreen(screen.get());
    stage->setFrameRate(60);

    // 底层：实心圆（绿色）
    auto solid = std::make_shared<Shape>();
    auto gs = solid->getGraphics();
    gs->beginFill(0x66CC66, 1.0);
    gs->drawCircle(0, 0, 80);
    gs->endFill();
    // 设置锚点居中
    {
        auto b = solid->getMeasuredBounds();
        solid->setAnchorOffsetX(b.getX() + b.getWidth()*0.5);
        solid->setAnchorOffsetY(b.getY() + b.getHeight()*0.5);
    }
    solid->setX(300); solid->setY(260);
    stage->addChild(solid.get());
    bindHitLog(solid.get(), "SOLID");

    // 上层：空心圆环（粗描边，红色），覆盖在上面
    auto ring = std::make_shared<Shape>();
    auto gr = ring->getGraphics();
    gr->lineStyle(30.0, 0xCC3333, 1.0); // 粗描边，形成可点的“环”
    gr->drawCircle(0, 0, 100);
    // 不设置填充，形成空心
    {
        auto b = ring->getMeasuredBounds();
        ring->setAnchorOffsetX(b.getX() + b.getWidth()*0.5);
        ring->setAnchorOffsetY(b.getY() + b.getHeight()*0.5);
    }
    ring->setX(340); ring->setY(280);
    stage->addChild(ring.get());
    bindHitLog(ring.get(), "RING");

    // 右侧：Even-Odd 填充的环（紫色），用两个同心圆并设置 EvenOdd 规则形成空心
    auto evenOdd = std::make_shared<Shape>();
    auto ge = evenOdd->getGraphics();
    ge->setFillEvenOdd(true);
    ge->beginFill(0x9933CC, 1.0);
    ge->drawCircle(0, 0, 70);  // 大圆
    ge->drawCircle(0, 0, 35);  // 小圆，因EvenOdd形成空心
    ge->endFill();
    {
        auto b = evenOdd->getMeasuredBounds();
        evenOdd->setAnchorOffsetX(b.getX() + b.getWidth()*0.5);
        evenOdd->setAnchorOffsetY(b.getY() + b.getHeight()*0.5);
    }
    evenOdd->setX(600); evenOdd->setY(320);
    stage->addChild(evenOdd.get());
    bindHitLog(evenOdd.get(), "EVENODD");

    // 左侧：仅描边矩形（蓝色）
    auto stroked = std::make_shared<Shape>();
    auto gst = stroked->getGraphics();
    gst->lineStyle(20.0, 0x3366CC, 1.0);
    gst->drawRect(0, 0, 160, 120);
    {
        auto b = stroked->getMeasuredBounds();
        stroked->setAnchorOffsetX(b.getX() + b.getWidth()*0.5);
        stroked->setAnchorOffsetY(b.getY() + b.getHeight()*0.5);
    }
    stroked->setX(120); stroked->setY(120);
    stage->addChild(stroked.get());
    bindHitLog(stroked.get(), "STROKED");

    player->runMainLoop();
    return 0;
}
