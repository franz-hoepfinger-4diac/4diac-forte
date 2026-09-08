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
 *   Franz Höpfinger
 *     - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "../../core/fbtests/fbtestfixture.h"
#include "forte/datatypes/forte_bool.h"

using namespace forte::literals;

namespace forte::SafeArithmetic::arithmetic {
  struct SAFE_SUB_TestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      SAFE_SUB_TestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_SUB"_STRID) {
        setInputData({&mIn1, &mIn2});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_DINT mIn1;
      CIEC_DINT mIn2;

      CIEC_DINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(SafeSubTests, SAFE_SUB_TestFixture)

  BOOST_AUTO_TEST_CASE(validSubtraction) {
    mIn1 = 10_DINT;
    mIn2 = 4_DINT;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(6, static_cast<CIEC_DINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(false, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()

  // Regression test for the exact scenario that motivated the SafeArithmetic module
  // (see Uebung_011b3): plain F_SUB silently wraps UDINT#1 - UDINT#12 around to
  // UDINT#4294967285 instead of reporting the underflow.
  struct SAFE_SUB_UnsignedUnderflowTestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      SAFE_SUB_UnsignedUnderflowTestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_SUB"_STRID) {
        setInputData({&mIn1, &mIn2});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_UDINT mIn1;
      CIEC_UDINT mIn2;

      CIEC_UDINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(SafeSubUnsignedUnderflowTests, SAFE_SUB_UnsignedUnderflowTestFixture)

  BOOST_AUTO_TEST_CASE(underflowClampsToZeroInsteadOfWrapping) {
    mIn1 = 1_UDINT;
    mIn2 = 12_UDINT;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(0u, static_cast<CIEC_UDINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(true, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()

  struct SAFE_SUB_MixedTypeTestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      SAFE_SUB_MixedTypeTestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_SUB"_STRID) {
        setInputData({&mIn1, &mIn2});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_DINT mIn1;
      CIEC_REAL mIn2;

      CIEC_DINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(SafeSubIncompatibleTypeTests, SAFE_SUB_MixedTypeTestFixture)

  BOOST_AUTO_TEST_CASE(incompatibleTypesResetOutputWithoutSettingLimitHit) {
    mIn1 = 5_DINT;
    mIn2 = 2.5_REAL;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(0, static_cast<CIEC_DINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(false, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::SafeArithmetic::arithmetic
