#pragma once

#include <Foundation/Containers/Set.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Strings/HashedString.h>
#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/Enum.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/Variant.h>
#include <ToolsFoundation/ToolsFoundationDLL.h>

class ezRTTI;
class ezPhantomRttiManager;
class ezReflectedTypeStorageManager;

/// \brief Event message used by the ezPhantomRttiManager.
struct EZ_TOOLSFOUNDATION_DLL ezPhantomTypeChange
{
  const ezRTTI* m_pChangedType = nullptr;
};

struct EZ_TOOLSFOUNDATION_DLL ezAttributeHolder
{
  ezAttributeHolder();
  ezAttributeHolder(const ezAttributeHolder& rhs);
  virtual ~ezAttributeHolder();

  ezUInt32 GetCount() const;
  const ezPropertyAttribute* GetValue(ezUInt32 uiIndex) const;
  void SetValue(ezUInt32 uiIndex, const ezPropertyAttribute* value);
  void Insert(ezUInt32 uiIndex, const ezPropertyAttribute* value);
  void Remove(ezUInt32 uiIndex);

  void operator=(const ezAttributeHolder& rhs);

  mutable ezHybridArray<const ezPropertyAttribute*, 2> m_Attributes;
  ezArrayPtr<const ezPropertyAttribute* const> m_ReferenceAttributes;
};
EZ_DECLARE_REFLECTABLE_TYPE(EZ_TOOLSFOUNDATION_DLL, ezAttributeHolder);

/// \brief Stores the description of a reflected property in a serializable form, used by ezReflectedTypeDescriptor.
struct EZ_TOOLSFOUNDATION_DLL ezReflectedPropertyDescriptor : public ezAttributeHolder
{
  ezReflectedPropertyDescriptor() = default;
  ezReflectedPropertyDescriptor(ezPropertyCategory::Enum category, ezStringView sName, ezStringView sType, ezBitflags<ezPropertyFlags> flags);
  ezReflectedPropertyDescriptor(ezPropertyCategory::Enum category, ezStringView sName, ezStringView sType, ezBitflags<ezPropertyFlags> flags,
    ezArrayPtr<const ezPropertyAttribute* const> attributes); // [tested]
  /// \brief Initialize to a constant.
  ezReflectedPropertyDescriptor(ezStringView sName, const ezVariant& constantValue, ezArrayPtr<const ezPropertyAttribute* const> attributes); // [tested]
  ezReflectedPropertyDescriptor(const ezReflectedPropertyDescriptor& rhs);
  ~ezReflectedPropertyDescriptor();

  void operator=(const ezReflectedPropertyDescriptor& rhs);

  ezEnum<ezPropertyCategory> m_Category;
  ezString m_sName; ///< The name of this property. E.g. what ezAbstractProperty::GetPropertyName() returns.
  ezString m_sType; ///< The name of the type of the property. E.g. ezAbstractProperty::GetSpecificType().GetTypeName()

  ezBitflags<ezPropertyFlags> m_Flags;
  ezVariant m_ConstantValue;
};
EZ_DECLARE_REFLECTABLE_TYPE(EZ_TOOLSFOUNDATION_DLL, ezReflectedPropertyDescriptor);

struct EZ_TOOLSFOUNDATION_DLL ezFunctionArgumentDescriptor
{
  ezFunctionArgumentDescriptor();
  ezFunctionArgumentDescriptor(ezStringView sType, ezBitflags<ezPropertyFlags> flags);
  ezString m_sType;
  ezBitflags<ezPropertyFlags> m_Flags;
};
EZ_DECLARE_REFLECTABLE_TYPE(EZ_TOOLSFOUNDATION_DLL, ezFunctionArgumentDescriptor);

/// \brief Stores the description of a reflected function in a serializable form, used by ezReflectedTypeDescriptor.
struct EZ_TOOLSFOUNDATION_DLL ezReflectedFunctionDescriptor : public ezAttributeHolder
{
  ezReflectedFunctionDescriptor();
  ezReflectedFunctionDescriptor(ezStringView sName, ezBitflags<ezPropertyFlags> flags, ezEnum<ezFunctionType> type, ezArrayPtr<const ezPropertyAttribute* const> attributes);

  ezReflectedFunctionDescriptor(const ezReflectedFunctionDescriptor& rhs);
  ~ezReflectedFunctionDescriptor();

  void operator=(const ezReflectedFunctionDescriptor& rhs);

  ezString m_sName;
  ezBitflags<ezPropertyFlags> m_Flags;
  ezEnum<ezFunctionType> m_Type;
  ezFunctionArgumentDescriptor m_ReturnValue;
  ezDynamicArray<ezFunctionArgumentDescriptor> m_Arguments;
};
EZ_DECLARE_REFLECTABLE_TYPE(EZ_TOOLSFOUNDATION_DLL, ezReflectedFunctionDescriptor);


/// \brief Stores the description of a reflected type in a serializable form. Used by ezPhantomRttiManager to add new types.
struct EZ_TOOLSFOUNDATION_DLL ezReflectedTypeDescriptor : public ezAttributeHolder
{
  ~ezReflectedTypeDescriptor();

  ezString m_sTypeName;
  ezString m_sPluginName;
  ezString m_sParentTypeName;

  ezBitflags<ezTypeFlags> m_Flags;
  ezDynamicArray<ezReflectedPropertyDescriptor> m_Properties;
  ezDynamicArray<ezReflectedFunctionDescriptor> m_Functions;
  ezUInt32 m_uiTypeVersion = 1;
};
EZ_DECLARE_REFLECTABLE_TYPE(EZ_TOOLSFOUNDATION_DLL, ezReflectedTypeDescriptor);
