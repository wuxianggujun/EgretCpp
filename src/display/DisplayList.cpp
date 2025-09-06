#include "display/DisplayList.hpp"
#include "player/SystemRenderer.hpp" 
#include "player/RenderBuffer.hpp"
#include "display/Stage.hpp"
#include "geom/Matrix.hpp"
#include "utils/Logger.hpp"

namespace egret {
namespace sys {

    DisplayList::DisplayList() 
        : HashObject()
        , m_dirty(false) {
    }
    
    int DisplayList::drawToSurface() {
        EGRET_DEBUG("DisplayList::drawToSurface() - Starting");
        
        if (!m_renderBuffer) {
            EGRET_WARN("DisplayList::drawToSurface() - renderBuffer is null");
            return 0;
        }
        
        if (!systemRenderer) {
            EGRET_ERROR("DisplayList::drawToSurface() - systemRenderer is null! Call initializeRenderers() first.");
            return 0;
        }
        
        if (!m_root) {
            EGRET_WARN("DisplayList::drawToSurface() - root is null");
            return 0;
        }
        
        EGRET_DEBUGF("DisplayList::drawToSurface() - Root object: x={}, y={}, visible={}", 
                    m_root->getX(), m_root->getY(), m_root->getVisible());
        
        // 如果root是DisplayObjectContainer，检查其子对象
        auto container = dynamic_cast<DisplayObjectContainer*>(m_root);
        if (container) {
            int childCount = container->getNumChildren();
            EGRET_DEBUGF("DisplayList::drawToSurface() - Root container has {} children", childCount);
        }
        
        // 清空渲染缓冲区
        EGRET_DEBUG("DisplayList::drawToSurface() - Clearing render buffer");
        m_renderBuffer->clear();
        
        // 设置画布缩放（对应TS的canvasScale）
        Matrix offsetMatrix;
        offsetMatrix.setTo(1.0, 0.0, 0.0, 1.0, 0.0, 0.0); // 默认单位矩阵
        
        EGRET_DEBUG("DisplayList::drawToSurface() - Calling SystemRenderer::render()");
        // 核心修复：调用SystemRenderer.render()递归渲染根对象
        int drawCalls = systemRenderer->render(m_root, m_renderBuffer.get(), offsetMatrix);
        
        EGRET_DEBUGF("DisplayList::drawToSurface() - SystemRenderer returned {} draw calls", drawCalls);
        
        m_dirty = false;
        
        EGRET_DEBUG("DisplayList::drawToSurface() - Finished");
        return drawCalls;
    }
    
    void DisplayList::clear() {
        m_renderNode.reset();
        m_renderBuffer.reset();
        m_dirty = false;
    }

} // namespace sys
} // namespace egret