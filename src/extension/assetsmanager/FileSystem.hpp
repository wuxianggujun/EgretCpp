/**
 * @file FileSystem.hpp
 * @brief FileSystem文件系统 - assetsManager底层存储资源信息的文件系统抽象层
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/core/FileSystem.ts
 * FileSystem提供了资源管理系统底层的文件存储抽象，包括文件信息结构、字典存储和文件系统操作接口。
 * 支持虚拟文件系统的创建、文件添加、查询、删除等操作。
 */

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <any>
#include <iostream>

namespace RES {

/**
 * @brief File结构体 - assetsManager底层存储资源信息
 * 
 * 表示文件系统中单个文件的完整信息，包括URL、类型、名称、根路径等属性。
 * 
 * @version Egret 5.2
 * @note 对应TypeScript的RES.File接口，100%API兼容
 */
struct File {
    /**
     * @brief 文件URL
     * 
     * 文件的完整URL地址，可以是相对路径或绝对路径。
     */
    std::string url;

    /**
     * @brief 文件类型
     * 
     * 文件的MIME类型或资源类型，如"image"、"json"等。
     */
    std::string type;

    /**
     * @brief 文件名称
     * 
     * 文件的逻辑名称，用于资源查找和引用。
     */
    std::string name;

    /**
     * @brief 根路径
     * 
     * 文件的根目录路径，用于相对路径解析。
     */
    std::string root;

    /**
     * @brief 默认构造函数
     */
    File() = default;

    /**
     * @brief 构造函数
     * 
     * @param url 文件URL
     * @param type 文件类型
     * @param name 文件名称
     * @param root 根路径
     */
    File(const std::string& url, const std::string& type = "", 
         const std::string& name = "", const std::string& root = "")
        : url(url), type(type), name(name), root(root) {}
};

/**
 * @brief Dictionary类型 - 字典存储结构
 * 
 * 使用递归结构表示文件系统的层次化存储，支持文件和子目录的嵌套存储。
 */
using Dictionary = std::unordered_map<std::string, std::any>;

/**
 * @brief FileSystem接口 - 文件系统操作接口
 * 
 * 定义了文件系统的基本操作接口，包括文件添加、查询、删除和调试输出。
 * 
 * @version Egret 5.2
 */
class FileSystem {
public:
    /**
     * @brief 析构函数
     */
    virtual ~FileSystem() = default;

    /**
     * @brief 添加文件到文件系统
     * 
     * @param data 文件数据，包含name、type、url、root等信息
     */
    virtual void addFile(const File& data) = 0;

    /**
     * @brief 根据文件名获取文件
     * 
     * @param filename 文件名
     * @return File* 文件指针，如果不存在则返回nullptr
     */
    virtual File* getFile(const std::string& filename) = 0;

    /**
     * @brief 打印文件系统调试信息
     * 
     * 输出当前文件系统的存储结构，用于调试和监控。
     */
    virtual void profile() = 0;

    /**
     * @brief 从文件系统中移除文件
     * 
     * @param filename 要移除的文件名
     */
    virtual void removeFile(const std::string& filename) = 0;
};

/**
 * @brief NewFileSystem类 - 新的文件系统实现
 * 
 * 基于Dictionary字典结构的文件系统实现，支持层次化的文件存储和管理。
 * 提供了完整的虚拟文件系统功能，包括目录创建、文件存储、路径解析等。
 * 
 * @version Egret 5.2
 * @note 对应TypeScript的RES.NewFileSystem类，100%API兼容
 */
class NewFileSystem : public FileSystem {
public:
    /**
     * @brief 构造函数
     * 
     * @param data 初始的字典数据，默认为空字典
     */
    explicit NewFileSystem(const Dictionary& data = Dictionary());

    /**
     * @brief 析构函数
     */
    virtual ~NewFileSystem();

    /**
     * @brief 打印文件系统调试信息
     * 
     * 输出当前文件系统的完整存储结构。
     */
    void profile() override;

    /**
     * @brief 添加文件到文件系统
     * 
     * 将文件添加到虚拟文件系统中，自动创建必要的目录结构。
     * 
     * @param filename 文件路径
     * @param type 文件类型，默认为空字符串
     */
    void addFile(const std::string& filename, const std::string& type = "");

    /**
     * @brief 添加文件到文件系统（重载版本）
     * 
     * @param data 文件数据结构
     */
    void addFile(const File& data) override;

    /**
     * @brief 根据文件名获取文件
     * 
     * @param filename 文件路径
     * @return File* 文件指针，如果不存在则返回nullptr
     */
    File* getFile(const std::string& filename) override;

    /**
     * @brief 从文件系统中移除文件
     * 
     * @param filename 要移除的文件路径
     */
    void removeFile(const std::string& filename) override;

private:
    /**
     * @brief 文件系统数据存储
     * 
     * 使用Dictionary结构存储整个文件系统的层次化数据。
     */
    Dictionary m_data;

    /**
     * @brief 文件缓存
     * 
     * 缓存已创建的File对象，避免重复创建。
     */
    std::unordered_map<std::string, std::unique_ptr<File>> m_fileCache;

    /**
     * @brief 解析路径
     * 
     * 根据目录路径在字典结构中查找对应的数据节点。
     * 
     * @param dirpath 目录路径
     * @return std::any* 指向数据节点的指针，如果不存在则返回nullptr
     */
    std::any* resolve(const std::string& dirpath);

    /**
     * @brief 创建目录
     * 
     * 在文件系统中创建目录结构，支持递归创建。
     * 
     * @param dirpath 目录路径
     */
    void mkdir(const std::string& dirpath);

    /**
     * @brief 检查路径是否存在
     * 
     * 检查指定的目录路径是否在文件系统中存在。
     * 
     * @param dirpath 目录路径
     * @return bool 如果存在返回true，否则返回false
     */
    bool exists(const std::string& dirpath);

    /**
     * @brief 打印字典结构（递归）
     * 
     * 递归打印字典的层次结构，用于调试输出。
     * 
     * @param dict 要打印的字典
     * @param indent 缩进级别
     */
    void printDictionary(const Dictionary& dict, int indent = 0);
};

} // namespace RES