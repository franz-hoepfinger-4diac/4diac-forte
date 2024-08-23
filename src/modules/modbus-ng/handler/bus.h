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

#ifndef SRC_MODULES_MODBUS_NG_HANDLER_BUS_H_
#define SRC_MODULES_MODBUS_NG_HANDLER_BUS_H_

#include <string>
#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include <pthread.h>


#include "fortelist.h"
#include "extevhan.h"
#include "devlog.h"
#include "timerha.h"
#include "forte_sync.h"
#include "forte_sem.h"
#include "forte_thread.h"

#include "io/device/io_controller_multi.h"

#include "../connection/connection.h"
#include "../slave/slave.h"

namespace forte {
  namespace modbus {
    class ModbusBusHandler : public forte::core::io::IODeviceMultiController {
      friend class ModbusSlaveHandler;

    public:
      struct Config : forte::core::io::IODeviceController::Config {
        std::string mAddress;
        std::string mPort;
      };

      class HandleDescriptor : public forte::core::io::IODeviceMultiController::HandleDescriptor {
      public:
        const CIEC_ANY::EDataTypeID mType;
        const std::uint16_t mStartAddress;
        const size_t mBitOffset;
        const size_t mBitWidth;
        const bool mNetworkByteOrder;

        HandleDescriptor(
            CIEC_WSTRING const &paId,
            forte::core::io::IOMapper::Direction paDirection,
            int paSlaveIndex,
            CIEC_ANY::EDataTypeID paType,
            std::uint16_t paStartAddress,
            size_t paBitOffset,
            size_t paBitWidth,
            bool paNetworkByteOrder
        ) : forte::core::io::IODeviceMultiController::HandleDescriptor(paId, paDirection, paSlaveIndex),
            mType(paType),
            mStartAddress(paStartAddress),
            mBitOffset(paBitOffset),
            mBitWidth(paBitWidth),
            mNetworkByteOrder(paNetworkByteOrder) {}
      };

      explicit ModbusBusHandler(CDeviceExecution &paDeviceExecution);

      void setConfig(struct forte::core::io::IODeviceController::Config *paConfig) override;

      ModbusSlaveHandler *getSlave(int paIndex);

      void addSlaveHandle(
          int paIndex,
          forte::core::io::IOHandle *paHandle
      ) override;

      void dropSlaveHandles(int paIndex) override;

    protected:
      const char *init() override;

      void deInit() override;

      forte::core::io::IOHandle *
      initHandle(forte::core::io::IODeviceController::HandleDescriptor *paHandleDescriptor) override;

      void runLoop() override;

      void update();

    private:
      struct Config mConfig;

      ModbusConnection *mConnection;

      ModbusSlaveHandler *mSlaves[256];

      std::uint32_t getMinimumSlaveUpdateInterval();

      bool isSlaveAvailable(int paIndex) override;

      bool checkSlaveType(
          int paIndex,
          int paType
      ) override;
    };
  }
}

#endif /* SRC_MODULES_MODBUS_NG_HANDLER_BUS_H_ */
