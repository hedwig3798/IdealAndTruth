WindowSetting = 
{
    -- 나중에 추가할거 있으면 추가할 것
    Width = 1024,
    Height = 720,
}

-- default textuer color (float : 0 ~ 1)
DefaultTextuerSetting = 
{
    Diffuse = {0.5, 0.5, 0.5},
    Noraml = {0.5, 0.5, 1.0},
    Roughness = {0.5, 0.5, 0.5},
    Metalic = {0.0, 0.0, 0.0},
}

-- 렌더 
RendererInitializeState = 
{
    -- 초기 렌더 벡터의 크기
    RenderVectorSize = 1000,

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
        format = DXGI_FORMAT["DXGI_FORMAT_R8G8B8A8_UNORM_SRGB"],
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
        cullMode = D3D11_CULL_MODE["D3D11_CULL_BACK"],
        isFrontCCW = true,
        isDepthClip = true,
    },

    RenderTargetViewState = 
    {
        mipLevel = 1,
        arraySize = 1,
        format = DXGI_FORMAT["DXGI_FORMAT_R8G8B8A8_UNORM"],
        sampleCount = 1,
        usage = DXGI_USAGE["D3D11_USAGE_DEFAULT"],
        bindFlags = 
        {
            D3D11_BIND_FLAG["D3D11_BIND_RENDER_TARGET"],
            D3D11_BIND_FLAG["D3D11_BIND_SHADER_RESOURCE"],
        },
        viewDimension = D3D_SRV_DIMENSION["D3D11_SRV_DIMENSION_TEXTURE2D"],

    },
}