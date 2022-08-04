#pragma once
#include "map_generator.h"
#include "core/graphics/graphics.h"

class MapViewer;

// Gui to view map sprite
class MapViewer : public Gui{
    public:
    core::Vector2i mouse_pos;
    core::Vector2i mouse_map_position;

    bool first = true;
    bool mouse_in_map = false;
    bool show_grid = false;
    bool draw_chunk = false;
    bool show_chunk_map = false;
    bool can_drag_camera = false;
    const char* view_mode = "map";
    
    MapViewer(MapGenerator& map, core::graphics::View2D& camera);

    void init(core::graphics::Window& window);
    void draw(core::graphics::Window& window);
    void update(float delta, core::graphics::Window& window);
    void destroy();

    void handle_event(core::Event event);
    
    private:
    core::graphics::View2D& camera;
    MapGenerator& map;
    //WorldChunkRenderer& chunk_renderer;

};

