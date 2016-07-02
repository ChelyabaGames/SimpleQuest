#include "Window.h"
#include "Renderer.h"

int main() {
    Window::setInitParams("Direct3D", { 1280, 720 });
    Renderer renderer;
    renderer.connect(&Window::singleton());
    return Window::singleton().exec();
}
