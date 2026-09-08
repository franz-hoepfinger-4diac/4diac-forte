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
  struct GEN_SAFE_ADD_3_TestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      GEN_SAFE_ADD_3_TestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_ADD_3"_STRID) {
        setInputData({&mIn1, &mIn2, &mIn3});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_DINT mIn1;
      CIEC_DINT mIn2;
      CIEC_DINT mIn3;

      CIEC_DINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(GenSafeAdd3Tests, GEN_SAFE_ADD_3_TestFixture)

  BOOST_AUTO_TEST_CASE(validAddition) {
    mIn1 = 2_DINT;
    mIn2 = 3_DINT;
    mIn3 = 4_DINT;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(9, static_cast<CIEC_DINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(false, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()

  struct GEN_SAFE_ADD_3_SaturationTestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      GEN_SAFE_ADD_3_SaturationTestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_ADD_3"_STRID) {
        setInputData({&mIn1, &mIn2, &mIn3});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_USINT mIn1;
      CIEC_USINT mIn2;
      CIEC_USINT mIn3;

      CIEC_USINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(GenSafeAdd3SaturationTests, GEN_SAFE_ADD_3_SaturationTestFixture)

  // First fold step (100+100=200) stays within range; the second step (200+100=300)
  // overflows USINT's range and saturates - LIMIT_HIT must stay set (sticky OR) even
  // though the very first step was fine.
  BOOST_AUTO_TEST_CASE(overflowOnLaterStepStillSetsLimitHit) {
    mIn1 = 100_USINT;
    mIn2 = 100_USINT;
    mIn3 = 100_USINT;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(255, static_cast<CIEC_USINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(true, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::SafeArithmetic::arithmetic
