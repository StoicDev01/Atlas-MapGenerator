#include "core/gameObject.h"
#include "core/graphics/view.h"
#include "imgui.h"

class FreeCamera : public Camera {
    public:
    
    void handle_event(sf::Time delta, sf::Event event);
    void update(sf::Time delta);
};

void FreeCamera::handle_event(sf::Time delta, sf::Event event){
    // move map
    if (event.type == sf::Event::MouseButtonReleased){
        if (event.mouseButton.button == sf::Mouse::Right){
            ImGui::ResetMouseDragDelta();
        }
    }
    
    // zoom in and zoom out
    if (event.type == sf::Event::MouseWheelMoved){
        int mouse_wheel_delta = event.mouseWheel.delta;
        GameObject& game = GameObject::Get();
        sf::Vector2f camera_size = getSize();
        setSize(camera_size - (camera_size * (float)mouse_wheel_delta) * delta.asSeconds() * (float)100);
    }
}

void FreeCamera::update(sf::Time delta){
    // move camera with mouse
    ImVec2 mouse_delta = ImGui::GetMouseDragDelta();
    
    setCenter(
        sf::Vector2f(
            getCenter().x - mouse_delta.x * delta.asSeconds() * 1,
            getCenter().y - mouse_delta.y * delta.asSeconds() * 1
        )
    );
}
