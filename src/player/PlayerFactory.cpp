#include "player/PlayerFactory.hpp"
#include "display/Stage.hpp"
#include <iostream>
#include "utils/Logger.hpp"

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
        
        EGRET_INFO("已注册入口类工厂");
    }
    
    void PlayerFactory::configurePlayer(std::shared_ptr<Player> player, const PlayerOption& option) {
        // 配置FPS和日志显示
        if (option.showFPS || option.showLog) {
            player->displayFPS(option.showFPS, option.showLog, option.logFilter);
        }
        
        // 更新舞台配置（语义对齐Egret）
        if (auto stage = player->getStage()) {
            // 设置缩放与方向（会触发Screen.updateScreenSize）
            stage->setScaleMode(option.scaleMode);
            stage->setOrientation(option.orientation);
            // 纹理缩放、触摸、帧率
            stage->setTextureScaleFactor(option.textureScaleFactor);
            stage->setMaxTouches(option.maxTouches);
            stage->setFrameRate(option.frameRate);
        }

        // 初始化舞台内容尺寸
        player->updateStageSize(option.contentWidth, option.contentHeight);
        
        EGRET_INFOF("Player配置: entryClass={}, size={}x{}, fps={}, scaleMode={}, showFPS={}, showLog={}",
                     option.entryClassName, option.contentWidth, option.contentHeight,
                     option.frameRate, option.scaleMode,
                     option.showFPS ? "true" : "false",
                     option.showLog ? "true" : "false");
    }
    
    std::shared_ptr<Stage> PlayerFactory::createDefaultStage(const PlayerOption& option) {
        auto stage = std::make_shared<Stage>();
        
        // 设置舞台基本属性
        stage->setStageWidth(option.contentWidth);
        stage->setStageHeight(option.contentHeight);
        
        // 设置其他舞台属性（保持与TS语义对齐）
        stage->setScaleMode(option.scaleMode);
        stage->setOrientation(option.orientation);
        stage->setMaxTouches(option.maxTouches);
        stage->setFrameRate(option.frameRate);
        stage->setTextureScaleFactor(option.textureScaleFactor);
        
        EGRET_INFOF("创建默认Stage: {}x{}", option.contentWidth, option.contentHeight);
        
        return stage;
    }

} // namespace sys
} // namespace egret
