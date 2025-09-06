// 基础日志
#include "utils/Logger.hpp"
// EgretCpp 头文件
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "events/Event.hpp"
#include "events/Keyboard.hpp"
#include "events/KeyCode.hpp"
#include "sys/Screen.hpp"
#include "player/Player.hpp"
#include "geom/Point.hpp"
#include <algorithm>

/**
 * 示例05：键盘快捷键
 * 功能：
 * - Ctrl+S: 保存（日志提示）
 * - 方向键：平移红色方块（持续按压匀速移动）
 * - ESC: 退出程序
 * - Shift: 加速移动
 * 目的：验证 getKeyboard() + KeyCode 的组合键与持续按压行为
 */

/**
 * 自定义的入口类，处理键盘输入和方块移动
 */
class KeyboardController : public egret::DisplayObjectContainer {
private:
    std::shared_ptr<egret::Shape> m_movableBox;
    egret::Point m_position{100.0, 100.0};
    float m_speed = 200.0f;  // pixels per second
    float m_shiftMultiplier = 2.0f;  // Shift key acceleration
    
    // 简单的帧计数器
    int m_frameCounter = 0;
    
    // 窗口边界
    static constexpr double WINDOW_WIDTH = 800.0;
    static constexpr double WINDOW_HEIGHT = 600.0;
    
public:
    KeyboardController() {
        setName("KeyboardController");
        
        // 监听ENTER_FRAME事件进行每帧更新
        addEventListener(egret::Event::ENTER_FRAME,
                         [this](egret::Event& event) { this->onEnterFrame(event); },
                         this, false, 0);
        
        EGRET_INFO("KeyboardController created");
    }
    
    void createMovableBox() {
        // 创建红色方块
        m_movableBox = std::make_shared<egret::Shape>();
        m_movableBox->setName("MovableBox");
        
        // 绘制红色方块
        auto graphics = m_movableBox->getGraphics();
        graphics->beginFill(0xFF4040);  // Red color
        graphics->drawRect(0, 0, 50, 50);
        graphics->endFill();
        
        // 设置初始位置
        m_movableBox->setX(m_position.getX());
        m_movableBox->setY(m_position.getY());
        
        // 添加到自己的显示列表
        addChild(m_movableBox.get());
        
        EGRET_INFO("Red movable box created at initial position");
    }
    
protected:
    void onAddToStage(egret::Stage* stage, int nestLevel) override {
        egret::DisplayObjectContainer::onAddToStage(stage, nestLevel);
        EGRET_INFO("KeyboardController added to stage - creating display objects");
        createMovableBox();
    }
    
private:
    void onEnterFrame(egret::Event& event) {
        m_frameCounter++;
        float deltaTime = 1.0f / 60.0f;  // 假设60FPS
        
        handleInput(deltaTime);
        updateMovableBox();
    }

