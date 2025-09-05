#pragma once
#include "player/RenderNode.hpp"
#include "display/BitmapData.hpp"
#include <memory>

namespace egret {
namespace sys {

    /**
     * NormalBitmapNode - 普通位图渲染节点
     * 基于TypeScript egret.sys.NormalBitmapNode实现
     * 用于简单的位图渲染，不包含复杂的变换或特效
     */
    class NormalBitmapNode : public RenderNode {
    public:
        NormalBitmapNode();
        virtual ~NormalBitmapNode() = default;
        
        /**
         * 设置图像数据和属性
         */
        void setImage(void* image) { m_image = image; }
        void* getImage() const { return m_image; }
        
        /**
         * 是否旋转
         */
        bool isRotated() const { return m_rotated; }
        void setRotated(bool rotated) { m_rotated = rotated; }
        
        /**
         * 平滑处理 - 公共属性访问
         */
        bool isSmooth() const { return m_smoothing; }
        void setSmooth(bool smooth) { m_smoothing = smooth; }
        
        /**
         * 设置位图数据
         */
        void setBitmapData(void* bitmapData) { m_bitmapData = bitmapData; }
        void* getBitmapData() const { return m_bitmapData; }
        
        /**
         * 实现基类的渲染方法
         */
        void render(void* renderer) override;
        
        /**
         * 更新纹理数据（简化版，用于普通位图）
         */
        void updateTextureData(void* bitmapData, double sourceX, double sourceY, 
                              double sourceW, double sourceH, double offsetX, double offsetY,
                              double textureW, double textureH, double destW, double destH,
                              double sourceWidth, double sourceHeight, bool smoothing);
        
        /**
         * 绘制图像方法 - 对应BitmapNode.cpp中调用的drawImage
         */
        void drawImage(double sourceX, double sourceY, double sourceW, double sourceH,
                       double drawX, double drawY, double drawW, double drawH);
        
        // ========== 公共属性 - 对应TypeScript版本的公共属性 ==========
        
        /**
         * 平滑处理标志 - 对应 public smoothing: boolean = true;
         */
        bool smoothing = true;
        
        /**
         * 位图数据 - 对应 public image: BitmapData = null;
         */
        std::shared_ptr<BitmapData> image = nullptr;
        
        /**
         * 图像宽度 - 对应 public imageWidth: number;
         */
        double imageWidth = 0.0;
        
        /**
         * 图像高度 - 对应 public imageHeight: number;
         */
        double imageHeight = 0.0;
        
    private:
        void* m_image = nullptr;
        void* m_bitmapData = nullptr;
        bool m_rotated = false;
        bool m_smoothing = true;
    };

} // namespace sys
} // namespace egret