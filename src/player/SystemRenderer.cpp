#include "player/SystemRenderer.hpp"
#include "player/SkiaRenderer.hpp"
#include "player/SkiaRenderBuffer.hpp"

namespace egret {
namespace sys {

    // ========== 全局渲染器实例定义 ==========
    
    std::unique_ptr<SystemRenderer> systemRenderer = nullptr;
    std::unique_ptr<SystemRenderer> canvasRenderer = nullptr;

    // ========== 工厂方法实现 ==========
    
    void initializeRenderers() {
        // 创建主渲染器
        systemRenderer = createSkiaRenderer();
        
        // 创建碰撞检测渲染器
        canvasRenderer = createSkiaRenderer();
    }
    
    std::shared_ptr<RenderBuffer> createRenderBuffer(double width, double height) {
        return createSkiaRenderBuffer(width, height);
    }
    
    void cleanupRenderers() {
        systemRenderer.reset();
        canvasRenderer.reset();
    }

} // namespace sys
} // namespace egret