#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/Containers/Deque.h>
#include <Foundation/IO/JSONReader.h>

namespace JSONReaderTestDetail
{

  class StringStream : public ezStreamReader
  {
  public:
    StringStream(const void* pData)
    {
      m_pData = pData;
      m_uiLength = ezStringUtils::GetStringElementCount((const char*)pData);
    }

    virtual ezUInt64 ReadBytes(void* pReadBuffer, ezUInt64 uiBytesToRead)
    {
      uiBytesToRead = ezMath::Min(uiBytesToRead, m_uiLength);
      m_uiLength -= uiBytesToRead;

      if (uiBytesToRead > 0)
      {
        ezMemoryUtils::Copy((ezUInt8*)pReadBuffer, (ezUInt8*)m_pData, (size_t)uiBytesToRead);
        m_pData = ezMemoryUtils::AddByteOffset(m_pData, (ptrdiff_t)uiBytesToRead);
      }

      return uiBytesToRead;
    }

  private:
    const void* m_pData;
    ezUInt64 m_uiLength;
  };

  void TraverseTree(const ezVariant& var, ezDeque<ezString>& ref_compare)
  {
    if (ref_compare.IsEmpty())
      return;

    switch (var.GetType())
    {
      case ezVariant::Type::VariantDictionary:
      {
        // ezLog::Printf("Expect: %s - Is: %s\n", "<object>", Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), "<object>");
        ref_compare.PopFront();

        const ezVariantDictionary& vd = var.Get<ezVariantDictionary>();

        for (auto it = vd.GetIterator(); it.IsValid(); ++it)
        {
          if (ref_compare.IsEmpty())
            return;

          // ezLog::Printf("Expect: %s - Is: %s\n", it.Key().GetData(), Compare.PeekFront().GetData());
          EZ_TEST_STRING(ref_compare.PeekFront().GetData(), it.Key().GetData());
          ref_compare.PopFront();

          TraverseTree(it.Value(), ref_compare);
        }

        if (ref_compare.IsEmpty())
          return;

        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), "</object>");
        // ezLog::Printf("Expect: %s - Is: %s\n", "</object>", Compare.PeekFront().GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::VariantArray:
      {
        // ezLog::Printf("Expect: %s - Is: %s\n", "<array>", Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), "<array>");
        ref_compare.PopFront();

        const ezVariantArray& va = var.Get<ezVariantArray>();

        for (ezUInt32 i = 0; i < va.GetCount(); ++i)
        {
          TraverseTree(va[i], ref_compare);
        }

        if (ref_compare.IsEmpty())
          return;

        // ezLog::Printf("Expect: %s - Is: %s\n", "</array>", Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), "</array>");
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Invalid:
        // ezLog::Printf("Expect: %s - Is: %s\n", "null", Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), "null");
        ref_compare.PopFront();
        break;

      case ezVariant::Type::Bool:
        // ezLog::Printf("Expect: %s - Is: %s\n", var.Get<bool>() ? "bool true" : "bool false", Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), var.Get<bool>() ? "bool true" : "bool false");
        ref_compare.PopFront();
        break;

      case ezVariant::Type::Int8:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("int8 {0}", var.Get<ezInt8>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::UInt8:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("uint8 {0}", var.Get<ezUInt8>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Int16:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("int16 {0}", var.Get<ezInt16>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::UInt16:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("uint16 {0}", var.Get<ezUInt16>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Int32:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("int32 {0}", var.Get<ezInt32>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::UInt32:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("uint32 {0}", var.Get<ezUInt32>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Int64:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("int64 {0}", var.Get<ezInt64>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::UInt64:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("uint64 {0}", var.Get<ezUInt64>());
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Float:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("float {0}", ezArgF(var.Get<float>(), 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Double:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("double {0}", ezArgF(var.Get<double>(), 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Time:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("time {0}", ezArgF(var.Get<ezTime>().GetSeconds(), 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Angle:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("angle {0}", ezArgF(var.Get<ezAngle>().GetDegree(), 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::String:
        // ezLog::Printf("Expect: %s - Is: %s\n", var.Get<ezString>().GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), var.Get<ezString>().GetData());
        ref_compare.PopFront();
        break;

      case ezVariant::Type::StringView:
        // ezLog::Printf("Expect: %s - Is: %s\n", var.Get<ezString>().GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront(), var.Get<ezStringView>());
        ref_compare.PopFront();
        break;

      case ezVariant::Type::Vector2:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("vec2 ({0}, {1})", ezArgF(var.Get<ezVec2>().x, 4), ezArgF(var.Get<ezVec2>().y, 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Vector3:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("vec3 ({0}, {1}, {2})", ezArgF(var.Get<ezVec3>().x, 4), ezArgF(var.Get<ezVec3>().y, 4), ezArgF(var.Get<ezVec3>().z, 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Vector4:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("vec4 ({0}, {1}, {2}, {3})", ezArgF(var.Get<ezVec4>().x, 4), ezArgF(var.Get<ezVec4>().y, 4), ezArgF(var.Get<ezVec4>().z, 4), ezArgF(var.Get<ezVec4>().w, 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Vector2I:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("vec2i ({0}, {1})", var.Get<ezVec2I32>().x, var.Get<ezVec2I32>().y);
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Vector3I:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("vec3i ({0}, {1}, {2})", var.Get<ezVec3I32>().x, var.Get<ezVec3I32>().y, var.Get<ezVec3I32>().z);
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Vector4I:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("vec4i ({0}, {1}, {2}, {3})", var.Get<ezVec4I32>().x, var.Get<ezVec4I32>().y, var.Get<ezVec4I32>().z, var.Get<ezVec4I32>().w);
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Color:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("color ({0}, {1}, {2}, {3})", ezArgF(var.Get<ezColor>().r, 4), ezArgF(var.Get<ezColor>().g, 4), ezArgF(var.Get<ezColor>().b, 4), ezArgF(var.Get<ezColor>().a, 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::ColorGamma:
      {
        ezStringBuilder sTemp;
        const ezColorGammaUB c = var.ConvertTo<ezColorGammaUB>();

        sTemp.SetFormat("gamma ({0}, {1}, {2}, {3})", c.r, c.g, c.b, c.a);
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Quaternion:
      {
        ezStringBuilder sTemp;
        sTemp.SetFormat("quat ({0}, {1}, {2}, {3})", ezArgF(var.Get<ezQuat>().x, 4), ezArgF(var.Get<ezQuat>().y, 4), ezArgF(var.Get<ezQuat>().z, 4), ezArgF(var.Get<ezQuat>().w, 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Matrix3:
      {
        ezMat3 m = var.Get<ezMat3>();

        ezStringBuilder sTemp;
        sTemp.SetFormat("mat3 ({0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8})", ezArgF(m.m_fElementsCM[0], 4), ezArgF(m.m_fElementsCM[1], 4), ezArgF(m.m_fElementsCM[2], 4), ezArgF(m.m_fElementsCM[3], 4), ezArgF(m.m_fElementsCM[4], 4), ezArgF(m.m_fElementsCM[5], 4), ezArgF(m.m_fElementsCM[6], 4), ezArgF(m.m_fElementsCM[7], 4), ezArgF(m.m_fElementsCM[8], 4));
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Matrix4:
      {
        ezMat4 m = var.Get<ezMat4>();

        ezStringBuilder sTemp;
        sTemp.SetPrintf("mat4 (%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f)", m.m_fElementsCM[0], m.m_fElementsCM[1], m.m_fElementsCM[2], m.m_fElementsCM[3], m.m_fElementsCM[4], m.m_fElementsCM[5], m.m_fElementsCM[6], m.m_fElementsCM[7], m.m_fElementsCM[8], m.m_fElementsCM[9], m.m_fElementsCM[10], m.m_fElementsCM[11], m.m_fElementsCM[12], m.m_fElementsCM[13], m.m_fElementsCM[14], m.m_fElementsCM[15]);
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      case ezVariant::Type::Uuid:
      {
        ezUuid uuid = var.Get<ezUuid>();
        ezStringBuilder sTemp;
        ezConversionUtils::ToString(uuid, sTemp);
        sTemp.Prepend("uuid ");
        // ezLog::Printf("Expect: %s - Is: %s\n", sTemp.GetData(), Compare.PeekFront().GetData());
        EZ_TEST_STRING(ref_compare.PeekFront().GetData(), sTemp.GetData());
        ref_compare.PopFront();
      }
      break;

      default:
        EZ_ASSERT_NOT_IMPLEMENTED;
        break;
    }
  }
} // namespace JSONReaderTestDetail

EZ_CREATE_SIMPLE_TEST(IO, JSONReader)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Test")
  {
    ezStringUtf8 sTD(L"{\n\
\"myarray2\":[\"\",2.2],\n\
\"myarray\" : [1, 2.2, 3.3, false, \"ende\" ],\n\
\"String\"/**/ : \"testvälue\",\n\
\"double\"/***/ : 43.56,//comment\n\
\"float\" :/**//*a*/ 64/*comment*/.720001,\n\
\"bool\" : tr/*asdf*/ue,\n\
\"int\" : 23,\n\
\"MyNüll\" : nu/*asdf*/ll,\n\
\"object\" :\n\
/* totally \n weird \t stuff \n\n\n going on here // thats a line comment \n */ \
// more line comments \n\n\n\n\
{\n\
  \"variable in object\" : \"bla\\\\\\\"\\/\",\n\
    \"Subobject\" :\n\
  {\n\
    \"variable in subobject\" : \"blub\\r\\f\\n\\b\\t\",\n\
      \"array in sub\" : [\n\
    {\n\
      \"obj var\" : 234\n\
            /*stuff ] */ \
    },\n\
    {\n\
      \"obj var 2\" : -235\n//breakingcomment\n\
    }, true, 4, false ]\n\
  }\n\
},\n\
\"test\" : \"text\"\n\
}");
    const char* szTestData = sTD.GetData();

    // NOTE: The way this test is implemented, it might break, if the HashMap uses another insertion algorithm.
    // ezVariantDictionary is an ezHashmap and this test currently relies on one exact order in of the result.
    // If this should ever change (or be arbitrary at runtime), the test needs to be implemented in a more robust way.

    JSONReaderTestDetail::StringStream stream(szTestData);

    ezJSONReader reader;
    EZ_TEST_BOOL(reader.Parse(stream).Succeeded());

    ezDeque<ezString> sCompare;
    sCompare.PushBack("<object>");
    sCompare.PushBack("int");
    sCompare.PushBack("double 23.0000");
    sCompare.PushBack("String");
    sCompare.PushBack(ezStringUtf8(L"testvälue").GetData()); // unicode literal

    sCompare.PushBack("double");
    sCompare.PushBack("double 43.5600");

    sCompare.PushBack("myarray");
    sCompare.PushBack("<array>");
    sCompare.PushBack("double 1.0000");
    sCompare.PushBack("double 2.2000");
    sCompare.PushBack("double 3.3000");
    sCompare.PushBack("bool false");
    sCompare.PushBack("ende");
    sCompare.PushBack("</array>");

    sCompare.PushBack("object");
    sCompare.PushBack("<object>");

    sCompare.PushBack("Subobject");
    sCompare.PushBack("<object>");

    sCompare.PushBack("array in sub");
    sCompare.PushBack("<array>");

    sCompare.PushBack("<object>");
    sCompare.PushBack("obj var");
    sCompare.PushBack("double 234.0000");
    sCompare.PushBack("</object>");

    sCompare.PushBack("<object>");
    sCompare.PushBack("obj var 2");
    sCompare.PushBack("double -235.0000");
    sCompare.PushBack("</object>");

    sCompare.PushBack("bool true");
    sCompare.PushBack("double 4.0000");
    sCompare.PushBack("bool false");

    sCompare.PushBack("</array>");


    sCompare.PushBack("variable in subobject");
    sCompare.PushBack("blub\r\f\n\b\t"); // escaped special characters

    sCompare.PushBack("</object>");

    sCompare.PushBack("variable in object");
    sCompare.PushBack("bla\\\"/"); // escaped backslash, quotation mark, slash

    sCompare.PushBack("</object>");

    sCompare.PushBack("float");
    sCompare.PushBack("double 64.7200");

    sCompare.PushBack("myarray2");
    sCompare.PushBack("<array>");
    sCompare.PushBack("");
    sCompare.PushBack("double 2.2000");
    sCompare.PushBack("</array>");

    sCompare.PushBack(ezStringUtf8(L"MyNüll").GetData()); // unicode literal
    sCompare.PushBack("null");

    sCompare.PushBack("test");
    sCompare.PushBack("text");

    sCompare.PushBack("bool");
    sCompare.PushBack("bool true");

    sCompare.PushBack("</object>");

    if (EZ_TEST_BOOL(reader.GetTopLevelElementType() == ezJSONReader::ElementType::Dictionary))
    {
      JSONReaderTestDetail::TraverseTree(reader.GetTopLevelObject(), sCompare);

      EZ_TEST_BOOL(sCompare.IsEmpty());
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Array document")
  {
    const char* szTestData = "[\"a\",\"b\"]";

    // NOTE: The way this test is implemented, it might break, if the HashMap uses another insertion algorithm.
    // ezVariantDictionary is an ezHashmap and this test currently relies on one exact order in of the result.
    // If this should ever change (or be arbitrary at runtime), the test needs to be implemented in a more robust way.

    JSONReaderTestDetail::StringStream stream(szTestData);

    ezJSONReader reader;
    EZ_TEST_BOOL(reader.Parse(stream).Succeeded());

    ezDeque<ezString> sCompare;
    sCompare.PushBack("<array>");
    sCompare.PushBack("a");
    sCompare.PushBack("b");
    sCompare.PushBack("</array>");

    if (EZ_TEST_BOOL(reader.GetTopLevelElementType() == ezJSONReader::ElementType::Array))
    {
      JSONReaderTestDetail::TraverseTree(reader.GetTopLevelArray(), sCompare);

      EZ_TEST_BOOL(sCompare.IsEmpty());
    }
  }
}
