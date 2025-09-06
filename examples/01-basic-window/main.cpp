#include "utils/Logger.hpp"
#include <memory>
#include <SDL3/SDL.h>
#include <core/SkCanvas.h>
#include <core/SkSurface.h>
#include <core/SkImageInfo.h>
#include <core/SkPaint.h>
#include <core/SkFont.h>
#include <core/SkColorSpace.h>
#include <gpu/ganesh/GrDirectContext.h>
#include <gpu/ganesh/GrBackendSurface.h>
#include <gpu/ganesh/gl/GrGLInterface.h>
#include <gpu/ganesh/gl/GrGLTypes.h>
#include <gpu/ganesh/gl/GrGLBackendSurface.h>
#include <gpu/ganesh/SkSurfaceGanesh.h>
#include <gpu/ganesh/gl/GrGLDirectContext.h>

// Include EgretCpp headers
#include "display/Stage.hpp"
#include "display/Shape.hpp"
#include "sys/Screen.hpp"
#include "player/Player.hpp"
#include "player/SystemTicker.hpp"

/**
 * Basic Window Example
 * 
 * This example demonstrates:
 * 1. SDL3 window creation and OpenGL context setup
 * 2. Skia rendering surface initialization
 * 3. EgretCpp Stage and Player initialization
 * 4. Basic rendering loop with event handling
 * 
 * Based on Egret TypeScript initialization flow:
 * runEgret() -> WebPlayer -> Stage -> Player -> render loop
 */
class BasicWindowExample {
private:
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    sk_sp<GrDirectContext> m_grContext;
    sk_sp<SkSurface> m_skSurface;
    
    std::shared_ptr<egret::Stage> m_stage;
    std::shared_ptr<egret::sys::Screen> m_screen;
    std::shared_ptr<egret::sys::Player> m_player;
    std::shared_ptr<egret::sys::SystemTicker> m_ticker;
    
    // Display objects for demonstration
    std::shared_ptr<egret::Shape> m_circle;
    std::shared_ptr<egret::Shape> m_rectangle;
    
    bool m_running;
    
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

public:
    BasicWindowExample() 
        : m_window(nullptr)
        , m_glContext(nullptr)
        , m_running(false) {
    }
    
    ~BasicWindowExample() {
        cleanup();
    }
    
    bool initialize() {
        // Initialize Egret engine components first (this will create SDL window)
        if (!initializeEgret()) {
            EGRET_ERROR("Failed to initialize Egret engine");
            return false;
        }
        
        // Create and add display objects to the stage
        if (!createDisplayObjects()) {
            EGRET_ERROR("Failed to create display objects");
            return false;
        }
        
        EGRET_INFO("EgretCpp Basic Window initialized successfully!");
        EGRET_INFOF("Window size: {}x{}", WINDOW_WIDTH, WINDOW_HEIGHT);
        EGRET_INFO("Press ESC to exit");
        
        return true;
    }
    
    void run() {
        EGRET_INFO("Starting main loop...");
        
        // Use Player's built-in main loop instead of our own
        // This avoids conflicts between different SDL event handling systems
        if (m_player) {
            m_player->runMainLoop();
        }
    }

private:
    bool initializeSkia() {
        // Create Skia OpenGL interface
        auto glInterface = GrGLMakeNativeInterface();
        if (!glInterface) {
            return false;
        }
        
        // Create Skia context - use new API
        m_grContext = GrDirectContexts::MakeGL(glInterface);
        if (!m_grContext) {
            return false;
        }
        
        // Create render target - use correct GL backend surface API
        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0; // Default framebuffer
        framebufferInfo.fFormat = 0x8058; // GL_RGBA8 constant value
        
        auto backendRT = GrBackendRenderTargets::MakeGL(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 8, framebufferInfo);
        
        // Create Skia surface - use correct API
        m_skSurface = SkSurfaces::WrapBackendRenderTarget(
            m_grContext.get(),
            backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr,
            nullptr
        );
        
        return m_skSurface != nullptr;
    }
    
