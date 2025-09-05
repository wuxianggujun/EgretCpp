#pragma once

#include "Event.hpp"
#include "../geom/Point.hpp"

namespace egret {

    class DisplayObject;
    class IEventDispatcher;

    class TouchEvent : public Event {
    public:
        static const std::string TOUCH_MOVE;
        static const std::string TOUCH_BEGIN;
        static const std::string TOUCH_END;
        static const std::string TOUCH_CANCEL;
        static const std::string TOUCH_TAP;
        static const std::string TOUCH_RELEASE_OUTSIDE;

        TouchEvent(const std::string& type, bool bubbles = false, bool cancelable = false, 
                  double stageX = 0.0, double stageY = 0.0, int touchPointID = 0);

        void initTo(double stageX, double stageY, int touchPointID);

        double getStageX() const;
        double getStageY() const;

        double getLocalX();
        double getLocalY();

        int getTouchPointID() const;
        void setTouchPointID(int value);

        bool isTouchDown() const;
        void setTouchDown(bool value);

        void updateAfterEvent();

        bool setTarget(const std::any& target) override;

        static bool dispatchTouchEvent(IEventDispatcher* target, const std::string& type, 
                                     bool bubbles = false, bool cancelable = false,
                                     double stageX = 0.0, double stageY = 0.0, 
                                     int touchPointID = 0, bool touchDown = false);

    private:
        double stageX_;
        double stageY_;
        double localX_;
        double localY_;
        int touchPointID_;
        bool touchDown_;
        bool targetChanged_;

        void getLocalXY();
        
        static Point localPoint_;
    };

}