/**
 * @file TextNode.cpp
 * @brief TextNode类实现 - 文本渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/TextNode.ts
 * TextNode继承自RenderNode，专门用于文本渲染。
 */

#include "player/nodes/TextNode.hpp"

namespace egret {
namespace sys {

// ========== TextNode文本节点实现 ==========

TextNode::TextNode() 
    : RenderNode(RenderNodeType::TextNode) {
    // 初始化已在头文件中完成（使用C++11默认成员初始化）
}

TextNode::~TextNode() {
    // 清理资源
    clean();
}

void TextNode::render(void* renderer) {
    if (!renderer) {
        return;
    }
    
    m_renderCount++;
    
    // TODO: 当集成Skia文本渲染时，在这里实现文本绘制
    // 这里将使用 m_drawCommands 中的绘制命令进行实际渲染
    
    // 示例：遍历所有绘制命令并渲染
    // for (const auto& command : m_drawCommands) {
    //     // 使用Skia渲染文本
    //     // SkCanvas* canvas = static_cast<SkCanvas*>(renderer);
    //     // SkFont font;
    //     // SkPaint paint;
    //     // 
    //     // 设置字体属性
    //     // font.setSize(command.format.size.value_or(size));
    //     // font.setTypefaceFromName(command.format.fontFamily.value_or(fontFamily).c_str(), 
    //     //                           command.format.bold.value_or(bold) ? SkFontStyle::kBold_Weight : SkFontStyle::kNormal_Weight);
    //     // 
    //     // 设置颜色
    //     // paint.setColor(command.format.textColor.value_or(textColor));
    //     // 
    //     // 绘制文本
    //     // canvas->drawSimpleText(command.text.c_str(), command.text.length(), 
    //     //                        SkTextEncoding::kUTF8, command.x, command.y, font, paint);
    // }
}

void TextNode::drawText(double x, double y, const std::string& text, const TextFormat& format) {
    // 完全对应TypeScript实现：
    // public drawText(x:number, y:number, text:string, format:TextFormat):void {
    //     this.drawData.push(x, y, text, format);
    //     this.renderCount++;
    //     this.dirtyRender = true;
    // }
    
    // 创建绘制命令并添加到列表
    DrawCommand command;
    command.x = x;
    command.y = y;
    command.text = text;
    command.format = format;
    
    m_drawCommands.push_back(command);
    
    // 增加渲染计数（对应TypeScript中的this.renderCount++）
    m_renderCount++;
    
    // 标记为脏渲染（对应TypeScript中的this.dirtyRender = true）
    dirtyRender = true;
}

void TextNode::clean() {
    // 完全对应TypeScript实现：
    // public clean():void {
    //     if(this.$texture) {
    //         WebGLUtils.deleteWebGLTexture(this.$texture);
    //         this.$texture = null;
    //         this.dirtyRender = true;
    //     }
    // }
    
    // 清除WebGL纹理等缓存数据
    if (texture != nullptr) {
        // TODO: 这里需要具体的WebGL纹理删除实现
        // WebGLUtils::deleteWebGLTexture(texture);
        texture = nullptr;
        dirtyRender = true;
    }
    
    // 标记为脏渲染
    dirtyRender = true;
}

void TextNode::cleanBeforeRender() {
    // 完全对应TypeScript实现：
    // public cleanBeforeRender():void{
    //     super.cleanBeforeRender();
    //     this.dirtyRender = true;
    // }
    
    // 调用父类的清理方法
    RenderNode::cleanBeforeRender();
    
    // 清空绘制命令（对应清空drawData）
    m_drawCommands.clear();
    
    // 标记为脏渲染
    dirtyRender = true;
}

} // namespace sys
} // namespace egret