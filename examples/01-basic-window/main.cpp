// 基础日志
#include "utils/Logger.hpp"
// EgretCpp 头文件
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "sys/Screen.hpp"
#include "player/Player.hpp"

/**
 * Basic Window Example
 * 
 * This example demonstrates:
 * 1. SDL3 window creation and OpenGL context setup
 * 2. Skia rendering surface initialization
 * 3. EgretCpp Stage and Player initialization
 * 4. Basic rendering loop with event handling
 * 
 * Based on Egret TypeScript initialization flow:
 * runEgret() -> WebPlayer -> Stage -> Player -> render loop
 */
class BasicWindowExample {
private:
    std::shared_ptr<egret::Stage> m_stage;
    std::shared_ptr<egret::sys::Screen> m_screen;
    std::shared_ptr<egret::sys::Player> m_player;

    // 演示用显示对象
    std::shared_ptr<egret::Shape> m_circle;
    std::shared_ptr<egret::Shape> m_rectangle;

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

public:
    BasicWindowExample() = default;
    ~BasicWindowExample() { cleanup(); }

    bool initialize() {
        if (!initializeEgret()) {
            EGRET_ERROR("初始化Egret引擎失败");
            return false;
        }
        if (!createDisplayObjects()) {
            EGRET_ERROR("创建显示对象失败");
            return false;
        }
        EGRET_INFO("EgretCpp 示例初始化完成");
        EGRET_INFOF("窗口: {}x{}", WINDOW_WIDTH, WINDOW_HEIGHT);
        return true;
    }

    void run() {
        EGRET_INFO("进入主循环...");
        if (m_player) {
            m_player->runMainLoop();
        }
    }

private:
    bool initializeEgret() {
        try {
            // 1. 创建 Screen（屏幕适配）
            m_screen = std::make_shared<egret::sys::Screen>();
            m_screen->setContentSize(WINDOW_WIDTH, WINDOW_HEIGHT);
            
            // 2. 创建 Player 并使用其 Stage
            m_player = std::make_shared<egret::sys::Player>(
                WINDOW_WIDTH, WINDOW_HEIGHT, "EgretCpp - Basic Window Example", "");
            
            // 3. 获取 Stage 并配置
            m_stage = m_player->getStage();
            if (!m_stage) {
                EGRET_ERROR("Player returned null Stage");
                return false;
            }
            m_stage->setScreen(m_screen.get());
            m_stage->setStageWidth(WINDOW_WIDTH);
            m_stage->setStageHeight(WINDOW_HEIGHT);
            m_stage->setFrameRate(60); // 60 FPS
            
            EGRET_INFO("Egret 引擎组件已初始化:");
            EGRET_INFOF("- Stage: {}x{} @ 60fps", WINDOW_WIDTH, WINDOW_HEIGHT);
            EGRET_INFO("- 屏幕适配已启用");
            
            return true;
        } catch (const std::exception& e) {
            EGRET_ERRORF("Egret 初始化异常: {}", e.what());
            return false;
        }
    }
    
    bool createDisplayObjects() {
        try {
            // 创建圆形
            m_circle = std::make_shared<egret::Shape>();
            auto circleGraphics = m_circle->getGraphics();
            
            // 红色圆形
            circleGraphics->beginFill(0xFF0000, 1.0);
            circleGraphics->drawCircle(0, 0, 50);
            circleGraphics->endFill();
            
            // 位置
            m_circle->setX(200);
            m_circle->setY(150);
            
            // 添加到舞台
            m_stage->addChild(m_circle.get());
            
            // 创建矩形
            m_rectangle = std::make_shared<egret::Shape>();
            auto rectGraphics = m_rectangle->getGraphics();
            
            // 蓝色描边、绿色填充
            rectGraphics->lineStyle(3.0, 0x0000FF, 1.0);
            rectGraphics->beginFill(0x00FF00, 0.7);
            rectGraphics->drawRect(0, 0, 120, 80);
            rectGraphics->endFill();
            
            // 位置
            m_rectangle->setX(400);
            m_rectangle->setY(200);
            
            // 添加到舞台
            m_stage->addChild(m_rectangle.get());
            
            EGRET_INFO("显示对象创建成功:");
            EGRET_INFO("- 红色圆形 (200, 150)");
            EGRET_INFO("- 绿色矩形带蓝边 (400, 200)");
            
            return true;
        } catch (const std::exception& e) {
            EGRET_ERRORF("创建显示对象异常: {}", e.what());
            return false;
        }
    }
    // 如需扩展：这里可添加 onTick 等更新逻辑

    void cleanup() {
        m_player.reset();
        m_stage.reset();
        m_screen.reset();
    }
};

int main(int argc, char* argv[]) {
    EGRET_INFO("=== Example 01 - Basic Window ===");
    EGRET_INFO("Initializing engine components...");
    
    BasicWindowExample app;
    
    if (!app.initialize()) {
        EGRET_ERROR("Failed to initialize application");
        return 1;
    }
    
    EGRET_INFO("Starting main loop...");
    app.run();
    
    EGRET_INFO("Application finished successfully");
    return 0;
}
