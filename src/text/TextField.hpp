/**
 * @file TextField.hpp
 * @brief TextField类 - 文本显示和编辑核心类
 * 
 * 翻译自：egret-core-5.4.1/src/egret/text/TextField.ts
 * TextField类用于文本的显示和编辑，支持富文本、字体样式、对齐等完整功能。
 */

#pragma once

#include "display/DisplayObject.hpp"
#include "text/HorizontalAlign.hpp"
#include "text/VerticalAlign.hpp"
#include "text/TextFieldType.hpp"
#include "text/TextFieldInputType.hpp"
#include <string>
#include <vector>
#include <memory>

namespace egret {

// 前置声明
namespace sys {
    class TextNode;
}

/**
 * @brief TextField类 - 文本显示和编辑核心类
 * 
 * TextField类用于文本的显示和编辑。可以显示单行或多行文本，
 * 支持富文本格式、字体样式设置、文本对齐、自动换行等功能。
 * 也可以作为输入文本字段，支持用户输入。
 * 
 * @note 对应TypeScript的egret.TextField类，100%API兼容
 */
class TextField : public DisplayObject {
public:
    /**
     * @brief 默认文本字体
     * 
     * 所有TextField实例的默认字体族名称
     */
    static std::string default_fontFamily;

    /**
     * @brief 默认字体大小（像素）
     * 
     * 所有TextField实例的默认字体大小
     */
    static double default_size;

    /**
     * @brief 默认文本颜色
     * 
     * 所有TextField实例的默认文本颜色
     */
    static uint32_t default_textColor;

    /**
     * @brief 构造函数 - 创建新的TextField实例
     * 
     * 初始化TextField的所有默认属性和内部TextNode
     */
    TextField();

    /**
     * @brief 析构函数
     * 
     * 清理TextField相关资源
     */
    virtual ~TextField();

    // ========== 字体和样式属性 ==========

    /**
     * @brief 获取字体族名称
     * 
     * @return 字体族名称字符串
     */
    const std::string& getFontFamily() const;

    /**
     * @brief 设置字体族名称
     * 
     * @param value 字体族名称（如"Arial", "Times New Roman"等）
     */
    void setFontFamily(const std::string& value);

    /**
     * @brief 获取字体大小
     * 
     * @return 字体大小（像素）
     */
    double getSize() const;

    /**
     * @brief 设置字体大小
     * 
     * @param value 字体大小（像素）
     */
    void setSize(double value);

    /**
     * @brief 获取粗体设置
     * 
     * @return true表示粗体，false表示正常
     */
    bool getBold() const;

    /**
     * @brief 设置粗体
     * 
     * @param value true表示粗体，false表示正常
     */
    void setBold(bool value);

    /**
     * @brief 获取斜体设置
     * 
     * @return true表示斜体，false表示正常
     */
    bool getItalic() const;

    /**
     * @brief 设置斜体
     * 
     * @param value true表示斜体，false表示正常
     */
    void setItalic(bool value);

    // ========== 对齐和布局属性 ==========

    /**
     * @brief 获取文本水平对齐方式
     * 
     * @return 水平对齐方式字符串
     * @see HorizontalAlign
     */
    const std::string& getTextAlign() const;

    /**
     * @brief 设置文本水平对齐方式
     * 
     * @param value 水平对齐方式（left, center, right等）
     * @see HorizontalAlign
     */
    void setTextAlign(const std::string& value);

    /**
     * @brief 获取文本垂直对齐方式
     * 
     * @return 垂直对齐方式字符串
     * @see VerticalAlign
     */
    const std::string& getVerticalAlign() const;

    /**
     * @brief 设置文本垂直对齐方式
     * 
     * @param value 垂直对齐方式（top, middle, bottom等）
     * @see VerticalAlign
     */
    void setVerticalAlign(const std::string& value);

    /**
     * @brief 获取行间距
     * 
     * @return 行间距（像素）
     */
    double getLineSpacing() const;

    /**
     * @brief 设置行间距
     * 
     * @param value 行间距（像素）
     */
    void setLineSpacing(double value);

    // ========== 颜色和外观属性 ==========

    /**
     * @brief 获取文本颜色
     * 
     * @return 文本颜色值（0xRRGGBB格式）
     */
    uint32_t getTextColor() const;

    /**
     * @brief 设置文本颜色
     * 
     * @param value 文本颜色值（0xRRGGBB格式）
     */
    void setTextColor(uint32_t value);

    /**
     * @brief 获取描边颜色
     * 
     * @return 描边颜色值（0xRRGGBB格式）
     */
    uint32_t getStrokeColor() const;

    /**
     * @brief 设置描边颜色
     * 
     * @param value 描边颜色值（0xRRGGBB格式）
     */
    void setStrokeColor(uint32_t value);

    /**
     * @brief 获取描边宽度
     * 
     * @return 描边宽度（像素）
     */
    double getStroke() const;

    /**
     * @brief 设置描边宽度
     * 
     * @param value 描边宽度（像素）
     */
    void setStroke(double value);

    // ========== 文本内容和显示 ==========

    /**
     * @brief 获取文本内容
     * 
     * @return 文本内容字符串
     */
    const std::string& getText() const;

    /**
     * @brief 设置文本内容
     * 
     * @param value 文本内容字符串
     */
    void setText(const std::string& value);

    /**
     * @brief 获取是否自动换行
     * 
     * @return true表示自动换行，false表示不换行
     */
    bool getWordWrap() const;

