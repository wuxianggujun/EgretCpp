// 基础日志
#include "utils/Logger.hpp"
// EgretCpp 头文件
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "events/Event.hpp"
#include "events/Keyboard.hpp"
#include "events/KeyCode.hpp"
#include "events/KeyboardEvent.hpp"
#include "sys/Screen.hpp"
#include "player/Player.hpp"
#include "geom/Rectangle.hpp"
#include <algorithm>

/**
 * 示例06：窗口缩放与缩放模式
 * 功能：
 * - 窗口缩放：可通过拖拽调整窗口大小
 * - 按键切换缩放模式：1=EXACT_FIT, 2=SHOW_ALL, 3=NO_SCALE, 4=NO_BORDER
 * - 显示当前窗口大小、Stage大小和缩放模式
 * - 渲染缓冲区正确重建
 * 目的：验证 Stage::resize、DisplayList 缓冲重建、scaleMode 语义
 */

/**
 * 自定义的缩放模式控制器
 */
class ScaleModeController : public egret::DisplayObjectContainer {
private:
    // 演示内容
    std::shared_ptr<egret::Shape> m_contentBox;
    std::shared_ptr<egret::Shape> m_cornerMarker1;
    std::shared_ptr<egret::Shape> m_cornerMarker2;
    std::shared_ptr<egret::Shape> m_cornerMarker3;
    std::shared_ptr<egret::Shape> m_cornerMarker4;

    std::vector<std::string> m_scaleModes;
    int m_currentModeIndex = 1;  // Default: SHOW_ALL
    int m_frameCounter = 0;
    
public:
    ScaleModeController() {
        setName("ScaleModeController");
        
        // 初始化缩放模式列表
        m_scaleModes = {
            egret::StageScaleMode::EXACT_FIT,
            egret::StageScaleMode::SHOW_ALL,
            egret::StageScaleMode::NO_SCALE,
            egret::StageScaleMode::NO_BORDER
        };
        
        // 监听ENTER_FRAME事件进行每帧更新（注意签名为 Event& 且需要传入thisObject等参数）
        addEventListener(egret::Event::ENTER_FRAME,
                         [this](egret::Event& e) { this->onEnterFrame(e); },
                         this, false, 0);

        // 监听键盘按下事件：立即切换缩放模式（更稳定，不依赖轮询）
        addEventListener(egret::KeyboardEvent::KEY_DOWN,
                         [this](egret::Event& e) {
                             if (auto *kev = dynamic_cast<egret::KeyboardEvent*>(&e)) {
                                 this->onKeyDown(*kev);
                             }
                         },
                         this, false, 0);
        
        EGRET_INFO("ScaleModeController created");
    }
    
    void createDisplayObjects() {
        try {
            // 创建主要内容区域（蓝色矩形）
            m_contentBox = std::make_shared<egret::Shape>();
            m_contentBox->setName("ContentBox");
            
            auto graphics = m_contentBox->getGraphics();
            graphics->beginFill(0x4080FF);  // Blue color
            graphics->drawRect(50, 50, 200, 150);  // Fixed content area
            graphics->endFill();
            
            // 添加白色边框
            graphics->lineStyle(2, 0xFFFFFF);
            graphics->drawRect(50, 50, 200, 150);
            
            // 添加中心十字线（黄色）
            graphics->lineStyle(2, 0xFFFF00);
            // 水平线
            graphics->moveTo(70, 125);
            graphics->lineTo(230, 125);
            // 垂直线
            graphics->moveTo(150, 70);
            graphics->lineTo(150, 180);
            
            addChild(m_contentBox.get());
            
            // 创建四个角落的红色标记
            createCornerMarker(m_cornerMarker1, 50, 50);      // 左上角
            createCornerMarker(m_cornerMarker2, 240, 50);     // 右上角
            createCornerMarker(m_cornerMarker3, 50, 190);     // 左下角
            createCornerMarker(m_cornerMarker4, 240, 190);    // 右下角
            
            EGRET_INFO("Content display objects created");
        } catch (const std::exception& e) {
            EGRET_ERRORF("创建显示对象异常: {}", e.what());
        }
    }

    void createCornerMarker(std::shared_ptr<egret::Shape>& marker, double x, double y) {
        marker = std::make_shared<egret::Shape>();
        marker->setName("CornerMarker");
        
        auto graphics = marker->getGraphics();
        graphics->beginFill(0xFF0000);  // Red color
        graphics->drawRect(0, 0, 10, 10);
        graphics->endFill();
        
        marker->setX(x);
        marker->setY(y);
        
        addChild(marker.get());
    }
    
protected:
    void onAddToStage(egret::Stage* stage, int nestLevel) override {
        egret::DisplayObjectContainer::onAddToStage(stage, nestLevel);
        EGRET_INFO("ScaleModeController added to stage - creating display objects");
        createDisplayObjects();
    }
    
private:
    void onEnterFrame(egret::Event& event) {
        m_frameCounter++;
        handleInput();
        updateInfoDisplay();
    }

