/*******************************************************************************
 * Copyright (c) 2026 HR Agrartechnik GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Franz Höpfinger
 *      - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "forte/SafeArithmetic/arithmetic/SAFE_SUB_fbt.h"
#include "forte/SafeArithmetic/arithmetic/safe_arithmetic_ops.h"

using namespace forte::literals;

namespace forte::SafeArithmetic::arithmetic {
  namespace {
    const auto cDataInputNames = std::array{"IN1"_STRID, "IN2"_STRID};

    const auto cDataOutputNames = std::array{"OUT"_STRID, "LIMIT_HIT"_STRID};

    const auto cEventInputNames = std::array{"REQ"_STRID};
    const auto cEventInputTypeIds = std::array{"Event"_STRID};

    const auto cEventOutputNames = std::array{"CNF"_STRID};
    const auto cEventOutputTypeIds = std::array{"Event"_STRID};

    const SFBInterfaceSpec cFBInterfaceSpec = {
        .mEINames = cEventInputNames,
        .mEITypeNames = {},
        .mEONames = cEventOutputNames,
        .mEOTypeNames = {},
        .mDINames = cDataInputNames,
        .mDONames = cDataOutputNames,
        .mDIONames = {},
        .mSocketNames = {},
        .mPlugNames = {},
    };
  } // namespace

  DEFINE_FIRMWARE_FB(FORTE_SAFE_SUB, "SafeArithmetic::arithmetic::SAFE_SUB"_STRID)

  FORTE_SAFE_SUB::FORTE_SAFE_SUB(const StringId paInstanceNameId, CFBContainer &paContainer) :
      CFunctionBlock(paContainer, cFBInterfaceSpec, paInstanceNameId),
      var_IN1(CIEC_ANY_MAGNITUDE_VARIANT()),
      var_IN2(CIEC_ANY_MAGNITUDE_VARIANT()),
      var_OUT(CIEC_ANY_MAGNITUDE_VARIANT()),
      var_LIMIT_HIT(false),
      conn_CNF(*this, 0),
      conn_IN1(nullptr),
      conn_IN2(nullptr),
      conn_OUT(*this, 0, var_OUT),
      conn_LIMIT_HIT(*this, 1, var_LIMIT_HIT) {};

  void FORTE_SAFE_SUB::executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) {
    switch (paEIID) {
      case scmEventREQID:
        var_LIMIT_HIT = CIEC_BOOL(false);
        var_OUT = std::visit(
            [this](auto &&paIN1, auto &&paIN2) -> CIEC_ANY_MAGNITUDE_VARIANT {
              using T = std::decay_t<decltype(paIN1)>;
              using U = std::decay_t<decltype(paIN2)>;
              using deductedType = typename mpl::get_sub_operator_result_type<T, U>::type;
              if constexpr (!std::is_same<deductedType, mpl::NullType>::value) {
                bool limitHit = false;
                auto result = safe_sub(paIN1, paIN2, limitHit);
                var_LIMIT_HIT = CIEC_BOOL(limitHit);
                return result;
              }
              DEVLOG_ERROR("Subtracting incompatible types %s and %s\n", paIN1.getTypeNameID().data(),
                           paIN2.getTypeNameID().data());
              return CIEC_ANY_MAGNITUDE_VARIANT();
            },
            static_cast<CIEC_ANY_MAGNITUDE_VARIANT::variant &>(var_IN1),
            static_cast<CIEC_ANY_MAGNITUDE_VARIANT::variant &>(var_IN2));
        sendOutputEvent(scmEventCNFID, paECET);
        break;
    }
  }

  void FORTE_SAFE_SUB::readInputData(TEventID paEIID) {
    switch (paEIID) {
      case scmEventREQID: {
        readData(0, var_IN1, conn_IN1);
        readData(1, var_IN2, conn_IN2);
        break;
      }
      default: break;
    }
  }

  void FORTE_SAFE_SUB::writeOutputData(TEventID paEIID) {
    switch (paEIID) {
      case scmEventCNFID: {
        writeData(cFBInterfaceSpec.getNumDIs() + 0, var_OUT, conn_OUT);
        writeData(cFBInterfaceSpec.getNumDIs() + 1, var_LIMIT_HIT, conn_LIMIT_HIT);
        break;
      }
      default: break;
    }
  }

  CIEC_ANY *FORTE_SAFE_SUB::getDI(size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_IN1;
      case 1: return &var_IN2;
    }
    return nullptr;
  }

  CIEC_ANY *FORTE_SAFE_SUB::getDO(size_t paIndex) {
    switch (paIndex) {
      case 0: return &var_OUT;
      case 1: return &var_LIMIT_HIT;
    }
    return nullptr;
  }

  CEventConnection *FORTE_SAFE_SUB::getEOConUnchecked(TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_CNF;
    }
    return nullptr;
  }

  CDataConnection **FORTE_SAFE_SUB::getDIConUnchecked(TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_IN1;
      case 1: return &conn_IN2;
    }
    return nullptr;
  }

  CDataConnection *FORTE_SAFE_SUB::getDOConUnchecked(TPortId paIndex) {
    switch (paIndex) {
      case 0: return &conn_OUT;
      case 1: return &conn_LIMIT_HIT;
    }
    return nullptr;
  }

  void FORTE_SAFE_SUB::setInitialValues() {
    var_IN1.reset();
    var_IN2.reset();
    var_OUT.reset();
    var_LIMIT_HIT = CIEC_BOOL(false);
  }

} // namespace forte::SafeArithmetic::arithmetic
