#pragma once

#include <EditorFramework/EditorFrameworkDLL.h>
#include <GuiFoundation/Action/BaseActions.h>
#include <GuiFoundation/GuiFoundationDLL.h>

class ezPreferences;
struct ezGameObjectEvent;
class ezGameObjectDocument;
///
class EZ_EDITORFRAMEWORK_DLL ezGameObjectDocumentActions
{
public:
  static void RegisterActions();
  static void UnregisterActions();

  static void MapMenuActions(ezStringView sMapping);
  static void MapMenuSimulationSpeed(ezStringView sMapping);

  static void MapToolbarActions(ezStringView sMapping);

  static ezActionDescriptorHandle s_hGameObjectCategory;
  static ezActionDescriptorHandle s_hRenderSelectionOverlay;
  static ezActionDescriptorHandle s_hRenderVisualizers;
  static ezActionDescriptorHandle s_hRenderShapeIcons;
  static ezActionDescriptorHandle s_hRenderGrid;
  static ezActionDescriptorHandle s_hAddAmbientLight;
  static ezActionDescriptorHandle s_hSimulationSpeedMenu;
  static ezActionDescriptorHandle s_hSimulationSpeed[10];
  static ezActionDescriptorHandle s_hCameraSpeed;
  static ezActionDescriptorHandle s_hPickTransparent;
};

///
class EZ_EDITORFRAMEWORK_DLL ezGameObjectDocumentAction : public ezButtonAction
{
  EZ_ADD_DYNAMIC_REFLECTION(ezGameObjectDocumentAction, ezButtonAction);

public:
  enum class ActionType
  {
    RenderSelectionOverlay,
    RenderVisualizers,
    RenderShapeIcons,
    RenderGrid,
    AddAmbientLight,
    SimulationSpeed,
    PickTransparent,
  };

  ezGameObjectDocumentAction(const ezActionContext& context, const char* szName, ActionType type, float fSimSpeed = 1.0f);
  ~ezGameObjectDocumentAction();

  virtual void Execute(const ezVariant& value) override;

private:
  void SceneEventHandler(const ezGameObjectEvent& e);
  void OnPreferenceChange(ezPreferences* pref);

  float m_fSimSpeed;
  ezGameObjectDocument* m_pGameObjectDocument;
  ActionType m_Type;
};


class EZ_EDITORFRAMEWORK_DLL ezCameraSpeedSliderAction : public ezSliderAction
{
  EZ_ADD_DYNAMIC_REFLECTION(ezCameraSpeedSliderAction, ezSliderAction);

public:
  enum class ActionType
  {
    CameraSpeed,
  };

  ezCameraSpeedSliderAction(const ezActionContext& context, const char* szName, ActionType type);
  ~ezCameraSpeedSliderAction();

  virtual void Execute(const ezVariant& value) override;

private:
  void OnPreferenceChange(ezPreferences* pref);
  void UpdateState();

  ezGameObjectDocument* m_pGameObjectDocument;
  ActionType m_Type;
};
