// 事件流演示：捕获 -> 目标 -> 冒泡

#include "utils/Logger.hpp"
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "display/Sprite.hpp"
#include "player/Player.hpp"
#include "sys/Screen.hpp"
#include "events/TouchEvent.hpp"

#include <memory>

using namespace egret;

static void bindLog(DisplayObject* obj, const char* name) {
    obj->addEventListener(TouchEvent::TOUCH_BEGIN, [name](Event& e) {
        EGRET_INFOF("[BUBBLE ] %s TOUCH_BEGIN phase=%d", name, static_cast<int>(e.getEventPhase()));
    }, nullptr, false, 0);
    obj->addEventListener(TouchEvent::TOUCH_BEGIN, [name](Event& e) {
        EGRET_INFOF("[CAPTURE] %s TOUCH_BEGIN phase=%d", name, static_cast<int>(e.getEventPhase()));
    }, nullptr, true, 0);
}

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    Logger::setConsoleEnabled(true);
    Logger::setLogFile("event-flow.log", true);
    EGRET_INFO("=== Example 03 - Event Flow Demo ===");

    auto screen = std::make_shared<sys::Screen>();
    auto player = std::make_shared<sys::Player>(800, 600, "Event Flow Demo", "");
    auto stage = player->getStage();
    stage->setScreen(screen.get());
    stage->setFrameRate(60);

    // 容器 A（大矩形，使用 Sprite 作为容器）
    auto A = std::make_shared<Sprite>();
    auto gA = A->getGraphics();
    gA->beginFill(0xCCCCFF, 1.0);
    gA->drawRect(0, 0, 500, 400);
    gA->endFill();
    A->setX(100); A->setY(100);
    stage->addChild(A.get());

    // 容器 B（中矩形，使用 Sprite 作为容器）
    auto B = std::make_shared<Sprite>();
    auto gB = B->getGraphics();
    gB->beginFill(0x99CC66, 1.0);
    gB->drawRect(0, 0, 300, 220);
    gB->endFill();
    B->setX(60); B->setY(60);
    A->addChild(B.get());

    // 叶子 C（小矩形，点击它有最深目标）
    auto C = std::make_shared<Shape>();
    auto gC = C->getGraphics();
    gC->beginFill(0xFF9966, 1.0);
    gC->drawRect(0, 0, 140, 100);
    gC->endFill();
    C->setX(80); C->setY(70);
    B->addChild(C.get());

    bindLog(stage.get(), "STAGE");
    bindLog(A.get(),     "A");
    bindLog(B.get(),     "B");
    bindLog(C.get(),     "C");

    player->runMainLoop();
    return 0;
}
