#pragma once

#include <Shaders/Common/BRDF.h>
#include <Shaders/Common/Common.h>
#include <Shaders/Common/GlobalConstants.h>
#include <Shaders/Common/ObjectConstants.h>
#include <Shaders/Materials/MaterialData.h>
#include <Shaders/Materials/MaterialInterpolator.h>

float3 GetNormal();

#if defined(USE_SIMPLE_MATERIAL_MODEL)
float3 GetBaseColor();
float GetMetallic();
float GetReflectance();
#else
float3 GetDiffuseColor();
float3 GetSpecularColor();
#endif

#if defined(USE_MATERIAL_EMISSIVE)
float3 GetEmissiveColor();
#endif

#if defined(USE_MATERIAL_REFRACTION)
float4 GetRefractionColor();
#endif

// Note that this function actually returns perceptualRoughness.
float GetRoughness();
float GetOpacity();

#if defined(USE_MATERIAL_OCCLUSION)
float GetOcclusion();
#endif

#if defined(USE_MATERIAL_SUBSURFACE_COLOR)
float3 GetSubsurfaceColor();
#endif

#if defined(USE_MATERIAL_SUBSURFACE_PARAMS)
void GetSubsurfaceParams(out float scatterPower, out float shadowFalloff);
#endif

struct PS_GLOBALS
{
  PS_IN Input;
#if defined(CUSTOM_GLOBALS)
  CUSTOM_GLOBALS
#endif
};
static PS_GLOBALS G;

#if defined(CUSTOM_GLOBALS)
void FillCustomGlobals();
#endif

#if defined(USE_CUSTOM_DITHER_NOISE)
float4 GetDitherNoise(float2 pixelCoord);
#endif

uint CalculateCoverage()
{
#if defined(USE_DITHERING)
  float4 worldOrigin = TransformToPosition(GetInstanceData().ObjectToWorld);
  float2 randomOffset;
  randomOffset.x = frac(dot(worldOrigin.xz, float2(13.57, 17.32)));
  randomOffset.y = frac(dot(worldOrigin.yz, float2(23.89, 19.43)));

  float2 pixelPos = G.Input.Position.xy + randomOffset * 233;

#  if defined(CAMERA_MODE) && CAMERA_MODE == CAMERA_MODE_ORTHO
  float4 screenOrigin = mul(GetWorldToScreenMatrix(), worldOrigin);
  screenOrigin.xy = screenOrigin.xy * float2(0.5, -0.5) + 0.5;
  screenOrigin.xy *= ViewportSize.xy;
  pixelPos -= screenOrigin.xy;
#  endif

#  if defined(USE_CUSTOM_DITHER_NOISE)
  float4 ditherNoise = GetDitherNoise(pixelPos);
#  else
  float4 bayerMatrix[] = {
    float4(01.0 / 16.0, 09.0 / 16.0, 03.0 / 16.0, 11.0 / 16.0),
    float4(13.0 / 16.0, 05.0 / 16.0, 15.0 / 16.0, 07.0 / 16.0),
    float4(04.0 / 16.0, 12.0 / 16.0, 02.0 / 16.0, 10.0 / 16.0),
    float4(16.0 / 16.0, 08.0 / 16.0, 14.0 / 16.0, 06.0 / 16.0),
  };

  uint2 pos = pixelPos - 4 * floor(pixelPos / 4);
#    if defined(WRITE_COVERAGE)
  float4 ditherNoise;
  ditherNoise.x = bayerMatrix[(pos.y + 0) % 4u][(pos.x + 0) % 4u];
  ditherNoise.y = bayerMatrix[(pos.y + 1) % 4u][(pos.x + 3) % 4u];
  ditherNoise.z = bayerMatrix[(pos.y + 2) % 4u][(pos.x + 1) % 4u];
  ditherNoise.w = bayerMatrix[(pos.y + 3) % 4u][(pos.x + 2) % 4u];
#    else
  float4 ditherNoise = bayerMatrix[pos.y % 4u][pos.x % 4u];
#    endif
#  endif

  float opacity = GetOpacity();

#  if defined(WRITE_COVERAGE)
  uint coverage = 0;

  for (uint i = 0; i < NumMsaaSamples; ++i)
  {
    coverage |= (opacity >= ditherNoise[i % 4u]) ? (1u << i) : 0;
  }

  return coverage;
#  else
  return opacity >= ditherNoise.x;
#  endif

#elif defined(WRITE_COVERAGE) && defined(USE_TEXCOORD0)
  uint coverage = 0;

  float2 texCoords = G.Input.TexCoord0;

  for (uint i = 0; i < NumMsaaSamples; ++i)
  {
    G.Input.TexCoord0 = ezEvaluateAttributeAtSample(texCoords, i, NumMsaaSamples);

    float opacity = GetOpacity();
    coverage |= (opacity > 0.0) ? (1u << i) : 0;
  }

  G.Input.TexCoord0 = texCoords;

  return coverage;
#else
  return GetOpacity() > 0.0;
#endif
}

