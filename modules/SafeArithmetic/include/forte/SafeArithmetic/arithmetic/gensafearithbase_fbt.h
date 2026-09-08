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

#include "forte/genfb.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/util/string_utils.h"

#include <memory>

// Shared plumbing for the generic (variable-arity) SafeArithmetic FBs (GEN_SAFE_ADD,
// GEN_SAFE_MUL), mirroring forte::iec61131::arithmetic::CGenArithBase. Unlike that base,
// these FBs have TWO fixed data outputs (OUT and LIMIT_HIT) instead of one, so this is a
// standalone base rather than a reuse of CGenArithBase - SafeArithmetic only depends on
// forte-core and deliberately does not link against forte-iec61131-3.
namespace forte::SafeArithmetic::arithmetic {
  template<typename TVariant>
  class CGenSafeArithBase : public CGenFunctionBlock<CFunctionBlock> {

    protected:
      CGenSafeArithBase(const StringId paInstanceNameId, CFBContainer &paContainer);
      ~CGenSafeArithBase() override = default;

      TVariant &var_IN(size_t paIndex) {
        return mGenDIs[paIndex];
      }

      size_t getGenEOOffset() override {
        return 1;
      }

      // OUT and LIMIT_HIT are fixed data outputs (not part of the variable-arity IN1..INn inputs).
      size_t getGenDOOffset() override {
        return 2;
      }

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;
      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId paDONum) override;
      void createGenInputData() override;

      static const TEventID scmEventREQID = 0;
      static const TEventID scmEventCNFID = 0;

      TVariant var_OUT;
      CIEC_BOOL var_LIMIT_HIT;

    private:
      std::vector<StringId> mDataInputNames;

      void readInputData(TEventID paEI) override;
      void writeOutputData(TEventID paEO) override;

      bool createInterfaceSpec(const char *paConfigString, SFBInterfaceSpec &paInterfaceSpec) override;

      std::unique_ptr<TVariant[]> mGenDIs;

      CEventConnection conn_CNF;

      COutDataConnection<TVariant> conn_OUT;
      COutDataConnection<CIEC_BOOL> conn_LIMIT_HIT;
  };

  template<typename TVariant>
  CGenSafeArithBase<TVariant>::CGenSafeArithBase(const StringId paInstanceNameId, CFBContainer &paContainer) :
      CGenFunctionBlock<CFunctionBlock>(paContainer, paInstanceNameId),
      var_LIMIT_HIT(false),
      conn_CNF(*this, 0),
      conn_OUT(*this, 0, var_OUT),
      conn_LIMIT_HIT(*this, 1, var_LIMIT_HIT) {
  }

  template<typename TVariant>
  void CGenSafeArithBase<TVariant>::readInputData(TEventID) {
    for (TPortId i = 0; i < getFBInterfaceSpec().getNumDIs(); ++i) {
      readData(i, mGenDIs[i], mGenDIConns[i]);
    }
  }

  template<typename TVariant>
  void CGenSafeArithBase<TVariant>::writeOutputData(TEventID) {
    writeData(getFBInterfaceSpec().getNumDIs() + 0, var_OUT, conn_OUT);
    writeData(getFBInterfaceSpec().getNumDIs() + 1, var_LIMIT_HIT, conn_LIMIT_HIT);
  }

  template<typename TVariant>
  bool CGenSafeArithBase<TVariant>::createInterfaceSpec(const char *paConfigString, SFBInterfaceSpec &paInterfaceSpec) {
    using namespace forte::literals;
    static const forte::StringId cEventInputNames[] = {"REQ"_STRID};
    static const forte::StringId cEventOutputNames[] = {"CNF"_STRID};
    static const forte::StringId cDataOutputNames[] = {"OUT"_STRID, "LIMIT_HIT"_STRID};

    const char *pcPos = strrchr(paConfigString, '_');
    if (pcPos == nullptr) {
      return false;
    }

    pcPos++;
    // we have an underscore and it is the last underscore, followed by the arity
    unsigned int numDIs = static_cast<unsigned int>(util::strtoul(pcPos, nullptr, 10));
    DEVLOG_DEBUG("DIs: %d;\n", numDIs);

    if (numDIs < 2 || numDIs >= scmMaxInterfaceEvents) {
      return false;
    }

    generateGenericInterfacePointNameArray("IN", mDataInputNames, numDIs);

    paInterfaceSpec.mEINames = cEventInputNames;
    paInterfaceSpec.mEONames = cEventOutputNames;
    paInterfaceSpec.mDINames = mDataInputNames;
    paInterfaceSpec.mDONames = cDataOutputNames;

    return true;
  }

  template<typename TVariant>
  CIEC_ANY *CGenSafeArithBase<TVariant>::getDI(size_t paDINum) {
    return &mGenDIs[paDINum];
  }

  template<typename TVariant>
  CIEC_ANY *CGenSafeArithBase<TVariant>::getDO(size_t paDONum) {
    switch (paDONum) {
      case 0: return &var_OUT;
      case 1: return &var_LIMIT_HIT;
    }
    return nullptr;
  }

  template<typename TVariant>
  CEventConnection *CGenSafeArithBase<TVariant>::getEOConUnchecked(TPortId paEONum) {
    return (paEONum == 0) ? &conn_CNF : nullptr;
  }

  template<typename TVariant>
  CDataConnection *CGenSafeArithBase<TVariant>::getDOConUnchecked(TPortId paDONum) {
    switch (paDONum) {
      case 0: return &conn_OUT;
      case 1: return &conn_LIMIT_HIT;
    }
    return nullptr;
  }

  template<typename TVariant>
  void CGenSafeArithBase<TVariant>::createGenInputData() {
    mGenDIs = std::make_unique<TVariant[]>(getFBInterfaceSpec().getNumDIs());
  }
} // namespace forte::SafeArithmetic::arithmetic
