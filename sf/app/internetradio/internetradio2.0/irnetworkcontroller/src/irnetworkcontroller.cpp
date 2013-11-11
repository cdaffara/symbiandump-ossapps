/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <centralrepository.h>
#include <commsdattypesv1_1.h>
#include <commdb.h>
#include <commdbconnpref.h>
#include <featdiscovery.h>
#include <features.hrh>
#include <ProfileEngineSDKCRKeys.h>
#include <httpstringconstants.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <extendedconnpref.h>

#include "irdebug.h"
#include "irfilteredapreader.h"
#include "irnetworkcontroller.h"
#include "irnetworkobserver.h"
#include "irsettings.h"
#include "iractivenetworkobserver.h"

const TInt KMaxIRUAProfLength = 250; // Max length of the UAProf string
const TInt KIRDefaultUAProfBufferSize = 0x80;
const TUint KIRESockMessageSlots = 16;
const TInt KArraySize = 5;
const TInt KWlanStringMaxLength = 9;
const TInt KTwo = 2;

// ---------------------------------------------------------------------------
//  define in WebUtilsSDKCRKeys.h
// ---------------------------------------------------------------------------
//
const TUid KCRUidWebUtils = {0x101F8731};//Web APIs removal and Internet Radio build break

// ---------------------------------------------------------------------------
// define in WebUtilsInternalCRKeys.h
// ---------------------------------------------------------------------------
//
// Compatability fragment of useragent string
const TUint32 KWebUtilsUsrAg2 =	0x00000001;

// PlatformVersion fragment of useragent string
const TUint32 KWebUtilsUsrAg3 =	0x00000002;

// MidPVersion fragment of useragent string
const TUint32 KWebUtilsUsrAg4 =	0x00000003;

// CLDConfiguration fragment of useragent string
const TUint32 KWebUtilsUsrAg5 =	0x00000004;

// Url to useragent profile
const TUint32 KWebUtilsUaProf =	0x00000008;
// To Format the UAProf string
_LIT( KIRUAProfSpace," " );
// WLan ( generic ) name string
_LIT( KIRWLanName,"Easy WLAN" );


// -----------------------------------------------------------------------------------------------
//  Creates an Instance of CIRNetworkController
// -----------------------------------------------------------------------------------------------
//
EXPORT_C CIRNetworkController* CIRNetworkController::OpenL( MIRNetworkController* aObserver )
    {
    IRLOG_DEBUG( "CIRNetworkController::OpenL - Entering" );
    CIRNetworkController* networkController = reinterpret_cast<CIRNetworkController*>( Dll::Tls() );

    if ( !networkController )
        {
        networkController = new ( ELeave ) CIRNetworkController;
        CleanupClosePushL( *networkController );
        networkController->iSingletonInstances = 1;
        networkController->ConstructL( aObserver );
        User::LeaveIfError( Dll::SetTls( networkController ) );
        networkController->iSingletonInstances = 0;
        CleanupStack::Pop( networkController );
        }
    networkController->iSingletonInstances++;
    if ( NULL != aObserver )
        {
        networkController->iObserver = aObserver;
        networkController->iIRNetworkObserver->SetObserver( aObserver );
        }
    IRLOG_DEBUG( "CIRNetworkController::OpenL - Exiting." );
    return networkController;
    }

