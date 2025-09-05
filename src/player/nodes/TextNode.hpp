/**
 * @file TextNode.hpp  
 * @brief TextNode类 - 文本渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/TextNode.ts
 * TextNode继承自RenderNode，专门用于文本渲染。
 */

#pragma once

#include "player/RenderNode.hpp"
#include "sys/TextFormat.hpp"
#include <string>
#include <vector>

namespace egret {
namespace sys {

/**
 * @brief TextNode类 - 文本渲染节点
 * 
 * TextNode继承自RenderNode，提供文本渲染功能。
 * 完全基于TypeScript egret.sys.TextNode实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.sys.TextNode类
 */
class TextNode : public RenderNode {
public:
    /**
     * @brief 构造函数
     */
    TextNode();
    
    /**
     * @brief 析构函数
     */
    virtual ~TextNode();
    
    /**
     * @brief 渲染方法实现
     * @param renderer 渲染器实例
     */
    void render(void* renderer) override;

    // ========== 文本样式属性（对应TypeScript公共属性）==========
    
    /**
     * @brief 颜色值 - 对应 public textColor:number = 0xFFFFFF;
     */
    uint32_t textColor = 0xFFFFFF;
    
    /**
     * @brief 描边颜色值 - 对应 public strokeColor:number = 0x000000;
     */
    uint32_t strokeColor = 0x000000;
    
    /**
     * @brief 字号 - 对应 public size:number = 30;
     */
    double size = 30.0;
    
    /**
     * @brief 描边大小 - 对应 public stroke:number = 0;
     */
    double stroke = 0.0;
    
    /**
     * @brief 是否加粗 - 对应 public bold:boolean = false;
     */
    bool bold = false;
    
    /**
     * @brief 是否倾斜 - 对应 public italic:boolean = false;
     */
    bool italic = false;
    
    /**
     * @brief 字体名称 - 对应 public fontFamily:string = "Arial";
     */
    std::string fontFamily = "Arial";

    // ========== WebGL相关属性 ==========
    
    /**
     * @brief 绘制坐标和尺寸
     */
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
    
    /**
     * @brief 脏渲染标记 - 对应 public dirtyRender:boolean = true;
     */
    bool dirtyRender = true;
    
    /**
     * @brief WebGL纹理相关
     */
    void* texture = nullptr;
    double textureWidth = 0.0;
    double textureHeight = 0.0;
    double canvasScaleX = 1.0;
    double canvasScaleY = 1.0;

    // ========== 公共方法 ==========
    
    /**
     * @brief 绘制一行文本 - 对应 public drawText(x:number, y:number, text:string, format:TextFormat):void
     * 
     * @param x 文本x坐标
     * @param y 文本y坐标
     * @param text 要绘制的文本内容
     * @param format 文本格式
     */
    void drawText(double x, double y, const std::string& text, const TextFormat& format);
    
    /**
     * @brief 清除缓存数据 - 对应 public clean():void
     * 
     * 清除WebGL纹理等非绘制的缓存数据
     */
    void clean();
    
    /**
     * @brief 清理渲染前数据 - 重写父类方法
     * 
     * 对应 public cleanBeforeRender():void
     * 在显示对象的$updateRenderNode()方法被调用前，自动清空自身的drawData数据。
     */
    void cleanBeforeRender() override;

private:
    /**
     * @brief 绘制命令结构
     * 
     * 存储单次drawText调用的所有参数
     */
    struct DrawCommand {
        double x;                ///< 文本x坐标
        double y;                ///< 文本y坐标
        std::string text;        ///< 文本内容
        TextFormat format;       ///< 文本格式
    };
    
    /**
     * @brief 绘制命令列表
     * 
     * 存储所有待绘制的文本命令，对应TypeScript中的drawData数组
     */
    std::vector<DrawCommand> m_drawCommands;
};

} // namespace sys
} // namespace egret