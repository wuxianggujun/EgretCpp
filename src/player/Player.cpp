#include "player/Player.hpp"
#include "player/SystemTicker.hpp"
#include "player/SkiaRenderBuffer.hpp"
#include "player/SystemRenderer.hpp"
#include "display/Stage.hpp"
#include "display/DisplayList.hpp"
#include "events/Event.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

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
            EGRET_INFO("开始初始化SDL...");
            
            // 初始化全局渲染器
            sys::initializeRenderers();
            EGRET_INFO("Skia渲染器初始化完成");
            
            // 清除之前的SDL错误状态
            SDL_ClearError();
            
            // 首先尝试初始化视频子系统
            EGRET_DEBUG("调用SDL_Init(SDL_INIT_VIDEO)...");
            bool result = SDL_Init(SDL_INIT_VIDEO);
            EGRET_DEBUGF("SDL_Init返回值: {}", result ? "true (success)" : "false (failed)");
            
            if (!result) {
                const char* sdlError = SDL_GetError();
                std::string errorDetails = sdlError ? std::string(sdlError) : "Unknown SDL error";
                
                // 尝试获取更详细的错误信息
                if (errorDetails.empty() || errorDetails == "Unknown SDL error") {
                    errorDetails = "SDL initialization failed - possible causes: missing graphics drivers, insufficient permissions, or corrupted SDL installation";
                }
                
                std::string errorMsg = std::string("Failed to initialize SDL VIDEO: ") + errorDetails;
                EGRET_ERRORF("SDL视频子系统初始化失败: 返回码={}, 错误={}", result ? "success" : "failed", errorDetails);
                
                // 尝试不同的初始化策略 - SDL3中使用多个标志位组合
                EGRET_INFO("尝试初始化基本SDL子系统...");
                SDL_ClearError();
                // SDL3中需要显式指定各个子系统
                Uint32 initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO;
                bool fallbackResult = SDL_Init(initFlags);
                if (!fallbackResult) {
                    const char* fallbackError = SDL_GetError();
                    EGRET_ERRORF("基本SDL初始化也失败: 返回码={}, 错误={}", fallbackResult ? "success" : "failed", fallbackError ? fallbackError : "Unknown error");
                    throw std::runtime_error(errorMsg);
                } else {
                    EGRET_INFO("SDL基本子系统初始化成功（作为后备方案）");
                }
            } else {
                EGRET_INFO("SDL视频子系统初始化成功");
            }
            
            // 然后尝试初始化事件子系统
            if (!SDL_InitSubSystem(SDL_INIT_EVENTS)) {
                std::string errorMsg = std::string("Failed to initialize SDL EVENTS: ") + SDL_GetError();
                EGRET_WARN("SDL事件子系统初始化失败: " + errorMsg);
                // 事件子系统失败不算严重错误，继续运行
            } else {
                EGRET_INFO("SDL事件子系统初始化成功");
            }
            
            // 最后尝试初始化音频子系统（可选）
            if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
                std::string errorMsg = std::string("Failed to initialize SDL AUDIO: ") + SDL_GetError();
                EGRET_WARN("SDL音频子系统初始化失败（可忽略）: " + errorMsg);
                // 音频子系统失败不算严重错误，继续运行
            } else {
                EGRET_INFO("SDL音频子系统初始化成功");
            }
            
            sdlInitialized = true;
            EGRET_INFO("SDL初始化完成");
        } else {
            EGRET_DEBUG("SDL已经初始化过了");
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
        
        EGRET_INFOF("SDL窗口已创建: {}x{} - {}", width, height, title);
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
        // 释放窗口呈现纹理
        if (m_presentTexture) {
            SDL_DestroyTexture(m_presentTexture);
            m_presentTexture = nullptr;
            m_presentTexW = 0;
            m_presentTexH = 0;
        }
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
        
        EGRET_INFOF("Player started with entry class: {}", m_entryClassName);
    }
    
    void Player::initialize() {
        // 如果有入口类工厂函数，使用它创建根对象
        if (m_entryClassFactory) {
            m_root = m_entryClassFactory();
            if (m_root) {
                m_stage->addChild(m_root.get());
                EGRET_INFO("Entry class instance created and added to stage");
            } else {
                EGRET_ERROR("Failed to create entry class instance");
            }
        } else {
            EGRET_DEBUG("No entry class factory provided");
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
        
        EGRET_INFO("Player paused");
    }
    
    void Player::render(bool triggerByFrame, int costTicker) {
        if (!m_stage) {
            return;
        }
        
        EGRET_DEBUGF("start: triggerByFrame={}, costTicker={}", triggerByFrame, costTicker);
        
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
        
        EGRET_DEBUG("buildRenderContent");
        // 首先构建渲染内容，确保DisplayList的root被正确设置
        m_stage->buildRenderContent();
        
        EGRET_DEBUG("drawToSurface");
        // 执行显示列表绘制
        int drawCalls = m_stage->getDisplayList()->drawToSurface();
        
        EGRET_DEBUGF("drawCalls={}", drawCalls);
        
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
        
        EGRET_INFOF("Stage size updated to: {}x{}", stageWidth, stageHeight);
    }
    
    void Player::handleSDLEvent(const SDL_Event& sdlEvent) {
        if (m_eventConverter) {
            m_eventConverter->handleSDLEvent(sdlEvent);
        }
    }
    
    int Player::runMainLoop() {
        if (!m_sdlWindow || !m_ownWindow) {
            EGRET_ERROR("不拥有SDL窗口，无法运行主循环");
            return -1;
        }
        
        // 启动播放器
        start();
        
        EGRET_INFO("Starting Player main loop...");
        
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

            // 将Skia的渲染结果复制到SDL窗口
            if (m_screenDisplayList && m_screenDisplayList->getRenderBuffer()) {
                auto renderBuffer = m_screenDisplayList->getRenderBuffer();

                // 一次性保存PNG以便调试
                static bool savedOnce = false;
                auto skiaBuffer = std::dynamic_pointer_cast<SkiaRenderBuffer>(renderBuffer);
                if (!savedOnce && skiaBuffer) {
                    skiaBuffer->saveToPNG("debug_render.png");
                    EGRET_INFO("Saved render buffer to debug_render.png");
                    savedOnce = true;
                }

                // 将CPU栅格的Skia Surface读成RGBA像素，并更新到SDL纹理
                if (skiaBuffer && skiaBuffer->isValid() && m_sdlWindow && m_sdlWindow->getRenderer()) {
                    int texW = static_cast<int>(renderBuffer->getWidth());
                    int texH = static_cast<int>(renderBuffer->getHeight());
                    if (texW > 0 && texH > 0) {
                        // 尺寸变化时重建纹理
                        if (!m_presentTexture || m_presentTexW != texW || m_presentTexH != texH) {
                            if (m_presentTexture) {
                                SDL_DestroyTexture(m_presentTexture);
                                m_presentTexture = nullptr;
                            }
                            m_presentTexture = SDL_CreateTexture(m_sdlWindow->getRenderer(),
                                                                 SDL_PIXELFORMAT_RGBA8888,
                                                                 SDL_TEXTUREACCESS_STREAMING,
                                                                 texW, texH);
                            m_presentTexW = texW;
                            m_presentTexH = texH;
                        }

                        if (m_presentTexture) {
                            // 从Skia Surface读取为RGBA8888（预乘Alpha）
                            std::vector<uint8_t> pixels(static_cast<size_t>(texW) * static_cast<size_t>(texH) * 4);
                            SkImageInfo info = SkImageInfo::Make(texW, texH, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
                            SkSurface* surface = skiaBuffer->getSkSurface();
                            if (surface && surface->readPixels(info, pixels.data(), texW * 4, 0, 0)) {
                                // 更新SDL纹理并绘制到窗口
                                SDL_UpdateTexture(m_presentTexture, nullptr, pixels.data(), texW * 4);
                                // SDL3渲染：渲染整张纹理到目标
                                SDL_FRect dstRect{0.0f, 0.0f, static_cast<float>(texW), static_cast<float>(texH)};
                                SDL_RenderTexture(m_sdlWindow->getRenderer(), m_presentTexture, nullptr, &dstRect);
                            }
                        }
                    }
                }
            }

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
        
        EGRET_INFO("Player main loop ended");
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
            
            EGRET_INFOF("显示FPS={} 显示Log={}", showFPS ? "true" : "false", showLog ? "true" : "false");
        }
        
        // 设置日志回调
        if (showLog) {
            // TODO: 实现日志系统重定向
            EGRET_INFOF("日志过滤器: {}", logFilter);
        }
    }

} // namespace sys
} // namespace egret
