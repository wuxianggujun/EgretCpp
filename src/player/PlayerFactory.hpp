#pragma once

#include "player/Player.hpp"
#include "player/PlayerOption.hpp"
#include "player/SimpleFPSDisplay.hpp"
#include "display/Stage.hpp"
#include "player/RenderBuffer.hpp"
#include <memory>
#include <functional>

namespace egret {
namespace sys {

    /**
     * 播放器工厂类
     * 简化Player的创建和配置过程
     */
    class PlayerFactory {
    public:
        /**
         * 创建播放器实例
         * @param buffer 渲染缓冲区
         * @param option 播放器选项
         * @return 播放器实例
         */
        static std::shared_ptr<Player> createPlayer(
            std::shared_ptr<RenderBuffer> buffer, 
            const PlayerOption& option = PlayerOption());
        
        /**
         * 创建带自定义舞台的播放器
         * @param buffer 渲染缓冲区
         * @param stage 自定义舞台
         * @param option 播放器选项
         * @return 播放器实例
         */
        static std::shared_ptr<Player> createPlayerWithStage(
            std::shared_ptr<RenderBuffer> buffer,
            std::shared_ptr<Stage> stage,
            const PlayerOption& option = PlayerOption());
        
        /**
         * 注册入口类工厂函数
         * @param player 播放器实例
         * @param factory 工厂函数
         */
        static void registerEntryClassFactory(
            std::shared_ptr<Player> player,
            std::function<std::shared_ptr<DisplayObject>()> factory);
        
    private:
        /**
         * 配置播放器选项
         */
        static void configurePlayer(std::shared_ptr<Player> player, const PlayerOption& option);
        
        /**
         * 创建默认舞台
         */
        static std::shared_ptr<Stage> createDefaultStage(const PlayerOption& option);
    };

} // namespace sys
} // namespace egret