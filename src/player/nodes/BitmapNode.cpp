/**
 * @file BitmapNode.cpp
 * @brief BitmapNode类实现 - 位图渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/BitmapNode.ts
 * BitmapNode继承自RenderNode，专门用于位图渲染，支持九宫格、填充模式等高级功能。
 */

#include "player/nodes/BitmapNode.hpp"
#include "player/NormalBitmapNode.hpp"
#include "display/BitmapFillMode.hpp"
#include <algorithm>
#include <cmath>

namespace egret {
namespace sys {

// 全局纹理缩放因子，对应TypeScript中的$TextureScaleFactor
static double TextureScaleFactor = 1.0;

// ========== BitmapNode位图节点实现 ==========

BitmapNode::BitmapNode() 
    : RenderNode(RenderNodeType::BitmapNode) {
    // 初始化已在头文件中完成（使用C++11默认成员初始化）
}

void BitmapNode::render(void* renderer) {
    if (!image || !renderer) {
        return;
    }
    
    m_renderCount++;
    
    // TODO: 当集成Skia渲染器时，在这里实现实际的图像绘制
    // 遍历所有绘制命令并渲染
    // for (const auto& command : m_drawCommands) {
    //     // 使用Skia渲染图像
    //     SkCanvas* canvas = static_cast<SkCanvas*>(renderer);
    //     if (image && image->getSkBitmap()) {
    //         SkRect src = SkRect::MakeXYWH(command.sourceX, command.sourceY, command.sourceW, command.sourceH);
    //         SkRect dst = SkRect::MakeXYWH(command.drawX, command.drawY, command.drawW, command.drawH);
    //         
    //         SkPaint paint;
    //         if (!std::isnan(alpha)) {
    //             paint.setAlpha(static_cast<SkAlpha>(alpha * 255));
    //         }
    //         
    //         canvas->drawBitmapRect(*image->getSkBitmap(), src, dst, &paint);
    //     }
    // }
}

void BitmapNode::drawImage(double sourceX, double sourceY, double sourceW, double sourceH,
                          double drawX, double drawY, double drawW, double drawH) {
    // 完全对应TypeScript实现：
    // public drawImage(sourceX: number, sourceY: number, sourceW: number, sourceH: number,
    //     drawX: number, drawY: number, drawW: number, drawH: number): void {
    //     this.drawData.push(sourceX, sourceY, sourceW, sourceH, drawX, drawY, drawW, drawH);
    //     this.renderCount++;
    // }
    
    // 创建绘制命令并添加到列表
    DrawImageCommand command;
    command.sourceX = sourceX;
    command.sourceY = sourceY; 
    command.sourceW = sourceW;
    command.sourceH = sourceH;
    command.drawX = drawX;
    command.drawY = drawY;
    command.drawW = drawW;
    command.drawH = drawH;
    
    m_drawCommands.push_back(command);
    
    // 增加渲染计数（对应TypeScript中的this.renderCount++）
    m_renderCount++;
}

void BitmapNode::cleanBeforeRender() {
    // 完全对应TypeScript实现：
    // public cleanBeforeRender(): void {
    //     super.cleanBeforeRender();
    //     this.image = null;
    //     this.matrix = null;
    //     this.blendMode = null;
    //     this.alpha = NaN;
    //     this.filter = null;
    // }
    
    // 调用父类的清理方法
    RenderNode::cleanBeforeRender();
    
    // 清空绘制命令（对应清空drawData）
    m_drawCommands.clear();
    
    // 重置属性为默认值
    image = nullptr;
    matrix = nullptr;
    blendMode = -1;
    alpha = std::numeric_limits<double>::quiet_NaN();
    filter = nullptr;
}

// ========== 静态工具方法实现 ==========

void BitmapNode::updateTextureData(std::shared_ptr<NormalBitmapNode> node, std::shared_ptr<BitmapData> image,
                                  double bitmapX, double bitmapY, double bitmapWidth, double bitmapHeight,
                                  double offsetX, double offsetY, double textureWidth, double textureHeight,
                                  double destW, double destH, double sourceWidth, double sourceHeight,
                                  const std::string& fillMode, bool smoothing) {
    // 完全对应TypeScript实现：
    // static $updateTextureData(node: sys.NormalBitmapNode, image: BitmapData, ...)
    
    if (!image) {
        return;
    }
    
    double scale = TextureScaleFactor;
    node->smoothing = smoothing;
    node->image = image;
    node->imageWidth = sourceWidth;
    node->imageHeight = sourceHeight;
    
    if (fillMode == BitmapFillMode::SCALE) {
        double tsX = destW / textureWidth * scale;
        double tsY = destH / textureHeight * scale;
        node->drawImage(bitmapX, bitmapY, bitmapWidth, bitmapHeight,
                       tsX * offsetX, tsY * offsetY, tsX * bitmapWidth, tsY * bitmapHeight);
    }
    else if (fillMode == BitmapFillMode::CLIP) {
        double displayW = std::min(textureWidth, destW);
        double displayH = std::min(textureHeight, destH);
        double scaledBitmapW = bitmapWidth * scale;
        double scaledBitmapH = bitmapHeight * scale;
        drawClipImage(node, scale, bitmapX, bitmapY, scaledBitmapW, scaledBitmapH,
                     offsetX, offsetY, displayW, displayH);
    }
    else { // REPEAT模式
        double scaledBitmapW = bitmapWidth * scale;
        double scaledBitmapH = bitmapHeight * scale;
        for (double startX = 0; startX < destW; startX += textureWidth) {
            for (double startY = 0; startY < destH; startY += textureHeight) {
                double displayW = std::min(destW - startX, textureWidth);
                double displayH = std::min(destH - startY, textureHeight);
                drawClipImage(node, scale, bitmapX, bitmapY, scaledBitmapW, scaledBitmapH,
                             offsetX, offsetY, displayW, displayH, startX, startY);
            }
        }
    }
}

void BitmapNode::updateTextureDataWithScale9Grid(std::shared_ptr<NormalBitmapNode> node, std::shared_ptr<BitmapData> image,
                                                 std::shared_ptr<Rectangle> scale9Grid,
                                                 double bitmapX, double bitmapY, double bitmapWidth, double bitmapHeight,
                                                 double offsetX, double offsetY, double textureWidth, double textureHeight,
                                                 double destW, double destH, double sourceWidth, double sourceHeight,
                                                 bool smoothing) {
    // 完全对应TypeScript实现：
    // static $updateTextureDataWithScale9Grid(node, image, scale9Grid, ...)
    
    node->smoothing = smoothing;
    node->image = image;
    node->imageWidth = sourceWidth;
    node->imageHeight = sourceHeight;
    
    double imageWidth = bitmapWidth;
    double imageHeight = bitmapHeight;
    
    destW = destW - (textureWidth - bitmapWidth * TextureScaleFactor);
    destH = destH - (textureHeight - bitmapHeight * TextureScaleFactor);
    
    double targetW0 = scale9Grid->x - offsetX;
    double targetH0 = scale9Grid->y - offsetY;
    
    double sourceW0 = targetW0 / TextureScaleFactor;
    double sourceH0 = targetH0 / TextureScaleFactor;
    double sourceW1 = scale9Grid->width / TextureScaleFactor;
    double sourceH1 = scale9Grid->height / TextureScaleFactor;
    
    // 防止空心的情况出现
    if (sourceH1 == 0) {
        sourceH1 = 1;
        if (sourceH0 >= imageHeight) {
            sourceH0--;
        }
    }
    if (sourceW1 == 0) {
        sourceW1 = 1;
        if (sourceW0 >= imageWidth) {
            sourceW0--;
        }
    }
    
    double sourceX0 = bitmapX;
    double sourceX1 = sourceX0 + sourceW0;
    double sourceX2 = sourceX1 + sourceW1;
    double sourceW2 = imageWidth - sourceW0 - sourceW1;
    
    double sourceY0 = bitmapY;
    double sourceY1 = sourceY0 + sourceH0;
    double sourceY2 = sourceY1 + sourceH1;
    double sourceH2 = imageHeight - sourceH0 - sourceH1;
    
    double targetW2 = sourceW2 * TextureScaleFactor;
    double targetH2 = sourceH2 * TextureScaleFactor;
    
    if ((sourceW0 + sourceW2) * TextureScaleFactor > destW || (sourceH0 + sourceH2) * TextureScaleFactor > destH) {
        node->drawImage(bitmapX, bitmapY, bitmapWidth, bitmapHeight, offsetX, offsetY, destW, destH);
        return;
    }
    
    double targetX0 = offsetX;
    double targetX1 = targetX0 + targetW0;
    double targetX2 = targetX0 + (destW - targetW2);
    double targetW1 = destW - targetW0 - targetW2;
    
    double targetY0 = offsetY;
    double targetY1 = targetY0 + targetH0;
    double targetY2 = targetY0 + destH - targetH2;
    double targetH1 = destH - targetH0 - targetH2;
    
    // 九宫格绘制：3x3网格
    // 
    //             x0     x1     x2
    //          y0 +------+------+------+
    //             |      |      |      | h0
    //             |      |      |      |
    //          y1 +------+------+------+
    //             |      |      |      | h1
    //             |      |      |      |
    //          y2 +------+------+------+
    //             |      |      |      | h2
    //             |      |      |      |
    //             +------+------+------+
    //                w0     w1     w2
    
    if (sourceH0 > 0) {
        if (sourceW0 > 0) node->drawImage(sourceX0, sourceY0, sourceW0, sourceH0, targetX0, targetY0, targetW0, targetH0);
        if (sourceW1 > 0) node->drawImage(sourceX1, sourceY0, sourceW1, sourceH0, targetX1, targetY0, targetW1, targetH0);
        if (sourceW2 > 0) node->drawImage(sourceX2, sourceY0, sourceW2, sourceH0, targetX2, targetY0, targetW2, targetH0);
    }
    if (sourceH1 > 0) {
        if (sourceW0 > 0) node->drawImage(sourceX0, sourceY1, sourceW0, sourceH1, targetX0, targetY1, targetW0, targetH1);
        if (sourceW1 > 0) node->drawImage(sourceX1, sourceY1, sourceW1, sourceH1, targetX1, targetY1, targetW1, targetH1);
        if (sourceW2 > 0) node->drawImage(sourceX2, sourceY1, sourceW2, sourceH1, targetX2, targetY1, targetW2, targetH1);
    }
    if (sourceH2 > 0) {
        if (sourceW0 > 0) node->drawImage(sourceX0, sourceY2, sourceW0, sourceH2, targetX0, targetY2, targetW0, targetH2);
        if (sourceW1 > 0) node->drawImage(sourceX1, sourceY2, sourceW1, sourceH2, targetX1, targetY2, targetW1, targetH2);
        if (sourceW2 > 0) node->drawImage(sourceX2, sourceY2, sourceW2, sourceH2, targetX2, targetY2, targetW2, targetH2);
    }
}

void BitmapNode::drawClipImage(std::shared_ptr<NormalBitmapNode> node, double scale,
                              double bitmapX, double bitmapY, double scaledBitmapW, double scaledBitmapH,
                              double offsetX, double offsetY, double destW, double destH,
                              double startX, double startY) {
    // 完全对应TypeScript实现：
    // private static drawClipImage(node, scale, bitmapX, bitmapY, scaledBitmapW, ...)
    
    double offset = offsetX + scaledBitmapW - destW;
    if (offset > 0) {
        scaledBitmapW -= offset;
    }
    offset = offsetY + scaledBitmapH - destH;
    if (offset > 0) {
        scaledBitmapH -= offset;
    }
    
    node->drawImage(bitmapX, bitmapY, scaledBitmapW / scale, scaledBitmapH / scale,
                   startX + offsetX, startY + offsetY, scaledBitmapW, scaledBitmapH);
}

} // namespace sys
} // namespace egret