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

#ifndef SRC_MODULES_MODBUS_SLAVE_SLAVE_H_
#define SRC_MODULES_MODBUS_SLAVE_SLAVE_H_

#include <cstdint>
#include <cstring>

#include "handle.h"
#include "fortelist.h"
#include "forte_sync.h"
#include "forte_wstring.h"
#include "devlog.h"

#include "io/mapper/io_mapper.h"

#include "../connection/connection.h"

namespace forte {
  namespace modbus {
    class ModbusBusHandler;

    class ModbusSlaveHandler {
      friend class ModbusBusHandler;

    public:
      enum struct SlaveType {
        Unknown = 0,
        FestoCMMP_AS = 1 //!< Festo CMMP-AS Motor Controller Family
      };

      struct Config {
        std::uint8_t mUnit;
        std::uint32_t mUpdateInterval;
      };

      void init();

      void deInit();

      void setConfig(Config paConfig);

      static SlaveType getSlaveType(ModbusBasicDeviceIdentification &paDeviceIdentification);

      void read(std::uint16_t paStartAddress, size_t paBitOffset, size_t paBitWidth, std::uint64_t &paValue);

      void write(std::uint16_t paStartAddress, size_t paBitOffset, size_t paBitWidth, std::uint64_t paValue);

      void addHandle(ModbusSlaveHandle *paHandle) {
        if (paHandle->is(forte::core::io::IOMapper::In)) {
          addHandle(&mInputs, paHandle);
        } else if (paHandle->is(forte::core::io::IOMapper::Out)) {
          addHandle(&mOutputs, paHandle);
        }
      }

      void dropHandles();

    private:
      Config mConfig;

      SlaveType mType;

      ModbusBusHandler *mBus;

      bool mInitialized;

      size_t mInProcessImageSize;
      CSyncObject mInProcessImageMutex;
      std::array<char, 256> mInProcessImage;

      size_t mOutProcessImageSize;
      CSyncObject mOutProcessImageMutex;
      std::array<char, 256> mOutProcessImage;

      CSyncObject mHandleMutex;
      typedef CSinglyLinkedList<ModbusSlaveHandle *> TSlaveHandleList;
      TSlaveHandleList mInputs;
      TSlaveHandleList mOutputs;

    protected:
      ModbusSlaveHandler(ModbusBusHandler *paBus, SlaveType paType);

      virtual ~ModbusSlaveHandler();

      void updateInProcessImage(const std::array<char, 256> &paInProcessImage, std::array<char, 256> &paChanges);

      void getOutProcessImage(std::array<char, 256> &paOutProcessImage);

      void update();

      void addHandle(
          TSlaveHandleList *paList,
          ModbusSlaveHandle *paHandle
      );

    public:
      ModbusSlaveHandler(const ModbusSlaveHandler &) = delete;
    };
  }
}

#endif /* SRC_MODULES_MODBUS_SLAVE_SLAVE_H_ */
