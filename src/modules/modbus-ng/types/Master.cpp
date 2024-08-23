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

#include "Master.h"

#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "Master_gen.cpp"
#endif

using namespace forte::modbus;

DEFINE_FIRMWARE_FB(ModbusMaster, g_nStringIdModbusMaster)

const CStringDictionary::TStringId ModbusMaster::scm_anDataInputNames[] = {
    g_nStringIdQI,
    g_nStringIdAddress,
    g_nStringIdPort
};

const CStringDictionary::TStringId ModbusMaster::scm_anDataInputTypeIds[] = {
    g_nStringIdBOOL,
    g_nStringIdWSTRING,
    g_nStringIdWSTRING
};

const CStringDictionary::TStringId ModbusMaster::scm_anDataOutputNames[] = {
    g_nStringIdQO,
    g_nStringIdSTATUS
};

const CStringDictionary::TStringId ModbusMaster::scm_anDataOutputTypeIds[] = {
    g_nStringIdBOOL,
    g_nStringIdWSTRING
};

const TForteInt16 ModbusMaster::scm_anEIWithIndexes[] = {
    0,
    -1
};

const TDataIOID ModbusMaster::scm_anEIWith[] = {
    0,
    1,
    2,
    255
};

const CStringDictionary::TStringId ModbusMaster::scm_anEventInputNames[] = {
    g_nStringIdINIT
};

const TDataIOID ModbusMaster::scm_anEOWith[] = {
    0,
    1,
    255,
    0,
    1,
    255
};

const TForteInt16 ModbusMaster::scm_anEOWithIndexes[] = {
    0,
    3,
    -1
};

const CStringDictionary::TStringId ModbusMaster::scm_anEventOutputNames[] = {
    g_nStringIdINITO,
    g_nStringIdIND
};

const SAdapterInstanceDef ModbusMaster::scm_astAdapterInstances[] = {
    {g_nStringIdModbusBusAdapter, g_nStringIdBusAdapterOut, true}
};

const SFBInterfaceSpec ModbusMaster::scm_stFBInterfaceSpec = {
    1,
    scm_anEventInputNames,
    scm_anEIWith,
    scm_anEIWithIndexes,
    2,
    scm_anEventOutputNames,
    scm_anEOWith,
    scm_anEOWithIndexes,
    3,
    scm_anDataInputNames,
    scm_anDataInputTypeIds,
    2,
    scm_anDataOutputNames,
    scm_anDataOutputTypeIds,
    1,
    scm_astAdapterInstances
};

forte::core::io::IODeviceController *ModbusMaster::createDeviceController(CDeviceExecution &paDeviceExecution) {
  return new ModbusBusHandler(paDeviceExecution);
}

void ModbusMaster::setConfig() {
  ModbusBusHandler::Config config;
  config.mAddress = Address().getValue();
  config.mPort = Port().getValue();
  getDeviceController()->setConfig(&config);
}