    bool initializeEgret() {
        try {
            // 1. Create Screen (equivalent to WebPlayer screen management)
            m_screen = std::make_shared<egret::sys::Screen>();
            m_screen->setContentSize(WINDOW_WIDTH, WINDOW_HEIGHT);
            
            // 2. Create Player first so we use the same Stage as renderer
            // 注意：Player 内部会创建并持有自己的 Stage 与 DisplayList
            m_player = std::make_shared<egret::sys::Player>(
                WINDOW_WIDTH, WINDOW_HEIGHT, "EgretCpp - Basic Window Example", "");
            
            // 3. Get Stage from Player and configure it
            // 关键修复：后续所有显示对象都应当添加到 Player 的 Stage 上
            m_stage = m_player->getStage();
            if (!m_stage) {
                EGRET_ERROR("Player returned null Stage");
                return false;
            }
            m_stage->setScreen(m_screen.get());  // 绑定屏幕适配
            m_stage->setStageWidth(WINDOW_WIDTH);
            m_stage->setStageHeight(WINDOW_HEIGHT);
            m_stage->setFrameRate(60); // 60 FPS
            
            // 4. Create SystemTicker (equivalent to egret.ticker)
            // Note: SystemTicker doesn't have getTicker() method, create directly
            m_ticker = std::make_shared<egret::sys::SystemTicker>();
            m_ticker->setFrameRate(60);

            
            // 5. Start the ticker (check actual API)
            // Note: Need to check actual SystemTicker API
            
            EGRET_INFOF("Egret engine components initialized:");
            EGRET_INFOF("- Stage: {}x{} @ 60fps", WINDOW_WIDTH, WINDOW_HEIGHT);
            EGRET_INFO("- Screen adaptation enabled");
            EGRET_INFO("- SystemTicker created");
            
            return true;
        } catch (const std::exception& e) {
            EGRET_ERRORF("Exception during Egret initialization: {}", e.what());
            return false;
        }
    }
    
    bool createDisplayObjects() {
        try {
            // Create a circle shape
            m_circle = std::make_shared<egret::Shape>();
            auto circleGraphics = m_circle->getGraphics();
            
            // Draw a red circle
            circleGraphics->beginFill(0xFF0000, 1.0);  // Red color
            circleGraphics->drawCircle(0, 0, 50);  // Center at (0,0), radius 50
            circleGraphics->endFill();
            
            // Position the circle
            m_circle->setX(200);
            m_circle->setY(150);
            
            // Add circle to stage
            m_stage->addChild(m_circle.get());
            
            // Create a rectangle shape  
            m_rectangle = std::make_shared<egret::Shape>();
            auto rectGraphics = m_rectangle->getGraphics();
            
            // Draw a blue rectangle with border
            rectGraphics->lineStyle(3.0, 0x0000FF, 1.0);  // Blue border, 3px thick
            rectGraphics->beginFill(0x00FF00, 0.7);       // Green fill, 70% alpha
            rectGraphics->drawRect(0, 0, 120, 80);         // 120x80 rectangle
            rectGraphics->endFill();
            
            // Position the rectangle
            m_rectangle->setX(400);
            m_rectangle->setY(200);
            
            // Add rectangle to stage
            m_stage->addChild(m_rectangle.get());
            
            EGRET_INFO("Display objects created successfully:");
            EGRET_INFO("- Red circle at (200, 150)");
            EGRET_INFO("- Green rectangle with blue border at (400, 200)");
            
            return true;
        } catch (const std::exception& e) {
            EGRET_ERRORF("Exception during display objects creation: {}", e.what());
            return false;
        }
    }
    
    // Note: These methods are no longer needed since we use Player's main loop
    // Kept for reference/future customization
    /*
    void processEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_running = false;
                    break;
                    
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {  // Use event.key.key instead of keysym
                        m_running = false;
                    }
                    break;
                    
                case SDL_EVENT_WINDOW_RESIZED:
                    handleWindowResize(event.window.data1, event.window.data2);
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    void handleWindowResize(int width, int height) {
        // Update screen size
        if (m_screen) {
            m_screen->onWindowResize(width, height);  // Use onWindowResize instead
        }
        
        // Update stage size
        if (m_stage) {
            m_stage->setStageWidth(width);
            m_stage->setStageHeight(height);
        }
        
        // Recreate Skia surface with new size
        // Note: In a production app, you'd want to be more efficient about this
        createSkiaSurface(width, height);
        
        std::cout << "Window resized to: " << width << "x" << height << std::endl;
    }
    */
    
