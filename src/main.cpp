#include "core/core.h"
#include "world_template_creator.h"

#include "GLFW/glfw3.h"
#include "thread"

core::graphics::Window window(1600, 900, "Atlas");
core::Scene* current_scene;

void logic(){

    std::clock_t end = 0;

    while (window.is_open())
    {
        std::clock_t start = std::clock();
        double delta = (double)(start - end) / CLOCKS_PER_SEC;
        end = std::clock();

        current_scene->update(delta);
    }
}

void client(){

    while (window.is_open()){
        core::Event event;

        glfwPollEvents();

        while (window.poll_event(event)){
            
            if (event.type == GLEQ_WINDOW_CLOSED){
                window.close();
            }

            current_scene->handle_event(event);
            core::free_event(event);
        }

        window.new_frame();
        current_scene->draw(window);
        window.render();
    }
}

int main(){
    window.create_context();

    current_scene = new scenes::WorldTemplateCreator(window);
    current_scene->init();

    std::thread logic_thread(logic);

    client();

    logic_thread.join();
    return 0;
}