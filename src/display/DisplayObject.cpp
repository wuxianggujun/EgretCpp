#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "display/Stage.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

namespace egret {

    DisplayObject::DisplayObject() : EventDispatcher() {
        m_tint = 0xFFFFFF;
        m_matrix = std::make_unique<Matrix>();
    }

    DisplayObject::~DisplayObject() {
        if (m_scrollRect) {
            delete m_scrollRect;
        }
    }

    // ========== 坐标设置实现 ==========

    bool DisplayObject::setXInternal(double value) {
        if (m_x == value) {
            return false;
        }
        m_x = value;
        onPropertyChanged();
        return true;
    }

    bool DisplayObject::setYInternal(double value) {
        if (m_y == value) {
            return false;
        }
        m_y = value;
        onPropertyChanged();
        return true;
    }

    // ========== 缩放设置实现 ==========

    void DisplayObject::setScaleXInternal(double value) {
        if (m_scaleX == value) {
            return;
        }
        m_scaleX = value;
        markMatrixDirty();
        updateUseTransform();
        onPropertyChanged();
    }

    void DisplayObject::setScaleYInternal(double value) {
        if (m_scaleY == value) {
            return;
        }
        m_scaleY = value;
        markMatrixDirty();
        updateUseTransform();
        onPropertyChanged();
    }

    // ========== 旋转设置实现 ==========

    void DisplayObject::setRotationInternal(double value) {
        value = clampRotation(value);
        if (m_rotation == value) {
            return;
        }
        m_rotation = value;
        
        // 将度数转换为弧度并更新倾斜值
        double radians = value * glm::pi<double>() / 180.0;
        m_skewX = radians;
        m_skewY = radians;
        m_skewXdeg = value;
        m_skewYdeg = value;
        
        markMatrixDirty();
        updateUseTransform();
        onPropertyChanged();
    }

    void DisplayObject::setSkewXInternal(double value) {
        if (m_skewX == value) {
            return;
        }
        m_skewX = value;
        m_skewXdeg = value * 180.0 / glm::pi<double>();
        
        // 更新旋转角度（使用两个倾斜值的平均值）
        m_rotation = (m_skewX + m_skewY) * 180.0 / (2.0 * glm::pi<double>());
        
        markMatrixDirty();
        updateUseTransform();
        onPropertyChanged();
    }

    void DisplayObject::setSkewYInternal(double value) {
        if (m_skewY == value) {
            return;
        }
        m_skewY = value;
        m_skewYdeg = value * 180.0 / glm::pi<double>();
        
        // 更新旋转角度（使用两个倾斜值的平均值）
        m_rotation = (m_skewX + m_skewY) * 180.0 / (2.0 * glm::pi<double>());
        
        markMatrixDirty();
        updateUseTransform();
        onPropertyChanged();
    }

    // ========== 显示属性设置实现 ==========

    void DisplayObject::setAlphaInternal(double value) {
        value = std::clamp(value, 0.0, 1.0);
        if (m_alpha == value) {
            return;
        }
        m_alpha = value;
        onPropertyChanged();
    }

    void DisplayObject::setVisibleInternal(bool value) {
        if (m_visible == value) {
            return;
        }
        m_visible = value;
        onPropertyChanged();
    }

    void DisplayObject::setTouchEnabledInternal(bool value) {
        if (m_touchEnabled == value) {
            return;
        }
        m_touchEnabled = value;
        onPropertyChanged();
    }

    // ========== 锚点设置实现 ==========

    void DisplayObject::setAnchorOffsetXInternal(double value) {
        if (m_anchorOffsetX == value) {
            return;
        }
        m_anchorOffsetX = value;
        onPropertyChanged();
    }

    void DisplayObject::setAnchorOffsetYInternal(double value) {
        if (m_anchorOffsetY == value) {
            return;
        }
        m_anchorOffsetY = value;
        onPropertyChanged();
    }

    // ========== 矩阵操作实现 ==========

    Matrix DisplayObject::getMatrix() {
        return *getMatrixInternal();
    }

    void DisplayObject::setMatrix(const Matrix& matrix) {
        setMatrixInternal(matrix, true);
    }

    Matrix* DisplayObject::getMatrixInternal() {
        if (m_matrixDirty) {
            m_matrixDirty = false;
            m_matrix->updateScaleAndRotation(m_scaleX, m_scaleY, m_skewX, m_skewY);
        }
        m_matrix->tx = m_x;
        m_matrix->ty = m_y;
        return m_matrix.get();
    }

