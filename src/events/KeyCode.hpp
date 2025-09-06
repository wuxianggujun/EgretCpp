#pragma once

#include <SDL3/SDL.h>

namespace egret {
namespace KeyCode {
    // 常用按键（与TS端 egret.Keyboard 类似的语义常量）
    // 字母
    constexpr int A = SDLK_A; constexpr int B = SDLK_B; constexpr int C = SDLK_C; constexpr int D = SDLK_D;
    constexpr int E = SDLK_E; constexpr int F = SDLK_F; constexpr int G = SDLK_G; constexpr int H = SDLK_H;
    constexpr int I = SDLK_I; constexpr int J = SDLK_J; constexpr int K = SDLK_K; constexpr int L = SDLK_L;
    constexpr int M = SDLK_M; constexpr int N = SDLK_N; constexpr int O = SDLK_O; constexpr int P = SDLK_P;
    constexpr int Q = SDLK_Q; constexpr int R = SDLK_R; constexpr int S = SDLK_S; constexpr int T = SDLK_T;
    constexpr int U = SDLK_U; constexpr int V = SDLK_V; constexpr int W = SDLK_W; constexpr int X = SDLK_X;
    constexpr int Y = SDLK_Y; constexpr int Z = SDLK_Z;

    // 数字（主键盘）
    constexpr int NUM_0 = SDLK_0; constexpr int NUM_1 = SDLK_1; constexpr int NUM_2 = SDLK_2; constexpr int NUM_3 = SDLK_3;
    constexpr int NUM_4 = SDLK_4; constexpr int NUM_5 = SDLK_5; constexpr int NUM_6 = SDLK_6; constexpr int NUM_7 = SDLK_7;
    constexpr int NUM_8 = SDLK_8; constexpr int NUM_9 = SDLK_9;

    // 方向键
    constexpr int LEFT = SDLK_LEFT; constexpr int RIGHT = SDLK_RIGHT;
    constexpr int UP = SDLK_UP;     constexpr int DOWN  = SDLK_DOWN;

    // 控制键
    constexpr int SPACE = SDLK_SPACE;
    constexpr int ENTER = SDLK_RETURN;
    constexpr int ESCAPE = SDLK_ESCAPE;
    constexpr int TAB = SDLK_TAB;
    constexpr int BACKSPACE = SDLK_BACKSPACE;
    constexpr int SHIFT = SDLK_LSHIFT; // 注意：左右Shift区分，按需判断RSHIFT
    constexpr int CTRL  = SDLK_LCTRL;  // 同理，区分RCTRL
    constexpr int ALT   = SDLK_LALT;   // 同理，区分RALT

    // 小键盘（示意）
    constexpr int KP_0 = SDLK_KP_0; constexpr int KP_1 = SDLK_KP_1; constexpr int KP_2 = SDLK_KP_2;
    constexpr int KP_3 = SDLK_KP_3; constexpr int KP_4 = SDLK_KP_4; constexpr int KP_5 = SDLK_KP_5;
    constexpr int KP_6 = SDLK_KP_6; constexpr int KP_7 = SDLK_KP_7; constexpr int KP_8 = SDLK_KP_8;
    constexpr int KP_9 = SDLK_KP_9;
}
} // namespace egret