    void onKeyDown(egret::KeyboardEvent& kev) {
        int kc = kev.getKeyCode();
        int newMode = -1;
        if (kc == egret::KeyCode::NUM_1 || kc == egret::KeyCode::KP_1) newMode = 0;
        else if (kc == egret::KeyCode::NUM_2 || kc == egret::KeyCode::KP_2) newMode = 1;
        else if (kc == egret::KeyCode::NUM_3 || kc == egret::KeyCode::KP_3) newMode = 2;
        else if (kc == egret::KeyCode::NUM_4 || kc == egret::KeyCode::KP_4) newMode = 3;

        if (newMode != -1 && newMode < static_cast<int>(m_scaleModes.size())) {
            m_currentModeIndex = newMode;
            const std::string& mode = m_scaleModes[m_currentModeIndex];
            if (auto stage = getStage()) {
                stage->setScaleMode(mode);
                EGRET_INFOF("[SCALE_MODE] Switched to: {} (via KEY_DOWN)", mode);
            }
        }
    }

    void handleInput() {
        auto& keyboard = egret::getKeyboard();
        
        // Handle scale mode switching (keys 1-4)
        static int lastMode = -1;
        int newMode = -1;
        
        if (keyboard.isDown(egret::KeyCode::NUM_1)) {
            newMode = 0;
        } else if (keyboard.isDown(egret::KeyCode::NUM_2)) {
            newMode = 1;
        } else if (keyboard.isDown(egret::KeyCode::NUM_3)) {
            newMode = 2;
        } else if (keyboard.isDown(egret::KeyCode::NUM_4)) {
            newMode = 3;
        }
        
        if (newMode != -1 && newMode != lastMode && newMode < static_cast<int>(m_scaleModes.size())) {
            m_currentModeIndex = newMode;
            lastMode = newMode;
            
            const std::string& mode = m_scaleModes[m_currentModeIndex];
            if (auto stage = getStage()) {
                stage->setScaleMode(mode);
                EGRET_INFOF("[SCALE_MODE] Switched to: {}", mode);
            }
        }
        
        if (newMode == -1) {
            lastMode = -1;  // Reset when no key pressed
        }
    }

    void updateInfoDisplay() {
        // Print info periodically (every 120 frames at 60fps = 2 seconds)
        if (m_frameCounter % 120 == 0) {
            if (auto stage = getStage()) {
                EGRET_INFO("\\n=== RESIZE & SCALE MODE INFO ===");
                EGRET_INFOF("Stage Size: {:.0f}x{:.0f}", stage->getStageWidth(), stage->getStageHeight());
                EGRET_INFOF("Scale Mode: {}", getCurrentScaleMode());
                EGRET_INFO("Press 1-4 to change scale modes");
                EGRET_INFO("Drag window edges to resize");
                EGRET_INFO("================================\\n");
            }
        }
    }

    std::string getCurrentScaleMode() const {
        if (m_currentModeIndex >= 0 && m_currentModeIndex < m_scaleModes.size()) {
            return m_scaleModes[m_currentModeIndex];
        }
        return "UNKNOWN";
    }
};

/**
 * 主示例类
 */
class ResizeScaleModeExample {
private:
    std::shared_ptr<egret::Stage> m_stage;
    std::shared_ptr<egret::sys::Player> m_player;

    static constexpr int INITIAL_WIDTH = 800;
    static constexpr int INITIAL_HEIGHT = 600;

public:
    ResizeScaleModeExample() = default;
    ~ResizeScaleModeExample() { cleanup(); }

    bool initialize() {
        if (!initializeEgret()) {
            EGRET_ERROR("初始化Egret引擎失败");
            return false;
        }
        
        EGRET_INFO("=== Example 06: Resize & ScaleMode ===");
        EGRET_INFO("Controls:");
        EGRET_INFO("  1: EXACT_FIT (stretch to fill)");
        EGRET_INFO("  2: SHOW_ALL (scale to fit, keep aspect)");
        EGRET_INFO("  3: NO_SCALE (fixed size)");
        EGRET_INFO("  4: NO_BORDER (scale to fill, keep aspect)");
        EGRET_INFO("  Drag window edges to resize");
        EGRET_INFO("=====================================");
        return true;
    }

    void run() {
        if (!initialize()) {
            EGRET_ERROR("初始化失败");
            return;
        }

        // 设置入口类工厂，创建我们的缩放模式控制器
        m_player->setEntryClassFactory([]() -> std::shared_ptr<egret::DisplayObject> {
            return std::make_shared<ScaleModeController>();
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
            // 1. 创建 Player（SDL窗口 + 内置Screen + Stage）
            m_player = std::make_shared<egret::sys::Player>(
                INITIAL_WIDTH, INITIAL_HEIGHT, "06-Resize & ScaleMode", "");
            
            // 2. 获取 Stage 并配置
            m_stage = m_player->getStage();
            if (!m_stage) {
                EGRET_ERROR("Player returned null Stage");
                return false;
            }
            m_stage->setStageWidth(INITIAL_WIDTH);
            m_stage->setStageHeight(INITIAL_HEIGHT);
            m_stage->setScaleMode(egret::StageScaleMode::SHOW_ALL);  // Default mode
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
        EGRET_INFO("ResizeScaleMode示例清理完成");
    }
};

int main() {
    ResizeScaleModeExample example;
    example.run();
    return 0;
}
