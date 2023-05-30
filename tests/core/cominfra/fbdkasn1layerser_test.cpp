/*******************************************************************************
 * Copyright (c) 2011 - 2014 ACIN, fortiss, nxtControl and Profactor
 *               2023 Martin Erich Jobst
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alois Zoitl, Ingo Hegny, Michael Hofmann, Stanislav Meduna - initial API and implementation and/or initial documentation
 *   Martin Jobst - account for new FB layout and varying data type size
 *******************************************************************************/
#include <boost/test/unit_test.hpp>
#include <algorithm>

#include "forte_boost_output_support.h"

#include "fbdkasn1layer_serdeserdata.h"

#include "../../../src/core/cominfra/fbdkasn1layer.h"

#include <boost/test/tools/floating_point_comparison.hpp>

#include "../../../src/core/datatypes/forte_real.h"

//BOOLEAN
#include "../../../src/core/datatypes/forte_bool.h"
//BIT-Datatypes
#include "../../../src/core/datatypes/forte_byte.h"
#include "../../../src/core/datatypes/forte_word.h"
#include "../../../src/core/datatypes/forte_dword.h"
//INT-Datatypes
#include "../../../src/core/datatypes/forte_sint.h"
#include "../../../src/core/datatypes/forte_usint.h"
#include "../../../src/core/datatypes/forte_int.h"
#include "../../../src/core/datatypes/forte_uint.h"
#include "../../../src/core/datatypes/forte_dint.h"
#include "../../../src/core/datatypes/forte_udint.h"
//STRING-Datatypes
#include "../../../src/core/datatypes/forte_string.h"
#include "../../../src/core/datatypes/forte_wstring.h"

#include "../../../src/core/datatypes/forte_time.h"

#include "../../../src/core/datatypes/forte_array.h"

#include "../../../src/core/datatypes/forte_lword.h"
#include "../../../src/core/datatypes/forte_lint.h"
#include "../../../src/core/datatypes/forte_ulint.h"
#include "../../../src/core/datatypes/forte_lreal.h"

#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "fbdkasn1layerser_test_gen.cpp"
#endif


class CFBDKASN1ComLayerTestMock: public forte::com_infra::CFBDKASN1ComLayer{
  public:
    CFBDKASN1ComLayerTestMock() : forte::com_infra::CFBDKASN1ComLayer(nullptr, nullptr){
      m_poBottomLayer = &m_oTestLayer;
    }

    ~CFBDKASN1ComLayerTestMock(){
      m_poBottomLayer = nullptr;
    }

    void *getSendDataPtr() {
      return m_oTestLayer.m_poData;
    }
    unsigned int getSendDataSize(){
      return m_oTestLayer.m_unSize;
    }

  private:
   class TestMockBottomLayer : public forte::com_infra::CComLayer{
     public:
     TestMockBottomLayer() : forte::com_infra::CComLayer(nullptr,nullptr){
       m_poData = nullptr;
       m_poAllocData = nullptr;
       m_unSize = 0;
     }

     ~TestMockBottomLayer(){
       if (nullptr != m_poAllocData) {
         delete[] m_poAllocData;
       }
     }

      forte::com_infra::EComResponse sendData(void *pa_pvData, unsigned int pa_unSize){
        if (nullptr != m_poAllocData) {
          delete[] m_poAllocData;
        }
        m_poAllocData = new TForteByte[pa_unSize];
        if (nullptr != m_poAllocData) {
          m_poData = m_poAllocData;
          memcpy(m_poData,pa_pvData,pa_unSize);
          //m_poData = pa_pvData;
          m_unSize = pa_unSize;
          return forte::com_infra::e_ProcessDataOk;
        } else {
          return forte::com_infra::e_ProcessDataSendFailed;
        }
       }

      virtual void closeConnection() {}
      virtual forte::com_infra::EComResponse recvData(const void *, unsigned int ) {
        return forte::com_infra::e_ProcessDataOk;
      }
      virtual forte::com_infra::EComResponse openConnection(char *){
        return forte::com_infra::e_ProcessDataOk;
      }

      TForteByte *m_poData;
      TForteByte *m_poAllocData;
       unsigned int m_unSize;
   };

   TestMockBottomLayer m_oTestLayer;
};



