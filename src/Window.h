#ifndef Window_h
#define Window_h

#include "Geom.h"

#include <string>

namespace {
struct WindowPrivate;
}

class Window {
public:
    Window(std::string title = "Window", const RectI& rect = { 0, 0, 800, 600 });
    Window(std::string title, int x, int y, int width, int height);
    Window(std::string title, int width, int height);
    virtual ~Window();

    int exec();
    void* handle() const;

    std::string title() const;
    void setTitle(const std::string& title);

    Point2DI pos() const;
    void setPos(const Point2DI&);
    void setPos(int x, int y);

    void setSize(int width, int height);

    RectI rect() const;
    void setRect(const RectI&);
    void setRect(int x, int y, int width, int height);

private:
    // Implementation specific
    ::WindowPrivate* m_impl;

    void initialize(std::string title, const RectI& rect);
};

#endif // Window_h