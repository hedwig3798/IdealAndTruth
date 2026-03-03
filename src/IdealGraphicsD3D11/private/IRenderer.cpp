#include "pch.h"
#include "IRenderer.h"
#include "D3D11Renderer.h"

IDEALGRAPHICS_DLL IRenderer* CreateD3D11Renderer()
{
    D3D11Renderer* renderer = new D3D11Renderer();
    return renderer;
}

