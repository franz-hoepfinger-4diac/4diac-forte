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

#ifndef SRC_MODULES_MODBUS_CONNECTION_FRAME_H_
#define SRC_MODULES_MODBUS_CONNECTION_FRAME_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <type_traits>
#include <netinet/in.h>

#pragma pack(push, 1)

namespace forte {
  namespace modbus {
    enum struct ModbusFunction : std::uint8_t {
      INVALID = 0,
      READ_COILS = 0x01,
      READ_DISCRETE_INPUTS = 0x02,
      READ_HOLDING_REGISTERS = 0x03,
      READ_INPUT_REGISTERS = 0x04,
      WRITE_SINGLE_COIL = 0x05,
      WRITE_SINGLE_REGISTER = 0x06,
      READ_EXCEPTION_STATUS = 0x07,
      DIAGNOSTICS = 0x08,
      GET_COM_EVENT_COUNTER = 0x0B,
      GET_COM_EVENT_LOG = 0x0C,
      WRITE_MULTIPLE_COILS = 0x0F,
      WRITE_MULTIPLE_REGISTERS = 0x10,
      REPORT_SERVER_ID = 0x11,
      READ_FILE_RECORD = 0x14,
      WRITE_FILE_RECORD = 0x15,
      MASK_WRITE_REGISTER = 0x16,
      READ_WRITE_MULTIPLE_REGISTERS = 0x17,
      READ_FIFO_QUEUE = 0x18,
      ENCAPSULATED_INTERFACE_TRANSPORT = 0x2B
    };

    enum struct ModbusMEIType : std::uint8_t {
      INVALID = 0,
      READ_DEVICE_IDENTIFICATION = 0x0E
    };

    enum struct ModbusReadDeviceIdentificationCode : std::uint8_t {
      INVALID = 0,
      BASIC = 0x01,
      REGULAR = 0x02,
      EXTENDED = 0x03,
      INDIVIDUAL = 0x04
    };

    enum struct ModbusReadDeviceIdentificationObjectId : std::uint8_t {
      VENDOR_NAME = 0x00,
      PRODUCT_CODE = 0x01,
      MAJOR_MINOR_REVISION = 0x02,
      VENDOR_URL = 0x03,
      PRODUCT_NAME = 0x04,
      MODEL_NAME = 0x05,
      USER_APPLICATION_NAME = 0x06
    };

    enum struct ModbusExceptionCode : std::uint8_t {
      SUCCESS = 0x00,
      ILLEGAL_FUNCTION = 0x01,
      ILLEGAL_DATA_ADDRESS = 0x02,
      ILLEGAL_DATA_VALUE = 0x03,
      SERVER_DEVICE_FAILURE = 0x04,
      ACKNOWLEDGE = 0x05,
      SERVER_DEVICE_BUSY = 0x06,
      MEMORY_PARITY_ERROR = 0x08,
      GATEWAY_PATH_UNAVAILABLE = 0x0A,
      GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B
    };

    struct ModbusFrame {
      std::uint16_t mTransaction;
      std::uint16_t mProtocol = 0;
      std::uint16_t mLength;
      std::uint8_t mUnit;
      ModbusFunction mFunction;

      ModbusFrame() = default;

      ModbusFrame(
          std::uint16_t paTransaction,
          std::uint16_t paLength,
          std::uint8_t paUnit,
          ModbusFunction paFunction
      ) : mTransaction(paTransaction), mLength(paLength), mUnit(paUnit), mFunction(paFunction) {}

      bool isException() const {
        return (static_cast<std::uint8_t>(mFunction) & 0x80) != 0;
      }
    };

    struct ModbusReadHoldingRegistersRequestFrame {
      ModbusFrame mFrame;
      std::uint16_t mStartAddress;
      std::uint16_t mLength;

      ModbusReadHoldingRegistersRequestFrame() = default;

