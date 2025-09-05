/**
 * @file ImageLoader.hpp
 * @brief ImageLoader类 - 图像文件加载器，支持JPG、PNG、GIF格式图像加载
 * 
 * 翻译自：egret-core-5.4.1/src/egret/net/ImageLoader.ts
 * ImageLoader类用于加载图像文件，使用load()方法启动加载，加载的图像数据存储在data属性中。
 * 支持跨域资源共享(CORS)，支持加载完成和加载失败事件。
 */

#pragma once

#include "events/EventDispatcher.hpp"
#include <string>
#include <memory>

namespace egret {

// 前向声明
class BitmapData;

/**
 * @brief ImageLoader类 - 图像文件加载器
 * 
 * ImageLoader类可用于加载图像（JPG、PNG或GIF）文件。使用load()方法来启动加载。
 * 被加载的图像对象数据将存储在ImageLoader.data属性上。
 * 
 * 事件：
 * - egret.Event.COMPLETE 加载完成时派发
 * - egret.IOErrorEvent.IO_ERROR 加载失败时派发
 * 
 * @note 对应TypeScript的egret.ImageLoader接口，100%API兼容
 * @version Egret 2.4
 * @see egret.HttpRequest
 */
class ImageLoader : public EventDispatcher {
public:
    /**
     * @brief 构造函数 - 创建新的ImageLoader实例
     * 
     * 初始化ImageLoader实例，设置默认属性值。
     */
    ImageLoader();

    /**
     * @brief 析构函数
     * 
     * 清理加载器资源和相关数据。
     */
    virtual ~ImageLoader();

    /**
     * @brief 获取加载的图像数据
     * 
     * 使用load()方法加载成功的BitmapData图像数据。
     * 
     * @return BitmapData* 加载成功的图像数据，未加载完成时返回nullptr
     * @default nullptr
     * @version Egret 2.4
     */
    BitmapData* getData() const;

    /**
     * @brief 设置图像数据（内部使用）
     * 
     * 内部方法，用于设置加载完成的图像数据。
     * 
     * @param data 图像数据指针
     */
    void setData(std::shared_ptr<BitmapData> data);

    /**
     * @brief 获取跨域资源共享设置
     * 
     * 当从其他站点加载一个图片时，指定是否启用跨域资源共享(CORS)。
     * 可能的值："anonymous"、"use-credentials"或空字符串。
     * 
     * @return std::string 当前的crossOrigin设置
     * @default ""
     * @version Egret 2.4
     */
    const std::string& getCrossOrigin() const;

    /**
     * @brief 设置跨域资源共享
     * 
     * 当从其他站点加载一个图片时，指定是否启用跨域资源共享(CORS)，默认值为空字符串。
     * 可以设置为"anonymous"、"use-credentials"或空字符串，设置为其他值将等同于"anonymous"。
     * 
     * @param crossOrigin CORS设置字符串
     * @version Egret 2.4
     */
    void setCrossOrigin(const std::string& crossOrigin);

    /**
     * @brief 启动图像加载
     * 
     * 启动一次图像加载。
     * 注意：若之前已经调用过加载请求，重新调用load()将终止先前的请求，并开始新的加载。
     * 
     * @param url 要加载的图像文件的地址
     * @version Egret 2.4
     */
    void load(const std::string& url);

    /**
     * @brief 取消当前加载操作
     * 
     * 取消正在进行的加载操作。如果没有正在进行的加载操作，此方法无效果。
     */
    void cancel();

    /**
     * @brief 检查是否正在加载
     * 
     * @return bool 如果正在加载返回true，否则返回false
     */
    bool isLoading() const;

    /**
     * @brief 获取当前加载的URL
     * 
     * @return std::string 当前加载的URL，如果没有加载操作则返回空字符串
     */
    const std::string& getCurrentUrl() const;

    // ========== 静态属性 ==========

    /**
     * @brief 获取全局跨域资源共享设置
     * 
     * 指定是否启用跨域资源共享，如果ImageLoader实例有设置过crossOrigin属性将使用设置的属性。
     * 
     * @return std::string 全局crossOrigin设置
     * @version Egret 2.5.7
     */
    static const std::string& getGlobalCrossOrigin();

    /**
     * @brief 设置全局跨域资源共享
     * 
     * 指定是否启用跨域资源共享，如果ImageLoader实例有设置过crossOrigin属性将使用设置的属性。
     * 
     * @param crossOrigin 全局CORS设置字符串
     * @version Egret 2.5.7
     */
    static void setGlobalCrossOrigin(const std::string& crossOrigin);

protected:
    /**
     * @brief 处理加载完成
     * 
     * 内部方法，处理图像加载完成的情况。
     * 创建BitmapData对象并派发COMPLETE事件。
     * 
     * @param imageData 加载完成的图像原始数据
     */
    void onLoadComplete(void* imageData);

    /**
     * @brief 处理加载错误
     * 
     * 内部方法，处理图像加载失败的情况。
     * 派发IO_ERROR事件。
     * 
     * @param errorMessage 错误信息
     */
    void onLoadError(const std::string& errorMessage);

    /**
     * @brief 清理加载状态
     * 
     * 内部方法，清理当前加载状态和相关资源。
     */
    void cleanup();

private:
    /**
     * @brief 加载的图像数据
     * 
     * 使用load()方法加载成功的BitmapData图像数据。
     */
    std::shared_ptr<BitmapData> m_data;

    /**
     * @brief 跨域资源共享设置
     * 
     * 当从其他站点加载图片时的CORS设置。
     */
    std::string m_crossOrigin;

    /**
     * @brief 当前加载的URL
     * 
     * 当前正在加载或最后加载的图像文件URL。
     */
    std::string m_currentUrl;

    /**
     * @brief 加载状态标志
     * 
     * 标识当前是否正在进行加载操作。
     */
    bool m_isLoading;

    /**
     * @brief 全局跨域资源共享设置
     * 
     * 全局的crossOrigin设置，影响所有ImageLoader实例。
     */
    static std::string s_globalCrossOrigin;

    /**
     * @brief 同步加载图像（内部实现）
     * 
     * 使用stb_image单头文件库同步加载图像文件。
     * 支持JPG、PNG、BMP、TGA、GIF等多种格式。
     * 注意：在生产环境中应该使用异步加载避免阻塞。
     * 
     * @param url 图像文件URL
     */
    void loadImageSync(const std::string& url);

    // 禁用拷贝构造和赋值操作
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader& operator=(const ImageLoader&) = delete;
};

} // namespace egret