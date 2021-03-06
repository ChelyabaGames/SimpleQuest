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
    ID3D11RenderTargetView* backBuffer;
};

} // impl

namespace {

void initializeD3D(impl::Renderer* impl, Window* wnd)
{
    // Direct3D initializarion
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1; // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = wnd->width();
    scd.BufferDesc.Height = wnd->height();
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = static_cast<HWND>(wnd->handle());
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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

    // Set render target
    ID3D11Texture2D* backBufferTexture;
    impl->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
    impl->device->CreateRenderTargetView(backBufferTexture, nullptr, &impl->backBuffer);
    backBufferTexture->Release();

    impl->context->OMSetRenderTargets(1, &impl->backBuffer, nullptr);
}

void finalizeD3D(impl::Renderer* impl)
{
    impl->swapChain->SetFullscreenState(false, nullptr);

    impl->swapChain->Release();
    impl->backBuffer->Release();
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
        ::initializeD3D(m_impl, m_window);
        setupViewport();
    }
}

void Renderer::draw()
{
    m_impl->context->ClearRenderTargetView(m_impl->backBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
    m_impl->swapChain->Present(0, 0);
}

void Renderer::setupViewport()
{
    // Viewport setup
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.Width = static_cast<FLOAT>(m_window->width());
    viewport.Height = static_cast<FLOAT>(m_window->height());

    m_impl->context->RSSetViewports(1, &viewport);
}
