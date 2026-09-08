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
  struct GEN_SAFE_MUL_2_TestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      GEN_SAFE_MUL_2_TestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_MUL_2"_STRID) {
        setInputData({&mIn1, &mIn2});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_DINT mIn1;
      CIEC_DINT mIn2;

      CIEC_DINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(GenSafeMul2Tests, GEN_SAFE_MUL_2_TestFixture)

  BOOST_AUTO_TEST_CASE(validMultiplication) {
    mIn1 = 3_DINT;
    mIn2 = 4_DINT;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(12, static_cast<CIEC_DINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(false, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()

  struct GEN_SAFE_MUL_2_SaturationTestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      GEN_SAFE_MUL_2_SaturationTestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_MUL_2"_STRID) {
        setInputData({&mIn1, &mIn2});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_USINT mIn1;
      CIEC_USINT mIn2;

      CIEC_USINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(GenSafeMul2SaturationTests, GEN_SAFE_MUL_2_SaturationTestFixture)

  BOOST_AUTO_TEST_CASE(overflowSaturatesAndSetsLimitHit) {
    mIn1 = 20_USINT;
    mIn2 = 20_USINT;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(255, static_cast<CIEC_USINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(true, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()

  struct GEN_SAFE_MUL_2_MixedTypeTestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID REQ = 0;
      static constexpr TEventID CNF = 0;

      GEN_SAFE_MUL_2_MixedTypeTestFixture() : CFBTestFixtureBase("SafeArithmetic::arithmetic::SAFE_MUL_2"_STRID) {
        setInputData({&mIn1, &mIn2});
        setOutputData({&mOut, &mLimitHit});
        setup();
      }

      CIEC_DINT mIn1;
      CIEC_REAL mIn2;

      CIEC_DINT mOut;
      CIEC_BOOL mLimitHit;
  };

  BOOST_FIXTURE_TEST_SUITE(GenSafeMul2IncompatibleTypeTests, GEN_SAFE_MUL_2_MixedTypeTestFixture)

  BOOST_AUTO_TEST_CASE(incompatibleTypesLeaveOutputUnchangedAndDoNotSetLimitHit) {
    mIn1 = 5_DINT;
    mIn2 = 2.5_REAL;
    triggerEvent(REQ);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(5, static_cast<CIEC_DINT::TValueType>(mOut));
    BOOST_CHECK_EQUAL(false, static_cast<bool>(mLimitHit));
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::SafeArithmetic::arithmetic
