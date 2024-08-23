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

#include "BusAdapter.h"

#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "BusAdapter_gen.cpp"
#endif

DEFINE_ADAPTER_TYPE(ModbusBusAdapter, g_nStringIdModbusBusAdapter)

const CStringDictionary::TStringId ModbusBusAdapter::scm_anDataInputNames[] = {g_nStringIdQO};

const CStringDictionary::TStringId ModbusBusAdapter::scm_anDataInputTypeIds[] = {g_nStringIdBOOL};

const CStringDictionary::TStringId ModbusBusAdapter::scm_anDataOutputNames[] = {
    g_nStringIdQI,
    g_nStringIdMasterId,
    g_nStringIdIndex
};

const CStringDictionary::TStringId ModbusBusAdapter::scm_anDataOutputTypeIds[] = {
    g_nStringIdBOOL,
    g_nStringIdUINT,
    g_nStringIdUINT
};

const TDataIOID ModbusBusAdapter::scm_anEIWith[] = {
    0,
    255
};
const TForteInt16 ModbusBusAdapter::scm_anEIWithIndexes[] = {
    0,
    -1
};
const CStringDictionary::TStringId ModbusBusAdapter::scm_anEventInputNames[] = {g_nStringIdINITO};

const TDataIOID ModbusBusAdapter::scm_anEOWith[] = {
    0,
    1,
    2,
    255
};
const TForteInt16 ModbusBusAdapter::scm_anEOWithIndexes[] = {
    0,
    -1
};
const CStringDictionary::TStringId ModbusBusAdapter::scm_anEventOutputNames[] = {g_nStringIdINIT};

const SFBInterfaceSpec ModbusBusAdapter::scm_stFBInterfaceSpecSocket = {
    1,
    scm_anEventInputNames,
    scm_anEIWith,
    scm_anEIWithIndexes,
    1,
    scm_anEventOutputNames,
    scm_anEOWith,
    scm_anEOWithIndexes,
    1,
    scm_anDataInputNames,
    scm_anDataInputTypeIds,
    3,
    scm_anDataOutputNames,
    scm_anDataOutputTypeIds,
    0,
    nullptr
};

const SFBInterfaceSpec ModbusBusAdapter::scm_stFBInterfaceSpecPlug = {
    1,
    scm_anEventOutputNames,
    scm_anEOWith,
    scm_anEOWithIndexes,
    1,
    scm_anEventInputNames,
    scm_anEIWith,
    scm_anEIWithIndexes,
    3,
    scm_anDataOutputNames,
    scm_anDataOutputTypeIds,
    1,
    scm_anDataInputNames,
    scm_anDataInputTypeIds,
    0,
    nullptr
};

const TForteUInt8 ModbusBusAdapter::scmSlaveConfigurationIO[] = {};
const TForteUInt8 ModbusBusAdapter::scmSlaveConfigurationIONum = 0;

