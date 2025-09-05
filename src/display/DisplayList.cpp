#include "display/DisplayList.hpp"

namespace egret {
namespace sys {

    DisplayList::DisplayList() 
        : HashObject()
        , m_dirty(false) {
    }
    
    int DisplayList::drawToSurface() {
        if (!m_renderNode) {
            return 0;
        }
        
        // 清空绘制数据准备新的渲染
        m_renderNode->cleanBeforeRender();
        
        // 通知渲染节点需要渲染
        // 实际的渲染工作由SystemRenderer完成
        m_dirty = false;
        
        // 返回1表示有一次绘制调用
        return 1;
    }
    
    void DisplayList::clear() {
        m_renderNode.reset();
        m_renderBuffer.reset();
        m_dirty = false;
    }

} // namespace sys
} // namespace egret