    void DisplayObject::setMatrixInternal(const Matrix& matrix, bool needUpdateProperties) {
        m_matrix->copyFrom(matrix);
        m_matrixDirty = false;
        
        if (needUpdateProperties) {
            // 从矩阵中提取属性
            m_x = matrix.tx;
            m_y = matrix.ty;
            m_scaleX = matrix.getScaleX();
            m_scaleY = matrix.getScaleY();
            m_skewX = matrix.getSkewX();
            m_skewY = matrix.getSkewY();
            m_rotation = matrix.getRotation() * 180.0 / glm::pi<double>();
            
            updateUseTransform();
        }
        
        onPropertyChanged();
    }

    Matrix* DisplayObject::getConcatenatedMatrix() {
        if (!m_concatenatedMatrix) {
            m_concatenatedMatrix = std::make_unique<Matrix>();
        }
        
        m_concatenatedMatrix->copyFrom(*getMatrixInternal());
        
        DisplayObject* current = m_parent;
        while (current) {
            m_concatenatedMatrix->prependMatrix(*current->getMatrixInternal());
            current = current->getParent();
        }
        
        return m_concatenatedMatrix.get();
    }

    Matrix* DisplayObject::getInvertedConcatenatedMatrix() {
        if (!m_invertedConcatenatedMatrix) {
            m_invertedConcatenatedMatrix = std::make_unique<Matrix>();
        }
        
        m_invertedConcatenatedMatrix->copyFrom(*getConcatenatedMatrix());
        m_invertedConcatenatedMatrix->invertSelf();
        
        return m_invertedConcatenatedMatrix.get();
    }

    // ========== 宽度高度实现 ==========

    double DisplayObject::getWidth() {
        Rectangle bounds = getBounds(nullptr, true);
        return bounds.getWidth();
    }

    void DisplayObject::setWidth(double value) {
        Rectangle bounds = getMeasuredBounds();
        if (bounds.getWidth() != 0.0) {
            setScaleX(value / bounds.getWidth());
        }
    }

    double DisplayObject::getHeight() {
        Rectangle bounds = getBounds(nullptr, true);
        return bounds.getHeight();
    }

    void DisplayObject::setHeight(double value) {
        Rectangle bounds = getMeasuredBounds();
        if (bounds.getHeight() != 0.0) {
            setScaleY(value / bounds.getHeight());
        }
    }

    // ========== 滚动矩形实现 ==========

    void DisplayObject::setScrollRectInternal(Rectangle* value) {
        if (m_scrollRect == value) {
            return;
        }
        
        if (m_scrollRect) {
            delete m_scrollRect;
        }
        
        m_scrollRect = value ? new Rectangle(*value) : nullptr;
        onPropertyChanged();
    }

    // ========== 显示效果实现 ==========

    void DisplayObject::setBlendMode(int value) {
        if (m_blendMode == value) {
            return;
        }
        m_blendMode = value;
        onPropertyChanged();
    }

    void DisplayObject::setMaskInternal(DisplayObject* value) {
        if (m_mask == value) {
            return;
        }
        
        if (m_mask) {
            m_mask->setMaskedObject(nullptr);
        }
        
        m_mask = value;
        
        if (m_mask) {
            m_mask->setMaskedObject(this);
        }
        
        onPropertyChanged();
    }

    void DisplayObject::setTintInternal(uint32_t value) {
        if (m_tint == value) {
            return;
        }
        m_tint = value;
        onPropertyChanged();
    }

    // ========== 坐标转换实现 ==========

    Point DisplayObject::globalToLocal(double stageX, double stageY, Point* resultPoint) {
        Matrix* invertedMatrix = getInvertedConcatenatedMatrix();
        Point point = invertedMatrix->transformPoint(Point(stageX, stageY));
        
        if (resultPoint) {
            resultPoint->copyFrom(point);
            return *resultPoint;
        }
        
        return point;
    }

    Point DisplayObject::localToGlobal(double localX, double localY, Point* resultPoint) {
        Matrix* matrix = getConcatenatedMatrix();
        Point point = matrix->transformPoint(Point(localX, localY));
        
        if (resultPoint) {
            resultPoint->copyFrom(point);
            return *resultPoint;
        }
        
        return point;
    }

    // ========== 碰撞检测实现 ==========

    bool DisplayObject::hitTestPoint(double x, double y, bool shapeFlag) {
        Point localPoint = globalToLocal(x, y);
        Rectangle bounds = getBounds(nullptr, false);
        return bounds.containsPoint(localPoint);
    }

    DisplayObject* DisplayObject::hitTestObject(DisplayObject* other) {
        if (!other) {
            return nullptr;
        }
        
        Rectangle thisBounds = getBounds(nullptr, true);
        Rectangle otherBounds = other->getBounds(nullptr, true);
        
        if (thisBounds.intersects(otherBounds)) {
            return this;
        }
        
        return nullptr;
    }

