cmake_minimum_required(VERSION 3.15)

add_definitions(-D__CMAKE_ROOT_DIRECTORY__="${CMAKE_CURRENT_SOURCE_DIR}/..")

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
    main.cc

    text/scanner.cc 
    text/scanner.hh

    server/server.cc 
    server/server.hh

    player/player.cc 
    player/player.hh 
    player/clothing.hh 
    player/inventory.hh
    
    math/vec2.hh 
    math/vec2i.hh 
    math/vec3.hh

    packet/gameupdatepacket.hh 
    packet/variant.hh

    itemdb/item_component.hh 
    itemdb/itemdb.cc 
    itemdb/itemdb.hh 
    itemdb/item.hh 
    itemdb/item_visuals.hh 
    itemdb/item_types.hh 
    itemdb/item_material.hh 
    itemdb/item_flags.hh 
    itemdb/item_collision.hh

    algorithm/lock.cc
    algorithm/lock.hh

    world/tile.cc 
    world/tile.hh 
    world/world.cc 
    world/world.hh 
    world/world_manager.cc 
    world/world_manager.hh
    
    events/event_manager.cc 
    events/event_manager.hh 
    events/event_context.hh 

    events/registered/text/guest/guest.cc 
    events/registered/text/guest/guest.hh
    events/registered/text/growid/growid.cc 
    events/registered/text/growid/growid.hh 

    
    events/registered/action/action.cc 
    events/registered/action/action.hh 
    events/registered/action/drop/drop.cc 
    events/registered/action/drop/drop.hh
    events/registered/action/quit/quit.cc 
    events/registered/action/quit/quit.hh 
    events/registered/action/input/input.cc 
    events/registered/action/input/input.hh
    events/registered/action/respawn/respawn.cc
    events/registered/action/respawn/respawn.hh
    events/registered/action/enter_game/enter_game.cc 
    events/registered/action/enter_game/enter_game.hh 
    events/registered/action/join_request/join_request.cc 
    events/registered/action/join_request/join_request.hh 
    events/registered/action/quit_to_exit/quit_to_exit.cc 
    events/registered/action/quit_to_exit/quit_to_exit.hh 
    events/registered/action/respawn_spike/respawn_spike.cc
    events/registered/action/respawn_spike/respawn_spike.hh
    events/registered/action/dialog_return/dialog_return.cc 
    events/registered/action/dialog_return/dialog_return.hh 
    events/registered/action/refresh_item_data/refresh_item_data.cc 
    events/registered/action/refresh_item_data/refresh_item_data.hh
    

    events/registered/tank/state/state.cc 
    events/registered/tank/state/state.hh 
    events/registered/tank/icon_state/icon_state.cc
    events/registered/tank/icon_state/icon_state.hh

    events/registered/tank/tile/change/change.cc 
    events/registered/tank/tile/change/change.hh 
    events/registered/tank/tile/activate/activate.cc 
    events/registered/tank/tile/activate/activate.hh 

    events/registered/tank/item/activate/activate.cc 
    events/registered/tank/item/activate/activate.hh 
    events/registered/tank/item/activate_object/activate_object.cc 
    events/registered/tank/item/activate_object/activate_object.hh

    commands/get/get.cc 
    commands/get/get.hh 
    commands/role/role.cc 
    commands/role/role.hh 
    commands/nick/nick.cc 
    commands/nick/nick.hh
    commands/help/help.cc 
    commands/help/help.hh
    commands/clear/clear.cc 
    commands/clear/clear.hh 
    commands/ban/ban.cc
    commands/ban/ban.hh
    
    constants/dialogs.hh 
    constants/config.hh

    utils/io.cc 
    utils/io.hh 
    utils/text.cc 
    utils/text.hh
    utils/time.cc
    utils/time.hh
    
    database/database.cc 
    database/database.hh 
    http/http.cc http/http.hh
)
target_link_libraries(${PROJECT_NAME} PUBLIC enet fmt nlohmann_json sandbird ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${PROJECT_NAME} PRIVATE .)
