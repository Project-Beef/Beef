#ifndef __CONSTANTS__CONFIG__HH__
#define __CONSTANTS__CONFIG__HH__

#include <stdint.h>

#include <string_view>

#include <fmt/core.h>
#include <enet/enet.h>


namespace beef::config
{
    constexpr std::string_view address = "127.0.0.1";

    namespace enet 
    {
        // Which address to listen in
        constexpr uint32_t server_address = ENET_HOST_ANY;

        // Which port to listen for connections in
        constexpr uint32_t server_port = 17095;

        // How many peers enet allocates data for
        constexpr uint32_t max_peers = 4095;

        // How many peers are really allowed to connect
        constexpr uint32_t real_peers = 4090;
    }


    namespace cdn
    {
        // What IP address to use for cdn
        constexpr std::string_view server_address = address;

        // What path to use for cdn
        constexpr std::string_view server_path = "";

        namespace ubisoft
        {
            // What is used as a MITM proxy for getting items not existing in cache
            constexpr std::string_view server_address = "ubistatic-a.akamaihd.net";

            // What is used as MITM proxy's path
            constexpr std::string_view server_path = "0098/45484/cache";
        }
    }

    namespace variant
    {
        namespace onsupermain
        {
            // Param 4 and 5 of OnSuperMainStartAttemptXXXXX variant
            constexpr std::string_view param4 = "cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster";
            constexpr std::string_view param5 = "proto=129|choosemusic=audio/mp3/titled.mp3|active_holiday=0|wing_week_day=0|server_tick=8184975|clash_active=1|drop_lavacheck_faster=1|isPayingUser=1|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|";
        }
    }

    namespace items
    {
        // Relative path from executable
        constexpr std::string_view path = "data/items.dat";
    }

    namespace db
    {
        constexpr uint32_t version = 2;
    }

    namespace settings 
    {
        constexpr bool beta = true;
        constexpr std::string_view version = "0.2.2";
        
        namespace fs
        {
            constexpr std::string_view data_root = "data/";
            constexpr std::string_view server_data_file = "data/srv_data.bin";
        }

        constexpr std::string_view admin_name = "moobpoop44";
        constexpr std::string_view admin_pass = "p00p@h0m35555";
    }
}

#endif // __CONSTANTS__CONFIG__HH__
