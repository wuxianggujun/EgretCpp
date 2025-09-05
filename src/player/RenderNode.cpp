#include "player/RenderNode.hpp"

namespace egret {
namespace sys {
    // ========== RenderNode基类实现 ==========
    
    RenderNode::RenderNode(RenderNodeType type) 
        : HashObject()
        , m_type(type)
        , m_renderCount(0) {
    }
    
    void RenderNode::cleanBeforeRender() {
        m_drawData.clear();
    }
    
    // BitmapNode实现已移动到单独的文件 player/nodes/BitmapNode.cpp

    // TextNode实现已移动到单独的文件 player/nodes/TextNode.cpp

    // GroupNode实现已移动到单独的文件 player/nodes/GroupNode.cpp

    // MeshNode实现已移动到单独的文件 player/nodes/MeshNode.cpp

}
}
