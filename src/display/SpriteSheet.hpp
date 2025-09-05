/**
 * @file SpriteSheet.hpp
 * @brief SpriteSheet类 - 精灵表/纹理集，多个子位图拼接而成的集合位图
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/SpriteSheet.ts
 * SpriteSheet是一张由多个子位图拼接而成的集合位图，包含多个Texture对象。
 * 每个Texture都共享SpriteSheet的集合位图，但指向不同区域，可显著提升WebGL/OpenGL性能。
 */

#pragma once

#include "core/HashObject.hpp"
#include <memory>
#include <unordered_map>
#include <string>

namespace egret {

// 前置声明
class Texture;

/**
 * @brief SpriteSheet类 - 精灵表/纹理集，性能优化的核心工具
 * 
 * SpriteSheet是一张由多个子位图拼接而成的集合位图，它包含多个Texture对象。
 * 每一个Texture都共享SpriteSheet的集合位图，但是指向它的不同的区域。
 * 在WebGL/OpenGL上，这种做法可以显著提升性能，同时可以方便地进行素材整合，
 * 降低HTTP请求数量。
 * 
 * SpriteSheet格式的具体规范：
 * https://github.com/egret-labs/egret-core/wiki/Egret-SpriteSheet-Specification
 * 
 * @note 对应TypeScript的egret.SpriteSheet类，100%API兼容
 * @extends HashObject
 */
class SpriteSheet : public HashObject {
public:
    /**
     * @brief 构造函数 - 创建新的SpriteSheet对象
     * 
     * 使用指定的Texture创建一个SpriteSheet对象。
     * 
     * @param texture 基础纹理，作为精灵表的底图
     * 
     * @note 对应TypeScript的构造函数
     */
    explicit SpriteSheet(std::shared_ptr<Texture> texture);

    /**
     * @brief 析构函数
     * 
     * 清理纹理缓存和相关资源。
     */
    virtual ~SpriteSheet();

    /**
     * @brief 根据指定纹理名称获取一个缓存的Texture对象
     * 
     * 从纹理缓存字典中获取指定名称的Texture对象。
     * 
     * @param name 缓存这个Texture对象所使用的名称
     * @return Texture对象，如果不存在则返回nullptr
     * 
     * @note 对应TypeScript的getTexture()方法
     */
    std::shared_ptr<Texture> getTexture(const std::string& name) const;

    /**
     * @brief 为SpriteSheet上的指定区域创建一个新的Texture对象并缓存它
     * 
     * 在SpriteSheet的指定区域创建一个新的Texture对象，并将其添加到缓存中。
     * 如果名称已存在，将会覆盖之前的Texture对象。
     * 
     * @param name 缓存这个Texture对象所使用的名称
     * @param bitmapX 纹理区域在bitmapData上的起始坐标x
     * @param bitmapY 纹理区域在bitmapData上的起始坐标y
     * @param bitmapWidth 纹理区域在bitmapData上的宽度
     * @param bitmapHeight 纹理区域在bitmapData上的高度
     * @param offsetX 原始位图的非透明区域x起始点，默认为0
     * @param offsetY 原始位图的非透明区域y起始点，默认为0
     * @param textureWidth 原始位图的宽度，若不传入则使用offsetX+bitmapWidth
     * @param textureHeight 原始位图的高度，若不传入则使用offsetY+bitmapHeight
     * @return 创建的Texture对象
     * 
     * @note 对应TypeScript的createTexture()方法
     */
    std::shared_ptr<Texture> createTexture(const std::string& name, 
                                          int bitmapX, int bitmapY, 
                                          int bitmapWidth, int bitmapHeight, 
                                          int offsetX = 0, int offsetY = 0, 
                                          int textureWidth = -1, int textureHeight = -1);

    /**
     * @brief 释放纹理资源
     * 
     * 释放SpriteSheet及其所有缓存的纹理对象。
     * 
     * @note 对应TypeScript的dispose()方法
     */
    void dispose();

    /**
     * @brief 获取基础纹理
     * 
     * 获取SpriteSheet使用的基础纹理对象。
     * 
     * @return 基础纹理对象
     */
    std::shared_ptr<Texture> getBaseTexture() const;

    /**
     * @brief 获取纹理缓存数量
     * 
     * 获取当前缓存的纹理对象数量。
     * 
     * @return 缓存的纹理数量
     */
    size_t getTextureCount() const;

    /**
     * @brief 检查是否存在指定名称的纹理
     * 
     * 检查纹理缓存中是否存在指定名称的纹理。
     * 
     * @param name 纹理名称
     * @return true如果存在，false如果不存在
     */
    bool hasTexture(const std::string& name) const;

    /**
     * @brief 移除指定名称的纹理
     * 
     * 从纹理缓存中移除指定名称的纹理对象。
     * 
     * @param name 要移除的纹理名称
     * @return true如果成功移除，false如果纹理不存在
     */
    bool removeTexture(const std::string& name);

protected:
    /**
     * @brief 基础纹理对象
     * 
     * SpriteSheet使用的底图纹理，所有子纹理都基于此纹理的不同区域。
     * 对应TypeScript中的$texture成员。
     */
    std::shared_ptr<Texture> m_texture;

    /**
     * @brief 纹理缓存字典
     * 
     * 存储所有创建的子纹理对象的映射表。
     * 对应TypeScript中的_textureMap成员。
     */
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureMap;

private:
    /**
     * @brief 位图区域在bitmapData上的起始位置x
     * 
     * 表示这个SpriteSheet的位图区域在bitmapData上的起始位置x。
     * 对应TypeScript中的_bitmapX成员。
     */
    int m_bitmapX;

    /**
     * @brief 位图区域在bitmapData上的起始位置y
     * 
     * 表示这个SpriteSheet的位图区域在bitmapData上的起始位置y。
     * 对应TypeScript中的_bitmapY成员。
     */
    int m_bitmapY;

    // 禁用拷贝构造和赋值操作
    SpriteSheet(const SpriteSheet&) = delete;
    SpriteSheet& operator=(const SpriteSheet&) = delete;
};

} // namespace egret