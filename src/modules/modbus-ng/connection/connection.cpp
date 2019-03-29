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

#include "connection.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <utility>

#include "frame.h"

using namespace forte::modbus;

ModbusConnection::ModbusConnection(
    std::string paHost,
    std::string paPort
) : mHost(std::move(paHost)), mPort(std::move(paPort)) {
}

ModbusConnection::ModbusConnection(const ModbusConnection &other) :
    mHost(other.mHost), mPort(other.mPort), mSocket(dup(other.mSocket)), mTransaction(other.mTransaction) {}

ModbusConnection::~ModbusConnection() {
  disconnect();
}

bool ModbusConnection::connect() {
  const struct addrinfo hints = {
      0,
      AF_UNSPEC,
      SOCK_STREAM,
      0,
      0,
      nullptr,
      nullptr,
      nullptr
  };

  struct addrinfo *res;
  int err = ::getaddrinfo(mHost.c_str(), mPort.c_str(), &hints, &res);
  if (err) {
    std::cerr << "Error resolving " << mHost << " " << mPort << ": " << gai_strerror(err) << std::endl;
    return false;
  }

  mSocket = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  freeaddrinfo(res);
  if (mSocket < 0) {
    std::cerr << "Error opening socket for " << mHost << " " << mPort << ": " << std::strerror(errno) << std::endl;
    return false;
  }

  err = ::connect(mSocket, res->ai_addr, res->ai_addrlen);
  if (err) {
    std::cerr << "Error connecting to " << mHost << " " << mPort << ": " << std::strerror(errno) << std::endl;
    close(mSocket);
    return false;
  }
  return true;
}

void ModbusConnection::disconnect() {
  close(mSocket);
  mSocket = -1;
}

bool ModbusConnection::readHoldingRegisters(
    std::uint8_t paUnit,
    std::uint16_t paStartAddress,
    size_t paLength,
    void *paBuffer
) {
  if (mSocket < 0) {
    return false;
  }

  ModbusReadHoldingRegistersRequestFrame request(mTransaction++, paUnit, htons(paStartAddress),
                                                 htons(static_cast<uint16_t>(paLength / 2)));
  ModbusResponse<ModbusReadHoldingRegistersResponseFrame<125>> response{};
  if (!sendFrame(sizeof(request), request)) {
    return false;
  }
  if (!receiveFrame(response)) {
    return false;
  }
  if (response.mResponse.mFrame.isException()) {
    return false;
  }

  std::memcpy(paBuffer, response.mResponse.mValues, response.mResponse.mByteCount);
  return true;
}

bool ModbusConnection::writeMultipleRegisters(
    std::uint8_t paUnit,
    std::uint16_t paStartAddress,
    size_t paLength,
    const void *paBuffer
) {
  if (mSocket < 0) {
    return false;
  }

  ModbusWriteMultipleRegistersRequestFrame<123> request(mTransaction++, paUnit, htons(paStartAddress),
                                                        htons(static_cast<uint16_t>(paLength / 2)),
                                                        static_cast<uint8_t>(paLength),
                                                        reinterpret_cast<const uint16_t *>(paBuffer));
  ModbusResponse<ModbusWriteMultipleRegistersResponseFrame> response{};
  if (!sendFrame(sizeof(ModbusWriteMultipleRegistersRequestFrame<0>) + paLength, request)) {
    return false;
  }
  if (!receiveFrame(response)) {
    return false;
  }
  return !response.mResponse.mFrame.isException();

}

