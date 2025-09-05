//
// Created by wuxianggujun on 25-9-4.
//

#pragma once

namespace egret
{
    enum class EventPhase : int
    {
        CAPTURING_PHASE = 1, // 捕获阶段
        AT_TARGET = 2, // 目标阶段
        BUBBLING_PHASE = 3 // 冒泡阶段
    };
}
