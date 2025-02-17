#include <ParticlePlugin/ParticlePluginPCH.h>

#include <Foundation/Math/Color16f.h>
#include <Foundation/Math/Float16.h>
#include <Foundation/Profiling/Profiling.h>
#include <ParticlePlugin/Effect/ParticleEffectInstance.h>
#include <ParticlePlugin/Type/Light/ParticleTypeLight.h>
#include <RendererCore/Lights/PointLightComponent.h>
#include <RendererCore/Pipeline/RenderData.h>
#include <RendererCore/Pipeline/View.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezParticleTypeLightFactory, 1, ezRTTIDefaultAllocator<ezParticleTypeLightFactory>)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_MEMBER_PROPERTY("SizeFactor", m_fSizeFactor)->AddAttributes(new ezDefaultValueAttribute(5.0f), new ezClampValueAttribute(0.0f, 1000.0f)),
    EZ_MEMBER_PROPERTY("Intensity", m_fIntensity)->AddAttributes(new ezDefaultValueAttribute(10.0f), new ezClampValueAttribute(0.0f, 100000.0f)),
    EZ_MEMBER_PROPERTY("Percentage", m_uiPercentage)->AddAttributes(new ezDefaultValueAttribute(50), new ezClampValueAttribute(1, 100)),
    EZ_MEMBER_PROPERTY("TintColorParam", m_sTintColorParameter),
    EZ_MEMBER_PROPERTY("IntensityScaleParam", m_sIntensityParameter),
    EZ_MEMBER_PROPERTY("SizeScaleParam", m_sSizeScaleParameter),
  }
  EZ_END_PROPERTIES;
}
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezParticleTypeLight, 1, ezRTTIDefaultAllocator<ezParticleTypeLight>)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezParticleTypeLightFactory::ezParticleTypeLightFactory()
{
  m_fSizeFactor = 5.0f;
  m_fIntensity = 10.0f;
  m_uiPercentage = 50;
}


const ezRTTI* ezParticleTypeLightFactory::GetTypeType() const
{
  return ezGetStaticRTTI<ezParticleTypeLight>();
}

void ezParticleTypeLightFactory::CopyTypeProperties(ezParticleType* pObject, bool bFirstTime) const
{
  ezParticleTypeLight* pType = static_cast<ezParticleTypeLight*>(pObject);

  pType->m_fSizeFactor = m_fSizeFactor;
  pType->m_fIntensity = m_fIntensity;
  pType->m_uiPercentage = m_uiPercentage;
  pType->m_sTintColorParameter = ezTempHashedString(m_sTintColorParameter.GetData());
  pType->m_sIntensityParameter = ezTempHashedString(m_sIntensityParameter.GetData());
  pType->m_sSizeScaleParameter = ezTempHashedString(m_sSizeScaleParameter.GetData());
}

enum class TypeLightVersion
{
  Version_0 = 0,
  Version_1,
  Version_2, // added tint color and intensity parameter

  // insert new version numbers above
  Version_Count,
  Version_Current = Version_Count - 1
};

void ezParticleTypeLightFactory::Save(ezStreamWriter& inout_stream) const
{
  const ezUInt8 uiVersion = (int)TypeLightVersion::Version_Current;
  inout_stream << uiVersion;

  inout_stream << m_fSizeFactor;
  inout_stream << m_fIntensity;
  inout_stream << m_uiPercentage;

  // Version 2
  inout_stream << m_sTintColorParameter;
  inout_stream << m_sIntensityParameter;
  inout_stream << m_sSizeScaleParameter;
}

void ezParticleTypeLightFactory::Load(ezStreamReader& inout_stream)
{
  ezUInt8 uiVersion = 0;
  inout_stream >> uiVersion;

  EZ_ASSERT_DEV(uiVersion <= (int)TypeLightVersion::Version_Current, "Invalid version {0}", uiVersion);

  inout_stream >> m_fSizeFactor;
  inout_stream >> m_fIntensity;
  inout_stream >> m_uiPercentage;

  if (uiVersion >= 2)
  {
    inout_stream >> m_sTintColorParameter;
    inout_stream >> m_sIntensityParameter;
    inout_stream >> m_sSizeScaleParameter;
  }
}

