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

#ifndef SRC_MODULES_MODBUS_SLAVE_HANDLE_H_
#define SRC_MODULES_MODBUS_SLAVE_HANDLE_H_

#include <cstdint>
#include <cstring>

#include "forte_sync.h"
#include "forte_bool.h"
#include "forte_dword.h"

#include "io/mapper/io_handle.h"

namespace forte {
  namespace modbus {
    class ModbusSlaveHandler;

    class ModbusSlaveHandle : public forte::core::io::IOHandle {
    public:
      ModbusSlaveHandle(
          forte::core::io::IODeviceController *paController,
          forte::core::io::IOMapper::Direction paDirection,
          CIEC_ANY::EDataTypeID type,
          ModbusSlaveHandler *paSlave,
          std::uint16_t paStartAddress,
          size_t paBitOffset,
          size_t paBitWidth,
          bool paNetworkByteOrder
      );

      ~ModbusSlaveHandle() override = default;

      void set(const CIEC_ANY &) override;

      void set(const CIEC_BOOL &paState);

      void set(const CIEC_WORD &paState);

      void set(const CIEC_DWORD &paState);

      void get(CIEC_ANY &) override;

      void get(CIEC_BOOL &paState);

      void get(CIEC_WORD &paState);

      void get(CIEC_DWORD &paState);

      bool changed(std::array<char, 256> paChanges);

    protected:
      ModbusSlaveHandler *mSlave;
      const std::uint16_t mStartAddress;
      const size_t mBitOffset;
      const size_t mBitWidth;
      const bool mNetworkByteOrder;
    };
  }
}

#endif /* SRC_MODULES_MODBUS_SLAVE_HANDLE_H_ */
