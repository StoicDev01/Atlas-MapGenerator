#include "map_generator_scene.h"

namespace scenes{
    AtlasScene::AtlasScene(core::graphics::Window& window): 
        Scene(window),
        mainmenu_bar(map_generator),
        map_generator_menu(map_generator, map_viewer, draw_mode),
        biome_menu(map_generator),
        view2d(0,0,0,0),
        map_viewer(map_generator, view2d)
    {

    }

    void AtlasScene::init(){
        float size = 800.0f;
        int width,height;
        map_viewer.init(m_window);;

        float aspect = (float)1600 / (float)900;

        view2d = core::graphics::View2D(-aspect*size, aspect*size, -size, size, 0.1f, 100.0f);
        view2d.m_position = core::Vector3f(0,0,-10);
    }

    void AtlasScene::handle_event(core::Event event){
        map_viewer.handle_event(event);
    }

    void AtlasScene::update(float delta){
        map_viewer.update(delta, m_window);
    }

    void AtlasScene::draw(core::graphics::Window& window){
        window.clear(core::graphics::Color(0,0,0,255));
        glViewport(0, 0, 1600, 900);

        mainmenu_bar.draw(window);
        biome_menu.draw(window);
        map_generator_menu.draw(window);
        map_viewer.draw(window);
        biome_menu.size_offset_x = -map_generator_menu.size.x;

        view2d.set_active();
    }
}
