#ifndef Window_h
#define Window_h

#include "Geom.h"

#include <memory>
#include <string>

namespace {
struct WindowPrivate;
}

enum MouseButton {
    Left,
    Right
};

class Window {
public:
    static void setInitParams(std::string title, const SizeI& size);
    static Window& singleton();

    void show();
    int exec();
    void* handle() const;

    std::string title() const;
    void setTitle(const std::string& title);

    Point2DI pos() const;
    void setPos(const Point2DI&);
    void setPos(int x, int y);

    SizeI size() const;
    void resize(const SizeI&);
    void resize(int width, int height);

    RectI rect() const;
    void setRect(const RectI&);
    void setRect(int x, int y, int width, int height);

private:
    struct Deleter {
        void operator()(Window* wnd) {
            delete wnd;
        }
    };

    static std::string s_initTitle;
    static SizeI s_initSize;
    static std::unique_ptr<Window, Deleter> s_instance;

    // Implementation specific
    ::WindowPrivate* m_impl;

    Window(std::string title, const SizeI& size);
    Window(const Window&);
    ~Window();
    Window& operator=(const Window&);
};

#endif // Window_h