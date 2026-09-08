/*******************************************************************************
 * Copyright (c) 2014 Profactor GmbH, fortiss GmbH
 *                      2018 Johannes Kepler University
 *               2023 Martin Erich Jobst
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Matthias Plasch, Alois Zoitl
 *   - initial API and implementation and/or initial documentation
 *    Alois Zoitl - introduced new CGenFB class for better handling generic FBs
 *   Martin Jobst
 *     - refactor for ANY variant
 *******************************************************************************/

#include "forte/iec61131/booleanOperators/GEN_OR_BOOL_fbt.h"

#include "forte/iec61131_functions/func_OR.h"

using namespace forte::literals;

namespace forte::iec61131::booleanOperators {
  DEFINE_GENERIC_FIRMWARE_FB(GEN_OR_BOOL, "iec61131::booleanOperators::GEN_OR_BOOL"_STRID)

  GEN_OR_BOOL::GEN_OR_BOOL(const StringId paInstanceNameId, CFBContainer &paContainer) :
      CGenBoolBase(paInstanceNameId, paContainer) {
  }

  void GEN_OR_BOOL::executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) {
    switch (paEIID) {
      case scmEventREQID:
        if (getFBInterfaceSpec().getNumDIs()) {
          var_OUT = var_IN(0);
          for (size_t i = 1; i < getFBInterfaceSpec().getNumDIs(); ++i) {
            var_OUT = func_OR(var_OUT, var_IN(i));
          }
        }
        sendOutputEvent(scmEventCNFID, paECET);
        break;
    }
  }
} // namespace forte::iec61131::booleanOperators