bool ModbusConnection::readWriteMultipleRegisters(
    std::uint8_t paUnit,
    std::uint16_t paReadStartAddress,
    size_t paReadLength,
    std::uint16_t paWriteStartAddress,
    size_t paWriteLength,
    void *paReadBuffer,
    const void *paWriteBuffer
) {
  if (mSocket < 0) {
    return false;
  }

  ModbusReadWriteMultipleRegistersRequestFrame<0x79> request(mTransaction++, paUnit, htons(paReadStartAddress),
                                                             htons(static_cast<uint16_t>(paReadLength / 2)),
                                                             htons(paWriteStartAddress),
                                                             htons(static_cast<uint16_t>(paWriteLength / 2)),
                                                             static_cast<uint8_t>(paReadLength),
                                                             reinterpret_cast<const uint16_t *>(paWriteBuffer));
  ModbusResponse<ModbusReadWriteMultipleRegistersResponseFrame<0x7D>> response{};
  if (!sendFrame(sizeof(ModbusReadWriteMultipleRegistersRequestFrame<0>) + paWriteLength, request)) {
    return false;
  }
  if (!receiveFrame(response)) {
    return false;
  }
  if (response.mResponse.mFrame.isException()) {
    return false;
  }
  std::memcpy(paReadBuffer, response.mResponse.mValues, response.mResponse.mByteCount);

  return true;
}

bool ModbusConnection::readBasicDeviceIdentification(
    std::uint8_t paUnit,
    ModbusBasicDeviceIdentification &paDeviceIdentification
) {
  if (mSocket < 0) {
    return false;
  }

  ModbusReadDeviceIdentificationRequestFrame request(mTransaction++, paUnit, ModbusReadDeviceIdentificationCode::BASIC,
                                                     ModbusReadDeviceIdentificationObjectId::VENDOR_NAME);
  ModbusResponse<ModbusReadDeviceIdentificationResponseFrame<4096>> response{};
  if (!sendFrame(sizeof(request), request)) {
    return false;
  }
  if (!receiveFrame(response)) {
    return false;
  }
  if (response.mResponse.mFrame.mFrame.isException()) {
    return false;
  }

  for (auto &object : response.mResponse) {
    switch (object.mId) {
      case ModbusReadDeviceIdentificationObjectId::VENDOR_NAME:
        paDeviceIdentification.mVendorName = std::string(object.mValue, object.mLength);
        break;
      case ModbusReadDeviceIdentificationObjectId::PRODUCT_CODE:
        paDeviceIdentification.mProductCode = std::string(object.mValue, object.mLength);
        break;
      case ModbusReadDeviceIdentificationObjectId::MAJOR_MINOR_REVISION:
        paDeviceIdentification.mMajorMinorRevision = std::string(object.mValue, object.mLength);
        break;
      default:
        break;
    }
  }
  return true;
}

template<typename F>
bool ModbusConnection::sendFrame(
    size_t paLength,
    const F &paFrame
) {
  size_t offset = 0;
  while (offset < paLength) {
    ssize_t res = send(mSocket, reinterpret_cast<const char *>(&paFrame) + offset, paLength - offset, 0);
    if (res < 0) {
      std::cerr << "Error sending request to " << mHost << " " << mPort << ": " << std::strerror(errno)
                << std::endl;
      return false;
    }
    offset += static_cast<size_t>(res);
  }
  return true;
}

template<typename F>
bool ModbusConnection::receiveFrame(ModbusResponse<F> &paBuffer) {
  // recv mFrame header
  size_t offset = 0;
  while (offset < sizeof(paBuffer.mFrame)) {
    ssize_t res = recv(mSocket, reinterpret_cast<char *>(&paBuffer) + offset, sizeof(paBuffer.mFrame) - offset, 0);
    if (res < 0) {
      std::cerr << "Error receiving response from " << mHost << " " << mPort << ": " << std::strerror(errno)
                << std::endl;
      return false;
    }
    offset += static_cast<size_t>(res);
  }

  // check mFrame length
  size_t frame_length = ntohs(paBuffer.mFrame.mLength) + offsetof(ModbusFrame, mUnit);
  if (frame_length > sizeof(paBuffer)) {
    std::cerr << "Error receiving response from " << mHost << " " << mPort << ": Frame too large (" << frame_length
              << " of " << sizeof(paBuffer) << ")" << std::endl;
    return false;
  }

  // receive rest of mFrame
  while (offset < frame_length) {
    ssize_t res = recv(mSocket, reinterpret_cast<char *>(&paBuffer) + offset, frame_length - offset, 0);
    if (res < 0) {
      std::cerr << "Error receiving response from " << mHost << " " << mPort << ": " << std::strerror(errno)
                << std::endl;
      return false;
    }
    offset += static_cast<size_t>(res);
  }
  return true;
}

