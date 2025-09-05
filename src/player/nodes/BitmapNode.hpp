/**
 * @file BitmapNode.hpp
 * @brief BitmapNode类 - 位图渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/BitmapNode.ts
 * BitmapNode继承自RenderNode，专门用于位图渲染，支持九宫格、填充模式等高级功能。
 */

#pragma once

#include "player/RenderNode.hpp"
#include "geom/Matrix.hpp"
#include "geom/Rectangle.hpp"
#include "display/BitmapData.hpp"
#include "display/BitmapFillMode.hpp"
#include "filters/ColorMatrixFilter.hpp"
#include <memory>
#include <limits>

namespace egret {
namespace sys {

// 前向声明
class NormalBitmapNode;
class ColorMatrixFilter;

/**
 * @brief BitmapNode类 - 位图渲染节点
 * 
 * BitmapNode继承自RenderNode，提供完整的位图渲染功能。
 * 完全基于TypeScript egret.sys.BitmapNode实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.sys.BitmapNode类
 */
class BitmapNode : public RenderNode {
public:
    /**
     * @brief 构造函数
     */
    BitmapNode();
    
    /**
     * @brief 析构函数
     */
    virtual ~BitmapNode() = default;
    
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
     * @brief 要绘制的位图 - 对应 public image: BitmapData = null;
     */
    std::shared_ptr<BitmapData> image = nullptr;
    
    /**
     * @brief 控制在缩放时是否对位图进行平滑处理 - 对应 public smoothing: boolean = true;
     */
    bool smoothing = true;
    
    /**
     * @brief 相对偏移矩阵 - 对应 public matrix: egret.Matrix;
     */
    std::shared_ptr<Matrix> matrix = nullptr;
    
    /**
     * @brief 图片宽度。WebGL渲染使用 - 对应 public imageWidth: number;
     */
    double imageWidth = 0.0;
    
    /**
     * @brief 图片高度。WebGL渲染使用 - 对应 public imageHeight: number;
     */
    double imageHeight = 0.0;
    
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
     * @brief 获取图像数据 - 对应 getImage() 方法
     * @return 图像数据指针
     */
    void* getImage() const {
        return image ? image.get() : nullptr;
    }
    
    /**
     * @brief 绘制一次位图 - 对应 public drawImage(sourceX, sourceY, sourceW, sourceH, drawX, drawY, drawW, drawH): void
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
    void drawImage(double sourceX, double sourceY, double sourceW, double sourceH,
                   double drawX, double drawY, double drawW, double drawH);

    // ========== 静态工具方法 ==========
    
    /**
     * @brief 更新纹理数据 - 对应静态方法 $updateTextureData
     * 
     * 支持不同的填充模式：SCALE、CLIP、REPEAT
     */
    static void updateTextureData(std::shared_ptr<NormalBitmapNode> node, std::shared_ptr<BitmapData> image,
                                 double bitmapX, double bitmapY, double bitmapWidth, double bitmapHeight,
                                 double offsetX, double offsetY, double textureWidth, double textureHeight,
                                 double destW, double destH, double sourceWidth, double sourceHeight,
                                 const std::string& fillMode, bool smoothing);
    
    /**
     * @brief 更新九宫格纹理数据 - 对应静态方法 $updateTextureDataWithScale9Grid
     * 
     * 实现复杂的九宫格绘制算法
     */
    static void updateTextureDataWithScale9Grid(std::shared_ptr<NormalBitmapNode> node, std::shared_ptr<BitmapData> image,
                                               std::shared_ptr<Rectangle> scale9Grid,
                                               double bitmapX, double bitmapY, double bitmapWidth, double bitmapHeight,
                                               double offsetX, double offsetY, double textureWidth, double textureHeight,
                                               double destW, double destH, double sourceWidth, double sourceHeight,
                                               bool smoothing);

private:
    /**
     * @brief 绘制裁剪图像的内部方法 - 对应私有静态方法 drawClipImage
     */
    static void drawClipImage(std::shared_ptr<NormalBitmapNode> node, double scale,
                             double bitmapX, double bitmapY, double scaledBitmapW, double scaledBitmapH,
                             double offsetX, double offsetY, double destW, double destH,
                             double startX = 0.0, double startY = 0.0);
    
    /**
     * @brief 绘制命令结构
     * 
     * 存储单次drawImage调用的所有参数
     */
    struct DrawImageCommand {
        double sourceX, sourceY, sourceW, sourceH;  ///< 源图像区域
        double drawX, drawY, drawW, drawH;          ///< 目标绘制区域
    };
    
    /**
     * @brief 绘制命令列表
     * 
     * 存储所有待绘制的图像命令，对应TypeScript中的drawData数组
     */
    std::vector<DrawImageCommand> m_drawCommands;
};

} // namespace sys
} // namespace egret