//
// Created by wuxianggujun on 25-9-4.
//

#include "Bitmap.hpp"
#include "Texture.hpp"
#include "BitmapData.hpp"
#include "Stage.hpp"
#include "geom/Rectangle.hpp"
#include "player/NormalBitmapNode.hpp"
#include <algorithm>
#include <cmath>

namespace egret
{
    // ========== 静态成员变量定义 ==========
    bool Bitmap::defaultSmoothing = true;
    
    // ========== 构造函数 ==========
    Bitmap::Bitmap(std::shared_ptr<Texture> value)
        : DisplayObject()
        , m_texture(nullptr)
        , m_bitmapData(nullptr)
        , m_bitmapX(0.0)
        , m_bitmapY(0.0)
        , m_bitmapWidth(0.0)
        , m_bitmapHeight(0.0)
        , m_offsetX(0.0)
        , m_offsetY(0.0)
        , m_textureWidth(0.0)
        , m_textureHeight(0.0)
        , m_sourceWidth(0.0)
        , m_sourceHeight(0.0)
        , m_smoothing(defaultSmoothing)
        , m_pixelSnapping(false)
        , m_explicitBitmapWidth(std::numeric_limits<double>::quiet_NaN())
        , m_explicitBitmapHeight(std::numeric_limits<double>::quiet_NaN())
        , m_scale9Grid(nullptr)
    {
        // 创建NormalBitmapNode渲染节点
        setRenderNode(std::make_shared<sys::NormalBitmapNode>());
        
        // 设置纹理
        setTextureInternal(value);
        
        // 如果有纹理，设置旋转属性
        if (value && getRenderNode()) {
            auto bitmapNode = std::dynamic_pointer_cast<sys::NormalBitmapNode>(getRenderNode());
            if (bitmapNode) {
                // bitmapNode->setRotated(value->isRotated());
            }
        }
    }
    
    // ========== 公开属性访问器实现 ==========
    
    void Bitmap::setTexture(std::shared_ptr<Texture> value)
    {
        if (setTextureInternal(value)) {
            if (value && getRenderNode()) {
                auto bitmapNode = std::dynamic_pointer_cast<sys::NormalBitmapNode>(getRenderNode());
                if (bitmapNode) {
                    // bitmapNode->setRotated(value->isRotated());
                }
            }
        }
    }
    
    void Bitmap::setBitmapData(std::shared_ptr<BitmapData> value)
    {
        // 在Egret中，setBitmapData实际上是调用setTexture
        // setTextureInternal(value);
    }
    
    void Bitmap::setSmoothing(bool value)
    {
        if (m_smoothing != value) {
            m_smoothing = value;
            markRenderDirty();
        }
    }
    
    void Bitmap::setScale9Grid(std::shared_ptr<Rectangle> value)
    {
        m_scale9Grid = value;
        markRenderDirty();
    }
    
    void Bitmap::setBitmapSize(double width, double height)
    {
        m_explicitBitmapWidth = width;
        m_explicitBitmapHeight = height;
        markRenderDirty();
    }
    
    // ========== DisplayObject虚函数重写 ==========
    
    std::shared_ptr<Rectangle> Bitmap::getBounds() const
    {
        auto bounds = Rectangle::create();
        
        // 如果有明确设置的尺寸，使用明确尺寸
        double width = !std::isnan(m_explicitBitmapWidth) ? m_explicitBitmapWidth : m_textureWidth;
        double height = !std::isnan(m_explicitBitmapHeight) ? m_explicitBitmapHeight : m_textureHeight;
        
        bounds->setRect(0, 0, width, height);
        return bounds;
    }
    
    std::shared_ptr<Rectangle> Bitmap::getMeasuredBounds() const
    {
        return getBounds();
    }
    
    // ========== 受保护的DisplayObject虚函数重写 ==========
    
    void Bitmap::onAddToStage(Stage* stage, int nestLevel)
    {
        DisplayObject::onAddToStage(stage, nestLevel);
        
        // 如果有纹理和位图数据，注册到BitmapData管理器
        if (m_texture && m_texture->getBitmapData()) {
            // BitmapData::addDisplayObject(shared_from_this(), m_texture->getBitmapData());
        }
    }
    
