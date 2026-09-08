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

#pragma once

#include "forte/funcbloc.h"
#include "forte/datatypes/forte_any_num_variant.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/forte_st_util.h"
#include "forte/datatypes/forte_array_common.h"
#include "forte/datatypes/forte_array.h"
#include "forte/datatypes/forte_array_fixed.h"
#include "forte/datatypes/forte_array_variable.h"

namespace forte::SafeArithmetic::arithmetic {
  class FORTE_SAFE_DIV : public CFunctionBlock {
      DECLARE_FIRMWARE_FB(FORTE_SAFE_DIV)

    private:
      static const TEventID scmEventREQID = 0;

      static const TEventID scmEventCNFID = 0;

      void executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) override;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;

    public:
      FORTE_SAFE_DIV(const StringId paInstanceNameId, CFBContainer &paContainer);

      CIEC_ANY_NUM_VARIANT var_IN1;
      CIEC_ANY_NUM_VARIANT var_IN2;
      CIEC_ANY_NUM_VARIANT var_OUT;
      CIEC_BOOL var_LIMIT_HIT;

      CEventConnection conn_CNF;
      CDataConnection *conn_IN1;
      CDataConnection *conn_IN2;
      COutDataConnection<CIEC_ANY_NUM_VARIANT> conn_OUT;
      COutDataConnection<CIEC_BOOL> conn_LIMIT_HIT;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;
      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

    protected:
      void setInitialValues() override;
  };
} // namespace forte::SafeArithmetic::arithmetic
