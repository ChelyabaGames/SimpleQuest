#include "config.h"
#include "Renderer.h"

#include "Window.h"

#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

namespace impl {

struct Renderer {
    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
};

} // impl

namespace {

void initializeD3D(impl::Renderer* impl, HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1; // двойная буферизация
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;

    D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &impl->swapChain,
        &impl->device,
        nullptr,
        &impl->context);
}

void finalizeD3D(impl::Renderer* impl)
{
    impl->swapChain->Release();
    impl->device->Release();
    impl->context->Release();
}

} // namespace

Renderer::Renderer()
    : m_impl(new impl::Renderer)
    , m_window(nullptr)
{ }

Renderer::~Renderer()
{
    if (m_window)
        ::finalizeD3D(m_impl);
    delete m_impl;
}

void Renderer::connect(Window* window)
{
    if (m_window != window) {
        m_window = window;
        window->connect(this);
        ::initializeD3D(m_impl, static_cast<HWND>(window->handle()));
    }
}
