#include "player/PlayerFactory.hpp"
#include "display/Stage.hpp"
#include <iostream>

namespace egret {
namespace sys {

    std::shared_ptr<Player> PlayerFactory::createPlayer(
        std::shared_ptr<RenderBuffer> buffer, 
        const PlayerOption& option) {
        
        // 创建默认舞台
        auto stage = createDefaultStage(option);
        
        // 创建播放器
        return createPlayerWithStage(buffer, stage, option);
    }
    
    std::shared_ptr<Player> PlayerFactory::createPlayerWithStage(
        std::shared_ptr<RenderBuffer> buffer,
        std::shared_ptr<Stage> stage,
        const PlayerOption& option) {
        
        if (!buffer) {
            throw std::invalid_argument("RenderBuffer cannot be null");
        }
        
        if (!stage) {
            throw std::invalid_argument("Stage cannot be null");
        }
        
        // 创建播放器实例
        auto player = std::make_shared<Player>(buffer, stage, option.entryClassName);
        
        // 配置播放器选项
        configurePlayer(player, option);
        
        return player;
    }
    
    void PlayerFactory::registerEntryClassFactory(
        std::shared_ptr<Player> player,
        std::function<std::shared_ptr<DisplayObject>()> factory) {
        
        if (!player) {
            throw std::invalid_argument("Player cannot be null");
        }
        
        if (!factory) {
            throw std::invalid_argument("Factory function cannot be null");
        }
        
        player->setEntryClassFactory(factory);
        
        std::cout << "Entry class factory registered for player" << std::endl;
    }
    
    void PlayerFactory::configurePlayer(std::shared_ptr<Player> player, const PlayerOption& option) {
        // 配置FPS和日志显示
        if (option.showFPS || option.showLog) {
            player->displayFPS(option.showFPS, option.showLog, option.logFilter);
        }
        
        // 更新舞台尺寸
        player->updateStageSize(option.contentWidth, option.contentHeight);
        
        std::cout << "Player configured:" << std::endl
                  << "  - Entry Class: " << option.entryClassName << std::endl
                  << "  - Content Size: " << option.contentWidth << "x" << option.contentHeight << std::endl
                  << "  - Frame Rate: " << option.frameRate << std::endl
                  << "  - Scale Mode: " << option.scaleMode << std::endl
                  << "  - Show FPS: " << (option.showFPS ? "true" : "false") << std::endl
                  << "  - Show Log: " << (option.showLog ? "true" : "false") << std::endl;
    }
    
    std::shared_ptr<Stage> PlayerFactory::createDefaultStage(const PlayerOption& option) {
        auto stage = std::make_shared<Stage>();
        
        // 设置舞台基本属性
        stage->setStageWidth(option.contentWidth);
        stage->setStageHeight(option.contentHeight);
        
        // TODO: 设置其他舞台属性
        // stage->setScaleMode(option.scaleMode);
        // stage->setOrientation(option.orientation);
        // stage->setMaxTouches(option.maxTouches);
        // stage->setFrameRate(option.frameRate);
        // stage->setTextureScaleFactor(option.textureScaleFactor);
        
        std::cout << "Default stage created with size: " 
                  << option.contentWidth << "x" << option.contentHeight << std::endl;
        
        return stage;
    }

} // namespace sys
} // namespace egret