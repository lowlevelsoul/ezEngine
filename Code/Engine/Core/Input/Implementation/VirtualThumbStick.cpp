#include <Core/CorePCH.h>

#include <Core/Input/VirtualThumbStick.h>
#include <Foundation/Time/Clock.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezVirtualThumbStick, 1, ezRTTINoAllocator)
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

ezInt32 ezVirtualThumbStick::s_iThumbsticks = 0;

ezVirtualThumbStick::ezVirtualThumbStick()
{
  SetAreaFocusMode(ezInputActionConfig::RequireKeyUp, ezInputActionConfig::KeepFocus);
  SetTriggerInputSlot(ezVirtualThumbStick::Input::Touchpoint);
  SetThumbstickOutput(ezVirtualThumbStick::Output::Controller0_LeftStick);

  SetInputArea(ezVec2(0.0f), ezVec2(0.0f), 0.0f, 0.0f);

  ezStringBuilder s;
  s.SetFormat("Thumbstick_{0}", s_iThumbsticks);
  m_sName = s;

  ++s_iThumbsticks;
}

ezVirtualThumbStick::~ezVirtualThumbStick()
{
  ezInputManager::RemoveInputAction(GetDynamicRTTI()->GetTypeName(), m_sName.GetData());
}

void ezVirtualThumbStick::SetTriggerInputSlot(ezVirtualThumbStick::Input::Enum input, const ezInputActionConfig* pCustomConfig)
{
  for (ezInt32 i = 0; i < ezInputActionConfig::MaxInputSlotAlternatives; ++i)
  {
    m_ActionConfig.m_sFilterByInputSlotX[i] = ezInputSlot_None;
    m_ActionConfig.m_sFilterByInputSlotY[i] = ezInputSlot_None;
    m_ActionConfig.m_sInputSlotTrigger[i] = ezInputSlot_None;
  }

  switch (input)
  {
    case ezVirtualThumbStick::Input::Touchpoint:
    {
      m_ActionConfig.m_sFilterByInputSlotX[0] = ezInputSlot_TouchPoint0_PositionX;
      m_ActionConfig.m_sFilterByInputSlotY[0] = ezInputSlot_TouchPoint0_PositionY;
      m_ActionConfig.m_sInputSlotTrigger[0] = ezInputSlot_TouchPoint0;

      m_ActionConfig.m_sFilterByInputSlotX[1] = ezInputSlot_TouchPoint1_PositionX;
      m_ActionConfig.m_sFilterByInputSlotY[1] = ezInputSlot_TouchPoint1_PositionY;
      m_ActionConfig.m_sInputSlotTrigger[1] = ezInputSlot_TouchPoint1;

      m_ActionConfig.m_sFilterByInputSlotX[2] = ezInputSlot_TouchPoint2_PositionX;
      m_ActionConfig.m_sFilterByInputSlotY[2] = ezInputSlot_TouchPoint2_PositionY;
      m_ActionConfig.m_sInputSlotTrigger[2] = ezInputSlot_TouchPoint2;
    }
    break;
    case ezVirtualThumbStick::Input::MousePosition:
    {
      m_ActionConfig.m_sFilterByInputSlotX[0] = ezInputSlot_MousePositionX;
      m_ActionConfig.m_sFilterByInputSlotY[0] = ezInputSlot_MousePositionY;
      m_ActionConfig.m_sInputSlotTrigger[0] = ezInputSlot_MouseButton0;
    }
    break;
    case ezVirtualThumbStick::Input::Custom:
    {
      EZ_ASSERT_DEV(pCustomConfig != nullptr, "Must pass a custom config, if you want to have a custom config.");

      for (ezInt32 i = 0; i < ezInputActionConfig::MaxInputSlotAlternatives; ++i)
      {
        m_ActionConfig.m_sFilterByInputSlotX[i] = pCustomConfig->m_sFilterByInputSlotX[i];
        m_ActionConfig.m_sFilterByInputSlotY[i] = pCustomConfig->m_sFilterByInputSlotY[i];
        m_ActionConfig.m_sInputSlotTrigger[i] = pCustomConfig->m_sInputSlotTrigger[i];
      }
    }
    break;
  }

  m_bConfigChanged = true;
}

