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
  struct GEN_E_PERMIT_1_TestFixture : public forte::test::CFBTestFixtureBase {
      static constexpr TEventID EI1 = 0;
      static constexpr TEventID EO1 = 0;

      GEN_E_PERMIT_1_TestFixture() : CFBTestFixtureBase("iec61499::events::E_PERMIT_1"_STRID) {
        setInputData({&mInPERMIT});
        setup();
      }

      CIEC_BOOL mInPERMIT; // DATA INPUT
  };

  BOOST_FIXTURE_TEST_SUITE(GenEPermit1Tests, GEN_E_PERMIT_1_TestFixture)

  // The single-channel generic instance must behave exactly like the fixed E_PERMIT.
  BOOST_AUTO_TEST_CASE(Permit) {
    mInPERMIT = true_BOOL;
    triggerEvent(EI1);
    BOOST_CHECK(checkForSingleOutputEventOccurence(EO1));
  }

  BOOST_AUTO_TEST_CASE(DontPermit) {
    mInPERMIT = false_BOOL;
    triggerEvent(EI1);
    BOOST_CHECK(eventChainEmpty());
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::iec61499::events::test
