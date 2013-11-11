/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "crealfactory.h"
#include "cimageprintengine.h"
#include "cdiscoverymanager.h"
#include "cprintjobmanager.h"
#include "cprinteventcatcher.h"
#include "cidleprinteventcatcher.h"
#include "csettingsmanager.h"
#include "imageprint.h"
#include "cimageprint.h"
#include "clog.h"

const TInt KMaxServerConnections( 3 );

// CONSTRUCTION
CRealFactory* CRealFactory::NewL(
    CImagePrintEngine* aEngine )
    {
    CRealFactory* self = new ( ELeave ) CRealFactory( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Default constructor
CRealFactory::CRealFactory( CImagePrintEngine* aEngine )
    : CIFFactory( aEngine )
    {
    }

// Destructor
CRealFactory::~CRealFactory()
    {
    LOG("CRealFactory::~CRealFactory BEGIN");
	if ( iEventCatcher )
		{
		iEventCatcher->UnRegisterObserver( iPrintJobMgr );
		}
    delete iPrintJobMgr;
    delete iDiscoveryMgr;
    delete iSettingsMgr;
    delete iEventCatcher;
    delete iIdleEventCatcher;

    if ( iImageServer )
    	{
	    iImageServer->Close();
	    delete iImageServer;
    	}

    iImagePrintClient.Close();
    LOG("CRealFactory::~CRealFactory END");
    }

// 2nd phase constructor
void CRealFactory::ConstructL()
    {
    iImageServer = new ( ELeave ) CImagePrint;
    iImageServer->ConnectL();

    iEventCatcher = CPrintEventCatcher::NewL( this, iPrintEngine );
    iIdleEventCatcher = CIdlePrintEventCatcher::NewL( this /*aIdleObserver*/ );
    iDiscoveryMgr = CDiscoveryManager::NewL( this );
    iPrintJobMgr = CPrintJobManager::NewL( this, iPrintEngine );
    iSettingsMgr = CSettingsManager::NewL( this );

    iEventCatcher->RegisterObserver( iPrintJobMgr );
    iImageServer->RegisterIdleObserver( iIdleEventCatcher );

    User::LeaveIfError( iImagePrintClient.ConnectL() );
    }

// Discovery interface
MDiscovery* CRealFactory::DiscoveryIF()
    {
    return iDiscoveryMgr;
    }

// Settings interface
MPrintSettings* CRealFactory::SettingsIF()
    {
    return iSettingsMgr;
    }

// Print job interface
MPrintJob* CRealFactory::PrintJobIF()
    {
    return iPrintJobMgr;
    }

// Returns event catcher
MPrintEventObserver* CRealFactory::PrintEventObserverIF()
    {
    return iEventCatcher;
    }

MPrintEventRegisterIF* CRealFactory::PrintEventRegisterIF()
    {
    return iEventCatcher;
    }

// Restarts Engine
void CRealFactory::RestartEngine()
    {
    // No implementation
    }

// Returns Engine
CImagePrint* CRealFactory::Engine()
    {
    return iImageServer;
    }

TBool CRealFactory::IsPrinting() const
    {
    return iPrintJobMgr->IsPrinting();
    }


MIdlePrintEventRegisterIF* CRealFactory::IdlePrintEventRegisterIF()
    {
    return iIdleEventCatcher;
    }

RImagePrintClient CRealFactory::ImagePrintClient()
    {
    return iImagePrintClient;
    }

TBool CRealFactory::IsApplicationAlreadyInUse()
    {
    TBool retValue = EFalse;
    TInt serverConnections = 0;
    TInt err = iImagePrintClient.CountConnections( serverConnections );
    if( err != KErrNone )
        {
        return EFalse;
        }
    
    // if server has already two connections
    if( serverConnections == KMaxServerConnections )
        {
        //if another connection IS NOT pictbridge
        TInt mode = iImagePrintClient.IsPictBridgeMode();
        if( mode < 0 )
            {
            retValue = EFalse;
            }
        }
    // too many connection
    if( serverConnections > KMaxServerConnections )
        {
        retValue = EFalse;
        }
    return retValue;
    }

//  End of File