    // Note: These methods are also no longer needed with Player's main loop
    /*
    
    void createSkiaSurface(int width, int height) {
        if (!m_grContext) return;
        
        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0;
        framebufferInfo.fFormat = 0x8058; // GL_RGBA8 constant value
        
        auto backendRT = GrBackendRenderTargets::MakeGL(width, height, 0, 8, framebufferInfo);
        
        m_skSurface = SkSurfaces::WrapBackendRenderTarget(
            m_grContext.get(),
            backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr,
            nullptr
        );
    }
    
    void update() {
        // Update ticker (this drives the Egret engine)
        if (m_ticker) {
            m_ticker->update();
        }
    }
    
    void render() {
        if (!m_skSurface) return;
        
        auto canvas = m_skSurface->getCanvas();
        
        // Clear with background color
        canvas->clear(SK_ColorLTGRAY);
        
        // Render stage content (simplified - just skip this for now)
        // Note: Stage::drawToSurface API needs to be checked
        // if (m_stage) {
        //     m_stage->drawToSurface(m_skSurface);
        // }
        
        // Draw example content
        drawExampleContent(canvas);
        
        // Flush and present
        m_grContext->flush();
        SDL_GL_SwapWindow(m_window);
    }
    
    void drawExampleContent(SkCanvas* canvas) {
        // Draw a welcome message
        SkPaint textPaint;
        textPaint.setColor(SK_ColorBLACK);
        textPaint.setAntiAlias(true);
        
        SkFont titleFont;
        titleFont.setSize(24);
        
        // Draw title
        canvas->drawString("EgretCpp Basic Window Example", 50, 50, titleFont, textPaint);
        
        // Draw some info
        SkFont infoFont;
        infoFont.setSize(16);
        canvas->drawString("Stage initialized and running", 50, 100, infoFont, textPaint);
        canvas->drawString("SystemTicker active", 50, 120, infoFont, textPaint);
        canvas->drawString("Skia rendering enabled", 50, 140, infoFont, textPaint);
        canvas->drawString("Press ESC to exit", 50, 180, infoFont, textPaint);
        
        // Draw a simple animated circle
        static float animTime = 0;
        animTime += 0.02f;
        
        SkPaint circlePaint;
        circlePaint.setColor(SkColorSetARGB(255, 255, 100, 100));
        circlePaint.setAntiAlias(true);
        
        float centerX = WINDOW_WIDTH * 0.7f;
        float centerY = WINDOW_HEIGHT * 0.5f;
        float radius = 30 + sin(animTime) * 10;
        
        canvas->drawCircle(centerX, centerY, radius, circlePaint);
        
        // Draw stage info
        if (m_stage) {
            SkFont smallFont;
            smallFont.setSize(14);
            textPaint.setColor(SK_ColorDKGRAY);
            
            std::string stageInfo = "Stage: " + 
                std::to_string((int)m_stage->getStageWidth()) + "x" + 
                std::to_string((int)m_stage->getStageHeight()) + 
                " @ " + std::to_string((int)m_stage->getFrameRate()) + "fps";
                
            canvas->drawString(stageInfo.c_str(), 50, WINDOW_HEIGHT - 60, smallFont, textPaint);
        }
    }
    */
    
    void onTick(int64_t timeStamp) {
        // This is called by SystemTicker for each frame
        // Here you would normally update game logic, animations, etc.
        
        // For this basic example, we just let the normal render loop handle everything
        // In a more complex app, you might do:
        // - Update display objects
        // - Process events
        // - Update animations
        // - Trigger stage invalidation if needed
    }
    
    void cleanup() {
        // Note: SystemTicker::stopTick API needs to be checked
        // if (m_ticker) {
        //     m_ticker->stopTick();
        //     m_ticker.reset();
        // }
        
        m_player.reset();
        m_stage.reset();
        m_screen.reset();
        
        m_skSurface.reset();
        m_grContext.reset();
        
        if (m_glContext) {
            SDL_GL_DestroyContext(m_glContext);  // Updated SDL3 API
            m_glContext = nullptr;
        }
        
        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    EGRET_INFO("=== EgretCpp Basic Window Example ===");
    EGRET_INFO("Initializing engine components...");
    
    BasicWindowExample app;
    
    if (!app.initialize()) {
        EGRET_ERROR("Failed to initialize application");
        return 1;
    }
    
    EGRET_INFO("Starting main loop...");
    app.run();
    
    EGRET_INFO("Application finished successfully");
    return 0;
}
