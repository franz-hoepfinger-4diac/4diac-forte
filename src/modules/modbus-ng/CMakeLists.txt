#*******************************************************************************
# * Copyright (c) 2019 fortiss GmbH
# * All rights reserved. This program and the accompanying materials
# * are made available under the terms of the Eclipse Public License v1.0
# * which accompanies this distribution, and is available at
# * http://www.eclipse.org/legal/epl-v10.html
# *
# * Contributors:
# *   Martin Jobst
# *   - initial API and implementation and/or initial documentation
# *******************************************************************************/

forte_add_io(MODBUS "Support for the Modbus TCP protocol")

if(FORTE_IO_MODBUS)

forte_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

forte_add_sourcefile_hcpp(connection/connection)

forte_add_sourcefile_hcpp(handler/bus)
forte_add_sourcefile_hcpp(slave/slave slave/handle)

forte_add_sourcefile_hcpp(types/BusAdapter)
forte_add_sourcefile_hcpp(types/Master)
forte_add_sourcefile_hcpp(types/Slave)

endif(FORTE_IO_MODBUS)
