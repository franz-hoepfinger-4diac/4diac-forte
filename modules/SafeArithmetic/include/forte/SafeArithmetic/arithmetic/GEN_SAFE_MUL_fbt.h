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

#include "gensafearithbase_fbt.h"
#include "forte/datatypes/forte_any_num_variant.h"

namespace forte::SafeArithmetic::arithmetic {
  class GEN_SAFE_MUL final : public CGenSafeArithBase<CIEC_ANY_NUM_VARIANT> {
      DECLARE_GENERIC_FIRMWARE_FB(GEN_SAFE_MUL)

    private:
      void executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) override;

    public:
      GEN_SAFE_MUL(const StringId paInstanceNameId, CFBContainer &paContainer);
      ~GEN_SAFE_MUL() override = default;
  };
} // namespace forte::SafeArithmetic::arithmetic
