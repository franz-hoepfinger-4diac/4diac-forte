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

target_sources(forte-safearithmetic PRIVATE
        GEN_SAFE_ADD_fbt.cpp
        GEN_SAFE_MUL_fbt.cpp
        SAFE_DIV_fbt.cpp
        SAFE_SUB_fbt.cpp
)
