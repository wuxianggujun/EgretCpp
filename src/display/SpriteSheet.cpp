/**
 * @file SpriteSheet.cpp
 * @brief SpriteSheet类实现 - 精灵表/纹理集，多个子位图拼接而成的集合位图
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/SpriteSheet.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "display/SpriteSheet.hpp"
#include "display/Texture.hpp"

namespace egret {

    // ========== 构造函数和析构函数 ==========

    SpriteSheet::SpriteSheet(std::shared_ptr<Texture> texture) 
        : HashObject(), m_texture(texture), m_bitmapX(0), m_bitmapY(0) {
        
        if (texture) {
            // 计算位图起始位置（对应TypeScript中的this._bitmapX = texture.$bitmapX - texture.$offsetX;）
            m_bitmapX = texture->getBitmapX() - texture->getOffsetX();
            m_bitmapY = texture->getBitmapY() - texture->getOffsetY();
        }
    }

    SpriteSheet::~SpriteSheet() {
        // textureMap通过unordered_map和shared_ptr自动清理
        // 基础纹理通过shared_ptr自动清理
    }

    // ========== 公共方法 ==========

    std::shared_ptr<Texture> SpriteSheet::getTexture(const std::string& name) const {
        // 对应TypeScript中的return this._textureMap[name];
        auto it = m_textureMap.find(name);
        if (it != m_textureMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<Texture> SpriteSheet::createTexture(const std::string& name, 
                                                        int bitmapX, int bitmapY, 
                                                        int bitmapWidth, int bitmapHeight, 
                                                        int offsetX, int offsetY, 
                                                        int textureWidth, int textureHeight) {
        if (!m_texture) {
            return nullptr;
        }

        // 处理默认参数（对应TypeScript中的if (textureWidth === void 0)判断）
        if (textureWidth < 0) {
            textureWidth = offsetX + bitmapWidth;
        }
        if (textureHeight < 0) {
            textureHeight = offsetY + bitmapHeight;
        }

        // 创建新的Texture对象（对应TypeScript中的let texture:Texture = new egret.Texture();）
        auto texture = std::make_shared<Texture>();
        
        // 设置不自动释放BitmapData（对应TypeScript中的texture.disposeBitmapData = false;）
        texture->setDisposeBitmapData(false);
        
        // 共享基础纹理的BitmapData（对应TypeScript中的texture.$bitmapData = this.$texture.$bitmapData;）
        texture->setBitmapData(m_texture->getBitmapData());
        
        // 初始化纹理数据（对应TypeScript中的texture.$initData(...)调用）
        texture->initData(
            m_bitmapX + bitmapX,          // 实际位图X位置
            m_bitmapY + bitmapY,          // 实际位图Y位置
            bitmapWidth,                  // 位图宽度
            bitmapHeight,                 // 位图高度
            offsetX,                      // 偏移X
            offsetY,                      // 偏移Y
            textureWidth,                 // 纹理宽度
            textureHeight,                // 纹理高度
            m_texture->getSourceWidth(),  // 源宽度
            m_texture->getSourceHeight()  // 源高度
        );

        // 缓存纹理对象（对应TypeScript中的this._textureMap[name] = texture;）
        m_textureMap[name] = texture;
        
        return texture;
    }

    void SpriteSheet::dispose() {
        // 释放基础纹理（对应TypeScript中的if (this.$texture) { this.$texture.dispose(); }）
        if (m_texture) {
            m_texture->dispose();
            m_texture.reset();
        }
        
        // 清空纹理缓存
        m_textureMap.clear();
    }

    std::shared_ptr<Texture> SpriteSheet::getBaseTexture() const {
        return m_texture;
    }

    size_t SpriteSheet::getTextureCount() const {
        return m_textureMap.size();
    }

    bool SpriteSheet::hasTexture(const std::string& name) const {
        return m_textureMap.find(name) != m_textureMap.end();
    }

    bool SpriteSheet::removeTexture(const std::string& name) {
        auto it = m_textureMap.find(name);
        if (it != m_textureMap.end()) {
            m_textureMap.erase(it);
            return true;
        }
        return false;
    }

} // namespace egret