/**
 * @file Filter.hpp
 * @brief Filter类 - 滤镜基类
 * 
 * 翻译自：egret-core-5.4.1/src/egret/filters/Filter.ts
 * Filter是所有滤镜效果的基类，提供滤镜的基础功能和接口。
 */

#pragma once

#include "core/HashObject.hpp"
#include "geom/Color4.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace egret {

/**
 * @brief Filter类 - 滤镜基类
 * 
 * Filter继承自HashObject，是所有滤镜效果的基类。
 * 完全基于TypeScript egret.Filter实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.Filter类
 * @version Egret 2.4
 * @platform Web,Native
 */
class Filter : public HashObject {
public:
    /**
     * @brief 构造函数
     * 
     * 对应 constructor()
     * @version Egret 2.4
     * @platform Web,Native
     */
    Filter();
    
    /**
     * @brief 析构函数
     */
    virtual ~Filter() = default;

    // ========== 公共属性（对应TypeScript公共属性）==========
    
    /**
     * @brief 滤镜类型 - 对应 public type:string = null;
     * @version Egret 2.4
     * @platform Web,Native
     */
    std::string type;
    
    /**
     * @brief 滤镜ID - 对应 public $id: number = null;
     * @private
     */
    int filterId = -1; // -1表示null

    // ========== 公共方法 ==========
    
    /**
     * @brief 属性变化时的回调 - 对应 public onPropertyChange(): void
     */
    virtual void onPropertyChange();
    
    /**
     * @brief 转换为JSON字符串 - 对应 public $toJson():string
     * @private
     */
    virtual std::string toJson() const;

protected:
    // ========== 受保护的属性（对应TypeScript protected属性）==========
    
    /**
     * @brief 上边距 - 对应 protected paddingTop: number = 0;
     * @private
     */
    double paddingTop = 0.0;
    
    /**
     * @brief 下边距 - 对应 protected paddingBottom: number = 0;
     * @private
     */
    double paddingBottom = 0.0;
    
    /**
     * @brief 左边距 - 对应 protected paddingLeft: number = 0;
     * @private
     */
    double paddingLeft = 0.0;
    
    /**
     * @brief 右边距 - 对应 protected paddingRight: number = 0;
     * @private
     */
    double paddingRight = 0.0;

    // ========== 内部数据结构 ==========
    
    /**
     * @brief Uniform参数存储
     * 
     * 对应TypeScript中的 public $uniforms:any;
     * 用于存储WebGL shader的uniform参数
     */
    struct UniformData {
        std::unordered_map<std::string, double> floats;
        std::unordered_map<std::string, std::vector<double>> arrays;
        std::unordered_map<std::string, Color4> colors;
        
        void clear() {
            floats.clear();
            arrays.clear();
            colors.clear();
        }
    };
    
    /**
     * @brief Uniform参数数据 - 对应 public $uniforms:any;
     * @private
     */
    UniformData uniforms;

    // ========== 受保护的方法 ==========
    
    /**
     * @brief 更新边距 - 对应 protected updatePadding(): void
     */
    virtual void updatePadding();

private:
    /**
     * @brief 原生渲染对象 - 对应 public $obj: any;
     * @private
     * @native Render
     */
    void* nativeObject = nullptr;
    
    /**
     * @brief 初始化滤镜
     */
    void initializeFilter();
};

} // namespace egret