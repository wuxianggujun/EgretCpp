/**
 * @file FileSystem.cpp
 * @brief FileSystem文件系统实现 - assetsManager底层存储资源信息的文件系统抽象层实现
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/core/FileSystem.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "extension/assetsmanager/FileSystem.hpp"
#include "extension/assetsmanager/Path.hpp"
#include <vector>
#include <sstream>
#include "utils/Logger.hpp"

namespace RES {

    // ========== NewFileSystem实现 ==========

    NewFileSystem::NewFileSystem(const Dictionary& data) : m_data(data) {
        // 构造函数
    }

    NewFileSystem::~NewFileSystem() {
        // 析构函数
    }

    void NewFileSystem::profile() {
        EGRET_INFO("FileSystem 内容:");
        printDictionary(m_data);
    }

    void NewFileSystem::addFile(const std::string& filename, const std::string& type) {
        std::string normalizedType = type;
        if (normalizedType.empty()) {
            normalizedType = "";
        }

        // 规范化文件路径
        std::string normalizedFilename = path::normalize(filename);
        std::string basefilename = path::basename(normalizedFilename);
        std::string folder = path::dirname(normalizedFilename);

        // 如果目录不存在，创建目录
        if (!exists(folder)) {
            mkdir(folder);
        }

        // 解析目录
        std::any* dirNode = resolve(folder);
        if (dirNode && dirNode->type() == typeid(Dictionary)) {
            Dictionary& dir = std::any_cast<Dictionary&>(*dirNode);
            
            // 创建File对象并存储
            File fileData(normalizedFilename, normalizedType);
            dir[basefilename] = fileData;
        }
    }

    void NewFileSystem::addFile(const File& data) {
        addFile(data.url, data.type);
    }

    File* NewFileSystem::getFile(const std::string& filename) {
        std::any* result = resolve(filename);
        
        if (result && result->type() == typeid(File)) {
            // 获取缓存中的File对象或创建新的
            auto it = m_fileCache.find(filename);
            if (it != m_fileCache.end()) {
                File* file = it->second.get();
                file->name = filename;
                return file;
            } else {
                // 创建新的File对象并缓存
                File sourceFile = std::any_cast<File>(*result);
                auto newFile = std::make_unique<File>(sourceFile);
                newFile->name = filename;
                
                File* filePtr = newFile.get();
                m_fileCache[filename] = std::move(newFile);
                return filePtr;
            }
        }
        
        return nullptr;
    }

    void NewFileSystem::removeFile(const std::string& filename) {
        // 从缓存中移除
        auto it = m_fileCache.find(filename);
        if (it != m_fileCache.end()) {
            m_fileCache.erase(it);
        }

        // 从文件系统结构中移除
        std::string normalizedFilename = path::normalize(filename);
        std::string basefilename = path::basename(normalizedFilename);
        std::string folder = path::dirname(normalizedFilename);

        std::any* dirNode = resolve(folder);
        if (dirNode && dirNode->type() == typeid(Dictionary)) {
            Dictionary& dir = std::any_cast<Dictionary&>(*dirNode);
            auto fileIt = dir.find(basefilename);
            if (fileIt != dir.end()) {
                dir.erase(fileIt);
            }
        }
    }

    // ========== 私有方法实现 ==========

    std::any* NewFileSystem::resolve(const std::string& dirpath) {
        if (dirpath.empty()) {
            return &m_data;
        }

        std::string normalizedPath = path::normalize(dirpath);
        std::vector<std::string> pathSegments;
        
        // 分割路径
        std::stringstream ss(normalizedPath);
        std::string segment;
        while (std::getline(ss, segment, '/')) {
            if (!segment.empty()) {
                pathSegments.push_back(segment);
            }
        }

        // 逐级查找
        std::any* current = &m_data;
        
        for (const std::string& segment : pathSegments) {
            if (!current) {
                return nullptr;
            }

            if (current->type() == typeid(Dictionary)) {
                Dictionary& dict = std::any_cast<Dictionary&>(*current);
                auto it = dict.find(segment);
                if (it != dict.end()) {
                    current = &(it->second);
                } else {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }

        return current;
    }

    void NewFileSystem::mkdir(const std::string& dirpath) {
        if (dirpath.empty()) {
            return;
        }

        std::string normalizedPath = path::normalize(dirpath);
        std::vector<std::string> pathSegments;
        
        // 分割路径
        std::stringstream ss(normalizedPath);
        std::string segment;
        while (std::getline(ss, segment, '/')) {
            if (!segment.empty()) {
                pathSegments.push_back(segment);
            }
        }

        // 逐级创建目录
        Dictionary* current = nullptr;
        
        try {
            current = &std::any_cast<Dictionary&>(m_data);
        } catch (const std::bad_any_cast&) {
            // 如果m_data不是Dictionary类型，初始化为Dictionary
            m_data = Dictionary{};
            current = &std::any_cast<Dictionary&>(m_data);
        }

        for (const std::string& segment : pathSegments) {
            auto it = current->find(segment);
            if (it == current->end()) {
                // 目录不存在，创建新目录
                (*current)[segment] = Dictionary{};
            }
            
            // 移动到下一级目录
            try {
                current = &std::any_cast<Dictionary&>((*current)[segment]);
            } catch (const std::bad_any_cast&) {
                // 如果不是Dictionary类型，替换为Dictionary
                (*current)[segment] = Dictionary{};
                current = &std::any_cast<Dictionary&>((*current)[segment]);
            }
        }
    }

    bool NewFileSystem::exists(const std::string& dirpath) {
        if (dirpath.empty()) {
            return true;
        }

        return resolve(dirpath) != nullptr;
    }

    void NewFileSystem::printDictionary(const Dictionary& dict, int indent) {
        std::string indentStr(indent * 2, ' ');
        
        for (const auto& pair : dict) {
            // 使用标准输出风格统一日志
            EGRET_INFOF("{}{}", indentStr, pair.first);
            
            if (pair.second.type() == typeid(Dictionary)) {
                // 目录行
                EGRET_INFO("/");
                const Dictionary& subDict = std::any_cast<const Dictionary&>(pair.second);
                printDictionary(subDict, indent + 1);
            } else if (pair.second.type() == typeid(File)) {
                const File& file = std::any_cast<const File&>(pair.second);
                EGRET_INFOF(" -> {} (type: {})", file.url, file.type);
            } else {
                EGRET_WARN(" (unknown type)");
            }
        }
    }

} // namespace RES
