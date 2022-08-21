#include "core/system/scene.h"
#include "core/graphics/graphics.h"
#include "map_generator_gui.h"
#include "map_generator.h"
#include "map_viewer.h"

namespace scenes{

    class AtlasScene : public core::Scene{
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

        AtlasScene(core::graphics::Window& window);

        void init();
        void handle_event(core::Event event);
        void update(float delta);
        void draw(core::graphics::Window& window);
    };
}
