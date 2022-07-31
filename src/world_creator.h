#pragma once
#include "core/core.h"
#include "map_generator.h"
#include "map_viewer.h"

bool save_world(std::string name){
    std::filesystem::path worlds_path("./data/worlds");

    std::filesystem::path world_path = worlds_path/ name;

    if (std::filesystem::is_directory(world_path) == false){
        if (!std::filesystem::create_directories(world_path)){
            std::cout << " [ Error ] could not create world directory" << std::endl;
            return false;
        }
    }

    return true;
}



// World Creator Scene
namespace scenes{
    class WorldCreator : public core::Scene{
        public:
        bool first = true;
        bool can_move_map = true;
        bool show_grid = false;
        bool selecting_template = true;

        ImVec2 mouse_delta;

        MapGenerator map;
        MapViewer map_viewer;
        core::graphics::View2D camera;

        WorldCreator(core::graphics::Window& window): 
            Scene(window, "World Creator"),
            camera(-(1600 / 2.0), (1600 / 2.0), -(900 / 2.0), (900 / 2.0), 0.1, 1000.0f),
            map_viewer(map, camera)
        {}
        
        /* Classes */

        // Gui to load Template
        class LoadTemplateGui: public Gui{
            public:
            ImVec2 size;

            LoadTemplateGui(WorldCreator& parent): Gui{"Load Template"}, parent(parent){}

            void init(core::graphics::Window& window){
            }

            void draw(core::graphics::Window& window){
                static std::string current_template;

                ImGui::SetNextWindowPos(
                    ImVec2(
                        window.get_size().x / 2 - size.x / 2,
                        window.get_size().y / 2 - size.y / 2 
                    )
                );

                ImGui::Begin(label, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

                ImGui::Text("Choose the world Template: "); ImGui::SameLine();
                if (ImGui::BeginCombo("##world_template", current_template.c_str())){
                    
                    for (auto& entry : iterate_templates_folder()){
                        auto template_path = entry.path();
                        std::string current_filename = template_path.filename();

                        bool is_selected = (current_template == current_filename);

                        if (ImGui::Selectable(current_filename.c_str(), is_selected)){
                            current_template = current_filename;
                        }

                        if (is_selected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    
                    ImGui::EndCombo();
                }
                
                AlignForWidth(
                    ImGui::CalcTextSize("Select").x + ImGui::GetStyle().FramePadding.x * 2,
                    1.0f
                );

                if (ImGui::Button("Select")){
                    std::filesystem::path templates_path("./data/world templates");
                    std::filesystem::path current_template_path = templates_path / current_template;

                    parent.map.load_from_template(current_template_path);
                    parent.map.generate("Biome map");
                    
                    parent.selecting_template = false;
                }


                size = ImGui::GetWindowSize();
                ImGui::End();
            }

            private:
            WorldCreator& parent;
        };

        // Main Menu Bar Gui
        class MainMenuBar : public Gui {
            public:
            ImVec2 current_size;
            MapGenerator* map;

            MainMenuBar(WorldCreator& parent): Gui{"MainBar"}, parent(parent){

            }

            void draw(core::graphics::Window& window){
                static bool show_save_window = false;
                ImVec2 window_size = ImVec2(window.get_size().x, window.get_size().y);

                if (ImGui::BeginMainMenuBar()){
                    current_size = ImGui::GetWindowSize();

                    if (ImGui::BeginMenu("Save")){
                        if (ImGui::MenuItem("Save world")){
                            show_save_window = true;
                        }
                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Load")){
                        if (ImGui::BeginMenu("Load World")){
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMainMenuBar();
                }

                if (show_save_window == true){
                    static ImVec2 save_window_size;

                    ImGui::SetNextWindowPos(ImVec2(window_size.x / 2 - save_window_size.x / 2, window_size.y /2 - save_window_size.y / 2));
                    ImGui::Begin("Saving", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                    ImGui::Text("World Name: "); ImGui::SameLine();            

                    static std::string world_name;

                    ImGui::InputText("##SaveInput", &world_name);

                    if (ImGui::Button("Save")){
                        // save
                        save_world(world_name);
                        show_save_window = false;
                    }

                    save_window_size = ImGui::GetWindowSize();
                    ImGui::End();
                }

            }
            private:
            WorldCreator& parent;
        };

        // Main panel with contents
        class MainPanel : public Gui{
            public:

            ImVec2 size;
            ImVec2 start_position;
            core::RandomMachine random_machine;

            MainPanel(WorldCreator& parent): Gui{"Main panel"}, parent(parent){

            }

            void draw(core::graphics::Window& window){
                ImVec2 window_size = ImVec2(window.get_size().x, window.get_size().y);

                ImGui::SetNextWindowPos(ImVec2(window_size.x  - size.x, start_position.y));
                ImGui::SetNextWindowSizeConstraints(ImVec2(400, window_size.y), ImVec2(600, window_size.y));
                ImGui::Begin("Main", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

                static std::string world_name;

                ImGui::Text("Seed : "); ImGui::SameLine(); ImGui::InputInt("##IntSeed", &parent.map.m_settings.m_seed_input);
                ImGui::SameLine();

                AlignForWidth(ImGui::CalcTextSize("Randomize").x + ImGui::GetStyle().FramePadding.x * 2, 1.0f);

                if (ImGui::Button("Randomize")){
                    parent.map.m_settings.m_seed_input = random_machine.rand_int(INT_MIN, INT_MAX);
                }

                if (ImGui::Button("Generate")){
                    parent.map.generate("Biome map");
                }

                size = ImGui::GetWindowSize();
                ImGui::End();
            }
            private:
            WorldCreator& parent;
        };

        void destroy(){

        }
        
        void init(){
        }

        void handle_event(core::Event event){
            map_viewer.handle_event(event);
        }

        void update(float delta){
            if (!selecting_template){
                map_viewer.update(delta, m_window);
            }
        }

        void draw(core::graphics::Window& window){
            // clear background
            window.clear(core::graphics::Color(0,0,0,255));
            // update camera
            camera.set_active();

            static MainMenuBar main_menu_bar(*this);

            // choose template
            if (selecting_template == true){
                static LoadTemplateGui load_template_gui(*this);

                load_template_gui.draw(window);
            }
            else{
                /// draw map
                map_viewer.draw(window);

                /// draw main menu panel
                static MainPanel main_panel(*this);
                main_panel.start_position = main_menu_bar.current_size;
                main_panel.draw(window);
            }

            main_menu_bar.draw(window);
        }
    };
}
