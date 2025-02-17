#pragma once

#include <Foundation/Strings/String.h>
#include <Foundation/Types/Delegate.h>
#include <ToolsFoundation/ToolsFoundationDLL.h>

class ezDocumentObjectManager;
class ezDocumentObject;

/// \brief Implements visitor pattern for content of the document object manager.
class EZ_TOOLSFOUNDATION_DLL ezDocumentObjectVisitor
{
public:
  /// \brief Constructor
  ///
  /// \param pManager
  ///   Manager that will be iterated through.
  /// \param szChildrenProperty
  ///   Name of the property that is used for finding children on an object.
  /// \param szRootProperty
  ///   Same as szChildrenProperty, but for the root object of the document.
  ezDocumentObjectVisitor(
    const ezDocumentObjectManager* pManager, ezStringView sChildrenProperty = "Children", ezStringView sRootProperty = "Children");

  using VisitorFunction = ezDelegate<bool(const ezDocumentObject*)>;
  /// \brief Executes depth first traversal starting at the given node.
  ///
  /// \param pObject
  ///   Object to start traversal at.
  /// \param bVisitStart
  ///   If true, function will be executed for the start object as well.
  /// \param function
  ///   Functions executed for each visited object. Should true if the object's children should be traversed.
  void Visit(const ezDocumentObject* pObject, bool bVisitStart, VisitorFunction function);

private:
  void TraverseChildren(const ezDocumentObject* pObject, ezStringView sProperty, VisitorFunction& function);

  const ezDocumentObjectManager* m_pManager = nullptr;
  ezString m_sChildrenProperty;
  ezString m_sRootProperty;
};
