#include "platform/sdl/SDLPlatform.hpp"
#include "player/PlayerFactory.hpp"
#include "player/SkiaRenderBuffer.hpp"
#include <iostream>
#include <chrono>
#include <thread>

namespace egret {
namespace platform {

    SDLPlatform::SDLPlatform() 
        : m_initialized(false)
        , m_running(false)
        , m_ticker(&sys::getTicker()) {
    }
    
    SDLPlatform::~SDLPlatform() {
        if (m_initialized) {
            // 停止播放器
            if (m_player) {
                m_player->stop();
                m_player = nullptr;
            }
            
            // 清理SDL
            SDL_Quit();
            std::cout << "SDL Platform destroyed" << std::endl;
        }
    }
    
    bool SDLPlatform::initialize(int width, int height, const std::string& title) {
        if (m_initialized) {
            std::cout << "SDL Platform already initialized" << std::endl;
            return true;
        }
        
        // 初始化SDL
        int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
        if (result < 0) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }
        
        try {
            // 创建SDL窗口
            m_window = std::make_shared<SDLWindow>(width, height, title);
            
            if (!m_window->isValid()) {
                std::cerr << "Failed to create SDL window" << std::endl;
                SDL_Quit();
                return false;
            }
            
            m_initialized = true;
            std::cout << "SDL Platform initialized successfully" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Exception during SDL initialization: " << e.what() << std::endl;
            SDL_Quit();
            return false;
        }
    }
    
    std::shared_ptr<sys::Player> SDLPlatform::createPlayer(const std::string& entryClassName) {
        if (!m_initialized) {
            std::cerr << "SDL Platform not initialized" << std::endl;
            return nullptr;
        }
        
        // 获取窗口尺寸
        int width, height;
        m_window->getSize(width, height);
        
        // 创建渲染缓冲区
        auto renderBuffer = sys::createSkiaRenderBuffer(width, height);
        
        // 创建播放器选项
        PlayerOption option(entryClassName, width, height, 60);
        option.showFPS = true;
        option.showLog = true;
        
        // 创建播放器
        m_player = sys::PlayerFactory::createPlayer(renderBuffer, option);
        
        // 创建事件转换器
        m_eventConverter = std::make_shared<SDLEventConverter>(m_player->getStage());
        
        std::cout << "Player created with entry class: " << entryClassName << std::endl;
        return m_player;
    }
    
    int SDLPlatform::runMainLoop() {
        if (!m_initialized) {
            std::cerr << "SDL Platform not initialized" << std::endl;
            return -1;
        }
        
        if (!m_player) {
            std::cerr << "No player created" << std::endl;
            return -1;
        }
        
        // 启动播放器
        m_player->start();
        
        m_running = true;
        std::cout << "Starting main loop..." << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (m_running && !m_window->shouldClose()) {
            // 处理事件
            processEvents();
            
            // 更新引擎
            updateEngine();
            
            // 渲染
            render();
            
            // 控制帧率 (60fps = ~16.67ms per frame)
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
            
            if (deltaTime.count() < 16) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16 - deltaTime.count()));
            }
            
            lastTime = std::chrono::high_resolution_clock::now();
        }
        
        std::cout << "Main loop ended" << std::endl;
        return 0;
    }
    
    void SDLPlatform::stop() {
        m_running = false;
    }
    
    void SDLPlatform::processEvents() {
        // 处理所有待处理的SDL事件
        while (m_window->pollEvents()) {
            const SDL_Event& sdlEvent = m_window->getCurrentEvent();
            
            // 检查退出事件
            if (sdlEvent.type == SDL_EVENT_QUIT) {
                m_running = false;
                continue;
            }
            
            // 将SDL事件转换为Egret事件
            if (m_eventConverter) {
                m_eventConverter->handleSDLEvent(sdlEvent);
            }
        }
    }
    
    void SDLPlatform::updateEngine() {
        // 更新SystemTicker，这会自动调用所有注册的Player的render方法
        if (m_ticker) {
            m_ticker->update();
        }
    }
    
    void SDLPlatform::render() {
        // 清空屏幕
        m_window->clear();
        
        // TODO: 在这里可以添加SDL特定的渲染逻辑
        // 比如将Skia的渲染结果复制到SDL纹理并显示
        
        // 呈现到屏幕
        m_window->present();
    }

} // namespace platform
} // namespace egret