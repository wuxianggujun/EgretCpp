/**
 * @file Path.hpp
 * @brief Path路径工具 - 文件路径操作工具函数集
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/core/Path.ts
 * Path命名空间提供了文件路径操作的实用工具函数，支持路径格式化、文件名提取、目录路径提取等功能。
 * 针对的是"C:/A/B/C/D/example.ts"这种格式的文件路径。
 */

#pragma once

#include <string>

namespace RES {

/**
 * @brief Path命名空间 - 文件路径操作工具
 * 
 * 对文件路径的一些操作，针对的是C:/A/B/C/D/example.ts这种格式的文件路径。
 * 提供路径格式化、文件名提取、目录提取等常用的路径处理功能。
 * 
 * @version Egret 5.2
 * @note 对应TypeScript的RES.path命名空间，100%API兼容
 */
namespace path {

    /**
     * @brief 格式化文件路径
     * 
     * 格式化文件路径，去除多余的斜杠，规范化路径格式。
     * 例如："C:/A/B//C//D//example.ts" => "C:/A/B/C/D/example.ts"
     * 
     * @param filename 传入的文件路径
     * @return std::string 格式化后的文件路径
     * @version Egret 5.2
     * @platform Web,Native
     */
    std::string normalize(const std::string& filename);

    /**
     * @brief 根据文件路径得到文件名
     * 
     * 从完整的文件路径中提取文件名（包括扩展名）。
     * 例如："C:/A/B/example.ts" => "example.ts"
     * 
     * @param filename 传入的文件路径
     * @return std::string 文件的名字
     * @version Egret 5.2
     * @platform Web,Native
     */
    std::string basename(const std::string& filename);

    /**
     * @brief 获取文件所在文件夹路径
     * 
     * 从完整的文件路径中提取目录路径（不包括文件名）。
     * 例如："C:/A/B/example.ts" => "C:/A/B"
     * 
     * @param filename 传入的文件路径
     * @return std::string 文件所在文件夹的地址
     * @version Egret 5.2
     * @platform Web,Native
     */
    std::string dirname(const std::string& filename);

    /**
     * @brief 连接路径
     * 
     * 将多个路径片段连接成一个完整路径，自动处理分隔符。
     * 
     * @param path1 第一个路径片段
     * @param path2 第二个路径片段
     * @return std::string 连接后的路径
     * @version Egret 5.2
     */
    std::string join(const std::string& path1, const std::string& path2);

    /**
     * @brief 获取文件扩展名
     * 
     * 从文件路径中提取文件扩展名（包括点号）。
     * 例如："example.ts" => ".ts"
     * 
     * @param filename 文件路径
     * @return std::string 文件扩展名，如果没有扩展名则返回空字符串
     * @version Egret 5.2
     */
    std::string extname(const std::string& filename);

    /**
     * @brief 检查路径是否为绝对路径
     * 
     * 判断给定路径是否为绝对路径。
     * 
     * @param path 要检查的路径
     * @return bool 如果是绝对路径返回true，否则返回false
     * @version Egret 5.2
     */
    bool isAbsolute(const std::string& path);

} // namespace path

} // namespace RES