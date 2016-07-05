#include "config.h"
#include "Window.h"

#include "Renderer.h"

#include <Windows.h>

namespace impl {

struct Window {
    HWND handle;
    WNDCLASSEX cls;
    DWORD winStyle;

    RECT clientRect() const
    {
        RECT rect;
        GetClientRect(handle, &rect);
        return rect;
    }
};

}

namespace {

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

std::string Window::s_initTitle = "Window";
SizeI Window::s_initSize = { 800, 600 };
std::unique_ptr<Window, Window::Deleter> Window::s_instance;

void Window::setInitParams(std::string title, const SizeI& size)
{
    s_initTitle = title;
    s_initSize = size;
}

Window& Window::singleton()
{
    if (!s_instance)
        s_instance.reset(new Window(s_initTitle, s_initSize));
    return *s_instance;
}

Window::Window(std::string title, const SizeI& size)
    : m_impl(new impl::Window)
    , m_renderer(nullptr)
{
    ZeroMemory(&m_impl->cls, sizeof(WNDCLASSEX));

    m_impl->cls.cbSize = sizeof(WNDCLASSEX);
    m_impl->cls.style = CS_HREDRAW | CS_VREDRAW;
    m_impl->cls.lpfnWndProc = ::wndProc;
    m_impl->cls.hInstance = GetModuleHandle(nullptr);
    m_impl->cls.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_impl->cls.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
    m_impl->cls.lpszClassName = "MainWindow";
    RegisterClassEx(&m_impl->cls);

    m_impl->winStyle = WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX;
    RECT winRect = { 0, 0, size.width, size.height };
    AdjustWindowRect(&winRect, m_impl->winStyle, false);
    m_impl->handle = CreateWindowEx(0,
        "MainWindow",
        title.c_str(),
        m_impl->winStyle,
        CW_USEDEFAULT,
        0,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        nullptr, nullptr,
        m_impl->cls.hInstance,
        nullptr);
}

Window::~Window()
{
    delete m_impl;
}

void Window::show()
{
    ShowWindow(m_impl->handle, SW_SHOWNORMAL);
}

int Window::exec()
{
    if (!m_renderer)
        throw std::runtime_error("NULL renderer");
    show();
    MSG msg;
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT)
            break;
        m_renderer->draw();
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
    POINT winPoint = { 0, 0 };
    ClientToScreen(m_impl->handle, &winPoint);
    return { winPoint.x, winPoint.y };
}

void Window::setPos(const Point2DI& pos)
{
    RECT winRect = { pos.x, pos.y, 0, 0 };
    AdjustWindowRect(&winRect, m_impl->winStyle, false);
    SetWindowPos(m_impl->handle,
        HWND_TOP,
        winRect.left,
        winRect.top,
        0, 0,
        SWP_NOSIZE);
}

void Window::setPos(int x, int y)
{
    setPos({ x, y });
}

SizeI Window::size() const
{
    RECT winRect = m_impl->clientRect();
    return { winRect.right, winRect.bottom };
}

int Window::width() const
{
    return m_impl->clientRect().right;
}

int Window::height() const
{
    return m_impl->clientRect().bottom;
}

void Window::resize(const SizeI& size)
{
    RECT winRect = { 0, 0, size.width, size.height };
    AdjustWindowRect(&winRect, m_impl->winStyle, false);
    SetWindowPos(m_impl->handle,
        HWND_TOP,
        0, 0,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        SWP_NOMOVE);
}

void Window::resize(int width, int height)
{
    resize({ width, height });
}

RectI Window::rect() const
{
    POINT winPoint = { 0, 0 };
    ClientToScreen(m_impl->handle, &winPoint);
    RECT winRect;
    GetClientRect(m_impl->handle, &winRect);
    return {
        winPoint.x,
        winPoint.y,
        winPoint.x + winRect.right,
        winPoint.y + winRect.bottom
    };
}

void Window::setRect(const RectI& rect)
{
    RECT winRect = { rect.left, rect.top, rect.right, rect.bottom };
    AdjustWindowRect(&winRect, m_impl->winStyle, false);
    SetWindowPos(m_impl->handle,
        HWND_TOP,
        winRect.left,
        winRect.top,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        0);
}

void Window::setRect(int x, int y, int width, int height)
{
    setRect({ x, y, x + width, y + height });
}

void Window::connect(Renderer* renderer)
{
    if (m_renderer != renderer) {
        m_renderer = renderer;
        renderer->connect(this);
    }
}