    void Bitmap::onRemoveFromStage()
    {
        DisplayObject::onRemoveFromStage();
        
        // 从BitmapData管理器移除
        if (m_texture && m_texture->getBitmapData()) {
            // BitmapData::removeDisplayObject(shared_from_this(), m_texture->getBitmapData());
        }
    }
    
    void Bitmap::measure()
    {
        // 测量位图尺寸
        if (m_texture) {
            double width = !std::isnan(m_explicitBitmapWidth) ? m_explicitBitmapWidth : m_textureWidth;
            double height = !std::isnan(m_explicitBitmapHeight) ? m_explicitBitmapHeight : m_textureHeight;
            
            // 设置测量尺寸
            setMeasuredSize(width, height);
        } else {
            setMeasuredSize(0, 0);
        }
    }
    
    // ========== 内部方法实现 ==========
    
    void Bitmap::refreshImageData()
    {
        if (!m_texture) {
            return;
        }
        
        // 获取纹理数据并设置到渲染节点
        auto bitmapData = m_texture->getBitmapData();
        if (bitmapData) {
            setImageData(bitmapData, 
                        m_texture->getBitmapX(), m_texture->getBitmapY(),
                        m_texture->getBitmapWidth(), m_texture->getBitmapHeight(),
                        m_texture->getOffsetX(), m_texture->getOffsetY(),
                        m_texture->getTextureWidth(), m_texture->getTextureHeight(),
                        m_texture->getSourceWidth(), m_texture->getSourceHeight());
        }
    }
    
    void Bitmap::setImageData(std::shared_ptr<BitmapData> bitmapData, double bitmapX, double bitmapY,
                             double bitmapWidth, double bitmapHeight, double offsetX, double offsetY,
                             double textureWidth, double textureHeight, double sourceWidth, double sourceHeight)
    {
        m_bitmapData = bitmapData;
        m_bitmapX = bitmapX;
        m_bitmapY = bitmapY;
        m_bitmapWidth = bitmapWidth;
        m_bitmapHeight = bitmapHeight;
        m_offsetX = offsetX;
        m_offsetY = offsetY;
        m_textureWidth = textureWidth;
        m_textureHeight = textureHeight;
        m_sourceWidth = sourceWidth;
        m_sourceHeight = sourceHeight;
        
        // 更新渲染节点
        auto bitmapNode = std::dynamic_pointer_cast<sys::NormalBitmapNode>(getRenderNode());
        if (bitmapNode) {
            bitmapNode->setBitmapData(bitmapData.get());
            // 设置其他渲染参数
        }
        
        markRenderDirty();
    }
    
    // ========== 私有辅助方法实现 ==========
    
    bool Bitmap::setTextureInternal(std::shared_ptr<Texture> value)
    {
        if (value == m_texture) {
            return false;
        }
        
        auto oldTexture = m_texture;
        m_texture = value;
        
        if (value) {
            refreshImageData();
        } else {
            // 清除纹理时，重置图像数据
            if (oldTexture && oldTexture->getBitmapData()) {
                // BitmapData::removeDisplayObject(shared_from_this(), oldTexture->getBitmapData());
            }
            setImageData(nullptr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            markRenderDirty();
            return true;
        }
        
        // 处理舞台上的纹理切换
        if (getStage()) {
            if (oldTexture && oldTexture->getBitmapData()) {
                // auto oldHashCode = oldTexture->getBitmapData()->getHashCode();
                // auto newHashCode = value->getBitmapData() ? value->getBitmapData()->getHashCode() : -1;
                // if (oldHashCode == newHashCode) {
                //     markRenderDirty();
                //     return true;
                // }
                // BitmapData::removeDisplayObject(shared_from_this(), oldTexture->getBitmapData());
            }
            if (value->getBitmapData()) {
                // BitmapData::addDisplayObject(shared_from_this(), value->getBitmapData());
            }
        }
        
        markRenderDirty();
        return true;
    }
    
    void Bitmap::markRenderDirty()
    {
        setRenderDirty(true);
        
        // 标记父级缓存脏标记
        auto parent = getParent();
        if (parent && !parent->isCacheDirty()) {
            parent->setCacheDirty(true);
            parent->cacheDirtyUp();
        }
        
        // 标记遮罩对象缓存脏标记
        auto maskedObject = getMaskedObject();
        if (maskedObject && !maskedObject->isCacheDirty()) {
            maskedObject->setCacheDirty(true);
            maskedObject->cacheDirtyUp();
        }
    }
    
} // namespace egret