void ezVirtualThumbStick::SetThumbstickOutput(ezVirtualThumbStick::Output::Enum output, ezStringView sOutputLeft, ezStringView sOutputRight, ezStringView sOutputUp, ezStringView sOutputDown)
{
  switch (output)
  {
    case ezVirtualThumbStick::Output::Controller0_LeftStick:
    {
      m_sOutputLeft = ezInputSlot_Controller0_LeftStick_NegX;
      m_sOutputRight = ezInputSlot_Controller0_LeftStick_PosX;
      m_sOutputUp = ezInputSlot_Controller0_LeftStick_PosY;
      m_sOutputDown = ezInputSlot_Controller0_LeftStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller0_RightStick:
    {
      m_sOutputLeft = ezInputSlot_Controller0_RightStick_NegX;
      m_sOutputRight = ezInputSlot_Controller0_RightStick_PosX;
      m_sOutputUp = ezInputSlot_Controller0_RightStick_PosY;
      m_sOutputDown = ezInputSlot_Controller0_RightStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller1_LeftStick:
    {
      m_sOutputLeft = ezInputSlot_Controller1_LeftStick_NegX;
      m_sOutputRight = ezInputSlot_Controller1_LeftStick_PosX;
      m_sOutputUp = ezInputSlot_Controller1_LeftStick_PosY;
      m_sOutputDown = ezInputSlot_Controller1_LeftStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller1_RightStick:
    {
      m_sOutputLeft = ezInputSlot_Controller1_RightStick_NegX;
      m_sOutputRight = ezInputSlot_Controller1_RightStick_PosX;
      m_sOutputUp = ezInputSlot_Controller1_RightStick_PosY;
      m_sOutputDown = ezInputSlot_Controller1_RightStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller2_LeftStick:
    {
      m_sOutputLeft = ezInputSlot_Controller2_LeftStick_NegX;
      m_sOutputRight = ezInputSlot_Controller2_LeftStick_PosX;
      m_sOutputUp = ezInputSlot_Controller2_LeftStick_PosY;
      m_sOutputDown = ezInputSlot_Controller2_LeftStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller2_RightStick:
    {
      m_sOutputLeft = ezInputSlot_Controller2_RightStick_NegX;
      m_sOutputRight = ezInputSlot_Controller2_RightStick_PosX;
      m_sOutputUp = ezInputSlot_Controller2_RightStick_PosY;
      m_sOutputDown = ezInputSlot_Controller2_RightStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller3_LeftStick:
    {
      m_sOutputLeft = ezInputSlot_Controller3_LeftStick_NegX;
      m_sOutputRight = ezInputSlot_Controller3_LeftStick_PosX;
      m_sOutputUp = ezInputSlot_Controller3_LeftStick_PosY;
      m_sOutputDown = ezInputSlot_Controller3_LeftStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Controller3_RightStick:
    {
      m_sOutputLeft = ezInputSlot_Controller3_RightStick_NegX;
      m_sOutputRight = ezInputSlot_Controller3_RightStick_PosX;
      m_sOutputUp = ezInputSlot_Controller3_RightStick_PosY;
      m_sOutputDown = ezInputSlot_Controller3_RightStick_NegY;
    }
    break;
    case ezVirtualThumbStick::Output::Custom:
    {
      m_sOutputLeft = sOutputLeft;
      m_sOutputRight = sOutputRight;
      m_sOutputUp = sOutputUp;
      m_sOutputDown = sOutputDown;
    }
    break;
  }

  m_bConfigChanged = true;
}

void ezVirtualThumbStick::SetAreaFocusMode(ezInputActionConfig::OnEnterArea onEnter, ezInputActionConfig::OnLeaveArea onLeave)
{
  m_bConfigChanged = true;

  m_ActionConfig.m_OnEnterArea = onEnter;
  m_ActionConfig.m_OnLeaveArea = onLeave;
}

void ezVirtualThumbStick::SetInputArea(const ezVec2& vLowerLeft, const ezVec2& vUpperRight, float fThumbstickRadius, float fPriority, CenterMode::Enum center)
{
  m_bConfigChanged = true;

  m_vLowerLeft = vLowerLeft;
  m_vUpperRight = vUpperRight;
  m_fRadius = fThumbstickRadius;
  m_ActionConfig.m_fFilteredPriority = fPriority;
  m_CenterMode = center;
}

void ezVirtualThumbStick::SetFlags(ezBitflags<Flags> flags)
{
  m_Flags = flags;
}

void ezVirtualThumbStick::SetInputCoordinateAspectRatio(float fWidthDivHeight)
{
  m_fAspectRatio = fWidthDivHeight;
}

void ezVirtualThumbStick::GetInputArea(ezVec2& out_vLowerLeft, ezVec2& out_vUpperRight) const
{
  out_vLowerLeft = m_vLowerLeft;
  out_vUpperRight = m_vUpperRight;
}

void ezVirtualThumbStick::UpdateActionMapping()
{
  if (!m_bConfigChanged)
    return;

  m_ActionConfig.m_fFilterXMinValue = m_vLowerLeft.x;
  m_ActionConfig.m_fFilterXMaxValue = m_vUpperRight.x;
  m_ActionConfig.m_fFilterYMinValue = m_vLowerLeft.y;
  m_ActionConfig.m_fFilterYMaxValue = m_vUpperRight.y;

  ezInputManager::SetInputActionConfig(GetDynamicRTTI()->GetTypeName(), m_sName.GetData(), m_ActionConfig, false);

  m_bConfigChanged = false;
}

void ezVirtualThumbStick::UpdateInputSlotValues()
{
  m_bIsActive = false;

  m_InputSlotValues[m_sOutputLeft] = 0.0f;
  m_InputSlotValues[m_sOutputRight] = 0.0f;
  m_InputSlotValues[m_sOutputUp] = 0.0f;
  m_InputSlotValues[m_sOutputDown] = 0.0f;

  if (!m_bEnabled)
  {
    ezInputManager::RemoveInputAction(GetDynamicRTTI()->GetTypeName(), m_sName.GetData());
    return;
  }

  UpdateActionMapping();

  float fValue;
  ezInt8 iTriggerAlt;

  const ezKeyState::Enum ks = ezInputManager::GetInputActionState(GetDynamicRTTI()->GetTypeName(), m_sName.GetData(), &fValue, &iTriggerAlt);

  if (ks != ezKeyState::Up)
  {
    m_bIsActive = true;

    if (m_CenterMode == CenterMode::Swipe)
    {
      const ezTime tDiff = ezClock::GetGlobalClock()->GetTimeDiff();

      m_vCenter = ezMath::Lerp(m_vCenter, m_vTouchPos, ezMath::Min(1.0f, tDiff.AsFloatInSeconds() * 4.0f));
    }

    m_vTouchPos.Set(0.0f);

    ezInputManager::GetInputSlotState(m_ActionConfig.m_sFilterByInputSlotX[(ezUInt32)iTriggerAlt].GetData(), &m_vTouchPos.x);
    ezInputManager::GetInputSlotState(m_ActionConfig.m_sFilterByInputSlotY[(ezUInt32)iTriggerAlt].GetData(), &m_vTouchPos.y);

    if (ks == ezKeyState::Pressed)
    {
      switch (m_CenterMode)
      {
        case CenterMode::InputArea:
          m_vCenter = m_vLowerLeft + (m_vUpperRight - m_vLowerLeft) * 0.5f;
          break;
        case CenterMode::ActivationPoint:
        case CenterMode::Swipe:
          m_vCenter = m_vTouchPos;
          break;
      }
    }

    m_vInputDirection = m_vTouchPos - m_vCenter;

    m_vInputDirection.y /= m_fAspectRatio;

    m_fInputStrength = ezMath::Min(m_vInputDirection.GetLength(), m_fRadius) / m_fRadius;
    m_vInputDirection.NormalizeIfNotZero(ezVec2::MakeZero()).IgnoreResult();

    const float fThreshold = 0.1f;

    float& l = m_InputSlotValues[m_sOutputLeft];
    float& r = m_InputSlotValues[m_sOutputRight];
    float& u = m_InputSlotValues[m_sOutputUp];
    float& d = m_InputSlotValues[m_sOutputDown];

    if (m_Flags.IsSet(Flags::OnlyMaxAxis))
    {
      const float maxVal = ezMath::Max(m_vInputDirection.x, -m_vInputDirection.x, m_vInputDirection.y, -m_vInputDirection.y);

      // only activate the output axis that has the strongest (absolute) value
      if (m_vInputDirection.x == maxVal)
      {
        r = maxVal * m_fInputStrength;
      }
      else if (-m_vInputDirection.x == maxVal)
      {
        l = maxVal * m_fInputStrength;
      }
      else if (m_vInputDirection.y == maxVal)
      {
        d = maxVal * m_fInputStrength;
      }
      else if (-m_vInputDirection.y == maxVal)
      {
        u = maxVal * m_fInputStrength;
      }
    }
    else
    {
      l = ezMath::Max(0.0f, -m_vInputDirection.x) * m_fInputStrength;
      r = ezMath::Max(0.0f, m_vInputDirection.x) * m_fInputStrength;
      u = ezMath::Max(0.0f, -m_vInputDirection.y) * m_fInputStrength;
      d = ezMath::Max(0.0f, m_vInputDirection.y) * m_fInputStrength;
    }

    if (l < fThreshold)
      l = 0.0f;
    if (r < fThreshold)
      r = 0.0f;
    if (u < fThreshold)
      u = 0.0f;
    if (d < fThreshold)
      d = 0.0f;
  }
}

void ezVirtualThumbStick::RegisterInputSlots()
{
  RegisterInputSlot(ezInputSlot_Controller0_LeftStick_NegX, "Left Stick Left", ezInputSlotFlags::IsAnalogStick);
  RegisterInputSlot(ezInputSlot_Controller0_LeftStick_PosX, "Left Stick Right", ezInputSlotFlags::IsAnalogStick);
  RegisterInputSlot(ezInputSlot_Controller0_LeftStick_NegY, "Left Stick Down", ezInputSlotFlags::IsAnalogStick);
  RegisterInputSlot(ezInputSlot_Controller0_LeftStick_PosY, "Left Stick Up", ezInputSlotFlags::IsAnalogStick);

  RegisterInputSlot(ezInputSlot_Controller0_RightStick_NegX, "Right Stick Left", ezInputSlotFlags::IsAnalogStick);
  RegisterInputSlot(ezInputSlot_Controller0_RightStick_PosX, "Right Stick Right", ezInputSlotFlags::IsAnalogStick);
  RegisterInputSlot(ezInputSlot_Controller0_RightStick_NegY, "Right Stick Down", ezInputSlotFlags::IsAnalogStick);
  RegisterInputSlot(ezInputSlot_Controller0_RightStick_PosY, "Right Stick Up", ezInputSlotFlags::IsAnalogStick);
}

EZ_STATICLINK_FILE(Core, Core_Input_Implementation_VirtualThumbStick);
