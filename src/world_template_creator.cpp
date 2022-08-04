#include "world_template_creator.h"
#include "imgui.h"

namespace gui{
    MainMenuBar::MainMenuBar(MapGenerator& map): 
        Gui("MainBar"),
        map(map)
    {}

    void MainMenuBar::load_menu(){
        if (ImGui::BeginMenu("Load")){
            if (ImGui::BeginMenu("Load template")){
                // load all presets from presets folder
                // show as menu item
                for (auto& directory_entry: atlas::iterate_templates_folder()){
                    auto preset_path = directory_entry.path();

                    // on click on preset
                    if (ImGui::MenuItem((const char*)preset_path.filename().u8string().c_str(), nullptr)){
                        // load template
                        map.load_from_template(preset_path);
                        // generate map
                        map.generate(MapGenerator::draw_modes::COLOR_MAP);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }

    void MainMenuBar::save_menu(){
        if (ImGui::BeginMenu("Save")){
            if (ImGui::MenuItem("Save template")){
                show_save_window = true;
            }
            ImGui::EndMenu();
        }
    }

    void MainMenuBar::save_window(ImVec2 window_size){
        static ImVec2 save_window_size;
        ImGui::SetNextWindowPos(ImVec2(window_size.x / 2 - save_window_size.x / 2, window_size.y /2 - save_window_size.y / 2));
        ImGui::Begin("Saving", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Template Name: "); 
        ImGui::SameLine();

        static std::string save_path;

        ImGui::InputText("##SaveInput", &save_path);

        if (ImGui::Button("Save")){
            // save
            map.save_to_template(save_path);
            show_save_window = false;
        }

        save_window_size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void MainMenuBar::draw(core::graphics::Window& window){
        window_size = ImVec2(window.get_size().x, window.get_size().y);

        if (ImGui::BeginMainMenuBar()){
            current_size = ImGui::GetWindowSize();

            save_menu();
            load_menu();

            ImGui::EndMainMenuBar();
        }

        if (show_save_window == true){
            save_window(window_size);
        }
    }
}

namespace scenes{
    WorldTemplateCreator::WorldTemplateCreator(core::graphics::Window& window): 
        Scene(window),
        mainmenu_bar(map_generator),
        map_generator_menu(map_generator, map_viewer, draw_mode),
        biome_menu(map_generator),
        view2d(0,0,0,0),
        map_viewer(map_generator, view2d)
    {

    }

    void WorldTemplateCreator::init(){
        float size = 800.0f;
        int width,height;
        map_viewer.init(m_window);;

        float aspect = (float)1600 / (float)900;

        view2d = core::graphics::View2D(-aspect*size, aspect*size, -size, size, 0.1f, 100.0f);
        view2d.m_position = core::Vector3f(0,0,-10);
    }

    void WorldTemplateCreator::handle_event(core::Event event){
        map_viewer.handle_event(event);
    }

    void WorldTemplateCreator::update(float delta){
        map_viewer.update(delta, m_window);
    }

    void WorldTemplateCreator::draw(core::graphics::Window& window){
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