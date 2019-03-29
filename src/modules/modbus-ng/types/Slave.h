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

#ifndef SRC_MODULES_MODBUS_TYPES_SLAVE_H_
#define SRC_MODULES_MODBUS_TYPES_SLAVE_H_

#include "forte_sync.h"

#include "io/configFB/io_slave_multi.h"

#include "handler/bus.h"
#include "slave/slave.h"
#include "slave/handle.h"

#include "BusAdapter.h"

class ModbusSlave : public forte::core::io::IOConfigFBMultiSlave {
public:
  ModbusSlave(const TForteUInt8 *paSlaveConfigurationIO, TForteUInt8 paSlaveConfigurationIO_num,
              forte::modbus::ModbusSlaveHandler::SlaveType paType, CResource *paSrcRes,
              const SFBInterfaceSpec *paInterfaceSpec, CStringDictionary::TStringId paInstanceNameId,
              TForteByte *paFBConnData, TForteByte *paFBVarsData);

  ~ModbusSlave() override {
    deInit();
  };

protected:
  CSyncObject mSlaveMutex;
  forte::modbus::ModbusSlaveHandler *mSlave;

  const char *init() override;

  void deInit() override;

  virtual CIEC_USINT &Unit() = 0;

  virtual CIEC_UINT &UpdateInterval() = 0;
};

#endif /* SRC_MODULES_MODBUS_TYPES_SLAVE_H_ */
