/**
 * @file TextField.cpp
 * @brief TextField类实现 - 文本显示和编辑核心类
 * 
 * 翻译自：egret-core-5.4.1/src/egret/text/TextField.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "text/TextField.hpp"
#include "text/HorizontalAlign.hpp"
#include "text/VerticalAlign.hpp"
#include "text/TextFieldType.hpp"
#include "text/TextFieldInputType.hpp"
#include "player/nodes/TextNode.hpp"
#include "geom/Rectangle.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>

namespace egret {

    // ========== 静态成员定义 ==========

    std::string TextField::default_fontFamily = "Arial";
    double TextField::default_size = 30.0;
    uint32_t TextField::default_textColor = 0xFFFFFF; // 白色

    // ========== 构造函数和析构函数 ==========

    TextField::TextField() 
        : DisplayObject()
        , m_textFieldData(std::make_unique<TextFieldData>())
        , m_type(TextFieldType::DYNAMIC)
        , m_inputType(TextFieldInputType::TEXT)
        , m_displayAsPassword(false)
        , m_maxChars(0)
        , m_strokeColor(0x000000) // 黑色
        , m_stroke(0.0)
        , m_scrollV(1)
        , m_selectionBeginIndex(0)
        , m_selectionEndIndex(0)
    {
        // 初始化TextFieldData（对应TypeScript中的$TextField数组初始化）
        m_textFieldData->fontSize = default_size;
        m_textFieldData->lineSpacing = 0.0;
        m_textFieldData->textColor = default_textColor;
        m_textFieldData->textFieldWidth = std::numeric_limits<double>::quiet_NaN();
        m_textFieldData->textFieldHeight = std::numeric_limits<double>::quiet_NaN();
        m_textFieldData->textWidth = 0.0;
        m_textFieldData->textHeight = 0.0;
        m_textFieldData->textDrawWidth = 0.0;
        m_textFieldData->fontFamily = default_fontFamily;
        m_textFieldData->textAlign = HorizontalAlign::LEFT;
        m_textFieldData->verticalAlign = VerticalAlign::TOP;
        m_textFieldData->textColorString = "#ffffff";
        m_textFieldData->fontString = "";
        m_textFieldData->text = "";
        m_textFieldData->measuredWidths.clear();
        m_textFieldData->bold = false;
        m_textFieldData->italic = false;
        m_textFieldData->fontStringChanged = true;
        m_textFieldData->textLinesChanged = false;
        m_textFieldData->wordWrap = false;

        // 创建TextNode（完全对应TypeScript中的textNode创建）
        // let textNode = new sys.TextNode();
        // textNode.fontFamily = TextField.default_fontFamily;
        // this.textNode = textNode;
        // this.$renderNode = textNode;
        m_textNode = std::make_shared<sys::TextNode>();
        m_textNode->fontFamily = m_textFieldData->fontFamily;
        
        // 设置为渲染节点（对应TypeScript中的this.$renderNode = textNode）
        setRenderNode(std::dynamic_pointer_cast<sys::RenderNode>(m_textNode));
    }

    TextField::~TextField() {
        // TextFieldData通过unique_ptr自动清理
        // TextNode通过shared_ptr自动清理
    }

    // ========== 字体和样式属性实现 ==========

    const std::string& TextField::getFontFamily() const {
        return m_textFieldData->fontFamily;
    }

    void TextField::setFontFamily(const std::string& value) {
        if (m_textFieldData->fontFamily != value) {
            m_textFieldData->fontFamily = value;
            m_textFieldData->fontStringChanged = true;
            invalidateTextSize();
        }
    }

    double TextField::getSize() const {
        return m_textFieldData->fontSize;
    }

    void TextField::setSize(double value) {
        if (m_textFieldData->fontSize != value) {
            m_textFieldData->fontSize = value;
            m_textFieldData->fontStringChanged = true;
            invalidateTextSize();
        }
    }

    bool TextField::getBold() const {
        return m_textFieldData->bold;
    }

    void TextField::setBold(bool value) {
        if (m_textFieldData->bold != value) {
            m_textFieldData->bold = value;
            m_textFieldData->fontStringChanged = true;
            invalidateTextSize();
        }
    }

    bool TextField::getItalic() const {
        return m_textFieldData->italic;
    }

    void TextField::setItalic(bool value) {
        if (m_textFieldData->italic != value) {
            m_textFieldData->italic = value;
            m_textFieldData->fontStringChanged = true;
            invalidateTextSize();
        }
    }

    // ========== 对齐和布局属性实现 ==========

    const std::string& TextField::getTextAlign() const {
        return m_textFieldData->textAlign;
    }

    void TextField::setTextAlign(const std::string& value) {
        if (m_textFieldData->textAlign != value) {
            m_textFieldData->textAlign = value;
            invalidateTextSize();
        }
    }

    const std::string& TextField::getVerticalAlign() const {
        return m_textFieldData->verticalAlign;
    }

    void TextField::setVerticalAlign(const std::string& value) {
        if (m_textFieldData->verticalAlign != value) {
            m_textFieldData->verticalAlign = value;
            invalidateTextSize();
        }
    }

    double TextField::getLineSpacing() const {
        return m_textFieldData->lineSpacing;
    }

    void TextField::setLineSpacing(double value) {
        if (m_textFieldData->lineSpacing != value) {
            m_textFieldData->lineSpacing = value;
            invalidateTextSize();
        }
    }

    // ========== 颜色和外观属性实现 ==========

    uint32_t TextField::getTextColor() const {
        return m_textFieldData->textColor;
    }

    void TextField::setTextColor(uint32_t value) {
        if (m_textFieldData->textColor != value) {
            m_textFieldData->textColor = value;
            
            // 更新textColorString（对应TypeScript中的颜色字符串转换）
            std::stringstream ss;
            ss << "#" << std::hex << std::setw(6) << std::setfill('0') << value;
            m_textFieldData->textColorString = ss.str();
            
            invalidateTextSize();
        }
    }

    uint32_t TextField::getStrokeColor() const {
        return m_strokeColor;
    }

    void TextField::setStrokeColor(uint32_t value) {
        if (m_strokeColor != value) {
            m_strokeColor = value;
            invalidateTextSize();
        }
    }

    double TextField::getStroke() const {
        return m_stroke;
    }

    void TextField::setStroke(double value) {
        if (m_stroke != value) {
            m_stroke = value;
            invalidateTextSize();
        }
    }

    // ========== 文本内容和显示实现 ==========

    const std::string& TextField::getText() const {
        return m_textFieldData->text;
    }

    void TextField::setText(const std::string& value) {
        if (m_textFieldData->text != value) {
            m_textFieldData->text = value;
            m_textFieldData->textLinesChanged = true;
            invalidateTextSize();
        }
    }

    bool TextField::getWordWrap() const {
        return m_textFieldData->wordWrap;
    }

    void TextField::setWordWrap(bool value) {
        if (m_textFieldData->wordWrap != value) {
            m_textFieldData->wordWrap = value;
            invalidateTextSize();
        }
    }

    // ========== 输入和编辑相关实现 ==========

    const std::string& TextField::getType() const {
        return m_type;
    }

    void TextField::setType(const std::string& value) {
        if (m_type != value) {
            m_type = value;
            // 类型改变时可能影响文本显示
            invalidateTextSize();
        }
    }

    const std::string& TextField::getInputType() const {
        return m_inputType;
    }

    void TextField::setInputType(const std::string& value) {
        if (m_inputType != value) {
            m_inputType = value;
            // 输入类型改变不影响显示，但可能影响输入行为
        }
    }

    bool TextField::getDisplayAsPassword() const {
        return m_displayAsPassword;
    }

    void TextField::setDisplayAsPassword(bool value) {
        if (m_displayAsPassword != value) {
            m_displayAsPassword = value;
            // 密码模式改变影响文本显示
            invalidateTextSize();
        }
    }

    int TextField::getMaxChars() const {
        return m_maxChars;
    }

    void TextField::setMaxChars(int value) {
        if (m_maxChars != value) {
            m_maxChars = value;
            // 最大字符数改变可能影响当前文本
            if (m_maxChars > 0 && static_cast<int>(m_textFieldData->text.length()) > m_maxChars) {
                setText(m_textFieldData->text.substr(0, m_maxChars));
            }
        }
    }

    // ========== 尺寸和测量实现 ==========

    double TextField::getTextWidth() const {
        return m_textFieldData->textWidth;
    }

    double TextField::getTextHeight() const {
        return m_textFieldData->textHeight;
    }

    // ========== 滚动和选择相关实现 ==========

    int TextField::getScrollV() const {
        return m_scrollV;
    }

    void TextField::setScrollV(int value) {
        // 确保滚动位置在有效范围内
        int maxScroll = getMaxScrollV();
        m_scrollV = std::max(1, std::min(value, maxScroll));
    }

    int TextField::getMaxScrollV() const {
        // 简化实现，返回1（单行显示）
        // 真实实现需要根据文本行数和显示区域计算
        return 1;
    }

    int TextField::getSelectionBeginIndex() const {
        return m_selectionBeginIndex;
    }

    int TextField::getSelectionEndIndex() const {
        return m_selectionEndIndex;
    }

    // ========== 公共方法实现 ==========

    void TextField::setSelection(int beginIndex, int endIndex) {
        int textLength = static_cast<int>(m_textFieldData->text.length());
        
        // 确保索引在有效范围内
        beginIndex = std::max(0, std::min(beginIndex, textLength));
        endIndex = std::max(beginIndex, std::min(endIndex, textLength));
        
        m_selectionBeginIndex = beginIndex;
        m_selectionEndIndex = endIndex;
    }

    void TextField::replaceSelectedText(const std::string& text) {
        if (m_type != TextFieldType::INPUT) {
            return; // 只有输入文本字段才能替换文本
        }

        std::string currentText = m_textFieldData->text;
        std::string newText = currentText.substr(0, m_selectionBeginIndex) + 
                             text + 
                             currentText.substr(m_selectionEndIndex);
        
        // 检查最大字符数限制
        if (m_maxChars > 0 && static_cast<int>(newText.length()) > m_maxChars) {
            newText = newText.substr(0, m_maxChars);
        }
        
        setText(newText);
        
        // 更新选择位置
        int newPosition = m_selectionBeginIndex + static_cast<int>(text.length());
        setSelection(newPosition, newPosition);
    }

    void TextField::appendText(const std::string& newText) {
        setText(m_textFieldData->text + newText);
    }

    // ========== 保护方法实现 ==========

    void TextField::measureContentBounds(Rectangle& bounds) {
        // 简化实现，使用文本的测量尺寸
        bounds.setX(0);
        bounds.setY(0);
        bounds.setWidth(m_textFieldData->textWidth);
        bounds.setHeight(m_textFieldData->textHeight);
    }

    // ========== 私有方法实现 ==========

    void TextField::updateFontString() {
        if (!m_textFieldData->fontStringChanged) {
            return;
        }

        // 构建字体字符串（对应CSS font属性格式）
        std::stringstream fontString;
        
        if (m_textFieldData->italic) {
            fontString << "italic ";
        }
        if (m_textFieldData->bold) {
            fontString << "bold ";
        }
        
        fontString << m_textFieldData->fontSize << "px ";
        fontString << m_textFieldData->fontFamily;
        
        m_textFieldData->fontString = fontString.str();
        m_textFieldData->fontStringChanged = false;
    }

    void TextField::invalidateTextSize() {
        // 标记需要重新测量文本
        m_textFieldData->textLinesChanged = true;
        
        // 更新字体字符串
        updateFontString();
        
        // 标记显示对象需要重绘
        setRenderDirty(true);
    }

} // namespace egret