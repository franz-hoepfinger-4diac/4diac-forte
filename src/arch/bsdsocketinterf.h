/*******************************************************************************
 * Copyright (c) 2010, 2023 ACIN, fortiss GmbH, OFFIS e.V.
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alois Zoitl, Ingo Hegny - initial API and implementation and/or initial documentation
 *   Jörg Walter - improve UDP multicast support
 *******************************************************************************/
#ifndef BSDSOCKETINTERF_H_
#define BSDSOCKETINTERF_H_

#include <sockhand.h>

class CBSDSocketInterface{
  public:
    using TSocketDescriptor = FORTE_SOCKET_TYPE;
    using TUDPDestAddr = struct sockaddr_in;
    static constexpr const char *scmAllInterfaces = "0.0.0.0";

    static void closeSocket(TSocketDescriptor paSockD);
    static TSocketDescriptor openTCPServerConnection(const char *const paIPAddr, unsigned short paPort);
    static TSocketDescriptor openTCPClientConnection(char *paIPAddr, unsigned short paPort);
    static TSocketDescriptor acceptTCPConnection(TSocketDescriptor paListeningSockD);
    static int sendDataOnTCP(TSocketDescriptor paSockD, const char *paData, unsigned int paSize);
    static int receiveDataFromTCP(TSocketDescriptor paSockD, char* paData, unsigned int paBufSize);

    static TSocketDescriptor openUDPSendPort(char *paIPAddr, unsigned short paPort, TUDPDestAddr *mtDestAddr, const char *paMCInterface = nullptr);
    static TSocketDescriptor openUDPReceivePort(char *paIPAddr, unsigned short paPort, const char *paMCInterface = scmAllInterfaces);
    static int sendDataOnUDP(TSocketDescriptor paSockD, TUDPDestAddr *paDestAddr, char* paData, unsigned int paSize);
    static int receiveDataFromUDP(TSocketDescriptor paSockD, char* paData, unsigned int paBufSize);


    static int handleError(int nRetVal, int err, const char* msg);


    CBSDSocketInterface() = delete;
};

#endif /* BSDSOCKETINTERF_H_ */
