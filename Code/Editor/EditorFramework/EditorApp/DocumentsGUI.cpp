#include <EditorFramework/EditorFrameworkPCH.h>

#include <EditorFramework/Assets/AssetBrowserDlg.moc.h>
#include <EditorFramework/Dialogs/DashboardDlg.moc.h>
#include <EditorFramework/EditorApp/EditorApp.moc.h>

void ezQtEditorApp::GuiCreateOrOpenDocument(bool bCreate)
{
  const ezString sAllFilters = BuildDocumentTypeFileFilter(bCreate);

  if (sAllFilters.IsEmpty())
  {
    ezQtUiServices::MessageBoxInformation("No file types are currently known. Load plugins to add file types.");
    return;
  }

  static QString sSelectedExt;
  const QString sDir = QString::fromUtf8(m_sLastDocumentFolder.GetData());

  ezString sFile;

  if (bCreate)
    sFile = QFileDialog::getSaveFileName(QApplication::activeWindow(), QLatin1String("Create Document"), sDir,
      QString::fromUtf8(sAllFilters.GetData()), &sSelectedExt, QFileDialog::Option::DontResolveSymlinks)
              .toUtf8()
              .data();
  else
    sFile = QFileDialog::getOpenFileName(QApplication::activeWindow(), QLatin1String("Open Document"), sDir, QString::fromUtf8(sAllFilters.GetData()),
      &sSelectedExt, QFileDialog::Option::DontResolveSymlinks)
              .toUtf8()
              .data();

  if (sFile.IsEmpty())
    return;

  m_sLastDocumentFolder = ezPathUtils::GetFileDirectory(sFile);

  const ezDocumentTypeDescriptor* pTypeDesc = nullptr;
  if (ezDocumentManager::FindDocumentTypeFromPath(sFile, bCreate, pTypeDesc).Succeeded())
  {
    sSelectedExt = pTypeDesc->m_sDocumentTypeName;
  }

  if (bCreate)
    CreateDocument(sFile, ezDocumentFlags::AddToRecentFilesList | ezDocumentFlags::RequestWindow);
  else
    OpenDocument(sFile, ezDocumentFlags::AddToRecentFilesList | ezDocumentFlags::RequestWindow);
}

void ezQtEditorApp::GuiCreateDocument()
{
  GuiCreateOrOpenDocument(true);
}

void ezQtEditorApp::GuiOpenDocument()
{
  ezQtAssetBrowserDlg dlg(QApplication::activeWindow(), ezUuid(), "", "");
  if (dlg.exec() == 0)
    return;

  ezQtEditorApp::GetSingleton()->OpenDocument(dlg.GetSelectedAssetPathAbsolute(), ezDocumentFlags::RequestWindow | ezDocumentFlags::AddToRecentFilesList);
}


ezString ezQtEditorApp::BuildDocumentTypeFileFilter(bool bForCreation)
{
  ezStringBuilder sAllFilters;
  const char* sepsep = "";

  if (!bForCreation)
  {
    sAllFilters = "All Files (*.*)";
    sepsep = ";;";
  }

  const auto& assetTypes = ezDocumentManager::GetAllDocumentDescriptors();

  // use translated strings
  ezMap<ezString, const ezDocumentTypeDescriptor*> allDesc;
  for (auto it : assetTypes)
  {
    allDesc[ezTranslate(it.Key())] = it.Value();
  }

  for (auto it : allDesc)
  {
    auto desc = it.Value();

    if (bForCreation && !desc->m_bCanCreate)
      continue;

    if (desc->m_sFileExtension.IsEmpty())
      continue;

    sAllFilters.Append(sepsep, ezTranslate(desc->m_sDocumentTypeName), " (*.", desc->m_sFileExtension, ")");
    sepsep = ";;";
  }

  return sAllFilters;
}
