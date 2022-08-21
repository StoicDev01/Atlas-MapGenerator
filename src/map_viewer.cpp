#include "map_viewer.h"

// map viewer
MapViewer::MapViewer(MapGenerator& map, core::graphics::View2D& camera) : 
    Gui{"World Viewer"}, 
    camera(camera),
    map(map)
{}


void MapViewer::init(core::graphics::Window& window){
    core::Vector2u window_size = window.get_size();
    mouse_map_position = core::Vector2i(0,0);

    // set map position
    map.m_sprite.m_position =  core::Vector3f(
        0, 0,10
    );
}

void MapViewer::update(float delta, core::graphics::Window& window){
    // get world mouse map position
    core::Vector2i mouse_pos = window.get_mouse_pos();
    core::Vector2f world_mouse_pos = window.screen_to_world_pos(mouse_pos);
    core::Vector2f map_bot_left = core::Vector2f(
        map.m_sprite.m_position.x -(map.m_sprite.m_scale.x / 2.0),
        map.m_sprite.m_position.y -(map.m_sprite.m_scale.y / 2.0)
    );

    // get mouse position relative to map sprite bottom left
    core::Vector2f difference_mouse_map = ( world_mouse_pos - map_bot_left);
    core::Vector2u map_texture_size = map.m_texture.get_size();
    core::Vector2f map_sprite_scale = map.m_sprite.m_scale;

    // move camera with mouse
    ImVec2 mouse_delta = ImGui::GetMouseDragDelta();

    if (
        difference_mouse_map.x < map.m_sprite.m_scale.x &&
        difference_mouse_map.y < map.m_sprite.m_scale.y &&
        difference_mouse_map.x >= 0 &&
        difference_mouse_map.y >= 0
    ){
        mouse_map_position = core::Vector2i(difference_mouse_map.x, difference_mouse_map.y);
        mouse_in_map = true;
    }
    else{
        mouse_in_map = false;
    }

    if (can_drag_camera){
        camera.m_position =(
            core::Vector3f(
                camera.m_position.x - mouse_delta.x * delta,
                camera.m_position.y + mouse_delta.y * delta,
                0
            )
        );
    }
}

void MapViewer::draw(core::graphics::Window& window){
    // draw map sprite
    map.m_sprite.draw();
}

void MapViewer::handle_event(core::Event event){
    // move map

    if (event.type == GLEQ_BUTTON_PRESSED){
        if (event.mouse.button == GLFW_MOUSE_BUTTON_LEFT){
            can_drag_camera =true;
        }
    }

    if (event.type == GLEQ_BUTTON_RELEASED){
        if (event.mouse.button == GLFW_MOUSE_BUTTON_LEFT){
            can_drag_camera = false;
        }
    }

    // zoom in and zoom out
    if (event.type == GLEQ_SCROLLED){
        int mouse_wheel_delta = -event.scroll.y;
        core::Vector2f camera_size = camera.m_scale;

        camera.m_scale = core::Vector3f(
            camera_size + (camera_size * (float)mouse_wheel_delta * 0.2f ), -1.0f
        );
    }
}
