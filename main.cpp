#include <iostream>
#include "core/SkCanvas.h"
#include "core/SkSurface.h"
#include "core/SkPaint.h"
#include "core/SkPath.h"
#include "encode/SkPngEncoder.h"
#include "core/SkStream.h"

// GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SDL3 headers
#include <SDL3/SDL.h>

// EgretCpp headers
#include "player/SkiaRenderBuffer.hpp"
#include "player/SystemRenderer.hpp"
#include "utils/Logger.hpp"

int main()
{
    std::cout << "Hello, World with Skia, GLM, and SDL3!" << std::endl;
    
    // ========== Logger功能测试 ==========
    std::cout << "\n=== Logger System Test ===" << std::endl;
    
    // 测试便利宏（自动包含文件名、行号、函数名）
    egret::Logger::setLogLevel(egret::Logger::DEBUG);
    EGRET_DEBUG("Debug message with location info");
    EGRET_INFO("Info message with location info");
    EGRET_WARN("Warning message with location info");
    EGRET_ERROR("Error message with location info");
    
    // 测试格式化日志宏
    int frameCount = 60;
    double deltaTime = 0.016;
    EGRET_INFOF("Frame {}: deltaTime = {:.3f}ms", frameCount, deltaTime * 1000);
    
    // 测试传统API（兼容TypeScript）
    std::cout << "\n--- Testing TypeScript compatible API ---" << std::endl;
    egret::Logger::debug("Traditional debug message");
    egret::Logger::info("Traditional info message");
    
    // 测试日志级别过滤
    std::cout << "\n--- Testing log level filtering ---" << std::endl;
    egret::Logger::setLogLevel(egret::Logger::WARN);
    EGRET_DEBUG("This debug message should NOT appear");
    EGRET_INFO("This info message should NOT appear");  
    EGRET_WARN("This warning message SHOULD appear");
    EGRET_ERROR("This error message SHOULD appear");
    
    std::cout << "\n=== Logger Test Complete ===" << std::endl;
    
    // Test SDL3 functionality
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL3 initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    
    int version = SDL_GetVersion();
    int major = (version >> 24) & 0xFF;
    int minor = (version >> 16) & 0xFF;
    int micro = version & 0xFFFF;
    
    std::cout << "SDL3 version: " << major << "." << minor << "." << micro << std::endl;
    
    // Test GLM functionality
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    
    std::cout << "GLM test - Position: (" 
              << position.x << ", " 
              << position.y << ", " 
              << position.z << ")" << std::endl;
    
    // Create a Skia surface
    SkImageInfo info = SkImageInfo::MakeN32Premul(256, 256);
    sk_sp<SkSurface> surface = SkSurfaces::Raster(info);
    
    if (!surface) {
        std::cout << "Failed to create Skia surface!" << std::endl;
        SDL_Quit();
        return -1;
    }
    
    SkCanvas* canvas = surface->getCanvas();
    
    // Clear the canvas with white
    canvas->clear(SK_ColorWHITE);
    
    // Draw a simple red circle
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setAntiAlias(true);
    
    canvas->drawCircle(128, 128, 50, paint);
    
    // Save as PNG
    sk_sp<SkImage> image = surface->makeImageSnapshot();
    sk_sp<SkData> pngData = SkPngEncoder::Encode(nullptr, image.get(), {});
    
    if (pngData) {
        SkFILEWStream stream("skia_test.png");
        stream.write(pngData->data(), pngData->size());
        std::cout << "Successfully created skia_test.png!" << std::endl;
    }
    
    // Cleanup SDL3
    SDL_Quit();
    
    return 0;
}
