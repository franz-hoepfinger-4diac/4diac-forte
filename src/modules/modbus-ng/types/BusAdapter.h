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

#ifndef SRC_MODULES_MODBUS_TYPES_BUSCONFIGADAPTER_H_
#define SRC_MODULES_MODBUS_TYPES_BUSCONFIGADAPTER_H_

#include "typelib.h"
#include "forte_bool.h"
#include "forte_uint.h"

#include "io/configFB/io_adapter_multi.h"

class ModbusBusAdapter : public forte::core::io::IOConfigFBMultiAdapter {
DECLARE_ADAPTER_TYPE(ModbusBusAdapter)

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

  static const SFBInterfaceSpec scm_stFBInterfaceSpecSocket;

  static const SFBInterfaceSpec scm_stFBInterfaceSpecPlug;

  static const TForteUInt8 scmSlaveConfigurationIO[];
  static const TForteUInt8 scmSlaveConfigurationIONum;

  FORTE_ADAPTER_DATA_ARRAY(1, 1, 1, 3, 0)

public:
  ADAPTER_CTOR_FOR_IO_MULTI(ModbusBusAdapter) {}

  ~ModbusBusAdapter() override = default;
};

#endif //SRC_MODULES_MODBUS_TYPES_BUSCONFIGADAPTER_H_