ezMaterialData FillMaterialData()
{
  ezMaterialData matData;

#if defined(USE_WORLDPOS)
  matData.worldPosition = G.Input.WorldPosition;
#else
  matData.worldPosition = float3(0.0, 0.0, 0.0);
#endif

#if SHADING_MODE == SHADING_MODE_FULLBRIGHT
  matData.worldNormal = float3(0, 0, 1);
#else
  float3 worldNormal = normalize(GetNormal());
#  if TWO_SIDED == TRUE && defined(USE_TWO_SIDED_LIGHTING)
#    if FLIP_WINDING == TRUE
  matData.worldNormal = G.Input.FrontFace ? -worldNormal : worldNormal;
#    else
  matData.worldNormal = G.Input.FrontFace ? worldNormal : -worldNormal;
#    endif
#  else
  matData.worldNormal = worldNormal;
#  endif
#endif

#if defined(USE_NORMAL)
  matData.vertexNormal = normalize(G.Input.Normal);
#else
  matData.vertexNormal = float3(0, 0, 1);
#endif

#if defined(USE_SIMPLE_MATERIAL_MODEL)
  float3 baseColor = GetBaseColor();

#  if SHADING_MODE == SHADING_MODE_FULLBRIGHT
  matData.diffuseColor = baseColor;
  matData.specularColor = float3(0, 0, 0);
#  else
  float metallic = GetMetallic();
  float reflectance = GetReflectance();
  float f0 = 0.16f * reflectance * reflectance;

  matData.diffuseColor = lerp(baseColor, 0.0f, metallic);
  matData.specularColor = lerp(float3(f0, f0, f0), baseColor, metallic);
#  endif

#else
  matData.diffuseColor = GetDiffuseColor();

#  if SHADING_MODE == SHADING_MODE_FULLBRIGHT
  matData.specularColor = float3(0, 0, 0);
#  else
  matData.specularColor = GetSpecularColor();
#  endif
#endif

#if defined(USE_MATERIAL_EMISSIVE)
  matData.emissiveColor = GetEmissiveColor();
#else
  matData.emissiveColor = 0.0f;
#endif

#if defined(USE_MATERIAL_REFRACTION)
  matData.refractionColor = GetRefractionColor();
#else
  matData.refractionColor = float4(0, 0, 0, 1);
#endif

#if SHADING_MODE == SHADING_MODE_FULLBRIGHT
  matData.perceptualRoughness = MIN_PERCEPTUAL_ROUGHNESS;
#else
  matData.perceptualRoughness = max(GetRoughness(), MIN_PERCEPTUAL_ROUGHNESS);
#endif

  matData.roughness = RoughnessFromPerceptualRoughness(matData.perceptualRoughness);

#if defined(USE_MATERIAL_OCCLUSION)
#  if defined(USE_NORMAL)
  float3 viewVector = normalize(GetCameraPosition() - matData.worldPosition);
  float occlusionFade = saturate(dot(matData.vertexNormal, viewVector));
#  else
  float occlusionFade = 1.0f;
#  endif
  matData.occlusion = lerp(1.0f, GetOcclusion(), occlusionFade);
#else
  matData.occlusion = 1.0f;
#endif

#if BLEND_MODE != BLEND_MODE_OPAQUE && BLEND_MODE != BLEND_MODE_MASKED
  matData.opacity = GetOpacity();
#else
  matData.opacity = 1.0f;
#endif

#if defined(USE_MATERIAL_SUBSURFACE_COLOR)
  matData.subsurfaceColor = GetSubsurfaceColor() * matData.diffuseColor;
#else
  matData.subsurfaceColor = 0.0;
#endif

#if defined(USE_MATERIAL_SUBSURFACE_PARAMS)
  GetSubsurfaceParams(matData.subsurfaceScatterPower, matData.subsurfaceShadowFalloff);
#else
  matData.subsurfaceScatterPower = 9.0;
  matData.subsurfaceShadowFalloff = 0.0;
#endif

  return matData;
}

#if defined(USE_NORMAL)
float3 TangentToWorldSpace(float3 normalTS)
{
#  if defined(USE_TANGENT)
  return normalTS.x * G.Input.Tangent + normalTS.y * G.Input.BiTangent + normalTS.z * G.Input.Normal;
#  else
  return normalTS.z * G.Input.Normal;
#  endif
}
#endif

float3 BlendNormals(float3 baseNormal, float3 detailNormal)
{
  float3 t = baseNormal + float3(0, 0, 1);
  float3 u = detailNormal * float3(-1, -1, 1);
  return t * dot(t, u) - u * t.z;
}

float4 SampleTexture3Way(Texture2D tex, SamplerState samplerState, float3 worldNormal, float3 worldPosition, float tiling)
{
  float3 blendWeights = abs(worldNormal);
  blendWeights = max((blendWeights - 0.2) * 7.0, 0.0);
  blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z);

  float3 ns = sign(worldNormal) * tiling;

  float4 color1 = tex.Sample(samplerState, worldPosition.yz * float2(-ns.x, -tiling));
  float4 color2 = tex.Sample(samplerState, worldPosition.xz * float2(ns.y, -tiling));
  float4 color3 = tex.Sample(samplerState, worldPosition.xy * float2(ns.z, tiling));

  return color1 * blendWeights.x + color2 * blendWeights.y + color3 * blendWeights.z;
}

float4 ColorizeGameObjectId(uint gameObjectId)
{
  float intensity = saturate(0.5f + (gameObjectId & 0xF) / 31.0f);
  bool isDynamic = gameObjectId & (1 << 31);
  return float4(isDynamic ? intensity : 0.0f, isDynamic ? 0.0f : intensity, 0.0f, 1.0f);
}

float4 SampleColorPalette(Texture2D paletteTex, uint row, float column)
{
  uint width, height;
  paletteTex.GetDimensions(width, height);

  float2 uv = float2(column, (row + 0.5f) / height);
  return paletteTex.SampleLevel(LinearClampSampler, uv, 0);
}
