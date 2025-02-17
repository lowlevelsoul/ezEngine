#include <ParticlePlugin/ParticlePluginPCH.h>

#include <Foundation/Types/ScopeExit.h>
#include <ParticlePlugin/Type/Trail/ParticleTypeTrail.h>
#include <ParticlePlugin/Type/Trail/TrailRenderer.h>
#include <RendererCore/Pipeline/RenderDataBatch.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererCore/Shader/ShaderResource.h>
#include <RendererFoundation/Device/Device.h>

#include <RendererCore/../../../Data/Base/Shaders/Particles/ParticleSystemConstants.h>

//clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezParticleTrailRenderData, 1, ezRTTINoAllocator)
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezParticleTrailRenderer, 1, ezRTTIDefaultAllocator<ezParticleTrailRenderer>)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezParticleTrailRenderer::ezParticleTrailRenderer()
{
  CreateParticleDataBuffer(m_BaseDataBuffer, sizeof(ezBaseParticleShaderData), s_uiParticlesPerBatch);
  CreateParticleDataBuffer(m_TrailDataBuffer, sizeof(ezTrailParticleShaderData), s_uiParticlesPerBatch);

  // this is kinda stupid, apparently due to stride enforcement I cannot reuse the same buffer for different sizes
  // and instead have to create one buffer with every size ...

  CreateParticleDataBuffer(m_TrailPointsDataBuffer8, sizeof(ezTrailParticlePointsData8), s_uiParticlesPerBatch);
  CreateParticleDataBuffer(m_TrailPointsDataBuffer16, sizeof(ezTrailParticlePointsData16), s_uiParticlesPerBatch);
  CreateParticleDataBuffer(m_TrailPointsDataBuffer32, sizeof(ezTrailParticlePointsData32), s_uiParticlesPerBatch);
  CreateParticleDataBuffer(m_TrailPointsDataBuffer64, sizeof(ezTrailParticlePointsData64), s_uiParticlesPerBatch);

  m_hShader = ezResourceManager::LoadResource<ezShaderResource>("Shaders/Particles/Trail.ezShader");
}

ezParticleTrailRenderer::~ezParticleTrailRenderer()
{
  DestroyParticleDataBuffer(m_BaseDataBuffer);
  DestroyParticleDataBuffer(m_TrailDataBuffer);
  DestroyParticleDataBuffer(m_TrailPointsDataBuffer8);
  DestroyParticleDataBuffer(m_TrailPointsDataBuffer16);
  DestroyParticleDataBuffer(m_TrailPointsDataBuffer32);
  DestroyParticleDataBuffer(m_TrailPointsDataBuffer64);
}

void ezParticleTrailRenderer::GetSupportedRenderDataTypes(ezHybridArray<const ezRTTI*, 8>& ref_types) const
{
  ref_types.PushBack(ezGetStaticRTTI<ezParticleTrailRenderData>());
}

void ezParticleTrailRenderer::RenderBatch(const ezRenderViewContext& renderViewContext, const ezRenderPipelinePass* pPass, const ezRenderDataBatch& batch) const
{
  ezRenderContext* pRenderContext = renderViewContext.m_pRenderContext;
  ezGALCommandEncoder* pGALCommandEncoder = pRenderContext->GetCommandEncoder();

  TempSystemCB systemConstants(pRenderContext);

  pRenderContext->BindShader(m_hShader);

  // now render all particle effects of type Trail
  for (auto it = batch.GetIterator<ezParticleTrailRenderData>(0, batch.GetCount()); it.IsValid(); ++it)
  {
    const ezParticleTrailRenderData* pRenderData = it;

    if (!ConfigureShader(pRenderData, renderViewContext))
      continue;

    const ezUInt32 uiBucketSize = ezParticleTypeTrail::ComputeTrailPointBucketSize(pRenderData->m_uiMaxTrailPoints);
    const ezUInt32 uiMaxTrailSegments = uiBucketSize - 1;
    const ezUInt32 uiPrimFactor = 2;
    const ezUInt32 uiMaxPrimitivesToRender = s_uiParticlesPerBatch * uiMaxTrailSegments * uiPrimFactor;


    pRenderContext->BindMeshBuffer(ezGALBufferHandle(), ezGALBufferHandle(), nullptr, ezGALPrimitiveTopology::Triangles, uiMaxPrimitivesToRender);

    const ezBaseParticleShaderData* pParticleBaseData = pRenderData->m_BaseParticleData.GetPtr();
    const ezTrailParticleShaderData* pParticleTrailData = pRenderData->m_TrailParticleData.GetPtr();


    const ezVec4* pParticlePointsData = pRenderData->m_TrailPointsShared.GetPtr();

    pRenderContext->BindTexture2D("ParticleTexture", pRenderData->m_hTexture);

    systemConstants.SetGenericData(
      pRenderData->m_bApplyObjectTransform, pRenderData->m_GlobalTransform, pRenderData->m_TotalEffectLifeTime, pRenderData->m_uiNumVariationsX, pRenderData->m_uiNumVariationsY, pRenderData->m_uiNumFlipbookAnimationsX, pRenderData->m_uiNumFlipbookAnimationsY, pRenderData->m_fDistortionStrength, pRenderData->m_fNormalCurvature, pRenderData->m_fLightDirectionality);
    systemConstants.SetTrailData(pRenderData->m_fSnapshotFraction, pRenderData->m_uiMaxTrailPoints);

    ezUInt32 uiNumParticles = pRenderData->m_BaseParticleData.GetCount();
    while (uiNumParticles > 0)
    {
      // Request and bind new buffers for this batch
      ezGALBufferHandle hBaseDataBuffer = m_BaseDataBuffer.GetNewBuffer();
      ezGALBufferHandle hTrailDataBuffer = m_TrailDataBuffer.GetNewBuffer();
      ezGALBufferHandle hActiveTrailPointsDataBuffer = m_pActiveTrailPointsDataBuffer->GetNewBuffer();
      pRenderContext->BindBuffer("particleBaseData", ezGALDevice::GetDefaultDevice()->GetDefaultResourceView(hBaseDataBuffer));
      pRenderContext->BindBuffer("particleTrailData", ezGALDevice::GetDefaultDevice()->GetDefaultResourceView(hTrailDataBuffer));
      pRenderContext->BindBuffer("particlePointsData", ezGALDevice::GetDefaultDevice()->GetDefaultResourceView(hActiveTrailPointsDataBuffer));

      // upload this batch of particle data
      const ezUInt32 uiNumParticlesInBatch = ezMath::Min<ezUInt32>(uiNumParticles, s_uiParticlesPerBatch);
      uiNumParticles -= uiNumParticlesInBatch;

      pGALCommandEncoder->UpdateBuffer(hBaseDataBuffer, 0, ezMakeArrayPtr(pParticleBaseData, uiNumParticlesInBatch).ToByteArray(), ezGALUpdateMode::AheadOfTime);
      pParticleBaseData += uiNumParticlesInBatch;

      pGALCommandEncoder->UpdateBuffer(hTrailDataBuffer, 0, ezMakeArrayPtr(pParticleTrailData, uiNumParticlesInBatch).ToByteArray(), ezGALUpdateMode::AheadOfTime);
      pParticleTrailData += uiNumParticlesInBatch;

      pGALCommandEncoder->UpdateBuffer(hActiveTrailPointsDataBuffer, 0, ezMakeArrayPtr(pParticlePointsData, uiNumParticlesInBatch * uiBucketSize).ToByteArray(), ezGALUpdateMode::AheadOfTime);
      pParticlePointsData += uiNumParticlesInBatch * uiBucketSize;

      // do one drawcall
      pRenderContext->DrawMeshBuffer(uiNumParticlesInBatch * uiMaxTrailSegments * uiPrimFactor).IgnoreResult();
    }
  }
}

