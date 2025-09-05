#pragma once

#include <string>

namespace egret {

    /**
     * 播放器配置选项
     * 对应 TypeScript 的 PlayerOption 接口
     */
    struct PlayerOption {
        /**
         * 入口类完整类名
         */
        std::string entryClassName = "";
        
        /**
         * 默认帧率
         */
        int frameRate = 30;
        
        /**
         * 屏幕适配模式
         */
        std::string scaleMode = "noScale";
        
        /**
         * 初始内容宽度
         */
        int contentWidth = 480;
        
        /**
         * 初始内容高度  
         */
        int contentHeight = 800;
        
        /**
         * 屏幕方向
         */
        std::string orientation = "auto";
        
        /**
         * 显示FPS
         */
        bool showFPS = false;
        
        /**
         * 显示日志
         */
        bool showLog = false;
        
        /**
         * 过滤日志的正则表达式
         */
        std::string logFilter = "";
        
        /**
         * 最大触摸点数
         */
        int maxTouches = 2;
        
        /**
         * 纹理缩放因子
         */
        double textureScaleFactor = 1.0;
        
        /**
         * 默认构造函数
         */
        PlayerOption() = default;
        
        /**
         * 带参数构造函数
         */
        PlayerOption(const std::string& entryClass, int width = 480, int height = 800, int fps = 30)
            : entryClassName(entryClass), contentWidth(width), contentHeight(height), frameRate(fps) {
        }
    };

} // namespace egret