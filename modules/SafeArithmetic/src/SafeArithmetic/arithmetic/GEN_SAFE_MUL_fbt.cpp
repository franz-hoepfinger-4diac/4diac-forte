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

#include "forte/SafeArithmetic/arithmetic/GEN_SAFE_MUL_fbt.h"
#include "forte/SafeArithmetic/arithmetic/safe_arithmetic_ops.h"

using namespace forte::literals;

namespace forte::SafeArithmetic::arithmetic {
  DEFINE_GENERIC_FIRMWARE_FB(GEN_SAFE_MUL, "SafeArithmetic::arithmetic::GEN_SAFE_MUL"_STRID)

  GEN_SAFE_MUL::GEN_SAFE_MUL(const StringId paInstanceNameId, CFBContainer &paContainer) :
      CGenSafeArithBase<CIEC_ANY_NUM_VARIANT>(paInstanceNameId, paContainer) {
  }

  void GEN_SAFE_MUL::executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) {
    if (paEIID == scmEventREQID) {
      var_OUT = var_IN(0);
      bool limitHit = false;
      for (size_t i = 1; i < getFBInterfaceSpec().getNumDIs(); ++i) {
        var_OUT = std::visit(
            [&limitHit](auto &&paOUT, auto &&paIN) -> CIEC_ANY_NUM_VARIANT {
              using T = std::decay_t<decltype(paOUT)>;
              using U = std::decay_t<decltype(paIN)>;
              using deductedType = typename mpl::get_mul_operator_result_type<T, U>::type;
              if constexpr (!std::is_same<deductedType, mpl::NullType>::value) {
                bool stepLimitHit = false;
                auto result = safe_mul(paOUT, paIN, stepLimitHit);
                limitHit = limitHit || stepLimitHit;
                return result;
              }
              DEVLOG_ERROR("Multiplying incompatible types %s and %s\n", paOUT.getTypeNameID().data(),
                           paIN.getTypeNameID().data());
              return paOUT;
            },
            static_cast<CIEC_ANY_NUM_VARIANT::variant &>(var_OUT),
            static_cast<CIEC_ANY_NUM_VARIANT::variant &>(var_IN(i)));
      }
      var_LIMIT_HIT = CIEC_BOOL(limitHit);
      sendOutputEvent(scmEventCNFID, paECET);
    }
  }
} // namespace forte::SafeArithmetic::arithmetic
