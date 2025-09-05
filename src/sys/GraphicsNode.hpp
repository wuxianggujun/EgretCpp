#pragma once
#include "player/RenderNode.hpp"
#include <vector>
#include <memory>
#include <string>

namespace egret {
    class Matrix;
    namespace sys {
        class Path2D;
        class FillPath;
        class StrokePath;
    }
}

namespace egret {
namespace sys {

    /**
     * 矢量图形渲染节点
     * 基于TypeScript egret.sys.GraphicsNode完整实现
     */
    class GraphicsNode : public RenderNode {
    public:
        /**
         * 构造函数
         */
        GraphicsNode();
        
        /**
         * 析构函数
         */
        virtual ~GraphicsNode();

        // ========== 填充相关方法（对应TypeScript接口） ==========
        
        /**
         * 开始填充 - 对应TypeScript beginFill方法
         */
        std::shared_ptr<Path2D> beginFill(uint32_t color, double alpha = 1.0, 
                                         std::shared_ptr<Path2D> beforePath = nullptr);

        /**
         * 开始渐变填充 - 对应TypeScript beginGradientFill方法
         */
        std::shared_ptr<Path2D> beginGradientFill(const std::string& type,
                                                 const std::vector<uint32_t>& colors,
                                                 const std::vector<double>& alphas,
                                                 const std::vector<double>& ratios,
                                                 const Matrix* matrix = nullptr,
                                                 std::shared_ptr<Path2D> beforePath = nullptr);

        /**
         * 线条样式 - 对应TypeScript lineStyle方法
         */
        std::shared_ptr<StrokePath> lineStyle(double thickness = 0, uint32_t color = 0, 
                                            double alpha = 1.0, const std::string& caps = "round",
                                            const std::string& joints = "round", double miterLimit = 3.0,
                                            const std::vector<double>& lineDash = {});

        /**
         * 清空绘制数据 - 对应TypeScript clear方法
         */
        void clear();

        /**
         * 重写父类方法，不自动清空缓存
         */
        void cleanBeforeRender() override;

        /**
         * 清除非绘制缓存数据
         */
        void clean();

        /**
         * 实现基类的渲染方法
         */
        void render(void* renderer) override;

        /**
         * 获取绘制数据（重写基类方法）
         */
        const std::vector<std::shared_ptr<Path2D>>& getDrawData() const { return m_drawData; }

        // ========== 对应TypeScript的公共属性 ==========
        double x = 0.0;           // 绘制x偏移
        double y = 0.0;           // 绘制y偏移  
        double width = 0.0;       // 绘制宽度
        double height = 0.0;      // 绘制高度
        bool dirtyRender = true;  // 脏渲染标记
        
    private:
        // 绘制数据存储（对应TypeScript的drawData）
        std::vector<std::shared_ptr<Path2D>> m_drawData;
    };

} // namespace sys
} // namespace egret