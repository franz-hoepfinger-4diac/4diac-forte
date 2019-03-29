/*******************************************************************************
 * Copyright (c) 2019 fortiss GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   Martin Jobst - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "Slave.h"

#include "io/mapper/io_mapper.h"

using namespace forte::modbus;

ModbusSlave::ModbusSlave(const TForteUInt8 *const paSlaveConfigurationIO, const TForteUInt8 paSlaveConfigurationIO_num,
                         ModbusSlaveHandler::SlaveType paType, CResource *paSrcRes,
                         const SFBInterfaceSpec *paInterfaceSpec, const CStringDictionary::TStringId paInstanceNameId,
                         TForteByte *paFBConnData, TForteByte *paFBVarsData) :
    forte::core::io::IOConfigFBMultiSlave(paSlaveConfigurationIO, paSlaveConfigurationIO_num, int(paType), paSrcRes,
                                          paInterfaceSpec, paInstanceNameId, paFBConnData, paFBVarsData),
    mSlave(nullptr) {
}

const char *ModbusSlave::init() {
  CCriticalRegion criticalRegion(mSlaveMutex);

  ModbusBusHandler &bus = *static_cast<ModbusBusHandler *>(&getController());

  mSlave = bus.getSlave(mIndex);

  ModbusSlaveHandler::Config config{
      Unit(),
      UpdateInterval()
  };
  mSlave->setConfig(config);
  mSlave->init();

  return nullptr;
}

void ModbusSlave::deInit() {
  CCriticalRegion criticalRegion(mSlaveMutex);
  mSlave->deInit();
  mSlave = nullptr;
}