// -----------------------------------------------------------------------------------------------
// Conditionally destroys the IRNetworkController object
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::Close()
    {
    IRLOG_DEBUG2( "CIRNetworkController::Close - singleton instances=%d", iSingletonInstances );
    iSingletonInstances--;

    if ( iSingletonInstances == 0 )
        {
        delete this;
        }
    IRLOG_DEBUG( "CIRNetworkController::Close - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::GetIRConnection
//  Returns the instance of RConnection
//  The same RConnection object is used across the entire iRAPP components to
//  provide central arbiter and monitoring of Network Connection
// -----------------------------------------------------------------------------------------------
//
EXPORT_C RConnection& CIRNetworkController::GetIRConnection()
    {
    IRLOG_DEBUG( "CIRNetworkController::GetIRConnection - Entering" );
    // If the RConnection is open return the reference
    switch ( iNetworkConnectionState )
        {
        case EIRNetworkConnectionActive:
            {
            // Connection is active and ready to use return the reference
            return ( *iIRNetworkConnection );
            }
        case EIRNetworkConnectionInActive:
            {
            // Connection is not active and application cannot continue hence panic
            _LIT( KComponentName,"NetworkController" );
            User::Panic( KComponentName,KErrCouldNotConnect );
            }
        break;
        default:
            {
            // no implementation
            }
        break;
        }
    // Inserted to remove the compiler warning
    IRLOG_DEBUG( "CIRNetworkController::GetIRConnection - Exiting." );
    return ( *iIRNetworkConnection );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::GetIRSocketServer
//  Returns the instance of RSocketServ
//  The same RSocketServ object is used across the entire iRAPP components to
//  provide central arbiter and monitoring of Network Connection
// -----------------------------------------------------------------------------------------------
//
EXPORT_C RSocketServ& CIRNetworkController::GetIRSocketServer()
    {
    IRLOG_DEBUG( "CIRNetworkController::GetIRSocketServer - Entering" );
    switch ( iSocketServerConnectionState )
        {
        case EIRSocketServerActive:
            {
            // Connection to Socket Server is active hence return the reference
            return iIRSocketServer;
            }
        case EIRSocketServerInActive:
            {
            // Connection to Socket Server not open hence open and return the reference
            // Connection is not active and application cannot continue hence panic
            _LIT( KComponentName,"NetworkController" );
            User::Panic( KComponentName,KErrCouldNotConnect );
            }
        break;
        default:
        {
            // no implemenatation
        }
        break;
        }
    // Inserted to remove the compiler warning
    IRLOG_DEBUG( "CIRNetworkController::GetIRSocketServer - Exiting." );
    return iIRSocketServer;
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::GetWapProfString
//  Returns a HBufC pointer to the x-wap-profile string
//  Transfers the ownership of the returned HBufC8 object
//  The caller must takecare of deleting the returned object
// -----------------------------------------------------------------------------------------------
//
EXPORT_C HBufC8* CIRNetworkController::GetWapProfString()
    {
    IRLOG_DEBUG( "CIRNetworkController::GetWapProfString" );
    return iIRWapProf;
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::GetUAProfString
//  Returns a HBufC pointer to the UAProf string
//  Transfers the ownership of the returned HBufC8 object
//  The caller must takecare of deleting the returned object
// -----------------------------------------------------------------------------------------------
//
EXPORT_C HBufC8* CIRNetworkController::GetUAProfString()
    {
    IRLOG_DEBUG( "CIRNetworkController::GetUAProfString" );
    return iIRUAProf;
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::GetNetworkStatus
//  Returns the variable which indicates if connection is active or not
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TBool CIRNetworkController::GetNetworkStatus() const
    {
    IRLOG_DEBUG( "CIRNetworkController::GetNetworkStatus" );
    return iIsConnectedToNetwork;
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::GetIAPId
// Gets the IAP Id of the chosen IAP
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TInt CIRNetworkController::GetIAPId( TUint32& aIapId ) const
    {
    IRLOG_DEBUG( "CIRNetworkController::GetIAPId - Entering" );
    TInt retVal( KErrNone );
    if ( iIsConnectedToNetwork )
        {
        if ( iIRNetworkObserver->iIsIAPIdAvailable )
            {
            aIapId = iIRNetworkObserver->iIAPId;
            }
        }
    else
        {
        retVal = KErrNotFound;
        }
    IRLOG_DEBUG( "CIRNetworkController::GetIAPId - Exiting." );
    return retVal;
    }

// ---------------------------------------------------------------------------
//  CIRNetworkController::ChooseAccessPointL
//  Configures the Access Point which is used by all the components for network
//  connectivity
// ---------------------------------------------------------------------------
//


EXPORT_C void CIRNetworkController::ChooseAccessPointL(TBool aDefaultConnection)
    {
    IRLOG_DEBUG( "CIRNetworkController::ChooseAccessPointL - Entering" );
    if (iHandingOver)
        {
        if (iObserver)
            {
            iObserver->IRNetworkEventL(ENetworkConnectionConnecting);
            }
        IRLOG_DEBUG("CIRNetworkController::ChooseAccessPointL, ALR is handing over, Exiting");
        return;
        }
    
    iIRNetworkObserver->SetNetworkMonitoring( ETrue );
    iDefaultConnection = aDefaultConnection;
    
    // Always validate the Access Points status
    if ( ValidateAccessPointsL() )
        { 
        if ( NULL != iMobility )
		    {
            iMobility->Cancel();
            delete iMobility;
            iMobility = NULL;
            }
            // Connect to the Symbian Socket Server
        iIRNetworkConnection->Close();
        iIRSocketServer.Close();
        TInt ReturnErrorCode = iIRSocketServer.Connect( KIRESockMessageSlots );

        if(ReturnErrorCode != KErrNone )
            {
            // Error in opening the connection to SocketServer
            iSocketServerConnectionState = EIRSocketServerInActive;
            IRLOG_DEBUG( "CIRNetworkController::ChooseAccessPointL - Exiting ( 1 )." );
            return ;
            }
        
        // Connection to RSocketServ is sucessful
        iSocketServerConnectionState = EIRSocketServerActive;

        // Open the RConnection over the iIRSocketServer
        ReturnErrorCode = iIRNetworkConnection->Open(iIRSocketServer);

        if( ReturnErrorCode != KErrNone )
            {
            // Error in opening the connection
            iNetworkConnectionState = EIRNetworkConnectionInActive;
            IRLOG_DEBUG( "CIRNetworkController::ChooseAccessPointL - Exiting ( 2 )." );
            return ;
            }

        //Added for ALR/SNAP
        if ( !IsActive() )
            {
            if (iDefaultConnection)
                {
                TConnPrefList prefList;
                TExtendedConnPref extPrefs;

                extPrefs.SetSnapPurpose(CMManager::ESnapPurposeInternet);
                extPrefs.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnDisableNotes);
                prefList.AppendL(&extPrefs);
                iIRNetworkConnection->Start(prefList, iStatus);
                }
            else
                {
                TCommDbConnPref connPref;
                connPref.SetDialogPreference(ECommDbDialogPrefPrompt);
                iIRNetworkConnection->Start(connPref, iStatus) ;
                }
            
            // Set the NetworkController state
            iNetworkControllerState = EConnectingToNetwork;

            SetActive();
            iIsConnectRequestIssued = ETrue;
            if ( iObserver )
                {
                IRLOG_DEBUG( "CIRNetworkController::ChooseAccessPointL - notify ENetworkConnectionConnecting" );
                iObserver->IRNetworkEventL( ENetworkConnectionConnecting );
                }
            }
        IRLOG_DEBUG( "CIRNetworkController::ChooseAccessPointL - Exiting ( 3 )." );
        return ;
        }
    else
        {
        iObserver->IRNetworkEventL(EAccessPointSelectionCancelled );
        }
    IRLOG_DEBUG( "CIRNetworkController::ChooseAccessPointL - Exiting ( 4 )." );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::CancelConnecting
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::CancelConnecting()
    {
    IRLOG_DEBUG("CIRNetworkController::CancelConnecting(), Entering");
    iIRNetworkObserver->Cancel();
    Cancel();
    ResetConnectionStatus();
    IRLOG_DEBUG("CIRNetworkController::CancelConnecting(), Exiting");
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::IsOfflineMode
//  This api is used to determine if the phone is in offline mode
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TBool CIRNetworkController::IsOfflineMode()
    {
    IRLOG_DEBUG( "CIRNetworkController::IsOfflineMode" );
    // System defined value for offline mode is 5
    const TInt KOfflineMode = 5;
    TInt returnValue = 0;
    returnValue = DetermineCurrentProfile();
    if ( returnValue == KOfflineMode )
        {
        IRLOG_DEBUG( "CIRNetworkController::IsOfflineMode - Exiting ( 1 )." );
        return ETrue;            
        }
    IRLOG_DEBUG( "CIRNetworkController::IsOfflineMode - Exiting ( 2 )." );
    return EFalse;    
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::IsWlanSupported
//  This api is used to determine if the phone supports WLan usage
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TBool CIRNetworkController::IsWlanSupported() const
    {
    IRLOG_DEBUG( "CIRNetworkController::IsWlanSupported" );
    return iIsWlanSupported;
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::ResetConnectionStatus
//  Reset the connection status to Disconnected statet
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::ResetConnectionStatus()
    {
    IRLOG_DEBUG( "CIRNetworkController::ResetConnectionStatus - Entering" );
    iIsConnectRequestIssued = EFalse;
    iNetworkConnectionState = EIRNetworkConnectionInActive;
    iIsConnectedToNetwork = EFalse;
    iConnectionPresent = EFalse;
    IRLOG_DEBUG( "CIRNetworkController::ResetConnectionStatus - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::IdentifyConnectionType
// Used to determine the type of connection
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TIRConnectionType CIRNetworkController::IdentifyConnectionType() const
    {
    IRLOG_DEBUG( "CIRNetworkController::IdentifyConnectionType" );
    IRRDEBUG2( "CIRNetworkController::IdentifyConnectionType - Entering iConnectionType = %d", iIRNetworkObserver->iIRConnectionType );

    return iIRNetworkObserver->iIRConnectionType;
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::DetermineCurrentProfile
// Used to determine the current profile
// -----------------------------------------------------------------------------------------------
//
TInt CIRNetworkController::DetermineCurrentProfile() const
    {
    IRLOG_DEBUG( "CIRNetworkController::DetermineCurrentProfile" );
    TInt value = KErrNone;

    TRAPD( err,
    CRepository* cRepositoryHandle = CRepository::NewLC( KCRUidProfileEngine );
    cRepositoryHandle->Get( KProEngActiveProfile,value );
    CleanupStack::PopAndDestroy( cRepositoryHandle ); )
    if ( err != KErrNone )
        {
        value = err;
        }

    IRLOG_DEBUG( "CIRNetworkController::DetermineCurrentProfile - Exiting." );
    return value;
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::CheckFeatureL
// Pops up the access point list.
// -----------------------------------------------------------------------------------------------
//
TBool CIRNetworkController::CheckFeatureL( TInt aFeatureId ) const
    {
    IRLOG_DEBUG( "CIRNetworkController::CheckFeatureL" );
    return CFeatureDiscovery::IsFeatureSupportedL( aFeatureId );
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::CIRNetworkController
// Default C++ Constructor
// -----------------------------------------------------------------------------------------------
//
CIRNetworkController::CIRNetworkController(): CActive(
    CActive::EPriorityHigh )
    {
    // Add the AO to the ActiveScheduler
    IRLOG_DEBUG( "CIRNetworkController::CIRNetworkController- Entering" );
    CActiveScheduler::Add( this );
    // Initialize the states
    iNetworkConnectionState = EIRNetworkConnectionInActive;
    iSocketServerConnectionState = EIRSocketServerInActive;
    iNetworkControllerState = EInActive;
    

    IRLOG_DEBUG( "CIRNetworkController::CIRNetworkController - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::~CIRNetworkController
//  Default C++ Destructor
// -----------------------------------------------------------------------------------------------
//
CIRNetworkController::~CIRNetworkController()
    {
    IRLOG_DEBUG( "CIRNetworkController::~CIRNetworkController- Entering" );
    _LIT( KErrorMsg,"Method Close not called" );
    __ASSERT_ALWAYS( iSingletonInstances == 0, User::Panic( KErrorMsg, KErrCorrupt ) );

    if ( NULL != iMobility )
        {
        iMobility->Cancel();
        delete iMobility;
        iMobility = NULL;
        }

    if ( IsActive() )
        {
        Cancel();
        }
    if ( iIRNetworkConnection )
        {
        iIRNetworkConnection->Close();
        delete iIRNetworkConnection;
        }

    // Set all the state variables to indicate network connection is closed
    delete iIRUAProf;
    delete iIRWapProf;
    if ( iIRSettings )
        {
        iIRSettings->Close();
        iIRSettings = NULL;
        }
    delete iIRNetworkObserver;
    iIRSocketServer.Close();

    if ( iIapList )
        {
        iIapList->Reset();
        delete iIapList;
        iIapList = NULL;
        }

    if ( iDataTransferTracker )
        {
        delete iDataTransferTracker;
        iDataTransferTracker = NULL;
        }
    if ( iLogoDataTransferTracker )
        {
        delete iLogoDataTransferTracker;
        iLogoDataTransferTracker = NULL;
        }
    iObserver = NULL;
    
    iActiveNetworkObserverArray.Close();

    Dll::FreeTls();
    IRLOG_DEBUG( "CIRNetworkController::~CIRNetworkController - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::ConstructL
//  Second Phase construction.
//    aObserver pointer to the observer class if an observer is needed
// -----------------------------------------------------------------------------------------------
//
void CIRNetworkController::ConstructL( MIRNetworkController* aObserver )
    {
    IRLOG_DEBUG( "CIRNetworkController::ConstructL- Entering" );
    iObserver = aObserver;
    iIRNetworkObserver = CIRNetworkObserver::NewL(this);
    iIRNetworkObserver->SetObserver( iObserver );

    // Create instance of DataTransferTracker ( Byte Counter Impl )
    iDataTransferTracker = CIRDataTransferTracker::NewL();
    iLogoDataTransferTracker = CIRDataTransferTracker::NewL();

    iIRNetworkConnection = new ( ELeave ) RConnection;
    
    iIRSettings = CIRSettings::OpenL();

    BuildUAProfStringL();
    iIapList = new ( ELeave ) CDesCArrayFlat( KArraySize );

    QueryCommsForIAPL();

    IRLOG_DEBUG( "CIRNetworkController::ConstructL - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::QueryCommsForIAPL
//  Determines the available access points using CommsDat Api
//  voilates PCLint Error 40 --Undeclared identifier 'KFeatureIdProtocolWlan'
//  as defined in featureinfo.h
// -----------------------------------------------------------------------------------------------
//
void CIRNetworkController::QueryCommsForIAPL()
    {
    IRLOG_DEBUG( "CIRNetworkController::QueryCommsForIAPL- Entering" );

    // Reset all the comms info
    iIapList->Reset();
    
#ifndef __WINS__
    CIRFilteredApReader* filteredReader = CIRFilteredApReader::
    NewLC( CIRFilteredApReader::KIRFilterWAPOnly |
            CIRFilteredApReader::KIRFilterWAPMandatory |
            CIRFilteredApReader::KIRFilterEasyWLAN );

    CCDIAPRecord* iapRecord = filteredReader->FirstRecordL();

    while ( iapRecord )
    {
        CleanupStack::PushL( iapRecord );

        iIapList->AppendL( iapRecord->iRecordName.GetL() );

        CleanupStack::PopAndDestroy( iapRecord );
        iapRecord = filteredReader->NextRecordL();
    }
    CleanupStack::PopAndDestroy( filteredReader );
#else
    CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
    CleanupStack::PushL(dbSession);

    CMDBRecordSet<CCDIAPRecord>* iapSet = new (ELeave) CMDBRecordSet<
            CCDIAPRecord> (KCDTIdIAPRecord);
    CleanupStack::PushL(iapSet);

    TRAP_IGNORE(iapSet->LoadL(*dbSession));
    
    for ( TInt i = 0; i < iapSet->iRecords.Count(); i++ )
        {
        CCDIAPRecord* iapRecord =
                static_cast<CCDIAPRecord*> (iapSet->iRecords[i]);
        iIapList->AppendL(iapRecord->iRecordName.GetL());
        }
    CleanupStack::PopAndDestroy(2, dbSession);
#endif
    
    // If WLan is supported on the device then add the WLan option
    // to the IAP List
#ifndef __WINS__
    iIsWlanSupported = CheckFeatureL( KFeatureIdProtocolWlan );
    if ( iIsWlanSupported )
        {
        TBuf<KWlanStringMaxLength> wlanString;
        wlanString.Copy( KIRWLanName );
        iIapList->AppendL( wlanString );
        }
#endif

    iIapList->Compress();

    IRLOG_DEBUG( "CIRNetworkController::QueryCommsForIAPL - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::BuildUAProfStringL
// Queries the system and extracts the UAProf information
//  Used by IRDataProvider and IRStreamSource
// -----------------------------------------------------------------------------------------------
//
void CIRNetworkController::BuildUAProfStringL()
    {
    IRLOG_DEBUG( "CIRNetworkController::BuildUAProfStringL- Entering" );
    // Create space on heap for the UAProf String
    iIRUAProf = HBufC8::NewL( KMaxIRUAProfLength );
    iIRWapProf = HBufC8::NewL( KMaxIRUAProfLength );
    HBufC* cenRepPtr;
    TPtr8 irUAProf = iIRUAProf->Des();
    TPtr8 irWapProf = iIRWapProf->Des();
    irUAProf.Append( iIRSettings->GetIrappVersionL() );

    cenRepPtr = NULL;
    irUAProf.Append( KIRUAProfSpace );
    // extract UAProf sub-string from cenrep
    cenRepPtr = CentralRepositoryStringValueL( KCRUidWebUtils,KWebUtilsUsrAg3 );
    // append it to form the UAProf
    irUAProf.Append( cenRepPtr->Des() );
    delete cenRepPtr;
    cenRepPtr = NULL;

    irUAProf.Append( KIRUAProfSpace );
    // extract UAProf sub-string from cenrep
    cenRepPtr = CentralRepositoryStringValueL( KCRUidWebUtils,KWebUtilsUsrAg2 );
    // append it to form the UAProf
    irUAProf.Append( cenRepPtr->Des() );
    delete cenRepPtr;
    cenRepPtr = NULL;

    irUAProf.Append( KIRUAProfSpace );
    // extract UAProf sub-string from cenrep
    cenRepPtr = CentralRepositoryStringValueL( KCRUidWebUtils,KWebUtilsUsrAg4 );
    // append it to form the UAProf
    irUAProf.Append( cenRepPtr->Des() );
    delete cenRepPtr;
    cenRepPtr = NULL;

    irUAProf.Append( KIRUAProfSpace );
    // extract UAProf sub-string from cenrep
    cenRepPtr = CentralRepositoryStringValueL( KCRUidWebUtils,KWebUtilsUsrAg5 );
    // append it to form the UAProf
    irUAProf.Append( cenRepPtr->Des() );
    delete cenRepPtr;
    cenRepPtr = NULL;
    // trim out the quotes
    irUAProf.Delete( irUAProf.Length()-1,1 );
     // extract x-wap-profile string from cenrep
    cenRepPtr = CentralRepositoryStringValueL( KCRUidWebUtils,KWebUtilsUaProf );
    // append it to form the UAProf
    irWapProf.Copy( cenRepPtr->Des() );
    delete cenRepPtr;
    cenRepPtr = NULL;

    IRLOG_DEBUG( "CIRNetworkController::BuildUAProfStringL - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
// CIRNetworkController::CentralRepositoryStringValueL
// Retrievs the string from Central Repository for the specified key and cen rep uid
// Transfers the ownership of the returned HBufC object
// The caller must takecare of deleting the returned object*
// -----------------------------------------------------------------------------------------------
//
HBufC* CIRNetworkController::CentralRepositoryStringValueL(
    const TUid& aRepositoryUid, TUint32 aKey ) const
    {
    IRLOG_DEBUG( "CIRNetworkController::CentralRepositoryStringValueL- Entering" );
    CRepository* pRepository = CRepository::NewL( aRepositoryUid );
    CleanupStack::PushL( pRepository );

    TInt length = KIRDefaultUAProfBufferSize;
    HBufC* valueString = HBufC::NewLC( length );
    TInt error( KErrTooBig );
    do
        {
        TPtr ptr = valueString->Des();
        error = pRepository->Get( aKey, ptr );
        if ( error == KErrTooBig )
            {
            CleanupStack::PopAndDestroy( valueString );
            length = KTwo * length;
            valueString = HBufC::NewLC( length );
            }
        } while ( error == KErrTooBig );

    if ( error )
        {
        TPtr ptr = valueString->Des();
        ptr.Copy( KNullDesC() );
        }

    CleanupStack::Pop( valueString );
    CleanupStack::PopAndDestroy( pRepository );
    IRLOG_DEBUG( "CIRNetworkController::CentralRepositoryStringValueL - Exiting." );
    return valueString;
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::ValidateAccessPointsL(
//  Validates the access point availablity etc.
//  voilates PCLint Error 40 --Undeclared identifier 'KFeatureIdProtocolWlan'
//  as defined in featureinfo.h
// -----------------------------------------------------------------------------------------------
//
TBool CIRNetworkController::ValidateAccessPointsL()
    {
    IRLOG_DEBUG( "CIRNetworkController::ValidateAccessPointsL- Entering" );
    TBool status = ETrue;
    #ifndef __WINS__
        // Check if the phone is in offline mode
        // If yes, do not allow GPRS access
        // If phone is in offline mode and device does not support
        // WIFI then exit the application
        // Do not make this check in Wins
        //violates PC Lint error : Error 40: Undeclared identifier KFeatureIdProtocolWlan
        if ( IsOfflineMode() && !( CheckFeatureL( KFeatureIdProtocolWlan) ) )
            {
            // The phone is in offline mode and WLan is not available
            iObserver->IRNetworkEventL( EDisplayOfflineMode );
            status = EFalse;
            }
        // If phone is not in offline mode and no access points are defined
        // check if device has WIFI support then try to launch WIFI
        // selection if there is no support then display a message
        if ( !IsOfflineMode() && iIapList->MdcaCount() == 0 )
            {
            // Check if there is WIFI support
            //violates PC Lint error : Error 40: Undeclared identifier KFeatureIdProtocolWlan
            if ( !CheckFeatureL( KFeatureIdProtocolWlan ) )
                {
                iObserver->IRNetworkEventL( EDisplayNoAccessPointsDefined );
                status = EFalse;
                }
            }
    #endif
    IRLOG_DEBUG( "CIRNetworkController::ValidateAccessPointsL - Exiting." );
    return status;
    }

// Derived from CActive
// -----------------------------------------------------------------------------------------------
// CIRNetworkController::RunL
// The function is called by the active scheduler when a request completion event occurs,
// -----------------------------------------------------------------------------------------------
//
void CIRNetworkController::RunL()
    {
    IRLOG_INFO2( "CIRNetworkController::RunL - iStatus=%d", iStatus.Int() );
    iHandingOver = EFalse;
    
    TInt statusCode = iStatus.Int();

    if ( iStatus == KErrNone )
        {
        HandleRunLSuccessL();
    }
    else
        {
        HandleRunLErrorL( statusCode );
        }
    IRLOG_DEBUG( "CIRNetworkController::RunL - Exiting." );
    }

// -----------------------------------------------------------------------------------------------
//  CIRNetworkController::DoCancel()
//  Cancels the pending requests on the CIRNetworkController Active object
// -----------------------------------------------------------------------------------------------
//
void CIRNetworkController::DoCancel()
    {
    IRLOG_DEBUG( "CIRNetworkController::DoCancel - Entering" );
    if ( NULL != iMobility )
        {
        iMobility->Cancel();
        delete iMobility;
        iMobility = NULL;
        }
    iIRNetworkConnection->Stop();
    iIRNetworkConnection->Close();
    iIRSocketServer.Close();
    IRLOG_DEBUG( "CIRNetworkController::DoCancel - Exiting." );
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::InitializeHttpSession
// -----------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::InitializeHttpSessionL( const RHTTPSession& aHTTPSession, 
                                            MIRDataTransferTracker::TIRTransferCategory aCategory )
    {
    IRLOG_DEBUG( "CVRConnection::InitializeHttpSessionL - enter" );

    iDataTransferTracker->BindL( aHTTPSession, aCategory );

    RStringF sockserv = aHTTPSession.StringPool().StringF(
            HTTP::EHttpSocketServ, RHTTPSession::GetTable());
    CleanupClosePushL( sockserv );

    RStringF connection = aHTTPSession.StringPool().StringF(
            HTTP::EHttpSocketConnection, RHTTPSession::GetTable());
    CleanupClosePushL( connection );

    RHTTPConnectionInfo cInfo = aHTTPSession.ConnectionInfo();
    cInfo.SetPropertyL( sockserv, THTTPHdrVal( iIRSocketServer.Handle() ) );
    cInfo.SetPropertyL(connection, THTTPHdrVal(
            reinterpret_cast<TInt> (iIRNetworkConnection)));
    CleanupStack::PopAndDestroy( &connection ); // sockserv.close, connection.close
    CleanupStack::PopAndDestroy( &sockserv ); // sockserv.close, connection.close
    
    IRLOG_DEBUG( "CIRNetworkController::InitializeHttpSessionL - exit" );
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::RegisterLogoDataTransferTrackerL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::RegisterLogoDataTransferTrackerL( RHTTPSession& aHTTPSession )
    {
    IRLOG_DEBUG( "CVRConnection::InitializeHttpSessionL - enter" );

    iLogoDataTransferTracker->BindL(aHTTPSession,
            MIRDataTransferTracker::EIRTransferCategoryIsds);

    RStringF sockserv = aHTTPSession.StringPool().StringF(
            HTTP::EHttpSocketServ, RHTTPSession::GetTable());
    CleanupClosePushL( sockserv );

    RStringF connection = aHTTPSession.StringPool().StringF(
            HTTP::EHttpSocketConnection, RHTTPSession::GetTable());
    CleanupClosePushL( connection );

    RHTTPConnectionInfo cInfo = aHTTPSession.ConnectionInfo();
    cInfo.SetPropertyL( sockserv, THTTPHdrVal( iIRSocketServer.Handle() ) );
    cInfo.SetPropertyL(connection, THTTPHdrVal(
            reinterpret_cast<TInt> (iIRNetworkConnection)));
    CleanupStack::PopAndDestroy( &connection ); // sockserv.close, connection.close
    CleanupStack::PopAndDestroy( &sockserv ); // sockserv.close, connection.close    

    // Subscribe to logo byte counter info
    iLogoDataTransferTracker->SetObserver( this );

    IRLOG_DEBUG( "CIRNetworkController::RegisterLogoDataTransferTrackerL - exit" );
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::DataTransferTracker()
// -----------------------------------------------------------------------------
//
EXPORT_C MIRDataTransferTracker& CIRNetworkController::DataTransferTracker()
    {
    IRLOG_DEBUG( "CIRNetworkController::DataTransferTracker" );
    return *iDataTransferTracker;
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::LogoDataTransferTracker()
// -----------------------------------------------------------------------------
//
EXPORT_C MIRDataTransferTracker& CIRNetworkController::LogoDataTransferTracker()
    {
    IRLOG_DEBUG( "CIRNetworkController::LogoDataTransferTracker" );
    return *iLogoDataTransferTracker;
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::LogoDataTransferTracker()
// -----------------------------------------------------------------------------
//
void CIRNetworkController::HandleDataTransferEventL( 
        const MIRDataTransferTracker::TIRDataTransferPckg& aData )
    {
    IRLOG_DEBUG( "CIRNetworkController::HandleDataTransferEventL - Entering" );
    iDataTransferTracker->RawDataTransferredL(aData.iBytesSentTotal,
            aData.iBytesReceivedTotal,
                                        MIRDataTransferTracker::EIRTransferCategoryIsds );
    IRLOG_DEBUG( "CIRNetworkController::HandleDataTransferEventL - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::PreferredCarrierAvailable()
// -----------------------------------------------------------------------------
//
void CIRNetworkController::PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                            TAccessPointInfo aNewAPInfo,
                                            TBool aIsUpgrade,
                                            TBool aIsSeamless )
    {
    IRLOG_DEBUG("CIRNetworkController::PreferredCarrierAvailable - Entering");

    (void)aOldAPInfo;
    (void)aNewAPInfo;
    (void)aIsUpgrade;
    IRLOG_DEBUG5("CIRNetworkController::PreferredCarrierAvailable, old ap : %d, new ap : %d, aIsUpgrade : %d, aIsSeamless : %d", 
                 aOldAPInfo.AccessPoint(), aNewAPInfo.AccessPoint(), aIsUpgrade, aIsSeamless);
    
    if (aIsSeamless)
        {
        // It is Seamless. E.g. Mobile IP enabled.
        }
    else
        {
        // sockets used by the connection should be closed here.

        // We ask to migrate to the Preferred Carrier.
        if (!IsOfflineMode())
            {
            //handling over connection may take some time, during handling over connection,
            //application should be in disconnected state
            TRAP_IGNORE( iObserver->IRNetworkEventL(ENetworkConnectionDisconnected ); )
            
            iIRNetworkObserver->SetNetworkMonitoring(EFalse);
            
            iMobility->MigrateToPreferredCarrier();
            iHandingOver = ETrue;
            }
        }

    IRLOG_DEBUG("CIRNetworkController::PreferredCarrierAvailable - Exiting");
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::NewCarrierActive()
// -----------------------------------------------------------------------------
//
void CIRNetworkController::NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless )
    {
    IRLOG_DEBUG("CIRNetworkController::NewCarrierActive - Entering");
    (void)aNewAPInfo;
    IRLOG_DEBUG3("CIRNetworkController::NewCarrierActive, new ap : %d, aIsSeamless : %d",
                 aNewAPInfo.AccessPoint(), aIsSeamless);
    
    if (aIsSeamless)
        {
        // It is Seamless. E.g. Mobile IP enabled.
        }
    else
        {
        // sockets used by the connection should be re-opened here.
        // We accept the new IAP.

        iMobility->NewCarrierAccepted();

        iIsConnectRequestIssued = EFalse;
        iNetworkConnectionState = EIRNetworkConnectionActive;
        iIsConnectedToNetwork = ETrue;
        iConnectionPresent = ETrue;
        
        iIRNetworkObserver->SetNetworkMonitoring(ETrue);
        iIRNetworkObserver->InitializeNetworkObserver();    
        }

    IRLOG_DEBUG("CIRNetworkController::NewCarrierActive - Exiting");
    }

// -----------------------------------------------------------------------------
// CIRNetworkController::Error()
// -----------------------------------------------------------------------------
//
void CIRNetworkController::Error( TInt /*aError*/ )
    {
    IRLOG_DEBUG( "CIRNetworkController::Error" );
    // Does nothing for the moment
    }
    
// -----------------------------------------------------------------------------
// HandleRunLSuccessL()
// Handles the success case of Network connection in RunL
// -------------------------------------------------------------------------------------------------
//
void CIRNetworkController::HandleRunLSuccessL()
    {
    IRLOG_DEBUG( "CIRNetworkController::HandleRunLSuccessL - Entering" );
        
    switch ( iNetworkControllerState )
        {
        case EConnectingToNetwork:
            {
            iIsConnectedToNetwork = ETrue;
            // Connection to network sucessful
            iNetworkConnectionState = EIRNetworkConnectionActive;
            iConnectionPresent = ETrue;
            iIsConnectRequestIssued = EFalse;
            iIRNetworkObserver->InitializeNetworkObserver();
            }
            break;
        default:
            {
            // no implementation
            }
            break;
        }

    if ( !iMobility  )
        {
        iMobility = CActiveCommsMobilityApiExt::NewL( *iIRNetworkConnection,
                *this );
        }
    IRLOG_DEBUG( "CIRNetworkController::HandleRunLSuccessL - Exiting" );
    }

// -------------------------------------------------------------------------------------------------
// HandleRunLErrorL()
// Handles the error case of Network connection in RunL
// -------------------------------------------------------------------------------------------------
//
void CIRNetworkController::HandleRunLErrorL( TInt aStatusCode )
    {
    IRLOG_DEBUG( "CIRNetworkController::HandleRunLErrorL - Entering" );
    
    switch ( iNetworkControllerState )
        {
        case EConnectingToNetwork:
            {
            // Connection to network failure
            iNetworkConnectionState = EIRNetworkConnectionInActive;
            iIsConnectedToNetwork = EFalse;
            iConnectionPresent = EFalse;
            iIsConnectRequestIssued = EFalse;
            if ( aStatusCode != KErrCancel )
                {
                if (iDefaultConnection)
                    {
                    //give user the second chance to select access point manually
                    ChooseAccessPointL(EFalse);
                    }
                else
                    {
                    iObserver->IRNetworkEventL(EDisplayNetworkMessageNoConnectivity);
                    }
                }
            else
                {
                iObserver->IRNetworkEventL( EAccessPointSelectionCancelled );
                ResetPendingRequests( EFalse );
                }
            }
            break;
        
        default:
            {
            // no implementation
            }
            break;
        }

    IRLOG_DEBUG( "CIRNetworkController::HandleRunLErrorL - Exiting" );
    }

// -------------------------------------------------------------------------------------------------
// Registers the observer that will be notified for 
// a network connection.
// Observer requires notification to reissue pending request
// -------------------------------------------------------------------------------------------------
//  
EXPORT_C void CIRNetworkController::RegisterActiveNetworkObserverL( MIRActiveNetworkObserver&
                                                                     aActiveNetworkObserver )
    {
    IRLOG_DEBUG( "CIRNetworkController::RegisterActiveNetworkObserverL - Entering" );
    iActiveNetworkObserverArray.AppendL( &aActiveNetworkObserver );
    IRLOG_DEBUG( "CIRNetworkController::RegisterActiveNetworkObserverL - Exiting" );
    }

// -----------------------------------------------------------------------------------------------
// DeleteActiveNetworkObserver()
// Used to remove an observer for roaming events from the observer array
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::DeleteActiveNetworkObserver( MIRActiveNetworkObserver&
                                                                 aActiveNetworkObserver )
    {
    IRLOG_DEBUG( "CIRNetworkController::DeleteActiveNetworkObserver - Entering" );
    TInt index = iActiveNetworkObserverArray.Find( &aActiveNetworkObserver );
    
    if ( index != KErrNotFound )
        {
        iActiveNetworkObserverArray.Remove( index );    
        }

    IRLOG_DEBUG( "CIRNetworkController::DeleteActiveNetworkObserver - Exiting" );
    }
    
// -----------------------------------------------------------------------------------------------
// NotifyActiveNetworkObserversL()
// Used to notify all observers for network events about a change in network event
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void CIRNetworkController::NotifyActiveNetworkObserversL( TIRNetworkEvent aEvent )
    {
    IRLOG_DEBUG( "CIRNetworkController::NotifyActiveNetworkObserversL - Entering" );

    for ( TInt i=iActiveNetworkObserverArray.Count()-1; i>=0; i-- )
        {
        iActiveNetworkObserverArray[i]->NotifyActiveNetworkObserversL( aEvent );    
        }
        

    IRLOG_DEBUG( "CIRNetworkController::NotifyActiveNetworkObserversL - Exiting" );
    }

// -------------------------------------------------------------------------------------------------
// IsHandingOverConnection()
// Indicates if the Hand over of Network connection has happened
// -------------------------------------------------------------------------------------------------
//
EXPORT_C TBool CIRNetworkController::IsHandingOverConnection()
    {
	  IRLOG_DEBUG("CIRNetworkController::IsHandingOverConnection - Entering");
    IRLOG_DEBUG("CIRNetworkController::IsHandingOverConnection - Exiting");
    return iHandingOver;
    }

// -------------------------------------------------------------------------------------------------
// Reset member iHandingOver to EFalse.
// -------------------------------------------------------------------------------------------------
//
void CIRNetworkController::ResetHandingOverConnection()
    {
    IRLOG_DEBUG("CIRNetworkController::ResetHandingOverConnection(), Entering");
    iHandingOver = EFalse;
    IRLOG_DEBUG("CIRNetworkController::ResetHandingOverConnection(), Exiting");
    }

// -------------------------------------------------------------------------------------------------
// Notifies observers when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -------------------------------------------------------------------------------------------------
//
void CIRNetworkController::ResetPendingRequests( TBool aValue )
    {
    for ( TInt i=0; i<iActiveNetworkObserverArray.Count(); i++ )
        {
        iActiveNetworkObserverArray[i]->ResetPendingRequests( aValue );    
        }
    }
