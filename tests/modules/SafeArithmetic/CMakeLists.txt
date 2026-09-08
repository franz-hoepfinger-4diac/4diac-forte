#############################################################################
# Copyright (c) 2026 HR Agrartechnik GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
#
# Contributors:
#    Franz Höpfinger
#      - initial API and implementation and/or initial documentation
#############################################################################

#############################################################################
# Tests for the SafeArithmetic function FBs
#############################################################################
forte_test_add_sourcefile_cpp(GEN_SAFE_ADD_2_tester.cpp)
forte_test_add_sourcefile_cpp(GEN_SAFE_ADD_3_tester.cpp)
forte_test_add_sourcefile_cpp(GEN_SAFE_ADD_4_tester.cpp)
forte_test_add_sourcefile_cpp(GEN_SAFE_MUL_2_tester.cpp)
forte_test_add_sourcefile_cpp(GEN_SAFE_MUL_3_tester.cpp)
forte_test_add_sourcefile_cpp(GEN_SAFE_MUL_4_tester.cpp)
forte_test_add_sourcefile_cpp(SAFE_SUB_tester.cpp)
forte_test_add_sourcefile_cpp(SAFE_DIV_tester.cpp)