bool ezParticleTrailRenderer::ConfigureShader(const ezParticleTrailRenderData* pRenderData, const ezRenderViewContext& renderViewContext) const
{
  auto pRenderContext = renderViewContext.m_pRenderContext;

  switch (pRenderData->m_RenderMode)
  {
    case ezParticleTypeRenderMode::Additive:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_RENDER_MODE", "PARTICLE_RENDER_MODE_ADDITIVE");
      break;
    case ezParticleTypeRenderMode::Blended:
    case ezParticleTypeRenderMode::BlendedForeground:
    case ezParticleTypeRenderMode::BlendedBackground:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_RENDER_MODE", "PARTICLE_RENDER_MODE_BLENDED");
      break;
    case ezParticleTypeRenderMode::Opaque:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_RENDER_MODE", "PARTICLE_RENDER_MODE_OPAQUE");
      break;
    case ezParticleTypeRenderMode::Distortion:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_RENDER_MODE", "PARTICLE_RENDER_MODE_DISTORTION");
      pRenderContext->BindTexture2D("ParticleDistortionTexture", pRenderData->m_hDistortionTexture);
      break;
    case ezParticleTypeRenderMode::BlendAdd:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_RENDER_MODE", "PARTICLE_RENDER_MODE_BLENDADD");
      break;
  }

  switch (pRenderData->m_LightingMode)
  {
    case ezParticleLightingMode::Fullbright:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_LIGHTING_MODE", "PARTICLE_LIGHTING_MODE_FULLBRIGHT");
      break;
    case ezParticleLightingMode::VertexLit:
      pRenderContext->SetShaderPermutationVariable("PARTICLE_LIGHTING_MODE", "PARTICLE_LIGHTING_MODE_VERTEX_LIT");
      break;
  }

  switch (ezParticleTypeTrail::ComputeTrailPointBucketSize(pRenderData->m_uiMaxTrailPoints))
  {
    case 8:
      renderViewContext.m_pRenderContext->SetShaderPermutationVariable("PARTICLE_TRAIL_POINTS", "PARTICLE_TRAIL_POINTS_COUNT8");
      m_pActiveTrailPointsDataBuffer = &m_TrailPointsDataBuffer8;
      break;
    case 16:
      renderViewContext.m_pRenderContext->SetShaderPermutationVariable("PARTICLE_TRAIL_POINTS", "PARTICLE_TRAIL_POINTS_COUNT16");
      m_pActiveTrailPointsDataBuffer = &m_TrailPointsDataBuffer16;
      break;
    case 32:
      renderViewContext.m_pRenderContext->SetShaderPermutationVariable("PARTICLE_TRAIL_POINTS", "PARTICLE_TRAIL_POINTS_COUNT32");
      m_pActiveTrailPointsDataBuffer = &m_TrailPointsDataBuffer32;
      break;
    case 64:
      renderViewContext.m_pRenderContext->SetShaderPermutationVariable("PARTICLE_TRAIL_POINTS", "PARTICLE_TRAIL_POINTS_COUNT64");
      m_pActiveTrailPointsDataBuffer = &m_TrailPointsDataBuffer64;
      break;

    default:
      return false;
  }

  return true;
}

EZ_STATICLINK_FILE(ParticlePlugin, ParticlePlugin_Type_Trail_TrailRenderer);
