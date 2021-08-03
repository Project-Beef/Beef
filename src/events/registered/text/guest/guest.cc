#include "guest.hh"

#include "constants/dialogs.hh"
#include "packet/variant.hh"

namespace beef::events::registered
{
    void guest(event_context& ctx)
    {
        ctx.m_player->send_variant(variantlist { "OnRequestWorldSelectMenu" });
        ctx.m_player->send_variant(variantlist {
            "OnDialogRequest",
            dialogs::register_dialog
        });
    }
}