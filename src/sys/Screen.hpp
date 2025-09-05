#pragma once
#include "core/HashObject.hpp"
#include <string>
#include <functional>

namespace egret {
    class Stage;
    class Matrix;
    
    namespace sys {
        
        // 前向声明
        class Player;
        class RenderBuffer;
        class ScreenAdapter;
        class DisplayList;
        
        /**
         * 舞台尺寸计算结果
         */
        struct StageSize {
            double stageWidth = 0.0;
            double stageHeight = 0.0;
            double displayWidth = 0.0;
            double displayHeight = 0.0;
        };
        
        /**
         * 播放器选项配置
         */
        struct PlayerOptions {
            std::string scaleMode = "showAll";
            std::string orientation = "auto";
            int maxTouches = 99;
            double frameRate = 30.0;
            double textureScaleFactor = 1.0;
            double contentWidth = 640.0;
            double contentHeight = 480.0;
            bool showFPS = false;
            bool showLog = false;
            std::string entryClassName = "";
        };

        /**
         * Screen屏幕管理类
         * 基于WebPlayer的完整实现，处理屏幕适配、缩放、旋转等
         */
        class Screen : public HashObject {
        public:
            Screen();
            virtual ~Screen();
            
            /**
             * 初始化屏幕管理器
             * @param options 播放器配置选项
             */
            virtual void initialize(const PlayerOptions& options);
            
            // ========== Screen接口实现 ==========
            
            /**
             * 更新屏幕视口尺寸
             * 对应TypeScript: updateScreenSize()
             */
            virtual void updateScreenSize();

            /**
             * 更新触摸数量
             * 对应TypeScript: updateMaxTouches()
             */
            virtual void updateMaxTouches();

            /**
             * 设置分辨率尺寸
             * 对应TypeScript: setContentSize(width, height)
             */
            virtual void setContentSize(double width, double height);
            
            // ========== 屏幕属性管理 ==========
            
            /**
             * 获取/设置舞台引用
             */
            Stage* getStage() const { return m_stage; }
            void setStage(Stage* stage) { m_stage = stage; }
            
            /**
             * 获取/设置播放器选项
             */
            const PlayerOptions& getPlayerOptions() const { return m_playerOptions; }
            void setPlayerOptions(const PlayerOptions& options) { m_playerOptions = options; }
            
            /**
             * 获取屏幕边界信息
             */
            double getBoundingClientWidth() const { return m_boundingClientWidth; }
            double getBoundingClientHeight() const { return m_boundingClientHeight; }
            
            /**
             * 获取显示区域信息
             */
            double getDisplayWidth() const { return m_displayWidth; }
            double getDisplayHeight() const { return m_displayHeight; }
            
            /**
             * 获取画布尺寸
             */
            double getCanvasWidth() const { return m_canvasWidth; }
            double getCanvasHeight() const { return m_canvasHeight; }
            
            /**
             * 获取缩放因子
             */
            double getScaleX() const { return m_scaleX; }
            double getScaleY() const { return m_scaleY; }
            
            /**
             * 获取旋转角度
             */
            double getRotation() const { return m_rotation; }
            
            // ========== 高级功能 ==========
            
            /**
             * 获取变换矩阵
             */
            Matrix* getTransformMatrix() const;
            
            /**
             * 设置屏幕适配器
             */
            void setScreenAdapter(ScreenAdapter* adapter) { m_screenAdapter = adapter; }
            
            /**
             * 设置显示列表引用
             */
            void setDisplayList(DisplayList* displayList) { m_displayList = displayList; }
            
            /**
             * 设置播放器引用
             */
            void setPlayer(Player* player) { m_player = player; }
            
            // ========== 事件处理 ==========
            
            /**
             * 屏幕方向改变事件处理
             */
            virtual void onOrientationChange();
            
            /**
             * 窗口尺寸改变事件处理
             */
            virtual void onWindowResize(double width, double height);
            
        protected:
            // ========== 受保护的辅助方法 ==========
            
            /**
             * 计算是否需要旋转
             */
            bool shouldRotateScreen() const;
            
            /**
             * 计算舞台尺寸
             */
            StageSize calculateStageSize() const;
            
            /**
             * 应用变换
             */
            void applyTransform(const StageSize& stageSize);
            
            /**
             * 更新画布缩放
             */
            void updateCanvasScale(double scaleX, double scaleY);
            
            /**
             * 更新触摸处理器
             */
            virtual void updateTouchHandler();
            
            /**
             * 更新播放器尺寸
             */
            void updatePlayerSize(double width, double height);
            
        private:
            // ========== 私有成员变量 ==========
            
            // 核心引用
            Stage* m_stage = nullptr;
            Player* m_player = nullptr;
            DisplayList* m_displayList = nullptr;
            ScreenAdapter* m_screenAdapter = nullptr;
            
            // 配置选项
            PlayerOptions m_playerOptions;
            
            // 屏幕尺寸信息
            double m_boundingClientWidth = 0.0;
            double m_boundingClientHeight = 0.0;
            double m_screenWidth = 0.0;
            double m_screenHeight = 0.0;
            
            // 显示尺寸信息
            double m_displayWidth = 0.0;
            double m_displayHeight = 0.0;
            double m_canvasWidth = 0.0;
            double m_canvasHeight = 0.0;
            
            // 变换信息
            double m_scaleX = 1.0;
            double m_scaleY = 1.0;
            double m_rotation = 0.0;
            
            // 画布缩放因子
            double m_canvasScaleX = 1.0;
            double m_canvasScaleY = 1.0;
            
            // 状态标记
            bool m_updateAfterTyping = false;
            bool m_initialized = false;
            
            // 变换矩阵缓存
            mutable Matrix* m_transformMatrix = nullptr;
        };
        
        /**
         * 屏幕适配器接口
         */
        class ScreenAdapter {
        public:
            virtual ~ScreenAdapter() = default;
            
            /**
             * 计算舞台尺寸
             */
            virtual StageSize calculateStageSize(
                const std::string& scaleMode,
                double screenWidth, double screenHeight,
                double contentWidth, double contentHeight) = 0;
        };
        
        /**
         * 默认屏幕适配器实现
         */
        class DefaultScreenAdapter : public ScreenAdapter {
        public:
            StageSize calculateStageSize(
                const std::string& scaleMode,
                double screenWidth, double screenHeight,
                double contentWidth, double contentHeight) override;
        };
        
        // 全局屏幕适配器实例
        extern ScreenAdapter* screenAdapter;

    } // namespace sys
    
} // namespace egret