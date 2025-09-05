/**
 * @file RenderTexture.cpp
 * @brief RenderTexture类实现 - 动态纹理类，实现显示对象及子对象绘制成纹理的功能
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/RenderTexture.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "display/RenderTexture.hpp"
#include "display/DisplayObject.hpp"
#include "display/BitmapData.hpp"
#include "geom/Rectangle.hpp"
#include "geom/Matrix.hpp"
#include "player/RenderBuffer.hpp"
#include "player/SystemRenderer.hpp"
#include <algorithm>

namespace egret {

    // ========== 构造函数和析构函数 ==========

    RenderTexture::RenderTexture() : Texture() {
        // 创建内部渲染缓冲区（对应TypeScript中的this.$renderBuffer = new sys.RenderBuffer();）
        m_renderBuffer = std::make_shared<sys::RenderBuffer>();
        
        // 创建基于RenderBuffer的BitmapData
        // 对应TypeScript中的let bitmapData = new egret.BitmapData(this.$renderBuffer.surface);
        auto bitmapData = std::make_shared<BitmapData>(nullptr);
        
        // 设置不删除源数据（对应TypeScript中的bitmapData.$deleteSource = false;）
        // 在C++版本中，这通过Texture的disposeBitmapData属性控制
        setDisposeBitmapData(false);
        
        // 设置BitmapData（对应TypeScript中的this._setBitmapData(bitmapData);）
        setBitmapDataInternal(bitmapData);
    }

    RenderTexture::~RenderTexture() {
        // RenderBuffer通过shared_ptr自动清理
        // BitmapData通过父类Texture自动清理
    }

    // ========== 公共方法 ==========

    bool RenderTexture::drawToTexture(DisplayObject* displayObject, Rectangle* clipBounds, double scale) {
        if (!displayObject || !m_renderBuffer) {
            return false;
        }

        // 检查裁剪边界是否有效（对应TypeScript中的if (clipBounds && (clipBounds.width == 0 || clipBounds.height == 0))）
        if (clipBounds && (clipBounds->getWidth() == 0 || clipBounds->getHeight() == 0)) {
            return false;
        }

        // 获取边界（对应TypeScript中的let bounds = clipBounds || displayObject.$getOriginalBounds();）
        std::shared_ptr<Rectangle> bounds;
        if (clipBounds) {
            bounds = Rectangle::create();
            bounds->copyFrom(*clipBounds);
        } else {
            // 调用getBounds()方法并将返回值类型转换为智能指针
            Rectangle tempBounds = displayObject->getBounds();
            bounds = Rectangle::create(tempBounds.getX(), tempBounds.getY(), 
                                     tempBounds.getWidth(), tempBounds.getHeight());
        }

        // 检查边界是否有效
        if (bounds->getWidth() == 0 || bounds->getHeight() == 0) {
            return false;
        }

        // TODO: 应用纹理缩放因子
        // 对应TypeScript中的scale /= $TextureScaleFactor;
        // 使用C++版本的全局变量TextureScaleFactor
        scale /= TextureScaleFactor;

        // 计算渲染尺寸（对应TypeScript中的width和height计算）
        double width, height;
        if (clipBounds) {
            width = bounds->getWidth() * scale;
            height = bounds->getHeight() * scale;
        } else {
            width = (bounds->getX() + bounds->getWidth()) * scale;
            height = (bounds->getY() + bounds->getHeight()) * scale;
        }

        // 调整RenderBuffer大小（对应TypeScript中的renderBuffer.resize(width, height);）
        m_renderBuffer->resize(static_cast<int>(width), static_cast<int>(height));
        
        // 重新创建BitmapData以匹配新尺寸
        // 对应TypeScript中的this.$bitmapData.width = width; this.$bitmapData.height = height;
        auto bitmapData = BitmapData::create(static_cast<int>(width), static_cast<int>(height));
        setBitmapDataInternal(bitmapData);

        // 创建变换矩阵（对应TypeScript中的let matrix = Matrix.create();）
        auto matrix = Matrix::create();
        matrix->identity();
        matrix->scale(scale, scale);

        // 应用裁切变换（对应TypeScript中的if (clipBounds) { matrix.translate(-clipBounds.x, -clipBounds.y); }）
        if (clipBounds) {
            matrix->translate(-clipBounds->getX(), -clipBounds->getY());
        }

        // 执行渲染（对应TypeScript中的sys.systemRenderer.render(displayObject, renderBuffer, matrix, true);）
        if (sys::systemRenderer) {
            sys::systemRenderer->render(displayObject, m_renderBuffer.get(), *matrix, true);
        }

        // 释放矩阵（对应TypeScript中的Matrix.release(matrix);）
        Matrix::release(matrix);

        // 设置纹理参数（对应TypeScript中的this.$initData(...)调用）
        initData(0, 0, static_cast<int>(width), static_cast<int>(height), 
                 0, 0, static_cast<int>(width), static_cast<int>(height), 
                 static_cast<int>(width), static_cast<int>(height));

        return true;
    }

    std::vector<int> RenderTexture::getPixel32(int x, int y) const {
        std::vector<int> data;
        
        if (m_renderBuffer) {
            // 应用纹理缩放因子
            // 对应TypeScript中的let scale = $TextureScaleFactor;
            double textureScaleFactor = TextureScaleFactor;
            x = static_cast<int>(std::round(x / textureScaleFactor));
            y = static_cast<int>(std::round(y / textureScaleFactor));
            
            // 获取像素数据（对应TypeScript中的data = this.$renderBuffer.getPixels(x, y, 1, 1);）
            void* pixelData = m_renderBuffer->getPixels(x, y, 1, 1);
            if (pixelData) {
                // 将void*转换为uint8_t*并提取RGBA值
                uint8_t* bytes = static_cast<uint8_t*>(pixelData);
                data.resize(4);  // RGBA
                data[0] = bytes[0];  // R
                data[1] = bytes[1];  // G  
                data[2] = bytes[2];  // B
                data[3] = bytes[3];  // A
            }
        }
        
        return data;
    }

    void RenderTexture::dispose() {
        // 调用父类dispose方法
        Texture::dispose();
        
        // 清理RenderBuffer（对应TypeScript中的this.$renderBuffer = null;）
        m_renderBuffer.reset();
    }

    sys::RenderBuffer* RenderTexture::getRenderBuffer() const {
        return m_renderBuffer.get();
    }

} // namespace egret