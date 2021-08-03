#ifndef __CONSTANTS__DIALOG__HH__
#define __CONSTANTS__DIALOG__HH__

#include <string>

#include <fmt/core.h>

#include "config.hh"

namespace beef::dialogs
{
    static std::string register_dialog =                 
                "set_default_color|`o|\n"
                "add_label_with_icon|big|GrowID Creation|left|204|\n"
                "add_spacer|small|\n"
                "add_textbox|You need a `wGrowID`` to play on `9beef`` (guest account support not planned). All fields are required. If one of them is left empty, registration fails.|left|\n"
                "add_text_input|name|GrowID:||18|\n"
                "add_text_input_password|password|Password:||18|\n"
                "add_spacer|small|\n"
                "add_textbox|Next, we need your `wEmail`` and `wDiscord`` tag. For example if you lost your account credentials you can recover them with either of these.|\n"
                "add_text_input|email|Email:||25|\n"
                "add_text_input|discord|Discord:||25|\n"
                "add_spacer|small|\n"
                "add_textbox|By creating this `wGrowID`` you will agree into our guidelines which can change anytime. Registering this account while server is in beta will reward you a `9beta tester`` role.|\n"
                "end_dialog|growid||Create!|\n";

    static std::string gazette = 
                "set_default_color|`o\n"
                "add_label_with_icon|big|`wThe Beef Gazette``|left|5016|\n"
                "add_spacer|small|\n"
                "add_image_button|banner|interface/large/beef_banner.rttex|bannerlayout|||\n"
                "add_spacer|small|\n"
                "add_textbox|`wJune 9th: `50.2.1 is here!``|left|\n"
                "add_spacer|small|\n"
                "add_image_button|iotm_layout|interface/large/gazette/gazette_3columns_feature_btn01.rttex|3imageslayout|OPENSTORE|main/itemomonth|\n"
                "add_image_button|iotm_layout|interface/large/gazette/gazette_3columns_feature_btn04.rttex|3imageslayout|OPENSTORE|main/voucherday|\n"
                "add_image_button|iotm_layout|interface/large/gazette/gazette_3columns_feature_btn02.rttex|3imageslayout|OPENSTORE|main/gems_bundle05|\n"
                "add_spacer|small|\n"
                "add_label|small|`50.2.1|left|\n"
                "add_textbox|- `wSigns`` are working!|\n"
                "add_textbox|- You can quit to `wEXIT`` via `wMain Door`` now!|\n"
                "add_spacer|small|\n"
                "add_url_button||`wView Commands``|NOFLAGS|https://discord.com/channels/721778904590843904/848598580359856169/851498472830271500|Do you want to view commands? This will open Discord.|0|0|\n"
                "add_url_button||`wView Custom Items``|NOFLAGS|https://discord.com/channels/721778904590843904/848598580359856169/851480109731020831|Do you want to view custom items? This will open Discord.|0|0|\n"
                "add_quick_exit\n"
                "end_dialog|gazette||OK|\n";
}

#endif // __CONSTANTS__DIALOG__HH__
