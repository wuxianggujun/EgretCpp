#include "player/Player.hpp"
#include "player/SystemTicker.hpp"
#include "player/SkiaRenderBuffer.hpp"
#include "display/Stage.hpp"
#include "display/DisplayList.hpp"
#include "events/Event.hpp"
#include <iostream>
#include <chrono>
#include <thread>

namespace egret {
namespace sys {

    // SDL窗口构造函数
    Player::Player(int width, int height, const std::string& title, const std::string& entryClassName)
        : m_entryClassName(entryClassName)
        , m_isPlaying(false)
        , m_showFPS(false)
        , m_showLog(false)
        , m_lastDrawCalls(0)
        , m_lastRenderTime(0)
        , m_ownWindow(true) {
        
        // 初始化SDL（如果还未初始化）
        static bool sdlInitialized = false;
        if (!sdlInitialized) {
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
                throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
            }
            sdlInitialized = true;
            std::cout << "SDL initialized by Player" << std::endl;
        }
        
        // 创建SDL窗口
        m_sdlWindow = std::make_shared<platform::SDLWindow>(width, height, title);
        if (!m_sdlWindow->isValid()) {
            throw std::runtime_error("Failed to create SDL window");
        }
        
        // 创建舞台
        m_stage = std::make_shared<Stage>();
        m_stage->setStageWidth(width);
        m_stage->setStageHeight(height);
        
        // 创建渲染缓冲区
        auto renderBuffer = createSkiaRenderBuffer(width, height);
        
        // 创建屏幕显示列表
        m_screenDisplayList = createDisplayList(m_stage, renderBuffer);
        
        // 创建事件转换器
        m_eventConverter = std::make_shared<platform::SDLEventConverter>(m_stage);
        
        std::cout << "Player created with SDL window: " << width << "x" << height << " - " << title << std::endl;
    }

    // 传统构造函数（兼容现有代码）
    Player::Player(std::shared_ptr<RenderBuffer> buffer, std::shared_ptr<Stage> stage, const std::string& entryClassName)
        : m_entryClassName(entryClassName)
        , m_stage(stage)
        , m_isPlaying(false)
        , m_showFPS(false)
        , m_showLog(false)
        , m_lastDrawCalls(0)
        , m_lastRenderTime(0)
        , m_ownWindow(false) {  // 不拥有窗口
        
        if (!buffer) {
            throw std::invalid_argument("RenderBuffer cannot be null");
        }
        
        // 创建屏幕显示列表
        m_screenDisplayList = createDisplayList(stage, buffer);
    }
    
    Player::~Player() {
        stop();
    }
    
    std::shared_ptr<DisplayList> Player::createDisplayList(std::shared_ptr<Stage> stage, std::shared_ptr<RenderBuffer> buffer) {
        auto displayList = std::make_shared<DisplayList>();
        displayList->setRenderBuffer(buffer);
        stage->setDisplayList(displayList);
        return displayList;
    }
    
    void Player::start() {
        if (m_isPlaying || !m_stage) {
            return;
        }
        
        m_isPlaying = true;
        
        if (!m_root) {
            initialize();
        }
        
        // 添加到ticker系统（对应TypeScript的ticker.$addPlayer(this)）
        auto sharedThis = std::enable_shared_from_this<Player>::shared_from_this();
        getTicker().addPlayer(sharedThis);
        
        std::cout << "Player started with entry class: " << m_entryClassName << std::endl;
    }
    
    void Player::initialize() {
        // 如果有入口类工厂函数，使用它创建根对象
        if (m_entryClassFactory) {
            m_root = m_entryClassFactory();
            if (m_root) {
                m_stage->addChild(m_root.get());
                std::cout << "Entry class instance created and added to stage" << std::endl;
            } else {
                std::cerr << "Failed to create entry class instance" << std::endl;
            }
        } else {
            std::cout << "No entry class factory provided" << std::endl;
        }
    }
    
    void Player::stop() {
        pause();
        m_stage = nullptr;
        m_root = nullptr;
    }
    
    void Player::pause() {
        if (!m_isPlaying) {
            return;
        }
        
        m_isPlaying = false;
        
        // 从ticker系统移除（对应TypeScript的ticker.$removePlayer(this)）
        auto sharedThis = std::enable_shared_from_this<Player>::shared_from_this();
        getTicker().removePlayer(sharedThis);
        
        std::cout << "Player paused" << std::endl;
    }
    