    void handleInput(float deltaTime) {
        auto& keyboard = egret::getKeyboard();
        
        // Check for Ctrl+S shortcut
        if (keyboard.isCtrlDown() && keyboard.isDown(egret::KeyCode::S)) {
            static bool s_saved = false;
            if (!s_saved) {
                EGRET_INFO("[SAVE] Ctrl+S pressed - Game saved!");
                s_saved = true;
            }
        } else {
            static bool s_saved = false;
            s_saved = false;  // Reset when keys released
        }
        
        // Calculate movement speed (with Shift acceleration)
        float currentSpeed = m_speed * deltaTime;
        if (keyboard.isShiftDown()) {
            currentSpeed *= m_shiftMultiplier;
            // 每30帧输出一次加速信息，避免日志刷屏
            if (m_frameCounter % 30 == 0) {
                EGRET_DEBUG("[BOOST] Shift acceleration active!");
            }
        }
        
        // Handle continuous arrow key movement
        bool moved = false;
        if (keyboard.isDown(egret::KeyCode::LEFT)) {
            m_position.setX(m_position.getX() - currentSpeed);
            moved = true;
        }
        if (keyboard.isDown(egret::KeyCode::RIGHT)) {
            m_position.setX(m_position.getX() + currentSpeed);
            moved = true;
        }
        if (keyboard.isDown(egret::KeyCode::UP)) {
            m_position.setY(m_position.getY() - currentSpeed);
            moved = true;
        }
        if (keyboard.isDown(egret::KeyCode::DOWN)) {
            m_position.setY(m_position.getY() + currentSpeed);
            moved = true;
        }
        
        if (moved) {
            // Keep box within window bounds (留50px的边距给方块大小)
            m_position.setX(std::max(0.0, std::min(WINDOW_WIDTH - 50.0, m_position.getX())));
            m_position.setY(std::max(0.0, std::min(WINDOW_HEIGHT - 50.0, m_position.getY())));
            
            // 每15帧输出一次位置信息，避免日志刷屏
            if (m_frameCounter % 15 == 0) {
                EGRET_DEBUGF("[MOVE] Box position: ({:.1f}, {:.1f})", m_position.getX(), m_position.getY());
            }
        }
        
        // Check for ESC key to exit
        if (keyboard.isDown(egret::KeyCode::ESCAPE)) {
            EGRET_INFO("[EXIT] ESC pressed - Exiting application...");
            // 通过舞台派发退出事件或停止播放器
            if (auto stage = getStage()) {
                // 这里需要找到合适的方式来停止应用
                // 暂时通过日志提示
                EGRET_WARN("Application should exit (mechanism needs implementation)");
            }
        }
    }

    void updateMovableBox() {
        if (m_movableBox) {
            m_movableBox->setX(m_position.getX());
            m_movableBox->setY(m_position.getY());
        }
    }
};

/**
 * 主示例类
 */
class KeyboardShortcutsExample {
private:
    std::shared_ptr<egret::Stage> m_stage;
    std::shared_ptr<egret::sys::Player> m_player;
    
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

public:
    KeyboardShortcutsExample() = default;
    ~KeyboardShortcutsExample() { cleanup(); }

    bool initialize() {
        if (!initializeEgret()) {
            EGRET_ERROR("初始化Egret引擎失败");
            return false;
        }
        
        EGRET_INFO("=== Example 05: Keyboard Shortcuts ===");
        EGRET_INFO("Controls:");
        EGRET_INFO("  Ctrl+S: Save (log message)");
        EGRET_INFO("  Arrow Keys: Move red box continuously");
        EGRET_INFO("  Shift: Accelerate movement");
        EGRET_INFO("  ESC: Exit program");
        EGRET_INFO("=======================================");
        return true;
    }

    void run() {
        if (!initialize()) {
            EGRET_ERROR("初始化失败");
            return;
        }

        // 设置入口类工厂，创建我们的键盘控制器
        m_player->setEntryClassFactory([]() -> std::shared_ptr<egret::DisplayObject> {
            return std::make_shared<KeyboardController>();
        });

        // 启动播放器
        m_player->start();
        
        // 主循环
        EGRET_INFO("进入主循环...");
        m_player->runMainLoop();
    }

private:
    bool initializeEgret() {
        try {
            // 1. 创建 Player（SDL 窗口 + 内置 Screen + Stage）
            m_player = std::make_shared<egret::sys::Player>(
                WINDOW_WIDTH, WINDOW_HEIGHT, "05-Keyboard Shortcuts", "");
            
            // 2. 获取 Stage 并配置
            m_stage = m_player->getStage();
            if (!m_stage) {
                EGRET_ERROR("Player returned null Stage");
                return false;
            }
            m_stage->setStageWidth(WINDOW_WIDTH);
            m_stage->setStageHeight(WINDOW_HEIGHT);
            m_stage->setFrameRate(60); // 60 FPS
            
            return true;
        } catch (const std::exception& e) {
            EGRET_ERRORF("Egret引擎初始化异常: {}", e.what());
            return false;
        }
    }

    void cleanup() {
        if (m_player) {
            m_player->stop();
            m_player.reset();
        }
        if (m_stage) {
            m_stage.reset();
        }
        // 屏幕对象由 Player 内部管理
        EGRET_INFO("KeyboardShortcuts示例清理完成");
    }
};

int main() {
    KeyboardShortcutsExample example;
    example.run();
    return 0;
}