    // ========== 内部方法实现 ==========

    void DisplayObject::setParentInternal(DisplayObjectContainer* parent) {
        m_parent = parent;
    }

    void DisplayObject::onAddToStageInternal(Stage* stage, int nestLevel) {
        m_stage = stage;
        m_nestLevel = nestLevel;
        m_hasAddToStage = true;
        
        // 添加到待处理的事件列表中（延迟处理）
        DisplayObjectContainer::s_eventAddToStageList.push_back(this);
    }

    void DisplayObject::onRemoveFromStageInternal() {
        m_stage = nullptr;
        m_nestLevel = 0;
        m_hasAddToStage = false;
        
        // 添加到待处理的事件列表中（延迟处理）
        DisplayObjectContainer::s_eventRemoveFromStageList.push_back(this);
    }

    // ========== 边界计算实现 ==========
    
    Rectangle DisplayObject::getBounds(Rectangle* resultRect, bool calculateAnchor) {
        Rectangle bounds = getMeasuredBounds();
        
        if (calculateAnchor) {
            if (m_anchorOffsetX != 0.0) {
                bounds.setX(bounds.getX() - m_anchorOffsetX);
            }
            if (m_anchorOffsetY != 0.0) {
                bounds.setY(bounds.getY() - m_anchorOffsetY);
            }
        }
        
        if (resultRect) {
            resultRect->copyFrom(bounds);
            return *resultRect;
        }
        
        return bounds;
    }

    Rectangle DisplayObject::getMeasuredBounds() {
        // 默认返回空矩形，子类应该重写这个方法
        return Rectangle(0, 0, 0, 0);
    }
    
    // ========== 智能指针版本的边界计算实现 ==========
    
    std::shared_ptr<Rectangle> DisplayObject::getBounds() const {
        // 默认返回空矩形，子类应该重写这个方法
        return Rectangle::create(0, 0, 0, 0);
    }
    
    std::shared_ptr<Rectangle> DisplayObject::getMeasuredBounds() const {
        // 默认返回空矩形，子类应该重写这个方法
        return Rectangle::create(0, 0, 0, 0);
    }
    
    // ========== 渲染相关方法实现 ==========
    
    std::shared_ptr<sys::RenderNode> DisplayObject::getRenderNode() const {
        if (m_displayList) {
            return m_displayList->getRenderNode();
        }
        return nullptr;
    }
    
    void DisplayObject::setRenderNode(std::shared_ptr<sys::RenderNode> node) {
        if (!m_displayList) {
            m_displayList = std::make_shared<sys::DisplayList>();
        }
        m_displayList->setRenderNode(node);
    }
    
    void DisplayObject::setRenderDirty(bool dirty) {
        if (m_displayList) {
            m_displayList->setDirty(dirty);
        }
    }
    
    void DisplayObject::setMeasuredSize(double width, double height) {
        // 默认实现为空，子类可以重写以实现具体的测量逻辑
        // 这个方法通常在布局系统中使用
    }

    // ========== 受保护的辅助方法实现 ==========

    void DisplayObject::updateUseTransform() {
        if (m_scaleX == 1.0 && m_scaleY == 1.0 && m_skewX == 0.0 && m_skewY == 0.0) {
            m_useTranslate = false;
        } else {
            m_useTranslate = true;
        }
    }

    void DisplayObject::cacheDirtyUp() {
        DisplayObject* current = m_parent;
        while (current && !current->getCacheDirty()) {
            current->setCacheDirty(true);
            current = current->getParent();
        }
    }

    // ========== 私有辅助方法实现 ==========

    double DisplayObject::clampRotation(double value) {
        value = fmod(value, 360.0);
        if (value > 180.0) {
            value -= 360.0;
        } else if (value < -180.0) {
            value += 360.0;
        }
        return value;
    }

    void DisplayObject::markMatrixDirty() {
        m_matrixDirty = true;
        
        // 清除缓存的连接矩阵
        if (m_concatenatedMatrix) {
            m_concatenatedMatrix.reset();
        }
        if (m_invertedConcatenatedMatrix) {
            m_invertedConcatenatedMatrix.reset();
        }
    }

    void DisplayObject::onPropertyChanged() {
        // 触发父级缓存更新
        if (m_parent && !m_parent->getCacheDirty()) {
            m_parent->setCacheDirty(true);
            m_parent->cacheDirtyUp();
        }
        
        // 触发遮罩对象缓存更新
        if (m_maskedObject && !m_maskedObject->getCacheDirty()) {
            m_maskedObject->setCacheDirty(true);
            m_maskedObject->cacheDirtyUp();
        }
        
        setCacheDirty(true);
    }

} // namespace egret