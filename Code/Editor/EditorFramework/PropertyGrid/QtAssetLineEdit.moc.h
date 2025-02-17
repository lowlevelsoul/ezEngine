#pragma once

#include <EditorFramework/EditorFrameworkDLL.h>
#include <GuiFoundation/PropertyGrid/Implementation/PropertyWidget.moc.h>
#include <QLineEdit>
#include <QModelIndex>

class ezQtAssetPropertyWidget;

/// \brief A QLineEdit that is used by ezQtAssetPropertyWidget
class EZ_EDITORFRAMEWORK_DLL ezQtAssetLineEdit : public QLineEdit
{
  Q_OBJECT

Q_SIGNALS:
  void OpenAsset();
  void SelectAsset();

public:
  explicit ezQtAssetLineEdit(QWidget* pParent = nullptr);
  virtual void dragMoveEvent(QDragMoveEvent* e) override;
  virtual void dragEnterEvent(QDragEnterEvent* e) override;
  virtual void dropEvent(QDropEvent* e) override;
  virtual void paintEvent(QPaintEvent* e) override;
  virtual void mousePressEvent(QMouseEvent* e) override;

  ezQtAssetPropertyWidget* m_pOwner = nullptr;
};
