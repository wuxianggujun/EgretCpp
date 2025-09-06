#include "sys/Screen.hpp"
#include "player/Player.hpp"
#include "display/Stage.hpp"
#include "geom/Matrix.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

namespace egret {
namespace sys {

    // 全局屏幕适配器实例
    ScreenAdapter* screenAdapter = nullptr;

    Screen::Screen() : HashObject() {
        // 如果没有全局适配器，创建默认适配器
        if (!screenAdapter) {
            screenAdapter = new DefaultScreenAdapter();
        }
        m_screenAdapter = screenAdapter;
    }

    Screen::~Screen() {
        if (m_transformMatrix) {
            delete m_transformMatrix;
        }
    }

    void Screen::initialize(const PlayerOptions& options) {
        m_playerOptions = options;
        m_initialized = true;
        
        // 初始化屏幕尺寸
        updateScreenSize();
        updateMaxTouches();
    }

    // ========== Screen接口实现 ==========

    void Screen::updateScreenSize() {
        if (!m_initialized || !m_stage) {
            return;
        }
        
        // 检查用户输入状态（在Web环境中这会阻止更新）
        if (m_updateAfterTyping) {
            return;
        }
        
        // 获取容器边界尺寸（在桌面环境中使用窗口尺寸）
        // 这里需要从SDL或其他窗口系统获取实际尺寸
        // 暂时使用配置的内容尺寸作为默认值
        m_boundingClientWidth = m_playerOptions.contentWidth;
        m_boundingClientHeight = m_playerOptions.contentHeight;
        
        if (m_boundingClientWidth == 0.0 || m_boundingClientHeight == 0.0) {
            return;
        }
        
        // 检查是否需要旋转
        bool shouldRotate = shouldRotateScreen();
        
        // 计算屏幕尺寸（考虑旋转）
        m_screenWidth = shouldRotate ? m_boundingClientHeight : m_boundingClientWidth;
        m_screenHeight = shouldRotate ? m_boundingClientWidth : m_boundingClientHeight;
        
        // 使用屏幕适配器计算舞台尺寸
        StageSize stageSize = calculateStageSize();
        
        // 应用变换
        applyTransform(stageSize);
        
        // 更新触摸处理
        updateTouchHandler();
        
        // 更新播放器尺寸
        updatePlayerSize(stageSize.stageWidth, stageSize.stageHeight);
    }

    void Screen::updateMaxTouches() {
        if (!m_stage) {
            return;
        }
        
        // 更新舞台的最大触摸点数
        m_stage->setMaxTouches(m_playerOptions.maxTouches);
        
        // 通知触摸处理器更新
        updateTouchHandler();
    }

    void Screen::setContentSize(double width, double height) {
        m_playerOptions.contentWidth = width;
        m_playerOptions.contentHeight = height;
        updateScreenSize();
    }

    // ========== 事件处理 ==========

    void Screen::onOrientationChange() {
        // 延迟更新以等待系统完成方向切换
        // 在实际实现中，这里应该设置一个定时器
        updateScreenSize();
    }

    void Screen::onWindowResize(double width, double height) {
        m_boundingClientWidth = width;
        m_boundingClientHeight = height;
        updateScreenSize();
    }

    // ========== 受保护的辅助方法 ==========

    bool Screen::shouldRotateScreen() const {
        if (!m_stage) {
            return false;
        }
        
        std::string orientation = m_stage->getOrientation();
        if (orientation == "auto") {
            return false;
        }
        
        // 判断是否需要旋转
        bool isPortraitOrientation = (orientation == "portrait");
        bool isScreenPortrait = m_boundingClientHeight > m_boundingClientWidth;
        
        // 当设备方向与期望方向不匹配时需要旋转
        return isPortraitOrientation != isScreenPortrait;
    }

    StageSize Screen::calculateStageSize() const {
        if (!m_screenAdapter) {
            // 如果没有适配器，返回默认尺寸
            StageSize size;
            size.stageWidth = m_playerOptions.contentWidth;
            size.stageHeight = m_playerOptions.contentHeight;
            size.displayWidth = m_screenWidth;
            size.displayHeight = m_screenHeight;
            return size;
        }
        
        return m_screenAdapter->calculateStageSize(
            m_stage ? m_stage->getScaleMode() : m_playerOptions.scaleMode,
            m_screenWidth, m_screenHeight,
            m_playerOptions.contentWidth, m_playerOptions.contentHeight
        );
    }

