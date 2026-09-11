/*******************************************************************************
 * Copyright (c) 2026 Meisterschulen am Ostbahnhof
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Franz Höpfinger
 *     - implement Generic GEN_E_PERMIT_fbt
 *******************************************************************************/

#pragma once

#include <vector>
#include "forte/genfb.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/stringid.h"

namespace forte::iec61499::events {
  /**
   * @brief A generic function block for permissive propagation of N parallel event channels.
   *
   * The number of channels is determined by the instance name, e.g., an instance
   * named "E_PERMIT_3" will have 3 independent channels (EI1/EO1, EI2/EO2, EI3/EO3),
   * all gated by one shared data input PERMIT: EIi is only forwarded as EOi while
   * PERMIT is true. Equivalent to N parallel instances of the fixed, single-channel
   * FB E_PERMIT sharing the same PERMIT condition.
   */
  class GEN_E_PERMIT final : public CGenFunctionBlock<CFunctionBlock> {
      DECLARE_GENERIC_FIRMWARE_FB(GEN_E_PERMIT)

    protected:
      /** PERMIT is the single fixed data input; it is not part of the generic set. */
      size_t getGenDIOffset() override {
        return 1;
      }

    private:
      void executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) override;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;

      bool createInterfaceSpec(const char *paConfigString, SFBInterfaceSpec &paInterfaceSpec) override;

      CIEC_ANY *getDI(size_t paIndex) override;
      CIEC_ANY *getDO(size_t) override;
      CDataConnection **getDIConUnchecked(TPortId paIndex) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

      /** Dynamically generated event names for EI1..EIn / EO1..EOn. */
      std::vector<StringId> mEINames;
      std::vector<StringId> mEONames;

      /** The single, fixed permit condition shared by all channels. */
      CIEC_BOOL var_PERMIT;
      CDataConnection *conn_PERMIT = nullptr;

    public:
      GEN_E_PERMIT(StringId paInstanceNameId, CFBContainer &paContainer);
  };
} // namespace forte::iec61499::events
