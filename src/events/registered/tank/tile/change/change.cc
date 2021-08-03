#include "change.hh"

#include "world/tile.hh"
#include "algorithm/lock.hh"
#include "constants/roles.hh"
#include "itemdb/item_types.hh"
#include "itemdb/item_component.hh"
#include "packet/gameupdatepacket.hh"

#include <chrono>

#include <enet/enet.h>

// follow me nows

namespace beef::events::registered
{
    void add_objects_when_breaking_tree(player* player_, world* world_, tile* tile_, const math::vec2& position)
    {
        const item& item_ = itemdb::get_item(tile_->get_fg_uint() - 1);

        uint8_t n = tile_->get_fruit_count() + 1;
        float c = static_cast<float>((std::pow(n, 2) + n) / 2);

        uint8_t blocks_yielded = static_cast<uint8_t>(c / tile_->get_fruit_count() * 2.5f);

        world_object obj;
        obj.m_pos = position;
        obj.m_item_id = item_.m_id;
        obj.m_item_amount = blocks_yielded;

        world_->add_object(obj, true);

        float seed_chance = 4.0f / (item_.m_rarity + 12);

        if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) <= seed_chance)
        {
            obj.m_item_amount = 1;
            obj.m_item_id = tile_->get_fg_uint();

            const item& fg = itemdb::get_item(tile_->get_fg_uint());

            world_->add_object(obj, true);
            player_->send_variant(variantlist{ "OnTalkBubble", player_->get_net_id(), fmt::format("`w{} falls out!", fg.m_name), 0 });
        }

        float max = item_.m_rarity / 3.954545454545455f;
        int max_gems = static_cast<int>(std::round(max)) * 5;

        if (max_gems == 0)
        {
            if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) >= max)
                return;

            max_gems = 5;
        }

        world_->add_gems_object(rand() % max_gems, position);
    }

    void add_objects_when_breaking_block(player* player_, world* world_, tile* tile_, const math::vec2& position)
    {
        static float seed_chance = 1.0f / 4.0f;
        static float gems_chance = 2.0f / 3.0f;
        float value = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        const item& item_ = beef::itemdb::get_item(tile_->get_base_uint());

        if (value <= gems_chance)
        {
            if (item_.m_rarity == 999)
                return;

            float max = item_.m_rarity / 3.954545454545455f;
            int max_gems = static_cast<int>(std::round(max)) * 5;

            if (max_gems == 0)
            {
                if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) >= max)
                    return;
                max_gems = 5;
            }

            world_->add_gems_object(rand() % max_gems, position);
        }
        else if (value <= gems_chance + seed_chance)
        {
            if (item_.m_action_type != ITEMTYPE_SEED && !(item_.m_editable_type & ITEMFLAG1_SEEDLESS))
            {
                world_object obj{};
                obj.m_pos = position;
                obj.m_item_id = item_.m_id + 1;
                obj.m_item_amount = 1;
                world_->add_object(obj, true);
            }
        }
        else
        {
            world_object obj{};
            obj.m_pos = position;
            obj.m_item_id = item_.m_id;
            obj.m_item_amount = 1;
            world_->add_object(obj, true);
        }
    }

    void tile_change(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in || ctx.m_player->m_guest) return;

        ctx.m_game_packet->m_net_id = ctx.m_player->get_net_id();

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world``");
            return;
        }

        tile* tile_ = world_->get_tile(ctx.m_game_packet->m_int_x, ctx.m_game_packet->m_int_y);

        if (!tile_)
            return;

        switch (ctx.m_game_packet->m_item_id)
        {
            case FIST:
            {
                const item& base = tile_->get_base_item();

                if (base.m_id == BLANK)
                    return;

                ctx.m_game_packet->m_type = PACKET_TILE_APPLY_DAMAGE;
                ctx.m_game_packet->m_tile_damage = 8;

                if (base.m_id == BEDROCK || base.m_id == MAINDOOR)
                {
                    if (ctx.m_player->m_admin_level < LVL_MODERATOR)
                    {
                        ctx.m_player->send_text("action|play_sfx\nfile|audio/cant_break_tile.wav\ndelayMS|0\n");
                        ctx.m_player->send_variant(beef::variantlist { "OnTalkBubble", ctx.m_player->get_net_id(), "It's too strong to break.", 0, 1 });
                        return;
                    }
                }
                else if (base.m_action_type == ITEMTYPE_SEED)
                {
                    if ((clock::now() - tile_->get_last_planted()) >= std::chrono::seconds(base.m_grow_time))
                    {
                        math::vec2 pos = { static_cast<float>(ctx.m_game_packet->m_int_x * 32), static_cast<float>(ctx.m_game_packet->m_int_y * 32) };
                        add_objects_when_breaking_tree(ctx.m_player, world_, tile_, pos);
                        tile_->remove_base();

                        ctx.m_game_packet->m_type = PACKET_SEND_TILE_TREE_STATE;
                        ctx.m_game_packet->m_item = -1;
                        ctx.m_game_packet->m_vec_x = 0;
                        ctx.m_game_packet->m_vec_y = 0;
                        ctx.m_game_packet->m_vec2_x = 0;
                        ctx.m_game_packet->m_vec2_y = 0;

                        world_->broadcast([&] (int net_id, player* player_) {
                            player_->send_packet(4, ctx.m_game_packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
                        });

                        return;
                    }
                }                

                auto diff = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - tile_->get_last_hit_time()).count();

                if (diff >= base.m_reset_time)
                    tile_->reset_hit();

                if (tile_->indicate_hit() >= base.m_break_hits)
                {
                    ctx.m_game_packet->m_type = PACKET_TILE_CHANGE_REQUEST;
                    ctx.m_game_packet->m_item_id = FIST;
                    
                    math::vec2 pos = { static_cast<float>(ctx.m_game_packet->m_int_x * 32), static_cast<float>(ctx.m_game_packet->m_int_y * 32) };
                    add_objects_when_breaking_block(ctx.m_player, world_, tile_, pos);

                    switch (base.m_action_type)
                    {
                        case ITEMTYPE_LOCK:
                        {
                            if (item_is_world_lock(base))
                            {
                                tile_->set_new_lock();
                                world_->set_new_lock();

                                gameupdatepacket* pck1 = variantlist{ "OnNameChanged", ctx.m_player->get_display_name() }.pack();
                                gameupdatepacket* pck2 = variantlist{ "OnConsoleMessage", fmt::format("`5[```w{}`` has had its `$World Lock`` removed!`5]``", world_->m_name) }.pack();

                                pck1->m_net_id = ctx.m_player->get_net_id();

                                world_->broadcast([&](int, player* ply)
                                {
                                    ply->send_packet(NET_MESSAGE_GAME_PACKET, pck1, sizeof(gameupdatepacket) + pck1->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                    ply->send_packet(NET_MESSAGE_GAME_PACKET, pck2, sizeof(gameupdatepacket) + pck2->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                });

                                free(pck1);
                                free(pck2);
                            }
                            else
                            {
                                int index = ctx.m_game_packet->m_int_x + ctx.m_game_packet->m_int_y * world_->m_width;

                                for (int i = 0; i < world_->m_tiles.size(); ++i)
                                {
                                    if (i == index)
                                        continue;

                                    auto& t = world_->m_tiles[i];

                                    if (t.is_locked() && t.get_lp() == index)
                                    {
                                        t.remove_lock();
                                    }
                                }
                            }
                            break;
                        }
                    }

                    tile_->remove_base();
                }

                world_->broadcast([&] (int net_id, player* player_) {
                    player_->send_packet(4, ctx.m_game_packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
                });

                break;
            }

            case WRENCH:
            {
                const item& it = beef::itemdb::get_item(tile_->get_base_uint());

                // if (world_->get_c_tile(ctx.m_game_packet->m_int_x * ctx.m_game_packet->m_int_y).get_base_uint() != it.m_id) return;

                switch (it.m_action_type)
                {
                    case ITEMTYPE_SIGN:
                    {
                        ctx.m_player->send_variant(variantlist { 
                            "OnDialogRequest",
                            fmt::format(
                                "set_default_color|`o\n"
                                "add_label_with_icon|big|`wEdit {}``|left|{}|\n"
                                "add_textbox|What would you like to write on this sign?``|left|\n"
                                "add_text_input|sign_text||{}|128|\n"
                                "embed_data|tilex|{}\n"
                                "embed_data|tiley|{}\n"
                                "end_dialog|sign_edit|Cancel|OK|\n",
                                it.m_name, it.m_id, tile_->get_label(),
                                ctx.m_game_packet->m_int_x, ctx.m_game_packet->m_int_y
                            )
                        });

                        break;
                    }
                    case ITEMTYPE_DISPLAY_BLOCK:
                    {
                        if (tile_->get_display_item() == BLANK)
                        {
                            ctx.m_player->send_variant(variantlist {
                                "OnDialogRequest",
                                "set_default_color|`o\n"
                                "add_label_with_icon|big|`wDisplay Block``|left|2946|\n"
                                "add_spacer|small|\n"
                                "add_textbox|The Display Block is empty. Use an item on it to display the item!|left|\n"
                                "end_dialog|displayblock|Okay||\n"
                            });
                        }
                        else
                        {
                            const item& display_item = itemdb::get_item(tile_->get_display_item());

                            ctx.m_player->send_variant(variantlist {
                                "OnDialogRequest",
                                fmt::format(
                                    "set_default_color|`o\n"
                                    "add_label_with_icon|big|`wDisplay Block``|left|2946|\n"
                                    "add_spacer|small|\n"
                                    "embed_data|tilex|{}\n"
                                    "embed_data|tiley|{}\n"
                                    "add_textbox|A {} is on display here.!|left|\n"
                                    "end_dialog|displayblock|Leave it|Pick it up|\n",
                                    ctx.m_game_packet->m_int_x, 
                                    ctx.m_game_packet->m_int_y,
                                    display_item.m_name
                                )
                            });
                        }
                        break;
                    }
                }
                break;
            }

            default:
            {
                const beef::item& base = beef::itemdb::get_item(ctx.m_game_packet->m_item_id);
                ctx.m_game_packet->m_type = PACKET_TILE_CHANGE_REQUEST;

                if (!ctx.m_player->has_inventory_item(ctx.m_game_packet->m_item_id))
                    return;

                if (base.m_action_type == ITEMTYPE_BACKGROUND)
                {
                    if (!ctx.m_player->remove_inventory_item(ctx.m_game_packet->m_item_id, 1, true)) break;
                    tile_->set_bg(ctx.m_game_packet->m_item_id);
                }
                else
                {
                    /*if (!base.m_has_extra)
                    {
                        if (base.m_action_type == ITEMTYPE_CLOTHES || base.m_action_type == ITEMTYPE_ANCES) return;
                    }
                    else
                    {*/
                    if (tile_->get_fg_uint() == BLANK)
                    {
                        switch (base.m_action_type)
                        {
                            case ITEMTYPE_SIGN:
                            case ITEMTYPE_DOOR:
                            {
                                tile_->set_label("");
                                //tile_->set_flag(TILEFLAG_TILEEXTRA);
                                break;
                            }

                            case ITEMTYPE_MAIN_DOOR:
                            {
                                tile_->set_label("EXIT");
                                //tile_->set_flag(TILEFLAG_TILEEXTRA);
                                break;
                            }

                            case ITEMTYPE_SEED:
                            {
                                tile_->set_new_seed();
                                ctx.m_game_packet->m_fruit_count = tile_->get_fruit_count();
                                break;
                            }

                            case ITEMTYPE_LOCK:
                            {
                                if (item_is_world_lock(base))
                                {
                                    if (world_->is_locked())
                                    {
                                        ctx.m_player->send_variant({ "OnTalkBubble", ctx.m_player->get_net_id(), "Only one `$World Lock`` can be placed in a world, you'd have to remove the other one first.", 0, 1 });
                                        return;
                                    }
                                    else
                                    {
                                        for (const auto& t : world_->m_tiles)
                                        {
                                            if (!item_is_world_lock(t.get_base_item()))
                                            {
                                                if (t.get_owner_id() != ctx.m_player->get_user_id())
                                                {
                                                    ctx.m_player->send_variant({ "OnTalkBubble", ctx.m_player->get_net_id(), "Your `$World Lock`` can't be placed in this world unless everyone else's locks are removed.", 0, 1 });
                                                    return;
                                                }
                                            }
                                        }

                                        if (!ctx.m_player->remove_inventory_item(ctx.m_game_packet->m_item_id, 1, true)) break;
                                        tile_->set_fg(ctx.m_game_packet->m_item_id);

                                        tile_->set_new_lock(ctx.m_player->get_user_id());
                                        world_->set_new_lock(ctx.m_player->get_user_id());

                                        std::string name = ctx.m_player->get_display_name();
                                        std::string text = fmt::format("`5[```w{}`` has been `$World Locked`` by {}`5]``", world_->m_name, name);

                                        gameupdatepacket visual_packet{ PACKET_SEND_LOCK };
                                        visual_packet.m_int_x = ctx.m_game_packet->m_int_x;
                                        visual_packet.m_int_y = ctx.m_game_packet->m_int_y;
                                        visual_packet.m_item_id = ctx.m_game_packet->m_item_id;
                                        visual_packet.m_owner_id = ctx.m_player->get_user_id();

                                        gameupdatepacket* pck1 = variantlist{ "OnNameChanged", name }.pack();
                                        gameupdatepacket* pck2 = variantlist{ "OnConsoleMessage", text }.pack();
                                        gameupdatepacket* pck3 = variantlist{ "OnPlayPositioned", "audio/use_lock.wav" }.pack();
                                        gameupdatepacket* pck4 = variantlist{ "OnTalkBubble", ctx.m_player->get_net_id(), text, 0, 0 }.pack();
                                        pck1->m_net_id = ctx.m_player->get_net_id();
                                        pck3->m_net_id = ctx.m_player->get_net_id();

                                        world_->broadcast([&](int, player* ply)
                                        {
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, &visual_packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, pck1, sizeof(gameupdatepacket) + pck1->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, pck2, sizeof(gameupdatepacket) + pck2->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, pck3, sizeof(gameupdatepacket) + pck3->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, pck4, sizeof(gameupdatepacket) + pck4->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                        });

                                        free(pck1);
                                        free(pck2);
                                        free(pck3);
                                        free(pck4);
                                    }
                                }
                                else
                                {
                                    if (!ctx.m_player->remove_inventory_item(ctx.m_game_packet->m_item_id, 1, true)) break;

                                    tile_->set_fg(ctx.m_game_packet->m_item_id);
                                    tile_->set_new_lock(ctx.m_player->get_user_id());

                                    ctx.m_player->send_variant({ "OnTalkBubble", ctx.m_player->get_net_id(), "Area locked.", 0, 0 });

                                    algorithm::lock::apply(ctx.m_player, world_, ctx.m_game_packet, false);
                                }
                                return;
                            }

                            case ITEMTYPE_CONSUMABLE:
                            {
                                switch (ctx.m_game_packet->m_item_id)
                                {
                                    case DOORMOVER:
                                    {
                                        tile* ti_bedrock = world_->get_tile(ctx.m_game_packet->m_int_x, ctx.m_game_packet->m_int_y + 1);

                                        math::vec2i door_pos = world_->get_tile_pos(MAINDOOR);

                                        tile* door = world_->get_tile(door_pos);
                                        tile* door_bedrock = world_->get_tile(door_pos.m_x, door_pos.m_y + 1);

                                        if (!door || !door_bedrock)
                                            return;

                                        if (!ti_bedrock || tile_->get_fg_uint() != BLANK || ti_bedrock->get_fg_uint() != BLANK)
                                        {
                                            ctx.m_player->send_text("action|play_sfx\nfile|audio/cant_break_tile.wav\ndelayMS|0\n");
                                            ctx.m_player->send_variant(variantlist { "OnTalkBubble", ctx.m_player->get_net_id(), "It's too tight for a door here.", 0, 1 });
                                            return;
                                        }

                                        door->set_fg(BLANK);
                                        door->remove_label();

                                        door_bedrock->set_fg(BLANK);

                                        tile_->set_fg(MAINDOOR);
                                        tile_->set_label("EXIT");

                                        ti_bedrock->set_fg(BEDROCK);
                                        ti_bedrock->reset_flag();

                                        uintmax_t estimate = 0;
                                        gameupdatepacket* packet = static_cast<gameupdatepacket*>(malloc(sizeof(gameupdatepacket) + (8 * tile_->estimate_memory_usage(estimate)) + 4));
                                        if (!packet)
                                            return;

                                        if (!ctx.m_player->remove_inventory_item(ctx.m_game_packet->m_item_id, 1, true)) break;

                                        packet->m_type = PACKET_SEND_TILE_UPDATE_DATA_MULTIPLE;
                                        packet->m_flags = 8;
                                        packet->m_int_x = -1;
                                        packet->m_int_y = -1;

                                        uintmax_t pos = 0;

                                        memcpy(&packet->m_data + pos, &ctx.m_game_packet->m_int_x, 4);
                                        memcpy(&packet->m_data + pos + 4, &ctx.m_game_packet->m_int_y, 4);
                                        pos += 8;

                                        tile_->pack(&packet->m_data, pos);

                                        int y = ctx.m_game_packet->m_int_y + 1;
                                        memcpy(&packet->m_data + pos, &ctx.m_game_packet->m_int_x, 4);
                                        memcpy(&packet->m_data + pos + 4, &y, 4);
                                        pos += 8;

                                        ti_bedrock->pack(&packet->m_data, pos);

                                        memcpy(&packet->m_data + pos, &door_pos.m_x, 4);
                                        memcpy(&packet->m_data + pos + 4, &door_pos.m_y, 4);
                                        pos += 8;

                                        door->pack(&packet->m_data, pos);

                                        int door_y = door_pos.m_y + 1;
                                        memcpy(&packet->m_data + pos, &door_pos.m_x, 4);
                                        memcpy(&packet->m_data + pos + 4, &door_y, 4);
                                        pos += 8;

                                        door_bedrock->pack(&packet->m_data, pos);

                                        *reinterpret_cast<int*>(&packet->m_data + pos) = -1;
                                        pos += 4;

                                        packet->m_data_size = pos;
                                        world_->broadcast([&] (int net_id, player* player_) {
                                            player_->send_packet(4, packet, sizeof(gameupdatepacket) + packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                        });

                                        free(packet);

                                        return;
                                    }

                                    default:
                                    {
                                        return;
                                    }
                                }

                                break;
                            }

                            case ITEMTYPE_CLOTHES:
                            case ITEMTYPE_ANCES:
                            {
                                break;
                            }

                            case ITEMTYPE_DISPLAY_BLOCK:
                            {
                                tile_->set_display_item(0);
                                break;
                            }

                            default:
                            {
                                if (base.m_has_extra)
                                {
                                    ctx.m_player->send_log(fmt::format("Unsupported item placed: `w{}``, with type: `w{}``.", base.m_name, base.m_action_type));
                                    return;
                                }
                            }
                        }
                    }
                    else
                    {
                        // checks by kevz

                        const item& fg = itemdb::get_item(tile_->get_fg_uint());

                        switch (fg.m_action_type)
                        {
                            case ITEMTYPE_DISPLAY_BLOCK:
                            {
                                if (tile_->get_display_item() != BLANK)
                                {
                                    ctx.m_player->send_variant(variantlist{ "OnTalkBubble", ctx.m_player->get_net_id(), "Remove what's in there first!", 0, 1 });
                                }
                                else
                                {
                                    if (base.m_action_type == ITEMTYPE_LOCK ||
                                        base.m_action_type == ITEMTYPE_DISPLAY_BLOCK)
                                    {
                                        ctx.m_player->send_variant(variantlist{ "OnTalkBubble", ctx.m_player->get_net_id(), "Sorry, no displaying Display Blocks or Locks.", 0, 1 });
                                    }
                                    else
                                    {
                                        ctx.m_player->remove_inventory_item(base.m_id, 1, true);
                                        tile_->set_display_item(base.m_id);

                                        gameupdatepacket effect_packet{ PACKET_ITEM_EFFECT };
                                        effect_packet.m_pos_x = static_cast<float>(ctx.m_game_packet->m_int_x * 32);
                                        effect_packet.m_pos_y = static_cast<float>(ctx.m_game_packet->m_int_y * 32);
                                        effect_packet.m_particle_id = 4;
                                        effect_packet.m_item_id_alt = base.m_id;
                                        effect_packet.m_target_net_id = ctx.m_player->get_net_id();
                                        effect_packet.m_particle_size_alt = 1;

                                        size_t estim = 0;
                                        gameupdatepacket* packet = static_cast<gameupdatepacket*>(malloc(sizeof(gameupdatepacket) + tile_->estimate_memory_usage(estim)));
                                        if (!packet)
                                            return;
                                        
                                        memset(packet, 0, sizeof(gameupdatepacket) + estim);

                                        packet->m_type = PACKET_SEND_TILE_UPDATE_DATA;
                                        packet->m_int_x = ctx.m_game_packet->m_int_x;
                                        packet->m_int_y = ctx.m_game_packet->m_int_y;
                                        packet->m_net_id = -1; // da fix

                                        packet->m_flags = 8;

                                        uintmax_t pos = 0;
                                        tile_->pack(&packet->m_data, pos);

                                        packet->m_data_size = pos;

                                        gameupdatepacket* audio_packet = variantlist{ "OnPlayPositioned", "audio/blorb.wav" }.pack();
                                        audio_packet->m_net_id = ctx.m_player->get_net_id();

                                        world_->broadcast([&](int, player* ply)
                                        {
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, &effect_packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, packet, sizeof(gameupdatepacket) + packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                            ply->send_packet(NET_MESSAGE_GAME_PACKET, audio_packet, sizeof(gameupdatepacket) + audio_packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
                                        });

                                        free(packet);
                                        free(audio_packet);
                                    }
                                }
                                break;
                            }
                        }

                        return;
                    }

                    if (!ctx.m_player->remove_inventory_item(ctx.m_game_packet->m_item_id, 1, true)) break;
                    tile_->set_fg(ctx.m_game_packet->m_item_id);

                    /*uintmax_t pos = 0;

                    uint8_t* alloc = static_cast<uint8_t*>(malloc(tile_->estimate_memory_usage(pos)));
                    pos = 0;

                    handle_tile_extra(alloc, pos, &tile_);

                    ctx.m_game_packet->m_data = 
                    ctx.m_game_packet->m_data_size = pos;*/
                }

                world_->broadcast([&] (int net_id, player* player_) {
                    player_->send_packet(4, ctx.m_game_packet, sizeof(gameupdatepacket), ENET_PACKET_FLAG_RELIABLE);
                });
                break;
            }
        }
    }

    void add_objects_when_breaking_tree()
    {

    }

    void add_objects_when_break_block()
    {

    }

}
