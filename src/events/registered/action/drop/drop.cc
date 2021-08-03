#include "drop.hh"
#include "itemdb/item_collision.hh"

namespace beef::events::registered
{
    void drop(event_context& ctx)
    {
        if (ctx.m_player->m_current_world == "EXIT" || !ctx.m_player->m_logged_in || ctx.m_player->m_guest) return;

        int id;

        if (!ctx.m_scanner->try_get("|itemID", id))
        {
            ctx.m_player->send_log("`4Invalid ItemID received.`` Please report this to `wfree#1234``.");
            return;
        }

        world* world_;
        if (!ctx.m_manager->get_world(ctx.m_player->m_current_world, &world_))
        {
            ctx.m_player->send_log("`4Invalid world``");
            return;
        }

        if (id >= itemdb::get_size())
        {
            ctx.m_player->send_log("`4Invalid out of range item ID received.`` Please report this to `wkevz#2211``.");
            return;
        }

        if (ctx.m_player->has_inventory_item(id))
        {
            const item& item_ = itemdb::get_item(id);

            if (item_.m_item_category & ITEMFLAG2_UNTRADABLE)
            {
                ctx.m_player->send_variant(variantlist{ "OnTextOverlay", "You can't drop that." });
                return;
            }

            tile* tile_next = world_->get_tile(ctx.m_player->m_pos.m_x / 32 + (ctx.m_player->m_rotated_left ? -1 : 1), ctx.m_player->m_pos.m_y / 32);

            if (!tile_next)
            {
                ctx.m_player->send_variant(variantlist{ "OnTextOverlay", "You can't drop that here, face somewhere with open space." });
                return;
            }

            const item& fg = itemdb::get_item(tile_next->get_fg_uint());

            if (fg.m_collision_type == ITEMCOLLISION_NORMAL)
            {
                ctx.m_player->send_variant(variantlist{ "OnTextOverlay", "You can't drop that here, face somewhere with open space." });
                return;
            }
            else if (fg.m_action_type == ITEMTYPE_MAIN_DOOR)
            {
                ctx.m_player->send_variant(variantlist{ "OnTextOverlay", "You can't drop items in the white door." });
                return;
            }

            uint8_t item_amount = ctx.m_player->get_inventory_item_count(id);

            static std::vector<std::string> messages = 
            {
                "`4Warning:`` Once you drop an item, it is no longer yours. Anyone can take it from you and we CANNOT return it!",
                "`4Warning:`` If someone is asking you to drop items, they are DEFINITELY trying to scam you. Do not drop items in other players' worlds.",
                "`4Warning:`` We cannot restore items you lose because you dropped them. Do not drop items you want to keep!",
                "If you are trying to trade an item with another player, use your wrench on them instead to use our Trade System! `4Dropping items is not safe!``",
                "`4Warning:`` Any player who asks you to drop items is scamming you. We cannot restore scammed items."
            };

            std::string dialog = fmt::format(
                "set_default_color|`o\n"
                "add_label_with_icon|big|`wDrop {}``|left|{}|\n"
                "add_textbox|How many to drop?|left|\n"
                "add_text_input|count||{}|5|\n"
                "embed_data|itemID|{}\n"
                "add_textbox|{}|left|\n"
                "end_dialog|drop_item|Cancel|OK|\n",
                item_.m_name,
                item_.m_id,
                item_amount,
                item_.m_id,
                messages[rand() % messages.size()]
            );
            ctx.m_player->send_variant(variantlist{ "OnDialogRequest", dialog });
        }
    }

}