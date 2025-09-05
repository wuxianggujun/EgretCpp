#include "player/NormalBitmapNode.hpp"

namespace egret {
namespace sys {

    NormalBitmapNode::NormalBitmapNode()
        : RenderNode(RenderNodeType::NormalBitmapNode) {
    }
    
    void NormalBitmapNode::render(void* renderer) {
        if (!m_image || !renderer) {
            return;
        }
        
        m_renderCount++;
        
        // TODO: 实现实际的位图渲染
        // 这里应该使用传入的renderer（通常是SkCanvas*）来绘制位图
        // 具体实现需要根据图像数据格式和Skia API来完成
    }
    
    void NormalBitmapNode::updateTextureData(void* bitmapData, double sourceX, double sourceY,
                                           double sourceW, double sourceH, double offsetX, double offsetY,
                                           double textureW, double textureH, double destW, double destH,
                                           double sourceWidth, double sourceHeight, bool smoothing) {
        // 更新位图数据
        m_bitmapData = bitmapData;
        m_smoothing = smoothing;
        
        // 清空并更新绘制数据
        m_drawData.clear();
        m_drawData.push_back(sourceX);
        m_drawData.push_back(sourceY);
        m_drawData.push_back(sourceW);
        m_drawData.push_back(sourceH);
        m_drawData.push_back(offsetX);
        m_drawData.push_back(offsetY);
        m_drawData.push_back(textureW);
        m_drawData.push_back(textureH);
        m_drawData.push_back(destW);
        m_drawData.push_back(destH);
        m_drawData.push_back(sourceWidth);
        m_drawData.push_back(sourceHeight);
    }
    
    void NormalBitmapNode::drawImage(double sourceX, double sourceY, double sourceW, double sourceH,
                                   double drawX, double drawY, double drawW, double drawH) {
        // 清空并更新绘制数据
        m_drawData.clear();
        m_drawData.push_back(sourceX);
        m_drawData.push_back(sourceY);
        m_drawData.push_back(sourceW);
        m_drawData.push_back(sourceH);
        m_drawData.push_back(drawX);
        m_drawData.push_back(drawY);
        m_drawData.push_back(drawW);
        m_drawData.push_back(drawH);
    }

} // namespace sys
} // namespace egret