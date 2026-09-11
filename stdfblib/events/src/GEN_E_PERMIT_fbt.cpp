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

#include "forte/iec61499/events/GEN_E_PERMIT_fbt.h"
#include "forte/util/string_utils.h"

using namespace forte::literals;

namespace forte::iec61499::events {
  namespace {
    const auto cDataInputNames = std::array{"PERMIT"_STRID};
  } // namespace

  DEFINE_GENERIC_FIRMWARE_FB(GEN_E_PERMIT, "iec61499::events::GEN_E_PERMIT"_STRID)

  GEN_E_PERMIT::GEN_E_PERMIT(const StringId paInstanceNameId, CFBContainer &paContainer) :
      CGenFunctionBlock<CFunctionBlock>(paContainer, paInstanceNameId) {
  }

  void GEN_E_PERMIT::executeEvent(const TEventID paEIID, CEventChainExecutionThread *const paECET) {
    // paEIID is reused directly as the EO index below: mEINames/mEONames are generated
    // with the same count N in the same order (EI1..EIn / EO1..EOn, see
    // createInterfaceSpec()), so the index of whichever EIi just fired is automatically
    // the index of its matching EOi. TEventID (forte/event.h) is just a plain size_t
    // with no distinct type for "EI space" vs. "EO space", and sendOutputEvent() only
    // checks the index against the EO count -- it has no notion of input events at all.
    if (var_PERMIT) {
      sendOutputEvent(paEIID, paECET);
    }
  }

  void GEN_E_PERMIT::readInputData(TEventID) {
    // Every EIi is WITH-associated with PERMIT (see e.g. E_PERMIT_3.fbt), so it is
    // read fresh regardless of which channel fired.
    readData(0, var_PERMIT, conn_PERMIT);
  }

  void GEN_E_PERMIT::writeOutputData(TEventID) {
    // Nothing to do, as there are no data outputs.
  }

  bool GEN_E_PERMIT::createInterfaceSpec(const char *paConfigString, SFBInterfaceSpec &paInterfaceSpec) {
    // Find the last underscore in the name, e.g., "E_PERMIT_3".
    const char *acPos = strrchr(paConfigString, '_');
    if (nullptr == acPos) {
      return false;
    }
    ++acPos; // Move pointer to the character after the underscore.

    char *acEnd = nullptr;
    const size_t numChannels = static_cast<size_t>(util::strtoul(acPos, &acEnd, 10));

    if (('\0' == *acEnd) && numChannels >= 1 && numChannels < CFunctionBlock::scmMaxInterfaceEvents) {
      generateGenericInterfacePointNameArray("EI", mEINames, numChannels);
      generateGenericInterfacePointNameArray("EO", mEONames, numChannels);

      paInterfaceSpec.mEINames = mEINames;
      paInterfaceSpec.mEONames = mEONames;
      paInterfaceSpec.mDINames = cDataInputNames;
      return true;
    }

    DEVLOG_ERROR("Cannot configure FB-Instance E_PERMIT_%zu. Number of channels must be within 1 and %u.\n",
                 numChannels, CFunctionBlock::scmMaxInterfaceEvents);
    return false;
  }

  CIEC_ANY *GEN_E_PERMIT::getDI(const size_t paIndex) {
    return (paIndex == 0) ? &var_PERMIT : nullptr;
  }

  CIEC_ANY *GEN_E_PERMIT::getDO(size_t) {
    return nullptr;
  }

  CDataConnection **GEN_E_PERMIT::getDIConUnchecked(const TPortId paIndex) {
    return (paIndex == 0) ? &conn_PERMIT : nullptr;
  }

  CDataConnection *GEN_E_PERMIT::getDOConUnchecked(TPortId) {
    return nullptr;
  }
} // namespace forte::iec61499::events
