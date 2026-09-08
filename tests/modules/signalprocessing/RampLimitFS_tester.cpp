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
 *   Franz Höpfinger - initial tests
 *******************************************************************************/
#include "../../core/fbtests/fbtestfixture.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_dint.h"

using namespace forte::literals;

namespace forte::eclipse4diac::signalprocessing {
  struct RampLimitFSTestFixture : public test::CFBTestFixtureBase {
      static constexpr TEventID INIT = 0;
      static constexpr TEventID ZERO = 1;
      static constexpr TEventID UP_SLOW = 2;
      static constexpr TEventID UP_FAST = 3;
      static constexpr TEventID DOWN_SLOW = 4;
      static constexpr TEventID DOWN_FAST = 5;
      static constexpr TEventID FULL = 6;
      static constexpr TEventID LOAD = 7;

      static constexpr TEventID INITO = 0;
      static constexpr TEventID CNF = 1;

      RampLimitFSTestFixture() : CFBTestFixtureBase("eclipse4diac::signalprocessing::RampLimitFS"_STRID) {
        setInputData({&mPV, &mVAL_ZERO, &mSLOW, &mFAST, &mVAL_FULL});
        setOutputData({&mOUT, &mQAtZero, &mQAtFull});
        setup();
      }

      static CIEC_DINT::TValueType out(const CIEC_DINT &paVal) {
        return static_cast<CIEC_DINT::TValueType>(paVal);
      }

      CIEC_DINT mPV = 0_DINT; // DATA INPUT
      CIEC_DINT mVAL_ZERO = 0_DINT; // DATA INPUT
      CIEC_DINT mSLOW = 0_DINT; // DATA INPUT
      CIEC_DINT mFAST = 0_DINT; // DATA INPUT
      CIEC_DINT mVAL_FULL = 0_DINT; // DATA INPUT

      CIEC_DINT mOUT;
      CIEC_BOOL mQAtZero;
      CIEC_BOOL mQAtFull;
  };

  BOOST_FIXTURE_TEST_SUITE(RampLimitFSTests, RampLimitFSTestFixture)

  BOOST_AUTO_TEST_CASE(InitSetsOutputToZeroValueAndFlags) {
    mVAL_ZERO = 10_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(INIT);
    BOOST_CHECK(checkForSingleOutputEventOccurence(INITO));
    BOOST_CHECK_EQUAL(10, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(InitDoesNotFireCNF) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(INIT);
    // Only INITO must be raised, never CNF
    BOOST_CHECK(checkForSingleOutputEventOccurence(INITO));
  }

  BOOST_AUTO_TEST_CASE(InitWithZeroEqualToFullSetsBothFlags) {
    mVAL_ZERO = 50_DINT;
    mVAL_FULL = 50_DINT;
    triggerEvent(INIT);
    BOOST_CHECK(checkForSingleOutputEventOccurence(INITO));
    BOOST_CHECK_EQUAL(50, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(true_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(ZeroEventSetsOutputToZeroValue) {
    mVAL_ZERO = 5_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(ZERO);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(5, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(UpSlowIncreasesOutputBySlowStep) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(ZERO);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));

    mSLOW = 10_DINT;
    triggerEvent(UP_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(10, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);

    triggerEvent(UP_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(20, out(mOUT));
  }

  BOOST_AUTO_TEST_CASE(UpSlowClampsAtFullValue) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(ZERO);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));

    mSLOW = 60_DINT;
    triggerEvent(UP_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(60, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);

    // 60 + 60 = 120, which exceeds VAL_FULL (100) and must clamp
    triggerEvent(UP_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(100, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(UpFastIncreasesOutputByFastStepAndClamps) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 50_DINT;
    triggerEvent(ZERO);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));

    mFAST = 40_DINT;
    triggerEvent(UP_FAST);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(40, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);

    // 40 + 40 = 80, which exceeds VAL_FULL (50) and must clamp
    triggerEvent(UP_FAST);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(50, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(DownSlowDecreasesOutputAndClampsAtZeroValue) {
    mVAL_ZERO = 10_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(FULL);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(100, out(mOUT));

    mSLOW = 30_DINT;
    triggerEvent(DOWN_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(70, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);

    triggerEvent(DOWN_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(40, out(mOUT));

    triggerEvent(DOWN_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(10, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);

    // 10 - 30 = -20, which is below VAL_ZERO (10) and must clamp
    triggerEvent(DOWN_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(10, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
  }

  BOOST_AUTO_TEST_CASE(DownFastDecreasesOutputAndClampsAtZeroValue) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;
    triggerEvent(FULL);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(100, out(mOUT));

    mFAST = 70_DINT;
    triggerEvent(DOWN_FAST);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(30, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);

    // 30 - 70 = -40, which is below VAL_ZERO (0) and must clamp
    triggerEvent(DOWN_FAST);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(0, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
  }

  BOOST_AUTO_TEST_CASE(FullEventSetsOutputToFullValue) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 42_DINT;
    triggerEvent(FULL);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(42, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(true_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(LoadEventSetsOutputToPresentValueWithoutClamping) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;

    // Within range: neither flag set
    mPV = 50_DINT;
    triggerEvent(LOAD);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(50, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);

    // Below VAL_ZERO: LOAD does not clamp the output value itself, but the
    // qAtZero indicator must reflect that the value is at/below the limit
    mPV = -20_DINT;
    triggerEvent(LOAD);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(-20, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);

    // Above VAL_FULL: LOAD does not clamp the output value itself, but the
    // qAtFull indicator must reflect that the value is at/above the limit
    mPV = 150_DINT;
    triggerEvent(LOAD);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(150, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(true_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(UpSlowRereadsUpdatedLimitsOnEveryEvent) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;
    mSLOW = 10_DINT;
    triggerEvent(ZERO);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));

    triggerEvent(UP_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(10, out(mOUT));

    // Lower the upper limit before the next event: since VAL_FULL is now read
    // on every ramp event, the FB must clamp against the new, lower value.
    mVAL_FULL = 15_DINT;
    triggerEvent(UP_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(15, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_CASE(DownSlowRereadsUpdatedLimitsOnEveryEvent) {
    mVAL_ZERO = 0_DINT;
    mVAL_FULL = 100_DINT;
    mSLOW = 10_DINT;
    triggerEvent(FULL);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));

    triggerEvent(DOWN_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(90, out(mOUT));

    // Raise the lower limit before the next event: since VAL_ZERO is now read
    // on every ramp event, the FB must clamp against the new, higher value.
    mVAL_ZERO = 85_DINT;
    triggerEvent(DOWN_SLOW);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(85, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
  }

  BOOST_AUTO_TEST_CASE(SupportsNegativeRange) {
    mVAL_ZERO = -50_DINT;
    mVAL_FULL = 50_DINT;
    triggerEvent(ZERO);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(-50, out(mOUT));
    BOOST_CHECK_EQUAL(true_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);

    mFAST = 30_DINT;
    triggerEvent(UP_FAST);
    BOOST_CHECK(checkForSingleOutputEventOccurence(CNF));
    BOOST_CHECK_EQUAL(-20, out(mOUT));
    BOOST_CHECK_EQUAL(false_BOOL, mQAtZero);
    BOOST_CHECK_EQUAL(false_BOOL, mQAtFull);
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::eclipse4diac::signalprocessing