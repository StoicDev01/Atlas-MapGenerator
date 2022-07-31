#pragma once

#include "core/graphics/window.h"
#include "gui.h"
#include "map_generator.h"
#include "map_viewer.h"


namespace gui{

    class MapGeneratorMenu : public Gui{
        public: 
        MapGeneratorMenu(MapGenerator& p_map, MapViewer& p_map_viewer, int& draw_mode);
        void draw(core::graphics::Window& window);

        ImVec2 start_position;
        ImVec2 size;

        private:
        MapGenerator& map;
        MapViewer& map_viewer;

        int& current_draw_mode;
    };

    class BiomesMenu : public Gui{
        public:
        BiomesMenu(MapGenerator& p_map_generator);
        void draw(core::graphics::Window& window);

        ImVec2 start_position;
        float size_offset_x = 0;
        
        private:
        MapGenerator& map;
    };
}
