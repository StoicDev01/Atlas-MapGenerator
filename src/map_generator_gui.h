#pragma once

#include "core/graphics/window.h"
#include "gui.h"
#include "map_generator.h"
#include "map_viewer.h"
#include "atlas_utils.h"

namespace ImGui{
    void HelpMarker(std::string info);
}


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

    class MainMenuBar : public Gui {
        public:

        enum class SaveTypes {
            IMAGE,
            TEMPLATE
        };

        SaveTypes save_type;

        ImVec2 current_size;
        ImVec2 window_size;
        bool show_save_window = false;


        MainMenuBar(::MapGenerator& map);

        void load_menu();
        void save_menu();
        void save_window(ImVec2 window_size);
        void draw(core::graphics::Window& window);
        
        private:
        ::MapGenerator& map;
    };
}
