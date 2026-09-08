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

option(FORTE_MODULE_SAFEARITHMETIC "SAFE Arithmetic FBs (saturating, overflow-checked ADD/SUB/MUL/DIV)" OFF)

if (NOT FORTE_MODULE_SAFEARITHMETIC)
    return()
endif ()

add_library(forte-safearithmetic)
target_link_libraries(forte-safearithmetic PUBLIC forte-core)
target_link_libraries(forte PUBLIC $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,forte-safearithmetic,$<LINK_LIBRARY:WHOLE_ARCHIVE,forte-safearithmetic>>)

add_subdirectory(include)
add_subdirectory(src)

install(TARGETS forte-safearithmetic EXPORT forte-export FILE_SET HEADERS)
