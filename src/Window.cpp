#include "config.h"
#include "Window.h"

#include <Windows.h>

namespace {

struct WindowPrivate {
    HWND handle;
    WNDCLASSEX cls;
};

LRESULT CALLBACK wndProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(handle, msg, wParam, lParam);
}

} // namespace

Window::Window(std::string title, const RectI& rect)
    : m_impl(nullptr)
{
    initialize(title, rect);
}

Window::Window(std::string title, int x, int y, int width, int height)
    : m_impl(nullptr)
{
    initialize(title, { x, y, width, height });
}

Window::Window(std::string title, int width, int height)
    : m_impl(nullptr)
{
    initialize(title, { 0, 0, width, height });
}

Window::~Window()
{
    if (m_impl)
        delete m_impl;
}

int Window::exec()
{
    ShowWindow(m_impl->handle, SW_SHOWNORMAL);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void* Window::handle() const
{
    return m_impl->handle;
}

std::string Window::title() const
{
    int len = GetWindowTextLength(m_impl->handle);
    std::string ans;
    ans.reserve(len + 1);
    ans.resize(len);
    GetWindowText(m_impl->handle, &ans[0], len + 1);
    return ans;
}

void Window::setTitle(const std::string& title)
{
    SetWindowText(m_impl->handle, title.c_str());
}

Point2DI Window::pos() const
{
    RECT rect;
    GetWindowRect(m_impl->handle, &rect);
    return { rect.left, rect.top };
}

void Window::setPos(const Point2DI& pos)
{
    SetWindowPos(m_impl->handle,
        HWND_TOP,
        pos.x,
        pos.y,
        0, 0,
        SWP_NOSIZE);
}

void Window::setPos(int x, int y)
{
    setPos({ x, y });
}

void Window::setSize(int width, int height)
{
    SetWindowPos(m_impl->handle,
        HWND_TOP,
        0, 0,
        width,
        height,
        SWP_NOMOVE);
}

RectI Window::rect() const
{
    RECT rect;
    GetWindowRect(m_impl->handle, &rect);
    return {
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top
    };
}

void Window::setRect(const RectI& rect)
{
    SetWindowPos(m_impl->handle,
        HWND_TOP,
        rect.x,
        rect.y,
        rect.width,
        rect.height,
        0);
}

void Window::setRect(int x, int y, int width, int height)
{
    setRect({ x, y, width, height });
}

inline void Window::initialize(std::string title, const RectI& rect)
{
    m_impl = new ::WindowPrivate;
    ZeroMemory(&m_impl->cls, sizeof(WNDCLASSEX));

    m_impl->cls.cbSize = sizeof(WNDCLASSEX);
    m_impl->cls.style = CS_HREDRAW | CS_VREDRAW;
    m_impl->cls.lpfnWndProc = ::wndProc;
    m_impl->cls.hInstance = GetModuleHandle(nullptr);
    m_impl->cls.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_impl->cls.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
    m_impl->cls.lpszClassName = "MainWindow";

    RegisterClassEx(&m_impl->cls);

    m_impl->handle = CreateWindowEx(0,
        "MainWindow",
        title.c_str(),
        WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX,
        rect.x, rect.y, rect.width, rect.height,
        nullptr, nullptr,
        m_impl->cls.hInstance,
        nullptr);
}
