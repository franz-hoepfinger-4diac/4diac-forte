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

#include "slave.h"

#include "criticalregion.h"
#include "processinterface.h"

#include "io/mapper/io_mapper.h"

#include "../handler/bus.h"

using namespace forte::modbus;

ModbusSlaveHandler::ModbusSlaveHandler(ModbusBusHandler *paBus, SlaveType paType) : mType(paType), mBus(paBus),
                                                                                    mInitialized(false) {
  switch (paType) {
    case SlaveType::Unknown:
      mInProcessImageSize = 0;
      mOutProcessImageSize = 0;
      break;
    case SlaveType::FestoCMMP_AS:
      mInProcessImageSize = 8;
      mOutProcessImageSize = 8;
      break;
  }
}

ModbusSlaveHandler::~ModbusSlaveHandler() {
  dropHandles();
}

void ModbusSlaveHandler::init() {
  mInitialized = true;
}

void ModbusSlaveHandler::deInit() {
  mInitialized = false;
}

void ModbusSlaveHandler::setConfig(Config paConfig) {
  mConfig = paConfig;
}

ModbusSlaveHandler::SlaveType
ModbusSlaveHandler::getSlaveType(ModbusBasicDeviceIdentification &paDeviceIdentification) {
  if ("Festo AG & Co. KG" == paDeviceIdentification.mVendorName &&
      "0x00002085" == paDeviceIdentification.mProductCode) {
    return SlaveType::FestoCMMP_AS;
  }
  return SlaveType::Unknown;
}

void
ModbusSlaveHandler::read(std::uint16_t paStartAddress, size_t paBitOffset, size_t paBitWidth, std::uint64_t &paValue) {
  CCriticalRegion criticalRegion(mInProcessImageMutex);

  std::memcpy(&paValue, mInProcessImage.data() + paStartAddress * sizeof(std::uint16_t),
              static_cast<size_t>(-(-(paBitOffset + paBitWidth) & ~size_t(0x7)) / 8));
  paValue >>= paBitOffset;
  paValue &= (std::uint64_t(1) << paBitWidth) - std::uint64_t(1);
}

void
ModbusSlaveHandler::write(std::uint16_t paStartAddress, size_t paBitOffset, size_t paBitWidth, std::uint64_t paValue) {
  CCriticalRegion criticalRegion(mOutProcessImageMutex);

  std::uint64_t value = 0;
  std::memcpy(&value, mOutProcessImage.data() + paStartAddress * sizeof(std::uint16_t),
              static_cast<size_t>(-(-(paBitOffset + paBitWidth) & ~size_t(0x7)) / 8));
  value &= ~((std::uint64_t(1) << (paBitOffset + paBitWidth)) - std::uint64_t(1)) |
           ((std::uint64_t(1) << paBitOffset) - std::uint64_t(1));
  value |= (paValue & ((std::uint64_t(1) << paBitWidth) - std::uint64_t(1))) << paBitOffset;
  std::memcpy(mOutProcessImage.data() + paStartAddress * sizeof(std::uint16_t), &value,
              static_cast<size_t>(-(-(paBitOffset + paBitWidth) & ~size_t(0x7)) / 8));
}

void ModbusSlaveHandler::updateInProcessImage(const std::array<char, 256> &paInProcessImage,
                                              std::array<char, 256> &paChanges) {
  CCriticalRegion criticalRegion(mInProcessImageMutex);

  for (size_t i = 0; i < paChanges.size(); i++) {
    paChanges[i] = mInProcessImage[i] ^ paInProcessImage[i];
  }

  mInProcessImage = paInProcessImage;
}

void ModbusSlaveHandler::getOutProcessImage(std::array<char, 256> &paOutProcessImage) {
  CCriticalRegion criticalRegion(mOutProcessImageMutex);

  paOutProcessImage = mOutProcessImage;
}

void ModbusSlaveHandler::update() {
  std::array<char, 256> inProcessImage{0};
  std::array<char, 256> outProcessImage{0};
  std::array<char, 256> changes{0};

  getOutProcessImage(outProcessImage);
  mBus->mConnection->writeMultipleRegisters(mConfig.mUnit, 0, mOutProcessImageSize, outProcessImage.data());

  mBus->mConnection->readHoldingRegisters(mConfig.mUnit, 0, mInProcessImageSize, inProcessImage.data());
  updateInProcessImage(inProcessImage, changes);

  TSlaveHandleList::Iterator itEnd = mInputs.end();
  for (TSlaveHandleList::Iterator it = mInputs.begin(); it != itEnd; ++it) {
    if (it->changed(changes)) {
      DEVLOG_DEBUG("[ModbusSlaveHandler] Change detected\n");
      it->onChange();
    }
  }
}

void ModbusSlaveHandler::dropHandles() {
  CCriticalRegion criticalRegion(mHandleMutex);

  forte::core::io::IOMapper &mapper = forte::core::io::IOMapper::getInstance();

  TSlaveHandleList::Iterator itEnd = mInputs.end();
  for (TSlaveHandleList::Iterator it = mInputs.begin(); it != itEnd; ++it) {
    mapper.deregisterHandle(*it);
    delete *it;
  }
  itEnd = mOutputs.end();
  for (TSlaveHandleList::Iterator it = mOutputs.begin(); it != itEnd; ++it) {
    mapper.deregisterHandle(*it);
    delete *it;
  }

  mInputs.clearAll();
  mOutputs.clearAll();
}

void ModbusSlaveHandler::addHandle(TSlaveHandleList *paList, ModbusSlaveHandle *paHandle) {
  CCriticalRegion criticalRegion(mHandleMutex);
  paList->pushBack(paHandle);
}

