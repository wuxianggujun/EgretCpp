// 事件演示示例：鼠标/触摸拖拽、点击变色、键盘控制

#include "utils/Logger.hpp"
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "sys/Screen.hpp"
#include "player/Player.hpp"
#include "events/TouchEvent.hpp"
#include "events/KeyboardEvent.hpp"
#include "events/KeyCode.hpp"

#include <memory>
#include <random>

class EventsDemo {
private:
    std::shared_ptr<egret::Stage> m_stage;
    std::shared_ptr<egret::sys::Screen> m_screen;
    std::shared_ptr<egret::sys::Player> m_player;

    std::shared_ptr<egret::Shape> m_circle;
    std::shared_ptr<egret::Shape> m_rect;

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

public:
    bool initialize() {
        try {
            // 屏幕与播放器
            m_screen = std::make_shared<egret::sys::Screen>();
            m_screen->setContentSize(WINDOW_WIDTH, WINDOW_HEIGHT);

            m_player = std::make_shared<egret::sys::Player>(WINDOW_WIDTH, WINDOW_HEIGHT,
                                                            "EgretCpp - Events Demo", "");
            m_stage = m_player->getStage();
            if (!m_stage) {
                EGRET_ERROR("Stage获取失败");
                return false;
            }
            m_stage->setScreen(m_screen.get());
            m_stage->setStageWidth(WINDOW_WIDTH);
            m_stage->setStageHeight(WINDOW_HEIGHT);
            m_stage->setFrameRate(60);

            // 创建显示对象
            createDisplayObjects();

            // 绑定事件
            bindEvents();

            EGRET_INFO("事件演示示例初始化完成");
            return true;
        } catch (const std::exception& e) {
            EGRET_ERRORF("初始化异常: {}", e.what());
            return false;
        }
    }

    void run() {
        if (m_player) {
            m_player->runMainLoop();
        }
    }

private:
    void createDisplayObjects() {
        using namespace egret;
        // 圆形（可点击移动）
        m_circle = std::make_shared<Shape>();
        auto cg = m_circle->getGraphics();
        cg->beginFill(0xFF6600, 1.0);
        cg->drawCircle(0, 0, 40);
        cg->endFill();
        // 将锚点设置到图形中心，保证设置 x/y 是以“中心点”为基准
        {
            auto bounds = m_circle->getMeasuredBounds();
            double anchorX = bounds.getX() + bounds.getWidth() * 0.5;
            double anchorY = bounds.getY() + bounds.getHeight() * 0.5;
            m_circle->setAnchorOffsetX(anchorX);
            m_circle->setAnchorOffsetY(anchorY);
        }
        m_circle->setX(150);
        m_circle->setY(120);
        m_stage->addChild(m_circle.get());

        // 矩形（可拖拽）
        m_rect = std::make_shared<Shape>();
        auto rg = m_rect->getGraphics();
        rg->lineStyle(3.0, 0x0066FF, 1.0);
        rg->beginFill(0x33CC66, 0.8);
        rg->drawRect(0, 0, 180, 100);
        rg->endFill();
        m_rect->setX(360);
        m_rect->setY(220);
        m_stage->addChild(m_rect.get());
    }

    void bindEvents() {
        using namespace egret;
        using egret::TouchEvent;
        using egret::KeyboardEvent;

        // 随机数工具
        auto rng = std::make_shared<std::mt19937>(std::random_device{}());
        auto randColor = [rng]() {
            std::uniform_int_distribution<int> dist(0, 0xFFFFFF);
            return static_cast<uint32_t>(dist(*rng));
        };

        // 圆形拖拽：按下记录偏移，移动时跟随
        struct DragState { bool dragging = false; double dx = 0; double dy = 0; };
        auto circleDrag = std::make_shared<DragState>();
        
        m_circle->addEventListener(TouchEvent::TOUCH_BEGIN, [this, circleDrag](Event& e) {
            auto& te = static_cast<TouchEvent&>(e);
            circleDrag->dragging = true;
            circleDrag->dx = te.getStageX() - m_circle->getX();
            circleDrag->dy = te.getStageY() - m_circle->getY();
        }, this, false, 0);

        // 矩形：拖拽逻辑（按下记录偏移，移动时更新位置，松开停止）
        auto rectDrag = std::make_shared<DragState>();

        m_rect->addEventListener(TouchEvent::TOUCH_BEGIN, [this, rectDrag](Event& e) {
            auto& te = static_cast<TouchEvent&>(e);
            rectDrag->dragging = true;
            rectDrag->dx = te.getStageX() - m_rect->getX();
            rectDrag->dy = te.getStageY() - m_rect->getY();
        }, this, false, 0);

        // 在Stage上监听移动/松开，便于拖拽离开图形后仍可响应
        m_stage->addEventListener(TouchEvent::TOUCH_MOVE, [this, rectDrag, circleDrag](Event& e) {
            auto& te = static_cast<TouchEvent&>(e);
            if (rectDrag->dragging) {
                m_rect->setX(te.getStageX() - rectDrag->dx);
                m_rect->setY(te.getStageY() - rectDrag->dy);
            }
            if (circleDrag->dragging) {
                m_circle->setX(te.getStageX() - circleDrag->dx);
                m_circle->setY(te.getStageY() - circleDrag->dy);
            }
        }, this, false, 0);

        m_stage->addEventListener(TouchEvent::TOUCH_END, [rectDrag, circleDrag](Event& e) {
            (void)e;
            rectDrag->dragging = false;
            circleDrag->dragging = false;
        }, this, false, 0);

        // 键盘：按下空格键随机改变矩形颜色；按下C键切换圆形可见
        m_stage->addEventListener(KeyboardEvent::KEY_DOWN, [this, randColor](Event& e) {
            auto& ke = static_cast<KeyboardEvent&>(e);
            int key = ke.getKeyCode();
            // 使用统一的 KeyCode 常量，语义与TS端 egret.Keyboard 类似
            if (key == egret::KeyCode::SPACE) { // 空格
                auto g = m_rect->getGraphics();
                g->clear();
                g->lineStyle(3.0, 0x0066FF, 1.0);
                g->beginFill(randColor(), 0.8);
                g->drawRect(0, 0, 180, 100);
                g->endFill();
            } else if (key == egret::KeyCode::C) {
                m_circle->setVisible(!m_circle->getVisible());
            }
        }, this, false, 0);
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    // 将日志输出到文件，并开启控制台输出
    egret::Logger::setLogFile("egret-events.log", true);
    egret::Logger::setConsoleEnabled(true);
    egret::Logger::setLogLevel(egret::Logger::Level::INFO);
    EGRET_INFO("=== EgretCpp Events Demo ===");
    EventsDemo app;
    if (!app.initialize()) {
        EGRET_ERROR("初始化失败");
        return 1;
    }
    app.run();
    return 0;
}
