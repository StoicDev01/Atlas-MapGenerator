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
        -0,0,0
    );
}

void MapViewer::update(float delta, core::graphics::Window& window){
    // get world mouse map position
    core::Vector2i mouse_pos = window.get_mouse_pos();
    core::Vector2f world_mouse_pos = window.screen_to_world_pos(mouse_pos);
    core::Vector2f difference_mouse_map = (core::Vector3f(world_mouse_pos, 1) - map.m_sprite.m_position);
    core::Vector2u map_texture_size = map.m_texture.get_size();
    core::Vector2f map_sprite_scale = map.m_sprite.m_scale;

    // move camera with mouse
    ImVec2 mouse_delta = ImGui::GetMouseDragDelta();
    //core::Vector2f map_position = map.m_sprite.m_position;
    
    camera.m_position =(
        core::Vector3f(
            camera.m_position.x + mouse_delta.x * delta,
            camera.m_position.y - mouse_delta.y * delta,
            -1
        )
    );
}

void MapViewer::draw(core::graphics::Window& window){
    // draw map sprite
    map.m_sprite.draw();
}

void MapViewer::handle_event(core::Event event){
    // move map
    if (event.type == GLEQ_BUTTON_PRESSED){
        if (event.mouse.button == GLFW_MOUSE_BUTTON_RIGHT){
            ImGui::ResetMouseDragDelta();
        }
    }
    
    // zoom in and zoom out
    if (event.type == GLEQ_SCROLLED){
        int mouse_wheel_delta = -event.scroll.y;
        core::Vector2f camera_size = camera.m_scale;

        camera.m_scale = core::Vector3f(
            camera_size - (camera_size * (float)mouse_wheel_delta * 0.2f ),
            1.0f
        );
    }
}
