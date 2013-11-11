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




#ifndef __CREATORCONNECTIONSETTINGS_H__
#define __CREATORCONNECTIONSETTINGS_H__

#include "engine.h"
#include "creator_modulebase.h"
#include "creator_connectionmethodbase.h"

#include <e32base.h>
#include <commdb.h>
#include <msvapi.h>
#include <mtclreg.h>
#include <mmsclient.h>

#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <cmpluginwlandef.h>
using namespace CMManager;

#include <etelpckt.h>//ETel Packet API needed for enums

class CCreatorEngine;
class CConnectionSettingsParameters;

class CCreatorConnectionSettings : public CCreatorConnectionSettingsBase, public MMsvSessionObserver
    {
public: 
    static CCreatorConnectionSettings* NewL(CCreatorEngine* aEngine);
    static CCreatorConnectionSettings* NewLC(CCreatorEngine* aEngine);
    ~CCreatorConnectionSettings();

private:
    CCreatorConnectionSettings();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase

    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3); // from MMsvSessionObserver
        
public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    
    TInt CreateConnectionSettingsEntryL(CCreatorModuleBaseParameters *aParameters);    
    TUint32 AccessPointNameToIdL(const TDesC& aAPName, TBool aAnyIfNotFound = EFalse );
    
    //Setting default connection method, if not set
    void SetDefaultCML(RCmConnectionMethodExt conMethod);
    //Set random parameters
    void SetRandomParametersL(CConnectionSettingsParameters& parameters);
    void SetRandomBearerTypeL(CConnectionSettingsParameters& parameters);
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();
private:
    void DeleteAllDestinationsL( TBool aOnlyCreatedWithCreator );
private:
    CCommsDatabase* iCommsDb;
    
    RCmManagerExt iCmManagerExt;

    CConnectionSettingsParameters* iParameters;
    
    RArray<TUint32> iEntryIds; // TUint32 RCmDestinationExt::Id()

public:
    };

   
class CConnectionSettingsParameters : public CCreatorModuleBaseParameters
    {
public: 
    HBufC*          iConnectionName; // connection name
    TUint           iBearerType; // bearer type
    HBufC*          iStartPage; // start page
    TWapWSPOption   iWapWspOption; // Indicating whether connection-oriented  or connectionless API should be used.
    
    RPacketContext::TProtocolType iProtocolType; //Type of protocol EPdpTypeIPv4 or EPdpTypeIPv4
    
    HBufC*          iLoginName; // username
    TBool           iSecureAuthentication; // password authentication
    HBufC*          iLoginPass; // password
    TBool           iPromptPassword; // prompt password
    HBufC*          iGatewayAddress; // gateway address
    HBufC*          iSubnetMask;  // subnet mask
    HBufC*          iIPAddr; // phone IP
    HBufC*          iIP4NameServer1;  // primary name server
    HBufC*          iIP4NameServer2;  // secondary name server
    HBufC*          iDefaultTelNumber; // datacall number
    TUint32         iBearerCallTypeIsdn; // datacall type
    TUint32         iBearerSpeed;  // max connection speed
    TBool           iUseProxy; //Use proxy or not
    HBufC*          iProxyServerAddress;  // proxy address
    TUint32         iProxyPortNumber; // proxy number
    HBufC*          iWLANName;//WLAN network name
    HBufC*          iWlanIpAddr;//IP address of EPOC.
    TWlanSecMode    iWLanSecMode;//WLAN security modes
    TWlanNetMode    iWlanNetMode;//WLAN connection mode. Ad-hoc or infrastructure.

    HBufC*          iIP6NameServer1;  // primary name server for IP v6
    HBufC*          iIP6NameServer2;  // secondary name server for IP v6
    TBool           iDisableTextAuth; // Disable plaintext authentication 
    
    CCreatorEngine* iEngine;
    
public:
    CConnectionSettingsParameters();
    ~CConnectionSettingsParameters();   
    
    void SetRandomCMNameL(CCreatorEngine& aEngine);
    void SetRandomWLANNameL(CCreatorEngine& aEngine);
    void SetRandomWLANNetModeL(CCreatorEngine& aEngine);
    void SetRandomWLANSecurityModeL(CCreatorEngine& aEngine);
    void SetRandomLoginNameL(CCreatorEngine& aEngine);
    void SetRandomLoginPassL(CCreatorEngine& aEngine);
    void SetRandomSecureAuthenticationL(CCreatorEngine& aEngine);
    void SetRandomPromptPasswordL(CCreatorEngine& aEngine);
    void SetRandomUseProxyL(CCreatorEngine& aEngine);
    void SetRandomProxyAddressL(CCreatorEngine& aEngine);
    void SetRandomProxyPortL(CCreatorEngine& aEngine);
    void SetRandomStartPageL(CCreatorEngine& aEngine);
    void SetRandomIPAddressL(CCreatorEngine& aEngine);
    void SetRandomIP4NameServer1L(CCreatorEngine& aEngine);
    void SetRandomIP4NameServer2L(CCreatorEngine& aEngine);
    void SetRandomProtocolTypeL(CCreatorEngine& aEngine);
    void SetRandomTelephoneNumberL(CCreatorEngine& aEngine);
    void SetRandomBearerTypeIsdnL(CCreatorEngine& aEngine);
    void SetRandomBearerSpeedL(CCreatorEngine& aEngine);
    void SetRandomWapWspOptionL(CCreatorEngine& aEngine);
    void SetRandomSubnetMaskL(CCreatorEngine& aEngine);
    void SetRandomGatewayAddressL(CCreatorEngine& aEngine);
    void SetRandomWlanIpAddrL(CCreatorEngine& aEngine);
    void SetRandomIPv6NameServer1L(CCreatorEngine& aEngine);
    void SetRandomIPv6NameServer2L(CCreatorEngine& aEngine);
    void SetRandomDisableTextAuthL(CCreatorEngine& aEngine);
    };



#endif // __CREATORCONNECTIONSETTINGS_H__
