#pragma once
#include "fmt/format.h"
#include "fstream"

#include "core/core.h"
#include "gui.h"
#include "map_generator.h"
#include "map_generator_gui.h"
#include "map_viewer.h"
#include "atlas_utils.h"
#include <magic_enum.hpp>

namespace scenes{
    class WorldTemplateCreator;
}

namespace gui {
    class MainMenuBar : public Gui {
        public:
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

namespace scenes {
    class WorldTemplateCreator : public core::Scene{
        public:

        // objects
        core::graphics::View2D view2d;
        core::graphics::Sprite sprite;
        core::graphics::Texture sprite_texture;

        MapGenerator map_generator;
        MapViewer map_viewer;

        // gui
        gui::MainMenuBar mainmenu_bar;
        gui::MapGeneratorMenu map_generator_menu;
        gui::BiomesMenu biome_menu;

        int draw_mode = 0;

        const char* contents[4] =  { "Map Generator", "Objects", "Actors", "Races"};
        const char* current_draw = contents[0];

        WorldTemplateCreator(core::graphics::Window& window);

        void init();
        void handle_event(core::Event event);
        void update(float delta);
        void draw(core::graphics::Window& window);
    };
}