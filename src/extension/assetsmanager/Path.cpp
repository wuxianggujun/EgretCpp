/**
 * @file Path.cpp
 * @brief Path路径工具实现 - 文件路径操作工具函数实现
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/core/Path.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "extension/assetsmanager/Path.hpp"
#include <vector>
#include <sstream>
#include <algorithm>

namespace RES {

namespace path {

    std::string normalize(const std::string& filename) {
        if (filename.empty()) {
            return filename;
        }

        // 将路径分割为数组
        std::vector<std::string> arr;
        std::stringstream ss(filename);
        std::string segment;
        
        while (std::getline(ss, segment, '/')) {
            arr.push_back(segment);
        }

        // 过滤掉空的片段，但保留最后一个空片段（如果存在）
        std::vector<std::string> filtered;
        for (size_t i = 0; i < arr.size(); ++i) {
            if (!arr[i].empty() || i == arr.size() - 1) {
                filtered.push_back(arr[i]);
            }
        }

        // 重新连接路径
        std::string result;
        for (size_t i = 0; i < filtered.size(); ++i) {
            if (i > 0) {
                result += "/";
            }
            result += filtered[i];
        }

        return result;
    }

    std::string basename(const std::string& filename) {
        if (filename.empty()) {
            return filename;
        }

        size_t pos = filename.find_last_of('/');
        if (pos == std::string::npos) {
            return filename; // 没有找到分隔符，返回整个字符串
        }

        return filename.substr(pos + 1);
    }

    std::string dirname(const std::string& filename) {
        if (filename.empty()) {
            return "";
        }

        size_t pos = filename.find_last_of('/');
        if (pos == std::string::npos) {
            return ""; // 没有找到分隔符，返回空字符串
        }

        return filename.substr(0, pos);
    }

    std::string join(const std::string& path1, const std::string& path2) {
        if (path1.empty()) {
            return path2;
        }
        if (path2.empty()) {
            return path1;
        }

        // 确保路径之间有正确的分隔符
        if (path1.back() == '/' || path2.front() == '/') {
            return normalize(path1 + path2);
        } else {
            return normalize(path1 + "/" + path2);
        }
    }

    std::string extname(const std::string& filename) {
        if (filename.empty()) {
            return "";
        }

        // 首先获取文件名（不包括路径）
        std::string name = basename(filename);
        
        size_t pos = name.find_last_of('.');
        if (pos == std::string::npos || pos == 0) {
            return ""; // 没有扩展名或者是隐藏文件
        }

        return name.substr(pos);
    }

    bool isAbsolute(const std::string& path) {
        if (path.empty()) {
            return false;
        }

        // 检查是否以斜杠开头（Unix样式）
        if (path[0] == '/') {
            return true;
        }

        // 检查是否是Windows样式的绝对路径（C:/ 等）
        if (path.length() >= 3 && path[1] == ':' && path[2] == '/') {
            return true;
        }

        return false;
    }

} // namespace path

} // namespace RES