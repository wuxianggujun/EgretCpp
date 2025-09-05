/**
 * @file ColorMatrixFilter.cpp
 * @brief ColorMatrixFilter类实现 - 颜色矩阵滤镜
 * 
 * 翻译自：egret-core-5.4.1/src/egret/filters/ColorMatrixFilter.ts
 * ColorMatrixFilter允许将4x5矩阵变换应用于输入图像上每个像素的RGBA颜色和Alpha值。
 */

#include "filters/ColorMatrixFilter.hpp"
#include <sstream>
#include <iomanip>

namespace egret {

// ========== ColorMatrixFilter颜色矩阵滤镜实现 ==========

ColorMatrixFilter::ColorMatrixFilter(const std::vector<double>& matrix) 
    : Filter()
    , tempMatrix(20) {
    // 完全对应TypeScript实现：
    // constructor(matrix:number[] = null) {
    //     super();
    //     this.type = "colorTransform";
    //     this.$uniforms.matrix = [
    //         1, 0, 0, 0,
    //         0, 1, 0, 0,
    //         0, 0, 1, 0,
    //         0, 0, 0, 1
    //     ];
    //     this.$uniforms.colorAdd = {x: 0, y: 0, z: 0, w: 0};
    //     this.setMatrix(matrix);
    //     this.onPropertyChange(); 
    // }
    
    // 设置滤镜类型
    type = "colorTransform";
    
    // 初始化WebGL uniforms
    initializeDefaultMatrix();
    
    // 设置矩阵
    setMatrixInternal(matrix);
    
    // 触发属性变化事件
    onPropertyChange();
}

std::vector<double> ColorMatrixFilter::getMatrix() const {
    // 完全对应TypeScript实现：
    // public get matrix():number[] {
    //     for (let i = 0; i < 20; i++) {
    //         this.matrix2[i] = this.$matrix[i];
    //     }
    //     return this.matrix2;
    // }
    
    // 将内部矩阵复制到临时矩阵
    for (int i = 0; i < 20; i++) {
        tempMatrix[i] = internalMatrix[i];
    }
    return tempMatrix;
}

void ColorMatrixFilter::setMatrix(const std::vector<double>& value) {
    // 完全对应TypeScript实现：
    // public set matrix(value:number[]) {
    //     this.setMatrix(value);
    // }
    
    setMatrixInternal(value);
}

std::string ColorMatrixFilter::toJson() const {
    // 完全对应TypeScript实现：
    // public $toJson():string {
    //     return '{"matrix": [' + this.$matrix.toString() + ']}';
    // }
    
    std::ostringstream oss;
    oss << "{\"matrix\": [";
    
    for (int i = 0; i < 20; i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss << std::fixed << std::setprecision(6) << internalMatrix[i];
    }
    
    oss << "]}";
    return oss.str();
}

void ColorMatrixFilter::setMatrixInternal(const std::vector<double>& value) {
    // 完全对应TypeScript实现：
    // private setMatrix(value:number[]):void {
    //     if(value) {
    //         for (let i = 0; i < 20; i++) {
    //             this.$matrix[i] = value[i];
    //         } 
    //     } else {
    //         for (let i = 0; i < 20; i++) {
    //             this.$matrix[i] = (i == 0 || i == 6 || i == 12 || i == 18) ? 1 : 0;
    //         } 
    //     }
    //
    //     let $matrix = this.$matrix;
    //     let matrix = this.$uniforms.matrix;
    //     let colorAdd = this.$uniforms.colorAdd;
    //
    //     for(let i = 0, j = 0; i < $matrix.length; i++) {
    //         if(i === 4) {
    //             colorAdd.r = $matrix[i] / 255;
    //         } else if(i === 9) {
    //             colorAdd.g = $matrix[i] / 255;
    //         } else if(i === 14) {
    //             colorAdd.b = $matrix[i] / 255;
    //         } else if(i === 19) {
    //             colorAdd.a = $matrix[i] / 255;
    //         } else {
    //             matrix[j] = $matrix[i];
    //             j++;
    //         }
    //     }
    //     this.onPropertyChange();
    // }
    
    // 设置矩阵值
    if (!value.empty()) {
        // 如果提供了矩阵值，复制到内部矩阵
        for (int i = 0; i < 20 && i < static_cast<int>(value.size()); i++) {
            internalMatrix[i] = value[i];
        }
        // 如果提供的值不足20个，其余位置填0
        for (int i = static_cast<int>(value.size()); i < 20; i++) {
            internalMatrix[i] = 0.0;
        }
    } else {
        // 如果没有提供矩阵值，设置为单位矩阵
        for (int i = 0; i < 20; i++) {
            internalMatrix[i] = (i == 0 || i == 6 || i == 12 || i == 18) ? 1.0 : 0.0;
        }
    }
    
    // 更新WebGL uniforms
    updateUniforms();
    
    // 触发属性变化事件
    onPropertyChange();
}

void ColorMatrixFilter::initializeDefaultMatrix() {
    // 初始化WebGL uniforms
    // 对应TypeScript中的：
    // this.$uniforms.matrix = [
    //     1, 0, 0, 0,
    //     0, 1, 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1
    // ];
    // this.$uniforms.colorAdd = {x: 0, y: 0, z: 0, w: 0};
    
    uniforms.arrays["matrix"] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    uniforms.colors["colorAdd"] = Color4(0.0, 0.0, 0.0, 0.0);
}

void ColorMatrixFilter::updateUniforms() {
    // 更新WebGL uniform参数
    // 对应TypeScript中的矩阵和颜色加法参数更新逻辑
    
    auto& matrix = uniforms.arrays["matrix"];
    auto& colorAdd = uniforms.colors["colorAdd"];
    
    // 重置矩阵为16个元素的数组（4x4矩阵）
    matrix.assign(16, 0.0);
    
    // 从20元素的4x5矩阵中提取4x4矩阵部分和颜色加法部分
    for (int i = 0, j = 0; i < 20; i++) {
        if (i == 4) {
            // 第1行的第5个元素 -> colorAdd.r (红色偏移)
            colorAdd.r = internalMatrix[i] / 255.0;
        } else if (i == 9) {
            // 第2行的第5个元素 -> colorAdd.g (绿色偏移)
            colorAdd.g = internalMatrix[i] / 255.0;
        } else if (i == 14) {
            // 第3行的第5个元素 -> colorAdd.b (蓝色偏移)
            colorAdd.b = internalMatrix[i] / 255.0;
        } else if (i == 19) {
            // 第4行的第5个元素 -> colorAdd.a (Alpha偏移)
            colorAdd.a = internalMatrix[i] / 255.0;
        } else {
            // 4x4矩阵部分
            matrix[j] = internalMatrix[i];
            j++;
        }
    }
}

} // namespace egret