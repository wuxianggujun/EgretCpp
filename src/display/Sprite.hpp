/**
 * @file Sprite.hpp
 * @brief Sprite类 - 可包含子项的显示列表节点，基于DisplayObjectContainer并集成Graphics绘图功能
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/Sprite.ts
 * Sprite类是基本显示列表构造块：一个可包含子项的显示列表节点，提供Graphics矢量绘图功能。
 */

#pragma once

#include "display/DisplayObjectContainer.hpp"
#include <memory>

namespace egret {

// 前置声明
class Graphics;

/**
 * @brief Sprite类 - 可包含子项的显示列表节点并提供Graphics绘图功能
 * 
 * Sprite类是基本显示列表构造块：一个可包含子项的显示列表节点。
 * 继承自DisplayObjectContainer，提供容器功能。
 * 内置Graphics对象，支持矢量图形绘制。
 * 
 * @note 对应TypeScript的egret.Sprite类，100%API兼容
 */
class Sprite : public DisplayObjectContainer {
public:
    /**
     * @brief 构造函数 - 创建新的Sprite实例
     * 
     * 自动创建内置的Graphics对象并设置target为当前Sprite。
     */
    Sprite();

    /**
     * @brief 析构函数
     * 
     * 清理Graphics对象和相关资源。
     */
    virtual ~Sprite();

    /**
     * @brief 获取Graphics对象
     * 
     * 获取Sprite中的Graphics对象。可通过此对象执行矢量绘图命令。
     * 
     * @return Graphics* Graphics绘图对象指针（非空）
     */
    Graphics* getGraphics() const;

    /**
     * @brief 命中测试 - 检测点击位置是否在显示对象内
     * 
     * 重写DisplayObjectContainer的命中测试方法，加入Graphics绘图内容的命中测试。
     * 测试顺序：
     * 1. 检查基本可见性和边界
     * 2. 递归测试所有子对象（从最上层开始）
     * 3. 如果子对象未命中，则测试自身的Graphics绘图内容
     * 
     * @param stageX 舞台X坐标
     * @param stageY 舞台Y坐标
     * @return DisplayObject* 命中的显示对象，未命中返回nullptr
     */
    DisplayObject* hitTest(double stageX, double stageY);

    /**
     * @brief 从舞台移除时的清理操作
     * 
     * 重写父类方法，在从舞台移除时清理Graphics对象。
     */
    void onRemoveFromStage();

protected:
    /**
     * @brief 测量内容边界
     * 
     * 重写父类方法，将Graphics绘图内容的边界包含在测量范围内。
     * 
     * @param bounds 输出的边界矩形
     */
    void measureContentBounds(Rectangle& bounds);

private:
    /**
     * @brief Graphics绘图对象
     * 
     * 内置的Graphics对象，用于矢量图形绘制。
     * 在构造函数中自动创建，析构时自动清理。
     */
    std::shared_ptr<Graphics> m_graphics;

    // 禁用拷贝构造和赋值操作
    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
};

} // namespace egret