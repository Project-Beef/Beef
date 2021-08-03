#include "activate.hh"

#include "packet/variant.hh"
#include "player/clothing.hh"

namespace beef::events::registered
{
    void item_activate(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in) return;

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world.``");
            return;
        }

        const item& base = itemdb::get_item(ctx.m_game_packet->m_int_data);
        clothing& cloth = ctx.m_player->m_cloth;

        switch (base.m_action_type)
        {
            case ITEMTYPE_ANCES:
            {
                if (cloth.m_ances == base.m_id)
                    cloth.m_ances = 0;
                else
                    cloth.m_ances = base.m_id;

                break;
            }

            case ITEMTYPE_CLOTHES:
            {
                switch (base.m_clothing_type)
                {
                    case CLOTHING_BACK:
                    {
                        if (cloth.m_back == base.m_id)
                            cloth.m_back = 0;
                        else
                            cloth.m_back = base.m_id;

                        break;
                    }

                    case CLOTHING_CHEST_ITEM:
                    {
                        if (cloth.m_necklace == base.m_id)
                            cloth.m_necklace = 0;
                        else
                            cloth.m_necklace = base.m_id;

                        break;
                    }

                    case CLOTHING_FACE_ITEM:
                    {
                        if (cloth.m_face == base.m_id)
                            cloth.m_face = 0;
                        else
                            cloth.m_face = base.m_id;

                        break;
                    }

                    case CLOTHING_HAIR:
                    {
                        if (cloth.m_mask == base.m_id)
                            cloth.m_mask = 0;
                        else
                            cloth.m_mask = base.m_id;

                        break;
                    }

                    case CLOTHING_HAND:
                    {
                        if (cloth.m_hand == base.m_id)
                            cloth.m_hand = 0;
                        else
                            cloth.m_hand = base.m_id;

                        break;
                    }

                    case CLOTHING_HAT:
                    {
                        if (cloth.m_hair == base.m_id)
                            cloth.m_hair = 0;
                        else
                            cloth.m_hair = base.m_id;

                        break;
                    }

                    case CLOTHING_PANTS:
                    {
                        if (cloth.m_pants == base.m_id)
                            cloth.m_pants = 0;
                        else
                            cloth.m_pants = base.m_id;

                        break;
                    }

                    case CLOTHING_SHIRT:
                    {
                        if (cloth.m_shirt == base.m_id)
                            cloth.m_shirt = 0;
                        else
                            cloth.m_shirt = base.m_id;

                        break;
                    }

                    case CLOTHING_SHOES:
                    {
                        if (cloth.m_feet == base.m_id)
                            cloth.m_feet = 0;
                        else
                            cloth.m_feet = base.m_id;

                        break;
                    }

                    default: break;
                }

                break;
            }

            default:
            {
                ctx.m_player->send_variant(variantlist { "OnTalkBubble", ctx.m_player->get_net_id(), "Only wearing items are supported at the moment.", 0, 1 });
                break;
            }
        }

        world_->broadcast([&] (int net_id, player* player_) {
            player_->send_cloth(ctx.m_player, true);
        });
    }
}