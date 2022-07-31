#include "gui.h"

/* Gui */
Gui::Gui(const char* label){
    this->label = label;
    visible = true;
    initialized = false;
}

void Gui::AlignForWidth(float width, float alignment)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

// Executed on init or after an window size change
void Gui::init(core::graphics::Window& window){
    
}

// Executed every frame
void Gui::draw(core::graphics::Window& window){

}

/* GuiManager */
GuiManager::GuiManager(){
    initialized = false;
    log_str = "[ GuiManager ] ";
}

void GuiManager::add_gui(Gui* gui){
    gui_list.push_back(gui);
}

void GuiManager::remove_gui(Gui* gui){
    for (auto it = gui_list.begin(); it != gui_list.end(); it++){
        // If Gui in gui_list
        if ((*it) == gui){
            // Add to remove queue
            to_remove.push_back((gui));
        }
    }
}

bool GuiManager::is_in_remove_queue(Gui* gui){
    for (auto rm = to_remove.begin(); rm != to_remove.end(); rm++){
        if (gui == *rm){
            to_remove.erase(rm);
            return true;
        }
    }
    return false;
}

void GuiManager::init(core::graphics::Window& window){
    std::cout << log_str << "Initializing Gui Manager" << std::endl;

    for (auto it = gui_list.begin(); it != gui_list.end(); it++){
        
        if ((*it)->visible){
            std::cout << log_str << "Initializing Gui: " << (*it)->label << std::endl;
            (*it)->init(window);
            (*it)->initialized = true;
        }
    }
    this->initialized = true;
}

void GuiManager::draw(core::graphics::Window& window){
    // If GuiManager is not initialized
    if (!initialized){
        // Initialize all gui
        init(window);
        return;
    }
    auto gui = gui_list.begin();

    // Gui loop
    while (gui != gui_list.end()){
        
        // If in remove queue
        if (is_in_remove_queue(*gui)){
            // Remove gui
            gui = gui_list.erase(gui);
        }
        else{
            // Continue loop
            if (!(*gui)){
                gui++;
                return;
            }

            if ((*gui)->visible ){
                // if this gui is not initialized
                if ((*gui)->initialized == false){
                    (*gui)->init(window);
                    (*gui)->initialized = true;
                }
                // Update
                else{
                    (*gui)->draw(window);
                }
            }
            gui++;
        }

    }
}
