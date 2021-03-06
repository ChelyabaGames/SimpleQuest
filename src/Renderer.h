#ifndef Renderer_h
#define Renderer_h

namespace impl {
struct Renderer;
}

class Window;
class Renderer {
public:
    Renderer();
    ~Renderer();

    void connect(Window*);
    void draw();

private:
    void setupViewport();

    impl::Renderer* m_impl;
    Window *m_window;
};

#endif // Renderer_h
