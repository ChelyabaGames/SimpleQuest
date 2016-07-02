#include "Window.h"

int main() {
    Window::setInitParams("Direct3D", { 1280, 720 });
    return Window::singleton().exec();
}
