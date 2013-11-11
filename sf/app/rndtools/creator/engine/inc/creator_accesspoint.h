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





#ifndef __CREATORACCESSPOINTS_H__
#define __CREATORACCESSPOINTS_H__

#include "engine.h"
#include "creator_modulebase.h"
#include "creator_connectionmethodbase.h"

#include <e32base.h>
#include <commdb.h>
//#include <ApSelect.h>
//#include <ApDataHandler.h>
//#include <ApAccessPointItem.h>
//#include <ApUtils.h>
//#include <ApEngineConsts.h>
#include <msvapi.h>
#include <mtclreg.h>
#include <mmsclient.h>


class CCreatorEngine;
class CAccessPointsParameters;



class CCreatorAccessPoints : public CCreatorConnectionSettingsBase, public MMsvSessionObserver
    {
public: 
    static CCreatorAccessPoints* NewL(CCreatorEngine* aEngine);
    static CCreatorAccessPoints* NewLC(CCreatorEngine* aEngine);
    ~CCreatorAccessPoints();

private:
    CCreatorAccessPoints();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase

    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3); // from MMsvSessionObserver

public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    
    TInt CreateConnectionSettingsEntryL(CCreatorModuleBaseParameters *aParameters);    
    TUint32 AccessPointNameToIdL(const TDesC& aAPName, TBool aAnyIfNotFound = EFalse );
    
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();

private:
    CCommsDatabase* iCommsDb;
    CApDataHandler* iApDataHandler;
    CApUtils* iApUtils;
    CAccessPointsParameters* iParameters;
    RArray<TUint32> iEntryIds; // TUint32 CApDataHandler::CreateFromDataL

public:
    };


class CAccessPointsParameters : public CCreatorModuleBaseParameters
    {
public: 
    HBufC*          iConnectionName; // connection name
    TApBearerType   iBearerType; // bearer type
    HBufC*          iWapStartPage; // WAP start page
    TUint32         iWapWspOption; // connection type
    HBufC*          iGprsAcessPointName; // access point name
    HBufC*          iIspLoginName; // username
    TBool           iSecureAuthentication; // password authentication
    HBufC*          iIspLoginPass; // password
    TBool           iPromptPassword; // prompt password
    HBufC*          iWapGatewayAddress; // WAP gateway address
    HBufC*          iIspIPAddr; // phone IP
    HBufC*          iIspIPNameServer1;  // primary name server
    HBufC*          iIspIPNameServer2;  // secondary name server
    HBufC*          iIspDefaultTelNumber; // datacall number
    TUint32         iIspBearerCallTypeIsdn; // datacall type
    TUint32         iIspBearerSpeed;  // max connection speed
    HBufC*          iProxyServerAddress;  // proxy address
    TUint32         iProxyPortNumber; // proxy number
    
    void SetRandomCMNameL(CCreatorEngine& aEngine);
    void SetRandomLoginNameL(CCreatorEngine& aEngine);
    void SetRandomLoginPassL(CCreatorEngine& aEngine);
    void SetRandomSecureAuthenticationL(CCreatorEngine& aEngine);
    void SetRandomPromptPasswordL(CCreatorEngine& aEngine);
    void SetRandomProxyAddressL(CCreatorEngine& aEngine);
    void SetRandomProxyPortL(CCreatorEngine& aEngine);
    void SetRandomStartPageL(CCreatorEngine& aEngine);
    void SetRandomIPAddressL(CCreatorEngine& aEngine);
    void SetRandomIP4NameServer1L(CCreatorEngine& aEngine);
    void SetRandomIP4NameServer2L(CCreatorEngine& aEngine);
    void SetRandomTelephoneNumberL(CCreatorEngine& aEngine);
    void SetRandomBearerTypeIsdnL(CCreatorEngine& aEngine);
    void SetRandomBearerSpeedL(CCreatorEngine& aEngine);
    void SetRandomWapWspOptionL(CCreatorEngine& aEngine);
    void SetRandomGatewayAddressL(CCreatorEngine& aEngine);
    

public:
    CAccessPointsParameters();
    ~CAccessPointsParameters();
    };



#endif // __CREATORACCESSPOINTS_H__