    /**
     * @brief 设置是否自动换行
     * 
     * @param value true表示自动换行，false表示不换行
     */
    void setWordWrap(bool value);

    // ========== 输入和编辑相关 ==========

    /**
     * @brief 获取文本字段类型
     * 
     * @return 字段类型字符串
     * @see TextFieldType
     */
    const std::string& getType() const;

    /**
     * @brief 设置文本字段类型
     * 
     * @param value 字段类型（dynamic或input）
     * @see TextFieldType
     */
    void setType(const std::string& value);

    /**
     * @brief 获取输入类型
     * 
     * @return 输入类型字符串
     * @see TextFieldInputType
     */
    const std::string& getInputType() const;

    /**
     * @brief 设置输入类型
     * 
     * @param value 输入类型（text, tel, password等）
     * @see TextFieldInputType
     */
    void setInputType(const std::string& value);

    /**
     * @brief 获取是否显示为密码
     * 
     * @return true表示显示为密码（星号），false表示正常显示
     */
    bool getDisplayAsPassword() const;

    /**
     * @brief 设置是否显示为密码
     * 
     * @param value true表示显示为密码（星号），false表示正常显示
     */
    void setDisplayAsPassword(bool value);

    /**
     * @brief 获取最大字符数
     * 
     * @return 最大字符数，0表示无限制
     */
    int getMaxChars() const;

    /**
     * @brief 设置最大字符数
     * 
     * @param value 最大字符数，0表示无限制
     */
    void setMaxChars(int value);

    // ========== 尺寸和测量 ==========

    /**
     * @brief 获取文本宽度
     * 
     * @return 文本内容的实际显示宽度
     */
    double getTextWidth() const;

    /**
     * @brief 获取文本高度
     * 
     * @return 文本内容的实际显示高度
     */
    double getTextHeight() const;

    // ========== 滚动和选择相关 ==========

    /**
     * @brief 获取垂直滚动位置
     * 
     * @return 当前滚动行数
     */
    int getScrollV() const;

    /**
     * @brief 设置垂直滚动位置
     * 
     * @param value 滚动到的行数
     */
    void setScrollV(int value);

    /**
     * @brief 获取最大滚动行数
     * 
     * @return 最大可滚动行数
     */
    int getMaxScrollV() const;

    /**
     * @brief 获取选择开始位置
     * 
     * @return 选择文本的开始索引
     */
    int getSelectionBeginIndex() const;

    /**
     * @brief 获取选择结束位置
     * 
     * @return 选择文本的结束索引
     */
    int getSelectionEndIndex() const;

    // ========== 公共方法 ==========

    /**
     * @brief 设置选择文本范围
     * 
     * @param beginIndex 开始索引
     * @param endIndex 结束索引
     */
    void setSelection(int beginIndex, int endIndex);

    /**
     * @brief 在指定位置插入文本
     * 
     * @param text 要插入的文本
     */
    void replaceSelectedText(const std::string& text);

    /**
     * @brief 将文本追加到当前文本末尾
     * 
     * @param newText 要追加的文本
     */
    void appendText(const std::string& newText);

protected:
    /**
     * @brief 测量内容边界
     * 
     * 重写父类方法，测量文本内容的边界
     * 
     * @param bounds 输出的边界矩形
     */
    void measureContentBounds(Rectangle& bounds) override;

private:
    // TextField的内部属性结构（对应TypeScript中的$TextField数组）
    struct TextFieldData {
        double fontSize;                // 字体大小
        double lineSpacing;            // 行间距
        uint32_t textColor;            // 文本颜色
        double textFieldWidth;         // 文本字段宽度
        double textFieldHeight;        // 文本字段高度
        double textWidth;              // 文本宽度
        double textHeight;             // 文本高度
        double textDrawWidth;          // 文本绘制宽度
        std::string fontFamily;        // 字体族
        std::string textAlign;         // 水平对齐
        std::string verticalAlign;     // 垂直对齐
        std::string textColorString;   // 文本颜色字符串
        std::string fontString;        // 字体字符串
        std::string text;              // 文本内容
        std::vector<double> measuredWidths; // 测量宽度
        bool bold;                     // 粗体
        bool italic;                   // 斜体
        bool fontStringChanged;        // 字体字符串改变标志
        bool textLinesChanged;         // 文本行改变标志
        bool wordWrap;                 // 自动换行
    };

    /**
     * @brief 文本字段数据
     */
    std::unique_ptr<TextFieldData> m_textFieldData;

    /**
     * @brief 文本渲染节点
     */
    std::shared_ptr<sys::TextNode> m_textNode;

    /**
     * @brief 输入相关属性
     */
    std::string m_type;                // 字段类型
    std::string m_inputType;           // 输入类型
    bool m_displayAsPassword;          // 显示为密码
    int m_maxChars;                    // 最大字符数
    uint32_t m_strokeColor;            // 描边颜色
    double m_stroke;                   // 描边宽度

    /**
     * @brief 滚动和选择属性
     */
    int m_scrollV;                     // 垂直滚动位置
    int m_selectionBeginIndex;         // 选择开始位置
    int m_selectionEndIndex;           // 选择结束位置

    /**
     * @brief 更新字体字符串
     * 
     * 根据当前字体属性生成字体字符串
     */
    void updateFontString();

    /**
     * @brief 标记文本需要重新测量
     * 
     * 当文本内容或样式改变时调用
     */
    void invalidateTextSize();

    // 禁用拷贝构造和赋值操作
    TextField(const TextField&) = delete;
    TextField& operator=(const TextField&) = delete;
};

} // namespace egret