RendererInitializeState = 
{
    Width = 1240,
    Height = 720,

    Device = 
    {
        D3D11_CREATE_DEVICE_FLAG["D3D11_CREATE_DEVICE_BGRA_SUPPORT"],
        D3D11_CREATE_DEVICE_FLAG["D3D11_CREATE_DEVICE_DEBUG"],
    },

    DepthStancil = 
    {
        mipLevel = 1,
        arraySize = 1,
        format = DXGI_FORMAT["DXGI_FORMAT_D24_UNORM_S8_UINT"],
        sampleCount = 1,
        sampleQuality = 0,
        usage = DXGI_USAGE["DXGI_USAGE_RENDER_TARGET_OUTPUT"],
    },

    SwapCahin = 
    {
        refreshRateNumerator = 60,
        refreshRateDenominator = 1,
        format = DXGI_FORMAT["DXGI_FORMAT_R8G8B8A8_UNORM"],
        scanlineOrdering = DXGI_MODE_SCANLINE_ORDER["DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED"],
        scaling = DXGI_MODE_SCANLINE_ORDER["DXGI_MODE_SCALING_UNSPECIFIED"],
        sampleCount = 1,
        sampleQuality = 0,
        bufferCount = 1,
        swapEffect = DXGI_SWAP_EFFECT["DXGI_SWAP_EFFECT_DISCARD"],
        isWindowed = true,
    },

    RaseterizerState = 
    {
        fillMode = D3D11_FILL_MODE["D3D11_FILL_SOLID"],
        cullMode = D3D11_FILL_MODE["D3D11_CULL_BACK"],
        isFrontCCW = true,
        isDepthClip = true,
    },
}