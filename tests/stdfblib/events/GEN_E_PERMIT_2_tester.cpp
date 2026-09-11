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
 *     - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "../../core/fbtests/fbtestfixture.h"
#include "forte/datatypes/forte_bool.h"

using namespace forte::literals;

namespace forte::iec61499::events::test {
  struct GEN_E_PERMIT_2_TestFixture : public forte::test::CFBTestFixtureBase {
      static constexpr TEventID EI1 = 0;
      static constexpr TEventID EI2 = 1;
      static constexpr TEventID EO1 = 0;
      static constexpr TEventID EO2 = 1;

      GEN_E_PERMIT_2_TestFixture() : CFBTestFixtureBase("iec61499::events::E_PERMIT_2"_STRID) {
        setInputData({&mInPERMIT});
        setup();
      }

      CIEC_BOOL mInPERMIT; // DATA INPUT, shared by both channels
  };

  BOOST_FIXTURE_TEST_SUITE(GenEPermit2Tests, GEN_E_PERMIT_2_TestFixture)

  BOOST_AUTO_TEST_CASE(Permit) {
    mInPERMIT = true_BOOL;
    triggerEvent(EI1);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO1));
    triggerEvent(EI2);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO2));
  }

  BOOST_AUTO_TEST_CASE(DontPermit) {
    mInPERMIT = false_BOOL;
    triggerEvent(EI1);
    BOOST_CHECK(eventChainEmpty());
    triggerEvent(EI2);
    BOOST_CHECK(eventChainEmpty());
  }

  // Each channel must only trigger its own matching output, never the other one.
  BOOST_AUTO_TEST_CASE(ChannelsAreIndependent) {
    mInPERMIT = true_BOOL;
    triggerEvent(EI2);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO2));
    triggerEvent(EI1);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO1));
  }

  // PERMIT is one shared condition, read fresh on every channel -- not latched per channel.
  BOOST_AUTO_TEST_CASE(PermitIsSharedAcrossChannels) {
    mInPERMIT = false_BOOL;
    triggerEvent(EI1);
    BOOST_CHECK(eventChainEmpty());

    mInPERMIT = true_BOOL;
    triggerEvent(EI2);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO2));
    triggerEvent(EI1);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO1));
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::iec61499::events::test
