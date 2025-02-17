
#pragma once

#include <RendererFoundation/Resources/Buffer.h>
#include <dxgi.h>

struct ID3D11Buffer;
struct D3D11_BUFFER_DESC;

class EZ_RENDERERDX11_DLL ezGALBufferDX11 : public ezGALBuffer
{
public:
  static ezResult CreateBufferDesc(const ezGALBufferCreationDescription& description, D3D11_BUFFER_DESC& out_bufferDesc, DXGI_FORMAT& out_indexFormat);

public:
  ID3D11Buffer* GetDXBuffer() const;
  DXGI_FORMAT GetIndexFormat() const;

protected:
  friend class ezGALDeviceDX11;
  friend class ezMemoryUtils;

  ezGALBufferDX11(const ezGALBufferCreationDescription& Description);
  virtual ~ezGALBufferDX11();

  virtual ezResult InitPlatform(ezGALDevice* pDevice, ezArrayPtr<const ezUInt8> pInitialData) override;
  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
  virtual void SetDebugNamePlatform(const char* szName) const override;

protected:
  ID3D11Buffer* m_pDXBuffer = nullptr;
  DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_UNKNOWN; // Only applicable for index buffers
};

#include <RendererDX11/Resources/Implementation/BufferDX11_inl.h>