    void Player::render(bool triggerByFrame, int costTicker) {
        if (!m_stage) {
            return;
        }
        
        // 记录渲染开始时间
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // TODO: 实现原生渲染支持
        // if (egret.nativeRender) {
        //     egret_native.updateNativeRender();
        //     egret_native.nrRender();
        //     return;
        // }
        
        // 清空渲染器（如果需要）
        // TODO: 实现systemRenderer
        // if (egret.sys.systemRenderer.renderClear) {
        //     egret.sys.systemRenderer.renderClear();
        // }
        
        // 执行显示列表绘制
        int drawCalls = m_stage->getDisplayList()->drawToSurface();
        
        // 记录渲染结束时间
        auto endTime = std::chrono::high_resolution_clock::now();
        auto costRender = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        // 更新FPS显示
        if (triggerByFrame && m_showFPS && m_fpsDisplay) {
            m_fpsDisplay->update(60, drawCalls, static_cast<int>(costRender), costTicker); // TODO: 计算实际FPS
        }
        
        m_lastDrawCalls = drawCalls;
        m_lastRenderTime = costRender;
    }
    
    void Player::updateStageSize(int stageWidth, int stageHeight) {
        if (!m_stage) {
            return;
        }
        
        // 更新舞台尺寸
        m_stage->setStageWidth(stageWidth);
        m_stage->setStageHeight(stageHeight);
        
        // TODO: 实现原生渲染支持
        // if (egret.nativeRender) {
        //     egret_native.nrResize(stageWidth, stageHeight);
        // } else {
            m_screenDisplayList->setClipRect(stageWidth, stageHeight);
        // }
        
        // 派发resize事件
        auto resizeEvent = std::make_shared<Event>(Event::RESIZE);
        m_stage->dispatchEvent(*resizeEvent);
        
        std::cout << "Stage size updated to: " << stageWidth << "x" << stageHeight << std::endl;
    }
    
    void Player::handleSDLEvent(const SDL_Event& sdlEvent) {
        if (m_eventConverter) {
            m_eventConverter->handleSDLEvent(sdlEvent);
        }
    }
    
    int Player::runMainLoop() {
        if (!m_sdlWindow || !m_ownWindow) {
            std::cerr << "Player does not own an SDL window, cannot run main loop" << std::endl;
            return -1;
        }
        
        // 启动播放器
        start();
        
        std::cout << "Starting Player main loop..." << std::endl;
        
        auto& ticker = getTicker();
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!m_sdlWindow->shouldClose()) {
            // 处理SDL事件
            while (m_sdlWindow->pollEvents()) {
                const SDL_Event& sdlEvent = m_sdlWindow->getCurrentEvent();
                
                // 处理窗口关闭事件
                if (sdlEvent.type == SDL_EVENT_QUIT) {
                    m_sdlWindow->setShouldClose(true);
                    continue;
                }
                
                // 处理其他事件
                handleSDLEvent(sdlEvent);
            }
            
            // 更新SystemTicker
            ticker.update();
            
            // 清空并呈现SDL窗口
            m_sdlWindow->clear();
            // TODO: 在这里可以将Skia的渲染结果复制到SDL纹理
            m_sdlWindow->present();
            
            // 控制帧率 (60fps = ~16.67ms per frame)
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
            
            if (deltaTime.count() < 16) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16 - deltaTime.count()));
            }
            
            lastTime = std::chrono::high_resolution_clock::now();
        }
        
        // 停止播放器
        stop();
        
        std::cout << "Player main loop ended" << std::endl;
        return 0;
    }
    
    void Player::displayFPS(bool showFPS, bool showLog, const std::string& logFilter) {
        m_showFPS = showFPS;
        m_showLog = showLog;
        
        if (showFPS || showLog) {
            // TODO: 创建FPS显示对象
            // if (!m_fpsDisplay) {
            //     m_fpsDisplay = std::make_shared<FPSDisplayImpl>(m_stage, showFPS, showLog, logFilter);
            // }
            
            std::cout << "FPS display enabled - FPS: " << (showFPS ? "true" : "false") 
                      << ", Log: " << (showLog ? "true" : "false") << std::endl;
        }
        
        // 设置日志回调
        if (showLog) {
            // TODO: 实现日志系统重定向
            std::cout << "Log display enabled with filter: " << logFilter << std::endl;
        }
    }

} // namespace sys
} // namespace egret