BOOST_AUTO_TEST_SUITE(fbdkasn1layer_serialize_test)

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_BOOL){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_BOOL nBool;
  TIEC_ANYPtr poArray[1];
  poArray[0] = &nBool;

  nBool = CIEC_BOOL(false);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unBoolSerSize);
  BOOST_CHECK_EQUAL(cg_abBoolFalse, *((TForteByte *)nTestee.getSendDataPtr()));

  nBool = CIEC_BOOL(true);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unBoolSerSize);
  BOOST_CHECK_EQUAL(cg_abBoolTrue, *((TForteByte *)nTestee.getSendDataPtr()));

  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(static_cast<TForteByte *>(nullptr), 0, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(static_cast<TForteByte *>(nullptr), 0, nBool), -1);
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_BYTE){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_BYTE nVal;
  TIEC_ANYPtr poArray[1];
  poArray[0] = &nVal;

  TForteByte acSmallBuf[1];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 1, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 1, nVal), -1);

  nVal= CIEC_BYTE(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unByteSerSize);
  BOOST_CHECK(std::equal(cg_abByte0, cg_abByte0 + cg_unByteSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_BYTE(12);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unByteSerSize);
  BOOST_CHECK(std::equal(cg_abByte12, cg_abByte12 + cg_unByteSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_BYTE(128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unByteSerSize);
  BOOST_CHECK(std::equal(cg_abByte128, cg_abByte128 + cg_unByteSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_BYTE(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unByteSerSize);
  BOOST_CHECK(std::equal(cg_abByte255, cg_abByte255 + cg_unByteSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_WORD){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_WORD nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[2];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 2, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 2, nVal), -1);

  nVal= CIEC_WORD(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWordSerSize);
  BOOST_CHECK(std::equal(cg_abWord0, cg_abWord0 + cg_unWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_WORD(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWordSerSize);
  BOOST_CHECK(std::equal(cg_abWord255, cg_abWord255 + cg_unWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_WORD(256);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWordSerSize);
  BOOST_CHECK(std::equal(cg_abWord256, cg_abWord256 + cg_unWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_WORD(65535);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWordSerSize);
  BOOST_CHECK(std::equal(cg_abWord65535, cg_abWord65535 + cg_unWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_WORD(40396);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWordSerSize);
  BOOST_CHECK(std::equal(cg_abWord40396, cg_abWord40396 + cg_unWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}


BOOST_AUTO_TEST_CASE(Single_Serialize_Test_DWORD){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_DWORD nVal;
  TIEC_ANYPtr poArray[1];
  poArray[0] = &nVal;

  TForteByte acSmallBuf[4];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 4, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 4, nVal), -1);

  nVal= CIEC_DWORD(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord0, cg_abDWord0 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_DWORD(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord255, cg_abDWord255 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_DWORD(256);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord256, cg_abDWord256 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_DWORD(65535);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord65535, cg_abDWord65535 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_DWORD(65536);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord65536, cg_abDWord65536 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_DWORD(4294967295UL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord4294967295, cg_abDWord4294967295 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_DWORD(690586453);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDWordSerSize);
  BOOST_CHECK(std::equal(cg_abDWord690586453, cg_abDWord690586453 + cg_unDWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

//LWORD
BOOST_AUTO_TEST_CASE(Single_Serialize_Test_LWORD){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_LWORD nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[8];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 8, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 8, nVal), -1);

  nVal= CIEC_LWORD(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord0, cg_abLWord0 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord255, cg_abLWord255 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(256);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord256, cg_abLWord256 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(65535);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord65535, cg_abLWord65535 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(65536);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord65536, cg_abLWord65536 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(4294967295LL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord4294967295, cg_abLWord4294967295 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(4294967296LL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord4294967296, cg_abLWord4294967296 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(18446744073709551615ULL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord18446744073709551615, cg_abLWord18446744073709551615 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_LWORD(18446744073709551615ULL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLWordSerSize);
  BOOST_CHECK(std::equal(cg_abLWord18446744073709551615, cg_abLWord18446744073709551615 + cg_unLWordSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_USINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_USINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[1];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 1, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 1, nVal), -1);

  nVal= CIEC_USINT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUSIntSerSize);
  BOOST_CHECK(std::equal(cg_abUSInt0, cg_abUSInt0 + cg_unUSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_USINT(12);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUSIntSerSize);
  BOOST_CHECK(std::equal(cg_abUSInt12, cg_abUSInt12 + cg_unUSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_USINT(128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUSIntSerSize);
  BOOST_CHECK(std::equal(cg_abUSInt128, cg_abUSInt128 + cg_unUSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_USINT(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUSIntSerSize);
  BOOST_CHECK(std::equal(cg_abUSInt255, cg_abUSInt255 + cg_unUSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_UINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_UINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[2];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 2, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 2, nVal), -1);

  nVal= CIEC_UINT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUIntSerSize);
  BOOST_CHECK(std::equal(cg_abUInt0, cg_abUInt0 + cg_unUIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UINT(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUIntSerSize);
  BOOST_CHECK(std::equal(cg_abUInt255, cg_abUInt255 + cg_unUIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UINT(256);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUIntSerSize);
  BOOST_CHECK(std::equal(cg_abUInt256, cg_abUInt256 + cg_unUIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));


  nVal= CIEC_UINT(65535);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUIntSerSize);
  BOOST_CHECK(std::equal(cg_abUInt65535, cg_abUInt65535 + cg_unUIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UINT(40396);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUIntSerSize);
  BOOST_CHECK(std::equal(cg_abUInt40396, cg_abUInt40396 + cg_unUIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}


BOOST_AUTO_TEST_CASE(Single_Serialize_Test_UDINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_UDINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[4];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 4, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 4, nVal), -1);

  nVal= CIEC_UDINT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt0, cg_abUDInt0 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UDINT(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt255, cg_abUDInt255 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UDINT(256);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt256, cg_abUDInt256 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UDINT(65535);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt65535, cg_abUDInt65535 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UDINT(65536);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt65536, cg_abUDInt65536 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UDINT(4294967295UL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt4294967295, cg_abUDInt4294967295 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_UDINT(690586453);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unUDIntSerSize);
  BOOST_CHECK(std::equal(cg_abUDInt690586453, cg_abUDInt690586453 + cg_unUDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

//LWORD
BOOST_AUTO_TEST_CASE(Single_Serialize_Test_ULINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_ULINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[8];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 8, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 8, nVal), -1);

  nVal= CIEC_ULINT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt0, cg_abULInt0 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_ULINT(255);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt255, cg_abULInt255 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));


  nVal= CIEC_ULINT(256);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt256, cg_abULInt256 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));


  nVal= CIEC_ULINT(65535);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt65535, cg_abULInt65535 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_ULINT(65536);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt65536, cg_abULInt65536 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_ULINT(4294967295ULL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt4294967295, cg_abULInt4294967295 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_ULINT(4294967296ULL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt4294967296, cg_abULInt4294967296 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_ULINT(18446744073709551615ULL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt18446744073709551615, cg_abULInt18446744073709551615 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal= CIEC_ULINT(18446744073709551615ULL);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unULIntSerSize);
  BOOST_CHECK(std::equal(cg_abULInt18446744073709551615, cg_abULInt18446744073709551615 + cg_unULIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}


BOOST_AUTO_TEST_CASE(Single_Serialize_Test_SINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_SINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[1];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 1, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 1, nVal), -1);

  nVal= CIEC_SINT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSIntSerSize);
  BOOST_CHECK(std::equal(cg_abSInt0, cg_abSInt0 + cg_unSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_SINT(-128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSIntSerSize);
  BOOST_CHECK(std::equal(cg_abSIntm128, cg_abSIntm128 + cg_unSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_SINT(127);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSIntSerSize);
  BOOST_CHECK(std::equal(cg_abSInt127, cg_abSInt127 + cg_unSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_SINT(-90);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSIntSerSize);
  BOOST_CHECK(std::equal(cg_abSIntm90, cg_abSIntm90 + cg_unSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_SINT(90);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSIntSerSize);
  BOOST_CHECK(std::equal(cg_abSInt90, cg_abSInt90 + cg_unSIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_INT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_INT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[2];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 2, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 2, nVal), -1);

  nVal= CIEC_INT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abInt0, cg_abInt0 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(-128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abIntm128, cg_abIntm128 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(-129);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abIntm129, cg_abIntm129 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(127);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abInt127, cg_abInt127 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abInt128, cg_abInt128 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(-32768);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abIntm32768, cg_abIntm32768 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(32767);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abInt32767, cg_abInt32767 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(-10934);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abIntm10934, cg_abIntm10934 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_INT(10934);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unIntSerSize);
  BOOST_CHECK(std::equal(cg_abInt10934, cg_abInt10934 + cg_unIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_DINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_DINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[4];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 4, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 4, nVal), -1);

  nVal= CIEC_DINT(0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt0, cg_abDInt0 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(-128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDIntm128, cg_abDIntm128 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(-129);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDIntm129, cg_abDIntm129 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(127);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt127, cg_abDInt127 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(128);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt128, cg_abDInt128 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(-32768);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDIntm32768, cg_abDIntm32768 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(-32769);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDIntm32769, cg_abDIntm32769 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(32767);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt32767, cg_abDInt32767 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(32768);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt32768, cg_abDInt32768 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(-2147483648L);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDIntm2147483648, cg_abDIntm2147483648 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(2147483647L);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt2147483647, cg_abDInt2147483647 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(-800058586);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDIntm800058586, cg_abDIntm800058586 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_DINT(800058586);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unDIntSerSize);
  BOOST_CHECK(std::equal(cg_abDInt800058586, cg_abDInt800058586 + cg_unDIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_LINT){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_LINT nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[8];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 8, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 8, nVal), -1);

  nVal= CIEC_LINT(0);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt0, cg_abLInt0 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-128);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm128, cg_abLIntm128 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-129);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm129, cg_abLIntm129 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(127);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt127, cg_abLInt127 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(128);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt128, cg_abLInt128 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-32768);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm32768, cg_abLIntm32768 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-32769);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm32769, cg_abLIntm32769 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(32767);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt32767, cg_abLInt32767 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(32768);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt32768, cg_abLInt32768 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-2147483648LL);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm2147483648, cg_abLIntm2147483648 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-2147483649LL);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm2147483649, cg_abLIntm2147483649 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(2147483647);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt2147483647, cg_abLInt2147483647 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(2147483648LL);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt2147483648, cg_abLInt2147483648 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-9223372036854775807LL - 1LL);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm9223372036854775808, cg_abLIntm9223372036854775808 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(9223372036854775807LL);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt9223372036854775807, cg_abLInt9223372036854775807 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(-800058586);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLIntm800058586, cg_abLIntm800058586 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

   nVal = CIEC_LINT(800058586);
   BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
   BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLIntSerSize);
   BOOST_CHECK(std::equal(cg_abLInt800058586, cg_abLInt800058586 + cg_unLIntSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_REAL){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_REAL nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[4];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 4, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 4, nVal), -1);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unRealSerSize);
  BOOST_CHECK(std::equal(cg_abReal0, cg_abReal0 + cg_unRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_REAL(2.2874e6f);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unRealSerSize);
  BOOST_CHECK(std::equal(cg_abReal2_2874e6, cg_abReal2_2874e6 + cg_unRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_REAL(-6.2587e-4f);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unRealSerSize);
  BOOST_CHECK(std::equal(cg_abRealm6_2587em4, cg_abRealm6_2587em4 + cg_unRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_REAL(1.0E-37f);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unRealSerSize);
  BOOST_CHECK(std::equal(cg_abReal1_0Em37, cg_abReal1_0Em37 + cg_unRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_REAL(36.0f);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unRealSerSize);
  BOOST_CHECK(std::equal(cg_abReal36_0, cg_abReal36_0 + cg_unRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_LREAL){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_LREAL nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[8];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 8, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 8, nVal), -1);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLRealSerSize);
  BOOST_CHECK(std::equal(cg_abLReal0, cg_abLReal0 + cg_unLRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_LREAL(2.28743e6);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLRealSerSize);
  BOOST_CHECK(std::equal(cg_abLReal2_28743e6, cg_abLReal2_28743e6 + cg_unLRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_LREAL(-6.2587e-4);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLRealSerSize);
  BOOST_CHECK(std::equal(cg_abLRealm6_2587em4, cg_abLRealm6_2587em4 + cg_unLRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_LREAL(1.0E-37);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLRealSerSize);
  BOOST_CHECK(std::equal(cg_abLReal1_0Em37, cg_abLReal1_0Em37 + cg_unLRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_LREAL(36.0);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unLRealSerSize);
  BOOST_CHECK(std::equal(cg_abLReal36_0, cg_abLReal36_0 + cg_unLRealSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_STRING){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_STRING nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[2];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 2, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 2, nVal), -1);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unStringEmptySerSize);
  BOOST_CHECK(std::equal(cg_abStringEmpty, cg_abStringEmpty + cg_unStringEmptySerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_STRING("HalloWorld");
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unStringHalloWorldSerSize);
  BOOST_CHECK(std::equal(cg_abStringHalloWorld, cg_abStringHalloWorld + cg_unStringHalloWorldSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  TForteByte acSecondSmallBuf[12];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSecondSmallBuf, 12, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSecondSmallBuf, 12, nVal), -1);
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_WSTRING){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_WSTRING nVal;
  TIEC_ANYPtr poArray[1];
    poArray[0] = &nVal;

  TForteByte acSmallBuf[2];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 2, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 2, nVal), -1);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWStringEmptySerSize);
  BOOST_CHECK(std::equal(cg_abWStringEmpty, cg_abWStringEmpty + cg_unWStringEmptySerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal = CIEC_WSTRING("HalloWorld");
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWStringHalloWorldSerSize);
  BOOST_CHECK(std::equal(cg_abWStringHalloWorld, cg_abWStringHalloWorld + cg_unWStringHalloWorldSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  TForteByte acSecondSmallBuf[3];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSecondSmallBuf, cg_unWStringHalloWorldSerSize-1, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSecondSmallBuf, cg_unWStringHalloWorldSerSize-1, nVal), -1);

  nVal = CIEC_WSTRING((const char *) cg_abWStringNihongoUTF8);
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unWStringNihongoSerSize);
  BOOST_CHECK(std::equal(cg_abWStringNihongo, cg_abWStringNihongo + cg_unWStringNihongoSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_TIME){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_TIME nVal;
  TIEC_ANYPtr poArray[1];
  poArray[0] = &nVal;

  TForteByte acSmallBuf[8];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 8, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 8, nVal), -1);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unTimeSerSize);
  BOOST_CHECK(std::equal(cg_abTime0, cg_abTime0 + cg_unStringEmptySerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal.fromString("T#3000ms");
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unTimeSerSize);
  BOOST_CHECK(std::equal(cg_abTime3000ms, cg_abTime3000ms + cg_unTimeSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  nVal.fromString("T#3s22ms");
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unTimeSerSize);
  BOOST_CHECK(std::equal(cg_abTime3s22ms, cg_abTime3s22ms + cg_unTimeSerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_MultiDatas){
  CFBDKASN1ComLayerTestMock nTestee;

  CIEC_TIME *poTimeVal1;
  CIEC_WORD *poWordVal;
  CIEC_STRING *poStringVal;
  CIEC_INT *poIntVal;
  CIEC_BOOL *poBoolVal;
  CIEC_TIME *poTimeVal2;

  TIEC_ANYPtr poArray[6];

  poArray[0] = poTimeVal1 = new CIEC_TIME();
  poArray[1] = poWordVal = new CIEC_WORD();
  poArray[2] = poStringVal = new CIEC_STRING();
  poArray[3] = poIntVal = new CIEC_INT();
  poArray[4] = poBoolVal = new CIEC_BOOL();
  poArray[5] = poTimeVal2 = new CIEC_TIME();

  poTimeVal1->fromString("T#3000ms");
  *poWordVal = CIEC_WORD(40396);
  *poStringVal = CIEC_STRING("HalloWorld");
  *poIntVal = CIEC_INT(-10934);
  *poBoolVal = CIEC_BOOL(true);
  poTimeVal2->fromString("T#3s22ms");

  const unsigned int nSerSize = cg_unTimeSerSize + cg_unWordSerSize + cg_unStringHalloWorldSerSize + cg_unIntSerSize + cg_unBoolSerSize + cg_unTimeSerSize;
  TForteByte anGoodResult[] = {0x4C, 0, 0, 0, 0, 0, 0x2D, 0xC6, 0xC0, 0x52, 0x9D, 0xCC, 0x50, 0, 0xA, 'H', 'a', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', 0x43, 0xD5, 0x4A, 0x41, 0x4C, 0, 0, 0, 0, 0, 0x2e, 0x1c, 0xb0};

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 6));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), nSerSize);
  BOOST_CHECK(std::equal(anGoodResult, anGoodResult + nSerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  TForteByte acSmallBuf[nSerSize - 1];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, nSerSize - 2, const_cast<TConstIEC_ANYPtr *>(poArray), 6), -1);
  delete poTimeVal1;
  delete poWordVal;
  delete poStringVal;
  delete poIntVal;
  delete poBoolVal;
  delete poTimeVal2;
}

BOOST_AUTO_TEST_CASE(Single_Serialize_Test_ARRAY){
  CFBDKASN1ComLayerTestMock nTestee;
  CIEC_ARRAY_DYNAMIC nVal(5, g_nStringIdBOOL);
  TIEC_ANYPtr poArray[1];
  poArray[0] = &nVal;


  static_cast<CIEC_BOOL &>(nVal[0]) = CIEC_BOOL(true);
  static_cast<CIEC_BOOL &>(nVal[1]) = CIEC_BOOL(false);
  static_cast<CIEC_BOOL &>(nVal[2]) = CIEC_BOOL(false);
  static_cast<CIEC_BOOL &>(nVal[3]) = CIEC_BOOL(true);
  static_cast<CIEC_BOOL &>(nVal[4]) = CIEC_BOOL(true);

  TForteByte acSmallBuf[7];
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 1, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 3, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPointArray(acSmallBuf, 7, const_cast<TConstIEC_ANYPtr *>(poArray), 1), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 1, nVal), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 3, nVal), -1);
  BOOST_CHECK_EQUAL(nTestee.serializeDataPoint(acSmallBuf, 7, nVal), -1);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unBOOL5SerSize);
  BOOST_CHECK(std::equal(cg_abArrayBool10011, cg_abArrayBool10011 + cg_unBOOL5SerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  static_cast<CIEC_BOOL &>(nVal[0]) = CIEC_BOOL(false);
  static_cast<CIEC_BOOL &>(nVal[1]) = CIEC_BOOL(true);
  static_cast<CIEC_BOOL &>(nVal[2]) = CIEC_BOOL(false);
  static_cast<CIEC_BOOL &>(nVal[3]) = CIEC_BOOL(true);
  static_cast<CIEC_BOOL &>(nVal[4]) = CIEC_BOOL(false);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unBOOL5SerSize);
  BOOST_CHECK(std::equal(cg_abArrayBool01010, cg_abArrayBool01010 + cg_unBOOL5SerSize, ((TForteByte *)nTestee.getSendDataPtr())));


  CIEC_ARRAY_DYNAMIC nSIntArray(4, g_nStringIdSINT);
  poArray[0] = &nSIntArray;

  static_cast<CIEC_SINT &>(nSIntArray[0]) = CIEC_SINT(-128);
  static_cast<CIEC_SINT &>(nSIntArray[1]) = CIEC_SINT(127);
  static_cast<CIEC_SINT &>(nSIntArray[2]) = CIEC_SINT(0);
  static_cast<CIEC_SINT &>(nSIntArray[3]) = CIEC_SINT(-90);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSINT4SerSize);
  BOOST_CHECK(std::equal(cg_abArraySINTm128_127_0_m90, cg_abArraySINTm128_127_0_m90 + cg_unSINT4SerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  static_cast<CIEC_SINT &>(nSIntArray[0]) = CIEC_SINT(-90);
  static_cast<CIEC_SINT &>(nSIntArray[1]) = CIEC_SINT(90);
  static_cast<CIEC_SINT &>(nSIntArray[2]) = CIEC_SINT(127);
  static_cast<CIEC_SINT &>(nSIntArray[3]) = CIEC_SINT(0);

  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unSINT4SerSize);
  BOOST_CHECK(std::equal(cg_abArraySINTm90_90_127_0, cg_abArraySINTm90_90_127_0 + cg_unSINT4SerSize, ((TForteByte *)nTestee.getSendDataPtr())));

  CIEC_ARRAY_DYNAMIC nStringArray(2, g_nStringIdSTRING);
  poArray[0] = &nStringArray;

  static_cast<CIEC_STRING &>(nStringArray[1]) = CIEC_STRING("HalloWorld");
  BOOST_CHECK_EQUAL(forte::com_infra::e_ProcessDataOk, nTestee.sendData(poArray, 1));
  BOOST_CHECK_EQUAL(nTestee.getSendDataSize(), cg_unString2SerSize);
  BOOST_CHECK(std::equal(cg_abArrayStringEmptyHalloWorld, cg_abArrayStringEmptyHalloWorld + cg_unString2SerSize, ((TForteByte *)nTestee.getSendDataPtr())));
}

BOOST_AUTO_TEST_SUITE_END()