      ModbusReadHoldingRegistersRequestFrame(
          std::uint16_t paTransaction,
          std::uint8_t paUnit,
          std::uint16_t paStartAddress,
          std::uint16_t paLength
      ) : mFrame(paTransaction, htons(sizeof(ModbusReadHoldingRegistersRequestFrame) - offsetof(ModbusFrame, mUnit)), paUnit,
                ModbusFunction::READ_HOLDING_REGISTERS),
          mStartAddress(paStartAddress),
          mLength(paLength) {}
    };

    template<int N>
    struct ModbusReadHoldingRegistersResponseFrame {
      ModbusFrame mFrame;
      std::uint8_t mByteCount{};
      std::uint16_t mValues[N]{};
    };

    template<int N>
    struct ModbusWriteMultipleRegistersRequestFrame {
      ModbusFrame mFrame;
      std::uint16_t mStartAddress{};
      std::uint16_t mLength{};
      std::uint8_t mByteCount{};
      std::uint16_t mValues[N]{};

      ModbusWriteMultipleRegistersRequestFrame() = default;

      ModbusWriteMultipleRegistersRequestFrame(
          std::uint16_t paTransaction,
          std::uint8_t paUnit,
          std::uint16_t paStartAddress,
          std::uint16_t paLength,
          std::uint8_t paByteCount,
          const std::uint16_t paValues[N]
      ) : mFrame(paTransaction, htons(
          static_cast<std::uint16_t >(sizeof(ModbusWriteMultipleRegistersRequestFrame<0>) + paByteCount -
                                      offsetof(ModbusFrame, mUnit))), paUnit, ModbusFunction::WRITE_MULTIPLE_REGISTERS),
          mStartAddress(paStartAddress),
          mLength(paLength),
          mByteCount(paByteCount) {
        std::memcpy(mValues, paValues, paByteCount);
      }
    };

    struct ModbusWriteMultipleRegistersResponseFrame {
      ModbusFrame mFrame;
      std::uint16_t mStartAddress{};
      std::uint16_t mLength{};
    };

    template<int N>
    struct ModbusReadWriteMultipleRegistersRequestFrame {
      ModbusFrame mFrame;
      std::uint16_t mReadStartAddress{};
      std::uint16_t mReadLength{};
      std::uint16_t mWriteStartAddress{};
      std::uint16_t mWriteLength{};
      std::uint8_t mByteCount{};
      std::uint16_t mValues[N]{};

      ModbusReadWriteMultipleRegistersRequestFrame() = default;

      ModbusReadWriteMultipleRegistersRequestFrame(
          std::uint16_t paTransaction,
          std::uint8_t paUnit,
          std::uint16_t paReadStartAddress,
          std::uint16_t paReadLength,
          std::uint16_t paWriteStartAddress,
          std::uint16_t paWriteLength,
          std::uint8_t paByteCount,
          const std::uint16_t paValues[N]
      ) : mFrame(paTransaction, htons(
          static_cast<uint16_t >(sizeof(ModbusReadWriteMultipleRegistersRequestFrame<0>) + paByteCount -
                                 offsetof(ModbusFrame, mUnit))), paUnit, ModbusFunction::READ_WRITE_MULTIPLE_REGISTERS),
          mReadStartAddress(paReadStartAddress),
          mReadLength(paReadLength),
          mWriteStartAddress(paWriteStartAddress),
          mWriteLength(paWriteLength),
          mByteCount(paByteCount) {
        std::memcpy(mValues, paValues, paByteCount);
      }
    };

    template<int N>
    struct ModbusReadWriteMultipleRegistersResponseFrame {
      ModbusFrame mFrame;
      std::uint8_t mByteCount{};
      std::uint16_t mValues[N]{};
    };

    struct ModbusEncapsulatedInterfaceTransportFrame {
      ModbusFrame mFrame;
      ModbusMEIType mMeiType{};

      ModbusEncapsulatedInterfaceTransportFrame() = default;

      ModbusEncapsulatedInterfaceTransportFrame(
          std::uint16_t paTransaction,
          std::uint16_t paLength,
          std::uint8_t paUnit,
          ModbusMEIType paMeiType
      ) : mFrame(paTransaction, paLength, paUnit, ModbusFunction::ENCAPSULATED_INTERFACE_TRANSPORT), mMeiType(paMeiType) {}
    };

