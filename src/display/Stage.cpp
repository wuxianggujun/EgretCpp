#include "Stage.hpp"
#include "events/Event.hpp"
#include "sys/Screen.hpp"
#include "display/DisplayList.hpp"
#include "player/RenderBuffer.hpp"
#include <algorithm>

namespace egret {

    // 静态成员定义
    double Stage::s_globalFrameRate = 30.0;

    Stage::Stage() : DisplayObjectContainer() {
        // 舞台自己就在舞台上，且是根节点
        m_hasAddToStage = true;
        m_nestLevel = 1;
        
        // Stage自己是Stage引用
        m_stage = this;
    }

    // ========== 帧率控制实现 ==========

    void Stage::setFrameRate(double value) {
        // 限制帧率范围
        value = std::clamp(value, 0.01, 1000.0);
        
        if (s_globalFrameRate == value) {
            return;
        }
        
        s_globalFrameRate = value;
        m_frameRate = value;
        
        // TODO: 这里需要通知渲染系统更新帧率
        // 在完整实现中，这里会调用 ticker.$setFrameRate(value);
    }

    // ========== 缩放模式实现 ==========

    void Stage::setScaleMode(const std::string& value) {
        if (m_scaleMode == value) {
            return;
        }
        m_scaleMode = value;
        updateScreenSize();
    }

    // ========== 屏幕方向实现 ==========

    void Stage::setOrientation(const std::string& value) {
        if (m_orientation == value) {
            return;
        }
        m_orientation = value;
        updateScreenSize();
    }

    // ========== 纹理缩放实现 ==========

    void Stage::setTextureScaleFactor(double value) {
        if (m_textureScaleFactor == value) {
            return;
        }
        m_textureScaleFactor = value;
        
        // TODO: 通知渲染系统更新全局纹理缩放因子
        // 在完整实现中，这里会设置 egret.$TextureScaleFactor = value;
    }

    // ========== 触摸控制实现 ==========

    void Stage::setMaxTouches(int value) {
        if (m_maxTouches == value) {
            return;
        }
        m_maxTouches = value;
        updateMaxTouches();
    }

    // ========== 内容尺寸实现 ==========

    void Stage::setContentSize(double width, double height) {
        if (m_screen) {
            m_screen->setContentSize(width, height);
        }
    }

    // ========== 渲染控制实现 ==========

    void Stage::invalidate() {
        m_invalidateRenderFlag = true;
    }

    // ========== 绘制方法实现 ==========

    void Stage::drawToSurfaceAutoClear() {
        if (m_displayList) {
            // 调用显示列表的绘制方法
            m_displayList->drawToSurface();
        }
    }

    void Stage::drawToSurface() {
        if (m_displayList) {
            // 调用显示列表的舞台渲染方法（根据TypeScript源码，应该调用stageRenderToSurface）
            m_displayList->stageRenderToSurface();
        }
    }

    void Stage::resize(double width, double height) {
        m_stageWidth = width;
        m_stageHeight = height;
        
        // TODO: 调整渲染缓冲区大小
        // 在完整实现中，这里会调用 this.m_displayList->getRenderBuffer()->resize(width, height);
        if (m_displayList && m_displayList->getRenderBuffer()) {
            m_displayList->getRenderBuffer()->resize(width, height);
        }
        
        dispatchResizeEvent();
    }

    // ========== 私有辅助方法实现 ==========

    void Stage::updateScreenSize() {
        if (m_screen) {
            m_screen->updateScreenSize();
        }
    }

    void Stage::updateMaxTouches() {
        if (m_screen) {
            m_screen->updateMaxTouches();
        }
    }

    void Stage::dispatchResizeEvent() {
        // 创建并分发RESIZE事件
        // 使用EventDispatcher的dispatchEventWith便捷方法
        dispatchEventWith(Event::RESIZE, false, nullptr, false);
    }

} // namespace egret