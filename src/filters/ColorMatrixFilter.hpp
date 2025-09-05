/**
 * @file ColorMatrixFilter.hpp
 * @brief ColorMatrixFilter类 - 颜色矩阵滤镜
 * 
 * 翻译自：egret-core-5.4.1/src/egret/filters/ColorMatrixFilter.ts
 * ColorMatrixFilter允许将4x5矩阵变换应用于输入图像上每个像素的RGBA颜色和Alpha值。
 */

#pragma once

#include "filters/Filter.hpp"
#include <vector>
#include <array>

namespace egret {

/**
 * @brief ColorMatrixFilter类 - 颜色矩阵滤镜
 * 
 * The ColorMatrixFilter class lets you apply a 4 x 5 matrix transformation on the RGBA color and alpha values of every pixel in the input image to produce a result with a new set of RGBA color and alpha values.
 * It allows saturation changes, hue rotation, luminance to alpha, and various other effects.
 * 
 * 使用 ColorMatrixFilter 类可以将 4 x 5 矩阵转换应用于输入图像上的每个像素的 RGBA 颜色和 Alpha 值，以生成具有一组新的 RGBA 颜色和 Alpha 值的结果。
 * 该类允许饱和度更改、色相旋转、亮度为 Alpha 以及各种其他效果。
 * 
 * 完全基于TypeScript egret.ColorMatrixFilter实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.ColorMatrixFilter类
 * @version Egret 3.1.0
 * @platform Web
 * @see http://edn.egret.com/cn/docs/page/947 颜色矩阵滤镜
 */
class ColorMatrixFilter : public Filter {
public:
    /**
     * @brief 构造函数
     * 
     * Initializes a ColorMatrixFilter object.
     * 创建一个 ColorMatrixFilter 对象。
     * 
     * @param matrix 4x5颜色矩阵（20个数字的数组）
     * @version Egret 3.1.0
     * @platform Web
     */
    ColorMatrixFilter(const std::vector<double>& matrix = {});
    
    /**
     * @brief 析构函数
     */
    virtual ~ColorMatrixFilter() = default;

    // ========== 公共属性（对应TypeScript公共属性）==========
    
    /**
     * @brief 颜色矩阵属性 - 对应 public get matrix():number[] / public set matrix(value:number[])
     * 
     * A comma delimited list of 20 doubles that comprise a 4x5 matrix applied to the rendered element.
     * The matrix is in row major order -- that is, the first five elements are multipled by the vector [srcR,srcG,srcB,srcA,1] to determine the output red value, the second five determine the output green value, etc.
     * The value must either be an array or comma delimited string of 20 numbers.
     * 
     * 构成应用于所呈示的元素的一个 4x5 矩阵的、以逗号分隔的 20 个双精度数的列表。
     * 矩阵以行作为主要顺序，即用第一行五个元素乘以矢量 [srcR,srcG,srcB,srcA,1] 以确定输出的红色值，用第二行的五个元素确定输出的绿色值，等等。
     * 该值必须为 20 个数字组成的数组或以逗号分隔的字符串。
     * 
     * @version Egret 3.1.0
     * @platform Web
     */
    std::vector<double> getMatrix() const;
    void setMatrix(const std::vector<double>& value);

    // ========== 公共方法（重写基类方法）==========
    
    /**
     * @brief 转换为JSON字符串 - 重写基类方法
     * 
     * 对应 public $toJson():string
     * @private
     */
    std::string toJson() const override;

private:
    // ========== 私有属性（对应TypeScript私有属性）==========
    
    /**
     * @brief 内部矩阵存储 - 对应 public $matrix:number[] = [];
     * @private
     */
    std::array<double, 20> internalMatrix;
    
    /**
     * @brief 临时矩阵存储 - 对应 private matrix2:number[] = [];
     * @private
     */
    mutable std::vector<double> tempMatrix;

    // ========== 私有方法 ==========
    
    /**
     * @brief 设置矩阵的内部实现 - 对应 private setMatrix(value:number[]):void
     * @private
     */
    void setMatrixInternal(const std::vector<double>& value);
    
    /**
     * @brief 初始化默认矩阵
     * @private
     */
    void initializeDefaultMatrix();
    
    /**
     * @brief 更新WebGL uniform参数
     * @private
     */
    void updateUniforms();
};

} // namespace egret