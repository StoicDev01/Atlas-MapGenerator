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
        map_viewer.init(m_window);;

        core::Vector2u window_size = m_window.get_size();

        view2d.create(
            -(window_size.x / 2.0),
             (window_size.x / 2.0),
            -(window_size.y / 2.0),
             (window_size.y / 2.0)
        );

        //map_generator.m_sprite.m_rotation = glm::quat(core::Vector3f(glm::radians(180.0f),0,0));
        view2d.m_position = core::Vector3f(500,-180,0);

        // invert Z axis so the view point towards positive Z axis
        view2d.m_scale = core::Vector3f(1.8,1.8, -1.0);
    }

    void AtlasScene::handle_event(core::Event event){
        map_viewer.handle_event(event);

        if (event.type == GLEQ_WINDOW_RESIZED){
            view2d.create(
                -(event.size.width / 2.0),
                 (event.size.width / 2.0),
                -(event.size.height / 2.0),
                 (event.size.height / 2.0)
            );
        }
    }

    void AtlasScene::update(float delta){
        map_viewer.update(delta, m_window);
    }

    void AtlasScene::draw(core::graphics::Window& window){
        window.clear(core::graphics::Color(0,0,0,255));

        mainmenu_bar.draw(window);
        biome_menu.draw(window);
        map_generator_menu.draw(window);
        map_viewer.draw(window);
        biome_menu.size_offset_x = -map_generator_menu.size.x;

        view2d.set_active();
    }
}
