#include "dialog_return.hh"

#include "constants/config.hh"
#include "constants/roles.hh"
#include "database/database.hh"
#include "enet/enet.h"
#include "packet/gameupdatepacket.hh"
#include "utils/io.hh"
#include "utils/text.hh"
#include "packet/variant.hh"
#include "constants/dialogs.hh"

namespace beef::events::registered
{
    std::vector<std::string> cache;
    void dialog_return(event_context& ctx)
    {
        std::string dialog;
        if (ctx.m_scanner->try_get("dialog_name", dialog))
        {
            uint64_t hash = text::quick_hash(dialog);

            using namespace text;

            switch (hash)
            {
                case "drop_item"_qh:
                {
                    if (ctx.m_player->m_current_world == "EXIT") return;

                    int id, count;

                    if (!ctx.m_scanner->try_get("count", count) || !ctx.m_scanner->try_get("itemID", id))
                    {
                        ctx.m_player->send_log("`4Advanced Crash Protection:`` Invalid packet");
                        return;
                    }

                    world* world_;
                    if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
                    {
                        ctx.m_player->send_log("`4Invalid world``");
                        return;
                    }

                    if (!ctx.m_player->has_inventory_item(id))
                    {
                        ctx.m_player->send_log("You don't have this item!");
                        return;
                    }
                    else if (ctx.m_player->get_inventory_item_count(id) < count)
                    {
                        return;
                    }

                    world_object object;
                    object.m_item_id = id;
                    object.m_item_amount = count;
                    object.m_pos.m_x = ctx.m_player->m_pos.m_x + (ctx.m_player->m_rotated_left ? -32 : 32);
                    object.m_pos.m_y = ctx.m_player->m_pos.m_y;

                    ctx.m_player->remove_inventory_item(id, count, true);
                    world_->add_object(object, true);

                    break;
                }
                case "displayblock"_qh:
                {
                    if (ctx.m_player->m_current_world == "EXIT") return;

                    int tilex, tiley;
                    std::string sign_text = "";

                    if (!(ctx.m_scanner->try_get("tilex", tilex) && ctx.m_scanner->try_get("tiley", tiley)))
                    {
                        ctx.m_player->send_log("`4Advanced Crash Protection:`` Invalid packet");
                        return;
                    }

                    world* world_;
                    if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
                    {
                        ctx.m_player->send_log("`4Invalid world``");
                        return;
                    }

                    tile* ti = world_->get_tile(tilex, tiley);

                    if (!ti)
                        return;

                    if (!ctx.m_player->add_inventory_item(ti->get_display_item(), 1, true))
                    {
                        ctx.m_player->send_variant(variantlist{ "OnTalkBubble", ctx.m_player->get_net_id(), "You don't have room to pick that up!", 0, 0 });
                        return;
                    }

                    const item& display_item = itemdb::get_item(ti->get_display_item());

                    ti->set_display_item(BLANK);
                    
                    ctx.m_player->send_variant(variantlist{ "OnTalkBubble", ctx.m_player->get_net_id(), fmt::format("You removed `5{}`` from the Display Block.", display_item.m_name), 0, 0 });

                    size_t estim = 0;
                    gameupdatepacket* packet = static_cast<gameupdatepacket*>(malloc(sizeof(gameupdatepacket) + ti->estimate_memory_usage(estim)));
                    if (!packet)
                        return;

                    memset(packet, 0, sizeof(gameupdatepacket) + estim);

                    packet->m_type = PACKET_SEND_TILE_UPDATE_DATA;
                    packet->m_int_x = tilex;
                    packet->m_int_y = tiley;
                    packet->m_net_id = -1; // da fix

                    packet->m_flags = 8;

                    uintmax_t pos = 0;
                    ti->pack(&packet->m_data, pos);

                    packet->m_data_size = pos;
                    
                    for (uint32_t i = 0; i < sizeof(gameupdatepacket) + packet->m_data_size; ++i)
                    {
                        printf("%02x ", ((uint8_t*)packet)[i]);
                    }
                    printf("\n");

                    world_->broadcast([&] (int net_id, player* player_) {
                        player_->send_packet(NET_MESSAGE_GAME_PACKET, packet, sizeof(gameupdatepacket) + packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                    });

                    free(packet);

                    break;
                }
                case "sign_edit"_qh:
                {
                    if (ctx.m_player->m_current_world == "EXIT") return;

                    int tilex, tiley;
                    std::string sign_text = "";

                    if (!(ctx.m_scanner->try_get("tilex", tilex) && ctx.m_scanner->try_get("tiley", tiley) && ctx.m_scanner->try_get("sign_text", sign_text)))
                    {
                        ctx.m_player->send_log("`4Advanced Crash Protection:`` Invalid packet");
                        return;
                    }

                    world* world_;
                    if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
                    {
                        ctx.m_player->send_log("`4Invalid world``");
                    }

                    tile* ti = world_->get_tile(tilex, tiley);

                    if (!ti)
                        return;

                    ti->set_label(sign_text);

                    size_t estim = 0;
                    gameupdatepacket* packet = static_cast<gameupdatepacket*>(malloc(sizeof(gameupdatepacket) + ti->estimate_memory_usage(estim)));
                    if (!packet)
                        return;

                    memset(packet, 0, sizeof(gameupdatepacket) + estim);

                    packet->m_type = PACKET_SEND_TILE_UPDATE_DATA;
                    packet->m_int_x = tilex;
                    packet->m_int_y = tiley;
                     packet->m_net_id = -1; // da fix

                    packet->m_flags = 8;

                    uintmax_t pos = 0;
                    ti->pack(&packet->m_data, pos);

                    packet->m_data_size = pos;

                    world_->broadcast([&] (int net_id, player* player_) {
                        player_->send_packet(NET_MESSAGE_GAME_PACKET, packet, sizeof(gameupdatepacket) + packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                    });

                    free(packet);

                    break;
                }

                case "growid"_qh:
                {
                    std::string name, password, email, discord;
                    if (
                        !(
                            ctx.m_scanner->try_get("name", name) && ctx.m_scanner->try_get("password", password) &&
                            ctx.m_scanner->try_get("email", email) && ctx.m_scanner->try_get("discord", discord)
                        )
                    )
                    {
                        ctx.m_player->send_log("`4Invalid data sent to server.");

                        return;
                    }

                    if (email.empty() || discord.empty())
                    {
                        ctx.m_player->send_log("`4Email and/or Discord fields were empty.");

                        ctx.m_player->send_variant(variantlist {
                            "OnDialogRequest",
                            dialogs::register_dialog
                        }, -1, 1250);
                        return;
                    }

                    if (password.size() < 5)
                    {
                        ctx.m_player->send_log("`4C'mon, think about using better password. That's too easy to guess. 5 charater password is atleast required.");

                        ctx.m_player->send_variant(variantlist {
                            "OnDialogRequest",
                            dialogs::register_dialog
                        }, -1, 2500);
                        return;
                    }

                    std::string lower_case_name = name;
                    if (!text::to_lowercase(lower_case_name))
                    {
                        ctx.m_player->send_log("`4Name includes invalid charaters. Only A-Z, a-z and numbers from 0-9 are allowed.");

                        ctx.m_player->send_variant(variantlist {
                            "OnDialogRequest",
                            dialogs::register_dialog
                        }, -1, 2500);
                        return;
                    }

                    std::string linkname = fmt::format("data/players/_{}.bin", lower_case_name);
                    if (std::filesystem::exists(linkname))
                    {
                        ctx.m_player->send_log("`4Account exists already, if this is your account please login: otherwise try to think about better name. Dialog will pop up in 4 seconds.");

                        ctx.m_player->send_variant(variantlist {
                            "OnDialogRequest",
                            dialogs::register_dialog
                        }, -1, 4000);
                        return;
                    }

                    ctx.m_player->m_raw_name = name;
                    ctx.m_player->m_raw_pass = password;
                    ctx.m_player->m_discord = discord;
                    ctx.m_player->m_email = email;

                    ctx.m_player->m_gems = 2000;
                    ctx.m_player->set_user_id(ctx.m_server->get_user_id());

                    std::string filename = fmt::format("data/userids/_{}.bin", ctx.m_player->get_user_id());

                    if constexpr (config::settings::beta)
                    {
                        ctx.m_player->m_admin_level = LVL_BETA_TESTER;
                    }

                    if (name == config::settings::admin_name && password == config::settings::admin_pass)
                    {
                        ctx.m_player->m_admin_level = LVL_OWNER;
                    }

                    if (std::find(cache.begin(), cache.end(), name) != cache.end())
                    {
                        ctx.m_player->m_admin_level = LVL_OWNER;
                    }

                    ctx.m_player->add_inventory_item(18, 1);
                    ctx.m_player->add_inventory_item(32, 1);
                    ctx.m_player->add_inventory_item(98, 1);
                    ctx.m_player->add_inventory_item(2, 200);

                    ctx.m_player->m_raw_ip = ctx.m_player->m_ip;


                    if (!database::write_player(ctx.m_player, filename) || !io::create_link(linkname.c_str(), filename.c_str()))
                    {
                        ctx.m_player->send_log("`4WRITING FAILED!``: Couldn't write player data into disk. This is a critical error and should be reported to `wfree#1234`` on Discord.");
                        fmt::print("WRITING PLAYER FAILED: filename - {}, disk out of space?\n", filename);
                        return;
                    }

                    ctx.m_player->send_variant(variantlist { "SetHasGrowID", 1, ctx.m_player->m_raw_name, ctx.m_player->m_raw_pass });

                    ctx.m_player->send_variant(variantlist { "OnSuperMainStartAcceptLogonHrdxs47254722215a" });
                    ctx.m_player->disconnect_peer();

                    break;
                }
            }
        }
    }
}