void ezParticleTypeLight::CreateRequiredStreams()
{
  m_pStreamOnOff = nullptr;

  CreateStream("Position", ezProcessingStream::DataType::Float4, &m_pStreamPosition, false);
  CreateStream("Size", ezProcessingStream::DataType::Half, &m_pStreamSize, false);
  CreateStream("Color", ezProcessingStream::DataType::Half4, &m_pStreamColor, false);

  if (m_uiPercentage < 100)
  {
    CreateStream("OnOff", ezProcessingStream::DataType::Int, &m_pStreamOnOff, false); /// \todo Initialize (instead of during extraction)
  }
}


void ezParticleTypeLight::ExtractTypeRenderData(ezMsgExtractRenderData& ref_msg, const ezTransform& instanceTransform) const
{
  EZ_PROFILE_SCOPE("PFX: Light");

  const ezVec4* pPosition = m_pStreamPosition->GetData<ezVec4>();
  const ezFloat16* pSize = m_pStreamSize->GetData<ezFloat16>();
  const ezColorLinear16f* pColor = m_pStreamColor->GetData<ezColorLinear16f>();

  if (pPosition == nullptr || pSize == nullptr || pColor == nullptr)
    return;

  ezInt32* pOnOff = nullptr;

  if (m_pStreamOnOff)
  {
    pOnOff = m_pStreamOnOff->GetWritableData<ezInt32>();

    if (pOnOff == nullptr)
      return;
  }

  ezRandom& rng = GetRNG();

  const ezUInt32 uiNumParticles = (ezUInt32)GetOwnerSystem()->GetNumActiveParticles();

  const ezUInt32 uiBatchId = 1; // no shadows

  const ezColor tintColor = GetOwnerEffect()->GetColorParameter(m_sTintColorParameter, ezColor::White);
  const float intensityScale = GetOwnerEffect()->GetFloatParameter(m_sIntensityParameter, 1.0f);
  const float sizeScale = GetOwnerEffect()->GetFloatParameter(m_sSizeScaleParameter, 1.0f);

  const float sizeFactor = m_fSizeFactor * sizeScale;
  const float intensity = intensityScale * m_fIntensity;

  ezTransform transform;

  if (this->GetOwnerEffect()->IsSimulatedInLocalSpace())
    transform = instanceTransform;
  else
    transform.SetIdentity();

  for (ezUInt32 i = 0; i < uiNumParticles; ++i)
  {
    if (pOnOff)
    {
      if (pOnOff[i] == 0)
      {
        if ((ezUInt32)rng.IntMinMax(0, 100) <= m_uiPercentage)
          pOnOff[i] = 1;
        else
          pOnOff[i] = -1;
      }

      if (pOnOff[i] < 0)
        continue;
    }

    auto pRenderData = ezCreateRenderDataForThisFrame<ezPointLightRenderData>(nullptr);

    pRenderData->m_GlobalTransform.SetIdentity();
    pRenderData->m_GlobalTransform.m_vPosition = transform * pPosition[i].GetAsVec3();
    pRenderData->m_LightColor = tintColor * pColor[i].ToLinearFloat();
    pRenderData->m_fIntensity = intensity;
    pRenderData->m_fSpecularMultiplier = 1.0f;
    pRenderData->m_fRange = pSize[i] * sizeFactor;
    pRenderData->m_uiShadowDataOffsetAndFadeOut = 0;

    float fScreenSpaceSize = ezLightComponent::CalculateScreenSpaceSize(ezBoundingSphere::MakeFromCenterAndRadius(pRenderData->m_GlobalTransform.m_vPosition, pRenderData->m_fRange * 0.5f), *ref_msg.m_pView->GetCullingCamera());
    pRenderData->FillBatchIdAndSortingKey(fScreenSpaceSize);

    ref_msg.AddRenderData(pRenderData, ezDefaultRenderDataCategories::Light, ezRenderData::Caching::Never);
  }
}



EZ_STATICLINK_FILE(ParticlePlugin, ParticlePlugin_Type_Light_ParticleTypeLight);
