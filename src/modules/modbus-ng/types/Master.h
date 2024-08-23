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

#ifndef SRC_MODULES_MODBUS_TYPES_MASTER_H_
#define SRC_MODULES_MODBUS_TYPES_MASTER_H_

#include "forte_bool.h"
#include "forte_wstring.h"
#include "devlog.h"

#include "io/configFB/io_master_multi.h"

#include "../handler/bus.h"

#include "BusAdapter.h"

class ModbusMaster : public forte::core::io::IOConfigFBMultiMaster {
DECLARE_FIRMWARE_FB(ModbusMaster)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];

  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];

  static const TForteInt16 scm_anEIWithIndexes[];
  static const TDataIOID scm_anEIWith[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TForteInt16 scm_anEOWithIndexes[];
  static const TDataIOID scm_anEOWith[];

  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SAdapterInstanceDef scm_astAdapterInstances[];

  static const int scm_nBusAdapterAdpNum = 0;
  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

  FORTE_FB_DATA_ARRAY(2, 3, 2, 1)

  CIEC_WSTRING &Address() {
    return *static_cast<CIEC_WSTRING *>(getDI(1));
  }

  CIEC_WSTRING &Port() {
    return *static_cast<CIEC_WSTRING *>(getDI(2));
  }

protected:
  forte::core::io::IODeviceController *createDeviceController(CDeviceExecution &paDeviceExecution) override;

  void setConfig() override;

public:
  FUNCTION_BLOCK_CTOR_WITH_BASE_CLASS(ModbusMaster, forte::core::io::IOConfigFBMultiMaster) {}

  ~ModbusMaster() override = default;
};

#endif //SRC_MODULES_MODBUS_TYPES_MASTER_H_

