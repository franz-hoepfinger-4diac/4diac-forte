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

#ifndef SRC_MODULES_MODBUS_CONNECTION_CONNECTION_H_
#define SRC_MODULES_MODBUS_CONNECTION_CONNECTION_H_

#include <string>

#include "frame.h"

namespace forte {
  namespace modbus {
    struct ModbusBasicDeviceIdentification {
      std::string mVendorName;
      std::string mProductCode;
      std::string mMajorMinorRevision;

      ModbusBasicDeviceIdentification() : mVendorName(), mProductCode(), mMajorMinorRevision() {
      }

      ModbusBasicDeviceIdentification(
          std::string paVendorName,
          std::string paProductCode,
          std::string paMajorMinorRevision
      ) : mVendorName(std::move(paVendorName)),
          mProductCode(std::move(paProductCode)),
          mMajorMinorRevision(std::move(paMajorMinorRevision)) {
      }
    };

    class ModbusConnection {
    private:
      std::string mHost;
      std::string mPort;
      int mSocket = -1;
      std::uint16_t mTransaction = 0;

      template<typename F>
      bool sendFrame(
          size_t paLength,
          const F &paFrame
      );

      template<typename F>
      bool receiveFrame(ModbusResponse<F> &paBuffer);

    public:
      ModbusConnection(
          std::string paHost,
          std::string paPort
      );

      ModbusConnection(const ModbusConnection &other);

      ModbusConnection(ModbusConnection &&other) noexcept:
          mHost(other.mHost), mPort(other.mPort), mSocket(other.mSocket), mTransaction(other.mTransaction) {
        other.mSocket = -1;
      }

      ~ModbusConnection();

      ModbusConnection &operator=(const ModbusConnection &other) {
        *this = ModbusConnection(other);
        return *this;
      }

      ModbusConnection &operator=(ModbusConnection &&other) noexcept {
        mHost = other.mHost;
        mPort = other.mPort;
        std::swap(mSocket, other.mSocket);
        mTransaction = other.mTransaction;
        return *this;
      }

      bool connect();

      void disconnect();

      bool readHoldingRegisters(
          std::uint8_t paUnit,
          std::uint16_t paStartAddress,
          size_t paLength,
          void *paBuffer
      );

      bool writeMultipleRegisters(
          std::uint8_t paUnit,
          std::uint16_t paStartAddress,
          size_t paLength,
          const void *paBuffer
      );

      bool readWriteMultipleRegisters(
          std::uint8_t paUnit,
          std::uint16_t paReadStartAddress,
          size_t paReadLength,
          std::uint16_t paWriteStartAddress,
          size_t paWriteLength,
          void *paReadBuffer,
          const void *paWriteBuffer
      );

      bool readBasicDeviceIdentification(
          std::uint8_t paUnit,
          ModbusBasicDeviceIdentification &paDeviceIdentification
      );
    };
  }
}

#endif //SRC_MODULES_MODBUS_CONNECTION_CONNECTION_H_
