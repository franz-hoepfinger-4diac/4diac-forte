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

#include "bus.h"

#include "criticalregion.h"

#include "../slave/slave.h"
#include "../slave/handle.h"

using namespace forte::modbus;

ModbusBusHandler::ModbusBusHandler(CDeviceExecution &paDeviceExecution) : forte::core::io::IODeviceMultiController(
    paDeviceExecution), mConnection(nullptr), mSlaves() {
}

void ModbusBusHandler::setConfig(struct forte::core::io::IODeviceController::Config *paConfig) {
  mConfig = *static_cast<Config *>(paConfig);
}

const char *ModbusBusHandler::init() {
  mConnection = new ModbusConnection(mConfig.mAddress, mConfig.mPort);
  bool result = mConnection->connect();
  if (!result) {
    return "Error connecting to remote endpoint";
  }

  for (int unit = 0; unit < 256; ++unit) {
    ModbusBasicDeviceIdentification deviceIdentification{};
    if (mConnection->readBasicDeviceIdentification(static_cast<uint8_t>(unit), deviceIdentification)) {
      ModbusSlaveHandler::SlaveType slaveType = ModbusSlaveHandler::getSlaveType(deviceIdentification);
      if (slaveType != ModbusSlaveHandler::SlaveType::Unknown) {
        mSlaves[unit] = new ModbusSlaveHandler(this, slaveType);
      }
    }
  }

  return nullptr;
}

void ModbusBusHandler::deInit() {
  for (auto &mSlave : mSlaves) {
    delete mSlave;
  }

  delete mConnection;
  mConnection = nullptr;
}

forte::core::io::IOHandle *ModbusBusHandler::initHandle(
    forte::core::io::IODeviceController::HandleDescriptor *paHandleDescriptor
) {
  HandleDescriptor &desc = *static_cast<HandleDescriptor *>(paHandleDescriptor);
  ModbusSlaveHandler *slave = getSlave(desc.mSlaveIndex);
  if (slave) {
    return new ModbusSlaveHandle(this, desc.mDirection, desc.mType, slave, desc.mStartAddress, desc.mBitOffset,
                                 desc.mBitWidth, desc.mNetworkByteOrder);
  }
  return nullptr;
}

void ModbusBusHandler::runLoop() {
  do {
    update();
    CThread::sleepThread(getMinimumSlaveUpdateInterval());
  } while (isAlive());
}

void ModbusBusHandler::update() {
  for (auto slave : mSlaves) {
    if (slave && slave->mInitialized) {
      slave->update();
    }
  }
}

std::uint32_t ModbusBusHandler::getMinimumSlaveUpdateInterval() {
  std::uint32_t updateInterval(1000);
  for (auto slave : mSlaves) {
    if (slave && slave->mConfig.mUpdateInterval < updateInterval) {
      updateInterval = slave->mConfig.mUpdateInterval;
    }
  }
  return updateInterval;
}

ModbusSlaveHandler *ModbusBusHandler::getSlave(int paIndex) {
  if (paIndex < 256) {
    return mSlaves[paIndex];
  }
  return nullptr;
}

void ModbusBusHandler::addSlaveHandle(
    int paIndex,
    forte::core::io::IOHandle *paHandle
) {
  ModbusSlaveHandler *slave = getSlave(paIndex);
  if (slave) {
    slave->addHandle(static_cast<ModbusSlaveHandle *>(paHandle));
  }
}

void ModbusBusHandler::dropSlaveHandles(int paIndex) {
  ModbusSlaveHandler *slave = getSlave(paIndex);
  if (slave) {
    slave->dropHandles();
  }
}

bool ModbusBusHandler::isSlaveAvailable(int paIndex) {
  return getSlave(paIndex) != nullptr;
}

bool ModbusBusHandler::checkSlaveType(
    int paIndex,
    int paType
) {
  ModbusSlaveHandler *slave = getSlave(paIndex);
  if (slave) {
    return slave->mType == ModbusSlaveHandler::SlaveType(paType);
  }
  return false;
}

