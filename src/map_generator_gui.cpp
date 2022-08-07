#include "map_generator_gui.h"

namespace gui{

    // MapGenerator Menu
    MapGeneratorMenu::MapGeneratorMenu(MapGenerator& p_map, MapViewer& p_map_viewer, int& draw_mode) :
        Gui("MapGeneratorMenu"),
        map(p_map),
        map_viewer(p_map_viewer),
        current_draw_mode(draw_mode)
    {

    }

    void MapGeneratorMenu::draw(core::graphics::Window& window) {
        core::Vector2u window_size = window.get_size();
        core::Vector2i mouse_map_position = map_viewer.mouse_map_position;

        const char* fallof_types[] = {"Square", "Circle"};
        map.m_settings.m_fallof_type = fallof_types[map.m_settings.m_current_fallof_type];

        ImGui::SetNextWindowPos(ImVec2(((float)window_size.x ) - (size.x), start_position.y));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, window_size.y), ImVec2(600, window_size.y));
        ImGui::Begin(label, nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

        if (ImGui::CollapsingHeader("Generation settings")){
            const char* draw_modes_string[] = {"Color Map", "Noise", "Temperature Map", "Humidity Map", "Fallof"};

            if (ImGui::TreeNode("Noise settings")){
                ImGui::Text("Draw Mode  : "); ImGui::SameLine(); ImGui::Combo("##DrawMode", &current_draw_mode, draw_modes_string, IM_ARRAYSIZE(draw_modes_string));

                ImGui::Text("Seed       : "); ImGui::SameLine(); ImGui::InputInt("##seedInput", &map.m_settings.m_seed_input);
                ImGui::Text("Octaves    : "); ImGui::SameLine(); ImGui::SliderInt("##octavesslider", &map.m_settings.m_octaves_slider, 0, 10);
                ImGui::Text("Frequency  : "); ImGui::SameLine(); ImGui::SliderFloat("##frequencySlider", &map.m_settings.m_frequency_slider, 0 , 1);
                ImGui::Text("Gain       : "); ImGui::SameLine(); ImGui::SliderFloat("##gainSlider", &map.m_settings.m_gain_slider, 0, 1);
                ImGui::Text("Lacunarity : "); ImGui::SameLine(); ImGui::SliderFloat("##lacunaritySlider", &map.m_settings.m_lacunatiry_slider, 0, 100);
                ImGui::Text("WeightedStr: "); ImGui::SameLine(); ImGui::SliderFloat("##weightedStrSlider", &map.m_settings.m_weightedStr_slider, 0, 5);
                ImGui::Text("PingPongStr: "); ImGui::SameLine(); ImGui::SliderFloat("##PingPongStrSlider", &map.m_settings.m_pingPongStr_slider, 0, 100);

                if (ImGui::RadioButton("Use map fallof ", map.m_settings.m_use_fallof_map)){
                    map.m_settings.m_use_fallof_map = !map.m_settings.m_use_fallof_map;
                }

                if (map.m_settings.m_use_fallof_map){
                    ImGui::Text("Fallof type: "); ImGui::SameLine(); ImGui::Combo("##FallofType", &map.m_settings.m_current_fallof_type, fallof_types, IM_ARRAYSIZE(fallof_types));
                    ImGui::Text("Fallof A   : "); ImGui::SameLine(); ImGui::InputFloat("##FallofA", &map.m_settings.m_fallof_a, 0, 5);
                    ImGui::Text("Fallof B   : "); ImGui::SameLine(); ImGui::SliderFloat("##FallofB", &map.m_settings.m_fallof_b, 0, 5);
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Elevation settings")){
                ImGui::Text("Min elevation                "); ImGui::SameLine(); ImGui::SliderFloat("##maxel", &map.m_settings.m_min_elevation, 0, 500);
                ImGui::Text("Max elevation                "); ImGui::SameLine(); ImGui::SliderFloat("##minel", &map.m_settings.m_min_elevation, 0, 500);
                ImGui::Text("sea level                    "); ImGui::SameLine(); ImGui::SliderFloat("##sealevel", &map.m_settings.m_sea_level, map.m_settings.m_min_elevation, map.m_settings.m_max_elevation);
                ImGui::TreePop();

            }

            if (ImGui::TreeNode("Temperature Settings")){
                ImGui::Text("min temperature latitude      "); ImGui::SameLine(); ImGui::SliderFloat("##mtl", &map.m_settings.m_min_temperature_latitude, -1000, 1000);
                ImGui::Text("max temperature latitude      "); ImGui::SameLine(); ImGui::SliderFloat("##xtl", &map.m_settings.m_max_temperature_latitude, -1000, 1000);
                ImGui::Text("latitude fallof a             "); ImGui::SameLine(); ImGui::SliderFloat("##lfa", &map.m_settings.m_latitude_a, 0, 5);
                ImGui::Text("latitude fallof b             "); ImGui::SameLine(); ImGui::SliderFloat("##lfb", &map.m_settings.m_latitude_b, 0, 5);
                ImGui::Text("Temperature value a           "); ImGui::SameLine(); ImGui::SliderFloat("##tva", &map.m_settings.m_temperature_a, 0, 1);
                ImGui::Text("Temperature value b           "); ImGui::SameLine(); ImGui::SliderFloat("##tvb", &map.m_settings.m_temperature_b, 0, 2);
                ImGui::Text("Temperature altitude increment"); ImGui::SameLine(); ImGui::SliderFloat("##tai", &map.m_settings.m_temperature_altitude_increment, 0, 5);
                ImGui::Text("max world temperature         "); ImGui::SameLine(); ImGui::SliderFloat("##xwt", &map.m_settings.m_max_temperature, -1000, 1000);
                ImGui::Text("min world temperature         "); ImGui::SameLine(); ImGui::SliderFloat("##mwt", &map.m_settings.m_min_temperature, -1000, 1000);
                ImGui::Text("temperature noise scale       "); ImGui::SameLine(); ImGui::SliderFloat("##tns", &map.m_settings.m_temperature_noise_scale, 0, 2);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Humidity Settings")){
                ImGui::Text("humidity temperature increment"); ImGui::SameLine(); ImGui::SliderFloat("##hti", &map.m_settings.m_humidity_temperature_increment, 0.0f, 5.0f);
                ImGui::Text("humidity noise scale          "); ImGui::SameLine(); ImGui::SliderFloat("##hns", &map.m_settings.m_humidity_noise_scale, 0.0f, 5.0f);
                ImGui::TreePop();
            }

            ImGuiStyle& style =ImGui::GetStyle();

            this->AlignForWidth(
                ImGui::CalcTextSize("Generate").x + style.FramePadding.x * 2
            );

            if (ImGui::Button("Generate")){
                map.generate((MapGenerator::draw_modes)current_draw_mode);
            }
        }

        if (ImGui::CollapsingHeader("Informations")){
            // display temperature in position
            float temperature_in_mouse_pos  = map.m_temperature_map[mouse_map_position.x][mouse_map_position.y];
            float humidity_in_mouse_pos     = map.m_humidity_map[mouse_map_position.x][mouse_map_position.y];
            float temperature_in_celcius    = map.temperature_to_celcius(temperature_in_mouse_pos);

            ImGui::Text("Temperature : "); ImGui::SameLine(); 
            ImGui::PushItemWidth(60.0f);
            ImGui::InputFloat("##Temperature", &temperature_in_mouse_pos,0.0f,0.0f,"%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine(); 

            std::string celcius_temperature;
            celcius_temperature.append(std::to_string(temperature_in_celcius));
            celcius_temperature.append("°C");
            ImGui::Text(celcius_temperature.c_str());

            // display humidity
            ImGui::Text("Humidity    : "); ImGui::SameLine();
            ImGui::InputFloat("##Humidity", &humidity_in_mouse_pos,0.0f,0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

            // display elevation and height in position
            float height_in_mouse_pos = map.m_height_map[mouse_map_position.x][mouse_map_position.y];
            float elevation = map.height_to_elevation(height_in_mouse_pos);

            ImGui::Text("Height      : "); ImGui::SameLine();
            ImGui::InputFloat("##height", &height_in_mouse_pos, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();

            std::string elevation_string;
            elevation_string.append(std::to_string(elevation));
            elevation_string.append("e");
            ImGui::Text(elevation_string.c_str());

            // display biome in position
            Biome* biome_in_position = map.m_biome_map[mouse_map_position.x][mouse_map_position.y];
            ImGui::Text("Biome       : "); ImGui::SameLine();
            if (biome_in_position){
                ImGui::Text(biome_in_position->name.c_str());
            }
            else{
                ImGui::Text("Null");
            }

        }

        size = ImGui::GetWindowSize();
        ImGui::End();
    }

    // Biomes Menu
    BiomesMenu::BiomesMenu(MapGenerator& p_map_generator) :
        Gui("BiomesMenu"),
        map(p_map_generator)
    {

    }

    void BiomesMenu::draw(core::graphics::Window& window){
        core::Vector2u window_size = window.get_size();

        static float biomes_window_size_y = 0;
        ImGui::SetNextWindowSizeConstraints(
            ImVec2(window_size.x + size_offset_x - start_position.x, 0),
            ImVec2(window_size.x + size_offset_x - start_position.x, 400)
        );

        ImGui::SetNextWindowPos(
            ImVec2(start_position.x, window_size.y - biomes_window_size_y)
        );

        ImGui::Begin(label, nullptr, ImGuiWindowFlags_NoMove );

        // sort biomes
        std::sort(map.m_biomes.begin(), map.m_biomes.end(), compareBiomesIndex);
        // loop trough all biomes
        int i = 0;

        for (Biome& biome : (map.m_biomes)){
            std::string selectable_label = fmt::format("##{}{}", biome.name, biome.index);
            ImGui::Selectable(selectable_label.c_str());

            if (ImGui::IsItemActive() && !ImGui::IsItemHovered()){
                int index_next = i + (ImGui::GetMouseDragDelta(0).y < 0.0f ? -1 : 1);
                if (index_next >= 0 && index_next < map.m_biomes.size()){
                    Biome* next = map.get_biome_by_index(index_next);
                    if (next){
                        next->index = i;
                    }
                    
                    biome.index = index_next;

                    ImGui::ResetMouseDragDelta();
                }
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SameLine();

            // biome name
            ImGui::PushItemWidth(200);
            std::string text_label = fmt::format("##{}{}_input_name", biome.name, biome.index);
            
            static std::string current_biome_name;
            current_biome_name = biome.name;
            if (ImGui::InputText(text_label.c_str(), &current_biome_name, ImGuiInputTextFlags_EnterReturnsTrue)){
                biome.name = current_biome_name;
            }
        
            ImGui::SameLine();

            // Biome min and max elevation
            ImGui::PushItemWidth(50);
            ImGui::Text("Elevation : "); 
            ImGui::SameLine();

            std::string min_label = fmt::format("##{}{}_input_min", biome.name, biome.index);
            ImGui::InputFloat(min_label.c_str(), &biome.height_range.min);
            ImGui::SameLine();

            std::string max_label = fmt::format("##{}{}_input_max", biome.name, biome.index);
            ImGui::InputFloat(max_label.c_str(), &biome.height_range.max);
            ImGui::SameLine();

            // biome min and max temperature
            ImGui::Text("Temperature : "); ImGui::SameLine();
            std::string min_temp_label = fmt::format("##{}{}_input_min_temperature", biome.name, biome.index);
            ImGui::InputFloat(min_temp_label.c_str(), &biome.temperature_range.min);
            ImGui::SameLine();

            std::string max_temp_label = fmt::format("##{}{}_input_max_temperature", biome.name, biome.index);
            ImGui::InputFloat(max_temp_label.c_str(), &biome.temperature_range.max);
            ImGui::SameLine();

            // biome min and max humidity
            ImGui::Text("Humidity : "); ImGui::SameLine();
            std::string min_hum_label = fmt::format("##{}{}_input_min_humidity", biome.name, biome.index);
            ImGui::InputFloat(min_hum_label.c_str(), &biome.humidity_range.min);
            ImGui::SameLine();

            std::string max_hum_label = fmt::format("##{}{}_input_max_humidity", biome.name, biome.index);
            ImGui::InputFloat(max_hum_label.c_str(), &biome.humidity_range.max);
            ImGui::SameLine();

            // get biome color with values from 0 to 1
            std::string color_label = fmt::format("##{}{}_color", biome.name, biome.index);
            float r = inverseLerp(0.0f, 255.0f, (float)biome.color.r); 
            float g = inverseLerp(0.0f, 255.0f, (float)biome.color.g);
            float b = inverseLerp(0.0f, 255.0f, (float)biome.color.b);

            // create imgui color edit
            float color[3] = { r, g, b};
            ImGui::ColorEdit3(color_label.c_str(), color);
            ImGui::SameLine();
            
            // set biome color from 0 to 255
            biome.color = core::graphics::Color((color[0] * 255), (color[1] * 255), (color[2] * 255), 255);

            // remove biome button
            std::string remove_label = fmt::format("REMOVE##{}{}_remove", biome.name, biome.index);
            if (ImGui::Button(remove_label.c_str())){
                for (auto b = map.m_biomes.begin(); b != map.m_biomes.end(); b++){
                    if (&(*b) == &biome){
                        map.m_biomes.erase(b);
                        break;
                    }
                }
            }
            i += 1;
        }
        
        if (ImGui::Button("Add")){
            // Create new undefined biome
            map.add_biome(
                Biome(
                    "Undefined", core::Range<float>(0, 100), 
                    core::Range<float>(0, 100), 
                    core::Range<float>(0, 100), 
                    core::graphics::Color(0, 0, 0, 255)
                )
            );
        }

        biomes_window_size_y = ImGui::GetWindowSize().y;
        ImGui::End();
    }
}
