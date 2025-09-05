/**
 * @file MeshNode.hpp
 * @brief MeshNode类 - 网格渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/MeshNode.ts
 * MeshNode继承自RenderNode，专门用于网格渲染，支持顶点、UV、索引等3D网格功能。
 */

#pragma once

#include "player/RenderNode.hpp"
#include "geom/Matrix.hpp"
#include "geom/Rectangle.hpp"
#include "display/BitmapData.hpp"
#include "filters/ColorMatrixFilter.hpp"
#include <memory>
#include <vector>
#include <limits>

namespace egret {
namespace sys {

/**
 * @brief MeshNode类 - 网格渲染节点
 * 
 * MeshNode继承自RenderNode，提供完整的网格渲染功能。
 * 完全基于TypeScript egret.sys.MeshNode实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.sys.MeshNode类
 */
class MeshNode : public RenderNode {
public:
    /**
     * @brief 构造函数
     */
    MeshNode();
    
    /**
     * @brief 析构函数
     */
    virtual ~MeshNode() = default;
    
    /**
     * @brief 渲染方法实现
     * @param renderer 渲染器实例
     */
    void render(void* renderer) override;
    
    /**
     * @brief 清理渲染前数据 - 重写父类方法
     * 
     * 对应 public cleanBeforeRender():void
     * 在显示对象的$updateRenderNode()方法被调用前，自动清空自身的数据。
     */
    void cleanBeforeRender() override;

    // ========== 核心属性（对应TypeScript公共属性）==========
    
    /**
     * @brief 要绘制的位图 - 对应 public image:BitmapData = null;
     */
    std::shared_ptr<BitmapData> image = nullptr;
    
    /**
     * @brief 控制在缩放时是否对位图进行平滑处理 - 对应 public smoothing:boolean = true;
     */
    bool smoothing = true;
    
    /**
     * @brief 图片宽度。WebGL渲染使用 - 对应 public imageWidth:number;
     */
    double imageWidth = 0.0;
    
    /**
     * @brief 图片高度。WebGL渲染使用 - 对应 public imageHeight:number;
     */
    double imageHeight = 0.0;
    
    /**
     * @brief 相对偏移矩阵 - 对应 public matrix:egret.Matrix;
     */
    std::shared_ptr<Matrix> matrix = nullptr;
    
    /**
     * @brief UV 坐标 - 对应 public uvs:number[];
     */
    std::vector<double> uvs;
    
    /**
     * @brief 顶点坐标 - 对应 public vertices:number[];
     */
    std::vector<double> vertices;
    
    /**
     * @brief 顶点索引 - 对应 public indices:number[];
     */
    std::vector<int> indices;
    
    /**
     * @brief 边界框 - 对应 public bounds:Rectangle = new Rectangle();
     */
    std::shared_ptr<Rectangle> bounds;
    
    /**
     * @brief 使用的混合模式 - 对应 public blendMode: number = null;
     */
    int blendMode = -1; // -1表示null
    
    /**
     * @brief 相对透明度 - 对应 public alpha: number = NaN;
     */
    double alpha = std::numeric_limits<double>::quiet_NaN();
    
    /**
     * @brief 颜色变换滤镜 - 对应 public filter: ColorMatrixFilter = null;
     */
    std::shared_ptr<ColorMatrixFilter> filter = nullptr;
    
    /**
     * @brief 翻转 - 对应 public rotated: boolean = false;
     */
    bool rotated = false;

    // ========== 公共方法 ==========
    
    /**
     * @brief 绘制一次网格 - 对应 public drawMesh(sourceX, sourceY, sourceW, sourceH, drawX, drawY, drawW, drawH): void
     * 
     * @param sourceX 源图像x坐标
     * @param sourceY 源图像y坐标
     * @param sourceW 源图像宽度
     * @param sourceH 源图像高度
     * @param drawX 绘制目标x坐标
     * @param drawY 绘制目标y坐标
     * @param drawW 绘制目标宽度
     * @param drawH 绘制目标高度
     */
    void drawMesh(double sourceX, double sourceY, double sourceW, double sourceH,
                  double drawX, double drawY, double drawW, double drawH);

private:
    /**
     * @brief 网格绘制命令结构
     * 
     * 存储单次drawMesh调用的所有参数
     */
    struct DrawMeshCommand {
        double sourceX, sourceY, sourceW, sourceH;  ///< 源图像区域
        double drawX, drawY, drawW, drawH;          ///< 目标绘制区域
    };
    
    /**
     * @brief 绘制命令列表
     * 
     * 存储所有待绘制的网格命令，对应TypeScript中的drawData数组
     */
    std::vector<DrawMeshCommand> m_drawCommands;
};

} // namespace sys
} // namespace egret