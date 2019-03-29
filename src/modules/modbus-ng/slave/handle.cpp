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

#include "handle.h"

#include "devlog.h"
#include "forte_bool.h"
#include "forte_word.h"
#include "forte_dword.h"

#include "io/mapper/io_mapper.h"

#include "slave.h"
#include "../handler/bus.h"

using namespace forte::modbus;

ModbusSlaveHandle::ModbusSlaveHandle(
    forte::core::io::IODeviceController *paController,
    forte::core::io::IOMapper::Direction paDirection,
    CIEC_ANY::EDataTypeID paType,
    ModbusSlaveHandler *paSlave,
    std::uint16_t paStartAddress,
    size_t paBitOffset,
    size_t paBitWidth,
    bool paNetworkByteOrder
) : forte::core::io::IOHandle(paController, paDirection, paType),
    mSlave(paSlave),
    mStartAddress(paStartAddress),
    mBitOffset(paBitOffset),
    mBitWidth(paBitWidth),
    mNetworkByteOrder(paNetworkByteOrder) {}

void ModbusSlaveHandle::set(const CIEC_ANY &paState) {
  switch (mType) {
    case CIEC_ANY::e_BOOL:
      set(static_cast<const CIEC_BOOL &>(paState));
      break;
    case CIEC_ANY::e_WORD:
      set(static_cast<const CIEC_WORD &>(paState));
      break;
    case CIEC_ANY::e_DWORD:
      set(static_cast<const CIEC_DWORD &>(paState));
      break;
    default:
      break;
  }
}

void ModbusSlaveHandle::set(const CIEC_BOOL &paState) {
  bool value = paState;
  mSlave->write(mStartAddress, mBitOffset, mBitWidth, value ? 1 : 0);
}

void ModbusSlaveHandle::set(const CIEC_WORD &paState) {
  TForteWord value = paState;
  mSlave->write(mStartAddress, mBitOffset, mBitWidth, mNetworkByteOrder ? htons(value) : value);
}

void ModbusSlaveHandle::set(const CIEC_DWORD &paState) {
  TForteDWord value = paState;
  mSlave->write(mStartAddress, mBitOffset, mBitWidth, mNetworkByteOrder ? htonl(value) : value);
}

void ModbusSlaveHandle::get(CIEC_ANY &paState) {
  switch (mType) {
    case CIEC_ANY::e_BOOL:
      get(static_cast<CIEC_BOOL &>(paState));
      break;
    case CIEC_ANY::e_WORD:
      get(static_cast<CIEC_WORD &>(paState));
      break;
    case CIEC_ANY::e_DWORD:
      get(static_cast<CIEC_DWORD &>(paState));
      break;
    default:
      break;
  }
}

void ModbusSlaveHandle::get(CIEC_BOOL &paState) {
  std::uint64_t value;
  mSlave->read(mStartAddress, mBitOffset, mBitWidth, value);
  paState = value != 0;
}

void ModbusSlaveHandle::get(CIEC_WORD &paState) {
  std::uint64_t value;
  mSlave->read(mStartAddress, mBitOffset, mBitWidth, value);
  paState = static_cast<TForteWord>(mNetworkByteOrder ? ntohs(static_cast<std::uint16_t>(value)) : value);
}

void ModbusSlaveHandle::get(CIEC_DWORD &paState) {
  std::uint64_t value;
  mSlave->read(mStartAddress, mBitOffset, mBitWidth, value);
  paState = static_cast<TForteDWord>(mNetworkByteOrder ? ntohl(static_cast<std::uint32_t>(value)) : value);
}

bool ModbusSlaveHandle::changed(std::array<char, 256> paChanges) {
  std::uint64_t value = 0;
  std::memcpy(&value, paChanges.data() + mStartAddress * sizeof(std::uint16_t),
              static_cast<size_t>(-(-(mBitOffset + mBitWidth) & ~size_t(0x7)) / 8));
  std::uint64_t mask = ((std::uint64_t(1) << (mBitOffset + mBitWidth)) - std::uint64_t(1)) &
                       ~((std::uint64_t(1) << mBitOffset) - std::uint64_t(1));
  return (value & mask) != 0;
}