    void Screen::applyTransform(const StageSize& stageSize) {
        // 保存尺寸信息
        m_displayWidth = stageSize.displayWidth;
        m_displayHeight = stageSize.displayHeight;
        m_canvasWidth = stageSize.stageWidth;
        m_canvasHeight = stageSize.stageHeight;
        
        // 计算缩放因子
        m_scaleX = m_displayWidth / m_canvasWidth;
        m_scaleY = m_displayHeight / m_canvasHeight;
        
        // 计算旋转角度
        if (shouldRotateScreen()) {
            std::string orientation = m_stage->getOrientation();
            if (orientation == "landscape") {
                m_rotation = 90.0;
            } else if (orientation == "landscapeFlipped") {
                m_rotation = -90.0;
            }
        } else {
            m_rotation = 0.0;
        }
        
        // 计算画布缩放因子（向上取整以避免模糊）
        m_canvasScaleX = std::ceil(m_scaleX);
        m_canvasScaleY = std::ceil(m_scaleY);
        
        // 更新变换矩阵
        if (m_transformMatrix) {
            delete m_transformMatrix;
        }
        m_transformMatrix = new Matrix();
        m_transformMatrix->identity();
        m_transformMatrix->scale(m_scaleX / m_canvasScaleX, m_scaleY / m_canvasScaleY);
        m_transformMatrix->rotate(m_rotation * glm::pi<double>() / 180.0);
        
        // 更新画布缩放
        updateCanvasScale(m_canvasScaleX, m_canvasScaleY);
    }

    void Screen::updateCanvasScale(double scaleX, double scaleY) {
        // 这里需要通知显示列表系统更新画布缩放
        if (m_displayList) {
            // m_displayList->setCanvasScale(scaleX, scaleY);
        }
    }

    void Screen::updateTouchHandler() {
        // 这里需要更新触摸处理器的缩放和旋转参数
        // 在完整实现中，这里会通知触摸系统更新变换参数
    }

    void Screen::updatePlayerSize(double width, double height) {
        if (m_player) {
            // 通知播放器更新舞台尺寸（含RenderBuffer/DisplayList同步）
            m_player->updateStageSize(static_cast<int>(std::round(width)), static_cast<int>(std::round(height)));
        }
        
        // 更新舞台尺寸
        if (m_stage) {
            m_stage->setStageWidthInternal(width);
            m_stage->setStageHeightInternal(height);
        }
    }

    Matrix* Screen::getTransformMatrix() const {
        return m_transformMatrix;
    }

    // ========== DefaultScreenAdapter实现 ==========

    StageSize DefaultScreenAdapter::calculateStageSize(
        const std::string& scaleMode,
        double screenWidth, double screenHeight,
        double contentWidth, double contentHeight) {
        
        StageSize result;
        
        if (scaleMode == "exactFit") {
            // 完全适应，可能变形
            result.stageWidth = contentWidth;
            result.stageHeight = contentHeight;
            result.displayWidth = screenWidth;
            result.displayHeight = screenHeight;
        }
        else if (scaleMode == "showAll") {
            // 显示全部内容，可能有边框
            double scaleX = screenWidth / contentWidth;
            double scaleY = screenHeight / contentHeight;
            double scale = std::min(scaleX, scaleY);
            
            result.stageWidth = contentWidth;
            result.stageHeight = contentHeight;
            result.displayWidth = contentWidth * scale;
            result.displayHeight = contentHeight * scale;
        }
        else if (scaleMode == "noBorder") {
            // 无边框，可能裁切
            double scaleX = screenWidth / contentWidth;
            double scaleY = screenHeight / contentHeight;
            double scale = std::max(scaleX, scaleY);
            
            result.stageWidth = contentWidth;
            result.stageHeight = contentHeight;
            result.displayWidth = contentWidth * scale;
            result.displayHeight = contentHeight * scale;
        }
        else if (scaleMode == "noScale") {
            // 不缩放
            result.stageWidth = contentWidth;
            result.stageHeight = contentHeight;
            result.displayWidth = contentWidth;
            result.displayHeight = contentHeight;
        }
        else if (scaleMode == "fixedWidth") {
            // 固定宽度
            double scale = screenWidth / contentWidth;
            result.stageWidth = contentWidth;
            result.stageHeight = screenHeight / scale;
            result.displayWidth = screenWidth;
            result.displayHeight = screenHeight;
        }
        else if (scaleMode == "fixedHeight") {
            // 固定高度
            double scale = screenHeight / contentHeight;
            result.stageWidth = screenWidth / scale;
            result.stageHeight = contentHeight;
            result.displayWidth = screenWidth;
            result.displayHeight = screenHeight;
        }
        else {
            // 默认为showAll
            return calculateStageSize("showAll", screenWidth, screenHeight, contentWidth, contentHeight);
        }
        
        return result;
    }

} // namespace sys
} // namespace egret