    struct ModbusReadDeviceIdentificationRequestFrame {
      ModbusEncapsulatedInterfaceTransportFrame mFrame;
      ModbusReadDeviceIdentificationCode mCode;
      ModbusReadDeviceIdentificationObjectId mObjectId;

      ModbusReadDeviceIdentificationRequestFrame(
          std::uint16_t paTransaction,
          std::uint8_t paUnit,
          ModbusReadDeviceIdentificationCode paCode,
          ModbusReadDeviceIdentificationObjectId paObjectId
      ) : mFrame(paTransaction,
                htons(sizeof(ModbusReadDeviceIdentificationRequestFrame) - offsetof(ModbusFrame, mUnit)), paUnit,
                ModbusMEIType::READ_DEVICE_IDENTIFICATION),
          mCode(paCode),
          mObjectId(paObjectId) {};
    };

    struct ModbusReadDeviceIdentificationObject {
      ModbusReadDeviceIdentificationObjectId mId;
      std::uint8_t mLength;
      char mValue[0];
    };

    template<int N>
    struct ModbusReadDeviceIdentificationResponseFrame {
      ModbusEncapsulatedInterfaceTransportFrame mFrame;
      ModbusReadDeviceIdentificationCode mCode;
      std::uint8_t mConformityLevel{};
      std::uint8_t mMoreFollows{};
      ModbusReadDeviceIdentificationObjectId mNextObjectId;
      std::uint8_t mNumberOfObjects{};
      char mObjects[N]{};

      class iterator : public std::iterator<std::forward_iterator_tag, ModbusReadDeviceIdentificationObject> {
        ModbusReadDeviceIdentificationResponseFrame *mFrame;
        off_t mOffset;
      public:
        iterator(
            ModbusReadDeviceIdentificationResponseFrame *paFrame,
            off_t paOffset
        ) : mFrame(paFrame), mOffset(paOffset) {}

        iterator &operator++() {
          mOffset += sizeof(ModbusReadDeviceIdentificationObject) + operator*().mLength;
          return *this;
        }

        const iterator operator++(int) {
          iterator tmp(*this);
          operator++();
          return tmp;
        }

        bool operator==(const iterator &rhs) const { return mFrame == rhs.mFrame && mOffset == rhs.mOffset; }

        bool operator!=(const iterator &rhs) const { return mFrame != rhs.mFrame || mOffset != rhs.mOffset; }

        ModbusReadDeviceIdentificationObject &operator*() {
          return *reinterpret_cast<ModbusReadDeviceIdentificationObject *>(mFrame->mObjects + mOffset);
        }
      };

      iterator begin() {
        return iterator(this, 0);
      }

      iterator end() {
        return iterator(this, ntohs(mFrame.mFrame.mLength) + offsetof(ModbusFrame, mUnit) -
                              offsetof(ModbusReadDeviceIdentificationResponseFrame, mObjects));
      }
    };

    struct ModbusExceptionResponseFrame {
      ModbusFrame mFrame;
      ModbusExceptionCode mExceptionCode;
    };

    template<typename T>
    union ModbusResponse {
      ModbusFrame mFrame;
      ModbusExceptionResponseFrame mException;
      T mResponse;
    };

    static_assert(std::is_standard_layout<ModbusFrame>::value, "A modbus mFrame must have standard layout");
    static_assert(std::is_standard_layout<ModbusEncapsulatedInterfaceTransportFrame>::value,
                  "A modbus encapsulated interface transport mFrame must have standard layout");
    static_assert(std::is_standard_layout<ModbusReadDeviceIdentificationRequestFrame>::value,
                  "A modbus read device identification request mFrame must have standard layout");
    static_assert(std::is_standard_layout<ModbusReadDeviceIdentificationResponseFrame<0>>::value,
                  "A modbus read device identification response mFrame must have standard layout");
    static_assert(std::is_standard_layout<ModbusReadDeviceIdentificationObject>::value,
                  "A modbus read device identification object must have standard layout");
  }
}

#pragma pack(pop)

#endif //SRC_MODULES_MODBUS_CONNECTION_FRAME_H_
