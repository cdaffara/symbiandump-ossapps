/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




#ifndef CREATORCONNECTIONMETHODELEMENT_H_
#define CREATORCONNECTIONMETHODELEMENT_H_

#include "creator_scriptelement.h"

namespace creatorconnectionmethod
{
    _LIT(KCm, "connectionmethod");
    _LIT(Kconnectionname, "connectionname");
    _LIT(Kbearertype, "bearertype");
    _LIT(Kstartpage, "startpage");
    _LIT(Kwapwspoption, "wapwspoption");
    _LIT(Kprotocoltype, "protocoltype");
    _LIT(Kloginname, "loginname");
    _LIT(Ksecureauthentication, "secureauthentication");
    _LIT(Kloginpass, "loginpass");
    _LIT(Kpromptpassword, "promptpassword");
    _LIT(Kgatewayaddress, "gatewayaddress");
    _LIT(Ksubnetmask, "subnetmask");
    _LIT(Kdeviceipaddr, "deviceipaddr");
    _LIT(Kip4nameserver1, "ip4nameserver1");
    _LIT(Kip4nameserver2, "ip4nameserver2");
    _LIT(Kdatacalltelnumber, "datacalltelnumber");
    _LIT(Kdatacalltypeisdn, "datacalltypeisdn");
    _LIT(Kdatacalllinespeed, "datacalllinespeed");
    _LIT(Kuseproxy, "useproxy");
    _LIT(Kproxyserveraddress, "proxyserveraddress");
    _LIT(Kproxyportnumber, "proxyportnumber");
    _LIT(Kip6nameserver1, "ip6nameserver1");
    _LIT(Kip6nameserver2, "ip6nameserver2");
    _LIT(Kdisabletextauth, "disabletextauth");
    _LIT(Kwlanname, "wlanname");
    _LIT(Kwlanipaddr, "wlanipaddr");
    _LIT(Kwlansecmode, "wlansecmode");
    _LIT(Kwlannetmode, "wlannetmode");
    
    // Bearer type:
    _LIT(Kwlan, "wlan");
    _LIT(Kgprs, "gprs");
    _LIT(Kdatacall, "datacall");
    _LIT(Khsgsm, "hsgsm");
    _LIT(Kembedded, "embedded");
    _LIT(Kvpn, "vpn");
    _LIT(Klan, "lan");
    
    // Data call type isdn:
    _LIT(Kanalogue, "analogue");
    _LIT(Kisdnv110, "isdnv110");
    _LIT(Kisdnv120, "isdnv120");
    
    // Line speed:
    _LIT(Kautomatic, "automatic");
    // WAP wps options:
    _LIT(Kconnectionless, "connectionless"); // ECmWapWspOptionConnectionless
    _LIT(Kconnectionoriented, "connectionoriented"); // ECmWapWspOptionConnectionOriented
    
    // Protocol
    _LIT(Kipv4, "ipv4");
    _LIT(Kipv6, "ipv6");

    // WLAN security mode:
    _LIT(Kopen, "open");
    _LIT(Kwep, "wep");
    _LIT(Ke802_1x, "e802_1x");
    _LIT(Kwpa, "wpa");
    _LIT(Kwpa2, "wpa2");
}

class CCreatorConnectionMethodElement : public CCreatorScriptElement
{
public:
    static CCreatorConnectionMethodElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    
protected:
    CCreatorConnectionMethodElement(CCreatorEngine* aEngine);
};

#endif /*CREATORCONNECTIONMETHODELEMENT_H_*/
