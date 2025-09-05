#include "TouchEvent.hpp"
#include "../display/DisplayObject.hpp"
#include "IEventDispatcher.hpp"

namespace egret {

    const std::string TouchEvent::TOUCH_MOVE = "touchMove";
    const std::string TouchEvent::TOUCH_BEGIN = "touchBegin";
    const std::string TouchEvent::TOUCH_END = "touchEnd";
    const std::string TouchEvent::TOUCH_CANCEL = "touchCancel";
    const std::string TouchEvent::TOUCH_TAP = "touchTap";
    const std::string TouchEvent::TOUCH_RELEASE_OUTSIDE = "touchReleaseOutside";

    Point TouchEvent::localPoint_;

    TouchEvent::TouchEvent(const std::string& type, bool bubbles, bool cancelable, 
                          double stageX, double stageY, int touchPointID)
        : Event(type, bubbles, cancelable)
        , stageX_(0.0)
        , stageY_(0.0)
        , localX_(0.0)
        , localY_(0.0)
        , touchPointID_(0)
        , touchDown_(false)
        , targetChanged_(true) {
        initTo(stageX, stageY, touchPointID);
    }

    void TouchEvent::initTo(double stageX, double stageY, int touchPointID) {
        touchPointID_ = touchPointID;
        stageX_ = stageX;
        stageY_ = stageY;
    }

    double TouchEvent::getStageX() const {
        return stageX_;
    }

    double TouchEvent::getStageY() const {
        return stageY_;
    }

    double TouchEvent::getLocalX() {
        if (targetChanged_) {
            getLocalXY();
        }
        return localX_;
    }

    double TouchEvent::getLocalY() {
        if (targetChanged_) {
            getLocalXY();
        }
        return localY_;
    }

    int TouchEvent::getTouchPointID() const {
        return touchPointID_;
    }

    void TouchEvent::setTouchPointID(int value) {
        touchPointID_ = value;
    }

    bool TouchEvent::isTouchDown() const {
        return touchDown_;
    }

    void TouchEvent::setTouchDown(bool value) {
        touchDown_ = value;
    }

    void TouchEvent::updateAfterEvent() {
        // TODO: Implement rendering flag
        // sys.$requestRenderingFlag = true;
    }

    bool TouchEvent::setTarget(const std::any& target) {
        Event::setTarget(target);
        targetChanged_ = true;
        return true;
    }

    void TouchEvent::getLocalXY() {
        targetChanged_ = false;
        try {
            if (auto displayObjectPtr = std::any_cast<DisplayObject*>(getTarget())) {
                // TODO: Implement getInvertedConcatenatedMatrix and transformPoint
                // Matrix m = displayObjectPtr->getInvertedConcatenatedMatrix();
                // m.transformPoint(stageX_, stageY_, localPoint_);
                // localX_ = localPoint_.getX();
                // localY_ = localPoint_.getY();
                
                // Temporary implementation
                localX_ = stageX_;
                localY_ = stageY_;
            }
        } catch (const std::bad_any_cast& e) {
            // Handle case where target is not a DisplayObject*
            localX_ = stageX_;
            localY_ = stageY_;
        }
    }

    bool TouchEvent::dispatchTouchEvent(IEventDispatcher* target, const std::string& type,
                                       bool bubbles, bool cancelable,
                                       double stageX, double stageY,
                                       int touchPointID, bool touchDown) {
        if (!bubbles && !target->hasEventListener(type)) {
            return true;
        }
        
        // TODO: Implement Event.create() and Event.release() for object pooling
        TouchEvent event(type, bubbles, cancelable);
        event.initTo(stageX, stageY, touchPointID);
        event.setTouchDown(touchDown);
        bool result = target->dispatchEvent(event);
        return result;
    }

}