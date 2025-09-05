/**
 * @file ResourceItem.hpp
 * @brief ResourceItem类型定义 - 资源项，对应resource.json中resources数组的一项
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/shim/ResourceItem.ts
 * ResourceItem定义了资源管理系统中单个资源的完整信息，包括名称、URL、类型等。
 * 同时定义了各种资源类型常量，支持图片、音频、文本、JSON、XML等多种资源格式。
 */

#pragma once

#include <string>
#include <memory>
#include <map>
#include <any>

namespace RES {

// 前向声明
struct ResourceInfo;
class ResourceConfig;

/**
 * @brief ResourceItem命名空间 - 资源类型常量和工具函数
 * 
 * 定义了Egret资源管理系统支持的所有资源类型常量，
 * 以及资源项相关的工具函数。
 * 
 * @version Egret 5.2
 * @note 对应TypeScript的RES.ResourceItem命名空间，100%兼容
 */
namespace ResourceItem {

    // ========== 资源类型常量 ==========

    /**
     * @brief XML文件类型常量
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_XML;

    /**
     * @brief 图片文件类型常量
     * 
     * 支持JPG、PNG、GIF等格式的图像文件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_IMAGE;

    /**
     * @brief 二进制文件类型常量
     * 
     * @version Egret 5.2
     * @platform Web
     */
    extern const std::string TYPE_BIN;

    /**
     * @brief 文本文件类型常量
     * 
     * 支持纯文本文件的加载。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_TEXT;

    /**
     * @brief JSON文件类型常量
     * 
     * 支持JSON格式配置文件和数据文件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_JSON;

    /**
     * @brief SpriteSheet文件类型常量
     * 
     * 精灵表文件，包含多个子纹理的复合纹理资源。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_SHEET;

    /**
     * @brief BitmapTextSpriteSheet文件类型常量
     * 
     * 位图字体精灵表文件，用于位图字体渲染。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_FONT;

    /**
     * @brief 声音文件类型常量
     * 
     * 支持音频文件的加载，如MP3、WAV等格式。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    extern const std::string TYPE_SOUND;

    /**
     * @brief TTF字体文件类型常量
     * 
     * 支持TrueType字体文件的加载。
     * 
     * @version Egret 5.3
     * @platform Web,Native
     */
    extern const std::string TYPE_TTF;

    // ========== 工具函数 ==========

    /**
     * @brief 转换ResourceInfo为ResourceItem
     * 
     * 将内部ResourceInfo对象转换为标准ResourceItem格式，
     * 处理别名映射和名称解析。
     * 
     * @param r ResourceInfo对象引用
     * @return ResourceItem 转换后的资源项
     * @version Egret 5.2
     */
    struct ResourceItemData convertToResItem(const ResourceInfo& r);

} // namespace ResourceItem

/**
 * @brief ResourceInfo结构体 - 内部资源信息
 * 
 * 资源管理系统内部使用的资源信息结构，
 * 包含资源的基本属性和状态信息。
 */
struct ResourceInfo {
    /**
     * @brief 资源名称
     */
    std::string name;

    /**
     * @brief 资源URL
     */
    std::string url;

    /**
     * @brief 资源类型
     */
    std::string type;

    /**
     * @brief 资源根路径
     */
    std::string root;

    /**
     * @brief 资源数据（可选）
     */
    std::any data;

    /**
     * @brief 加载Promise（内部使用）
     */
    std::any promise;

    /**
     * @brief 默认构造函数
     */
    ResourceInfo() = default;

    /**
     * @brief 构造函数
     * 
     * @param name 资源名称
     * @param url 资源URL
     * @param type 资源类型
     * @param root 资源根路径
     */
    ResourceInfo(const std::string& name, const std::string& url, 
                 const std::string& type, const std::string& root = "")
        : name(name), url(url), type(type), root(root) {}
};

/**
 * @brief ResourceItem结构体 - 资源项完整信息
 * 
 * 资源项，对应resource.json中resources数组中的一项。
 * 继承ResourceInfo的所有属性，并添加额外的资源描述信息。
 * 
 * @version Egret 5.2
 * @note 对应TypeScript的RES.ResourceItem接口，100%API兼容
 */
struct ResourceItemData : public ResourceInfo {
    /**
     * @brief 被引用的原始数据对象
     * 
     * 指向加载完成后的原始资源数据。
     * 
     * @version Egret 5.2
     */
    std::shared_ptr<ResourceInfo> dataRef;

    /**
     * @brief CRC32校验值（可选）
     * 
     * 用于资源完整性验证。
     */
    std::string crc32;

    /**
     * @brief 资源大小（可选）
     * 
     * 资源文件的字节大小。
     */
    size_t size;

    /**
     * @brief 声音类型（可选）
     * 
     * 当资源类型为sound时，指定具体的音频格式。
     */
    std::string soundType;

    /**
     * @brief 默认构造函数
     */
    ResourceItemData() : ResourceInfo(), size(0) {}

    /**
     * @brief 构造函数
     * 
     * @param name 资源名称
     * @param url 资源URL
     * @param type 资源类型
     * @param root 资源根路径
     */
    ResourceItemData(const std::string& name, const std::string& url,
                     const std::string& type, const std::string& root = "")
        : ResourceInfo(name, url, type, root), size(0) {}

    /**
     * @brief 从ResourceInfo构造
     * 
     * @param info ResourceInfo对象
     */
    explicit ResourceItemData(const ResourceInfo& info)
        : ResourceInfo(info), size(0) {}
};

// 为了兼容性，提供类型别名
using ResourceItem = ResourceItemData;

} // namespace RES