#pragma once

#include <EditorEngineProcessFramework/Gizmos/GizmoHandle.h>
#include <EditorFramework/EditorFrameworkDLL.h>
#include <EditorFramework/Visualizers/VisualizerAdapter.h>

struct ezGizmoEvent;

class ezConeVisualizerAdapter : public ezVisualizerAdapter
{
public:
  ezConeVisualizerAdapter();
  ~ezConeVisualizerAdapter();

protected:
  virtual void Finalize() override;
  virtual void Update() override;

  virtual void UpdateGizmoTransform() override;

  float m_fFinalScale;
  float m_fAngleScale;
  ezEngineGizmoHandle m_hGizmo;
};
