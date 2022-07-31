#pragma once

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "fmt/format.h"

#include <math.h>
#include <vector>

#include "core/core.h"
#include "string"

class Gui {
    private:
    public:
        const char* label;
        bool visible;
        bool initialized;

        Gui(const char* label);

        // set alignment for width
        void AlignForWidth(float width, float alignment = 0.5f);

        // Executed on init or after an window size change
        virtual void init(core::graphics::Window& window);

        // Executed every frame
        virtual void draw(core::graphics::Window& window);
};

class GuiManager {
    private:
        bool initialized;

    public:
        std::vector<Gui*> gui_list;
        std::vector<Gui*> to_remove;

        const char* log_str;

        GuiManager();

        void add_gui(Gui* gui);

        void remove_gui(Gui* gui);

        template <typename T>
        bool has_gui_type(){
            for (auto gui = gui_list.begin(); gui != gui_list.end(); gui++){
                if ( typeid(*(*gui)) == typeid(T)){
                    return true;
                }
            }
            return false;
        }

        template <typename T>
        Gui* get_gui_type(){
            for (auto gui = gui_list.begin(); gui != gui_list.end(); gui++){
                if ( typeid(*(*gui)) == typeid(T)){
                    return (*gui);
                }
            }
            return nullptr;
        }


        bool is_in_remove_queue(Gui* gui);

        void init(core::graphics::Window& window);
        void draw(core::graphics::Window& window);
};
