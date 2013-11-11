/*
 * Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: CSocketsEngine implements an engine for connection
 * component checking: Interface opening/closing, connecting,
 * sending/receiving data, etc.
 * CSocketsEngine is an active object
 *
 */

// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_partner.h>
#include <es_enum_internal.h>
#endif
#include <eikgted.h>
#include <in_sock.h>
#include <e32svr.h>
#include <es_enum.h>
#include <commdb.h>
#include <in_iface.h>
#include <apgcli.h>
#include <apaid.h>

#include "SocketsEngine.h"
#include "TimeOutTimer.h"
#include "SocketsRead.h"
#include "SocketsWrite.h"
#include "ProgressNotifier.h"
#include "ConnTest.pan"
#include "uinotify.h"
#include "HttpHandler.h"
#include "SettingData.h"
#include "CustomPrefsData.h"
#include "Utils.h"
#include "SendTimer.h"
#include "alractiveobject.h"

#include <hal.h>
#include <hal_data.h>

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmapplicationsettingsui.h>

// CONSTANTS
const TInt KTimeOut = 120000000; // 2 minutes time-out

_LIT(KDefaultServerName, "127.0.0.1");
const TInt KDefaultPortNumber = 25;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSocketsEngine::NewL(MUINotify& aConsole)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSocketsEngine* CSocketsEngine::NewL( MUINotify& aConsole )
    {
    CSocketsEngine* self = CSocketsEngine::NewLC( aConsole );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CSocketsEngine::NewLC(MUINotify& aConsole)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSocketsEngine* CSocketsEngine::NewLC( MUINotify& aConsole )
    {
    CSocketsEngine* self = new (ELeave) CSocketsEngine( aConsole );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CSocketsEngine::CSocketsEngine(MUINotify& aConsole)
// EPOC constructor
// ---------------------------------------------------------
//
CSocketsEngine::CSocketsEngine( MUINotify& aConsole ) :
    CActive( EPriorityStandard ), iConsole( aConsole ), iPort(
            KDefaultPortNumber ), iServerName( KDefaultServerName )
    {
    iStartTime = 0;
    }

// ---------------------------------------------------------
// CSocketsEngine::~CSocketsEngine()
// Destructor
// ---------------------------------------------------------
//
CSocketsEngine::~CSocketsEngine()
    {
    Cancel();
    if (iProgressNotifier)
        {
        iProgressNotifier->Cancel();
        }

    delete iSocketsRead;
    iSocketsRead = NULL;

    delete iSocketsWrite;
    iSocketsWrite = NULL;

    delete iProgressNotifier;
    iProgressNotifier = NULL;

    delete iTimer;
    iTimer = NULL;

    delete iSendTimer;

    delete iHttpClient;
    iHttpClient = NULL;

    if (iData)
        {
        delete iData;
        iData = NULL;
        }
    //delete pointer to mobility API
    if (iMobility)
        {
        delete iMobility;
        iMobility = NULL;
        }

    delete iExtPrefs;
    delete iPrefsList;

    // Note! Closing order matters. Panic occurs if the
    // closing is made in different order. 

    iConnection.Close();
    iSocket.Close();
    iSocketServ.Close();
    }

// ---------------------------------------------------------
// CSocketsEngine::ConstructL()
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CSocketsEngine::ConstructL()
    {
    ChangeStatus( EInterfaceDown );

    iData = HBufC8::NewL( KMaxSendBuffer);
    // Start a timer
    iTimer = CTimeOutTimer::NewL( EPriorityHigh, *this );

    CActiveScheduler::Add( this );

    // Open channel to Socket Server
    User::LeaveIfError( iSocketServ.Connect() );

    // Open connection
    User::LeaveIfError( iConnection.Open( iSocketServ ) );

    // Create socket read and write active objects
    iSocketsRead = CSocketsRead::NewL( iConsole, iSocket, *this );
    iSocketsWrite = CSocketsWrite::NewL( iConsole, iSocket, *this );

    // Create interface notification active object
    iProgressNotifier = CProgressNotifier::NewL( iConnection, *this );
    iProgressNotifier->StartNotify();

    //
    // HTTP framework initialization
    //
    iHttpClient = CHttpClient::NewL( iConsole );
    iHttpClient->SetHttpConnectionInfoL( ETrue, iConnection, iSocketServ );

    // Timer for sending multiple packets
    if (!iSendTimer)
        {
        iSendTimer = CSendTimer::NewL( EPriorityHigh, this );
        }

    iUseTTime = EFalse;
    TInt freq;
    TInt err = HAL::Get( HAL::EFastCounterFrequency, freq );
    if (err != KErrNone || freq == 0)
        {
        iUseTTime = ETrue;
        }

    //set mobility API flag to false
    iIsRegisteredToMobAPI = EFalse;
    SetSocketEngineConnType( ETypeUnknown );

    iExtPrefs = new (ELeave) TExtendedConnPref;
    iPrefsList = TConnPrefList::NewL();
    }

// ---------------------------------------------------------
// CSocketsEngine::GetSocketEngineState()
// Return the current state of the socket engine.
// ---------------------------------------------------------
//
CSocketsEngine::TSocketsEngineState CSocketsEngine::GetSocketEngineState()
    {
    return iEngineStatus;
    }

// ---------------------------------------------------------
// CSocketsEngine::GetSocketEngineRoamingState()
// ---------------------------------------------------------
// 
CSocketsEngine::TRoamingUIState CSocketsEngine::GetSocketEngineRoamingState()
    {
    return iRoamingState;
    }

// ---------------------------------------------------------
// CSocketsEngine::GetMobilityAPI()
// ---------------------------------------------------------
// 
CALRActiveObject* CSocketsEngine::GetMobilityAPI()
    {
    return iMobility;
    }

// ---------------------------------------------------------
// CSocketsEngine::GetSocketEngineConnType()
// Return the current connection type of the socket engine.
// ---------------------------------------------------------
//
CSocketsEngine::TSocketsEngineStartType CSocketsEngine::GetSocketEngineConnType()
    {
    return iSockEngineStartType;
    }
// ---------------------------------------------------------
// CSocketsEngine::SetSocketEngineConnType()
// Set the current conn type of the socket engine.
// ---------------------------------------------------------
//
void CSocketsEngine::SetSocketEngineConnType( const CSocketsEngine::TSocketsEngineStartType aConnStartType )
    {
    iSockEngineStartType = aConnStartType;
    }

// ---------------------------------------------------------
// CSocketsEngine::StartConnWithSnapL()
// Start the connection with snap
// ---------------------------------------------------------
//
void CSocketsEngine::StartConnWithSnapL( TBool aConnect )
    {
    if (iEngineStatus == EConnected || iEngineStatus == EInterfaceUp)
        {
        return;
        }

    //start the connection  dialog
    CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL( settings );
    TCmSettingSelection selection;
    settings->RunApplicationSettingsL( selection );
    CleanupStack::PopAndDestroy( settings );

    switch (selection.iResult)
        {
        case CMManager::EConnectionMethod:
            {
            // start connection with Iap Id
            iPrefs.SetIapId( selection.iId );
            iPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );

            StartTickCount();
            iConnection.Start( iPrefs, iStatus );
            break;
            }
        case CMManager::EDestination:
            {
            TConnSnapPref pref;
            
            pref.SetSnap( selection.iId );

            StartTickCount();
            //start connection with snap Id
            iConnection.Start( pref, iStatus );
            //Set connection type as Snap	
            SetSocketEngineConnType( ESnap );
            
            break;
            }
        case CMManager::EAlwaysAsk:
            {
            // For testing start without preferences
            StartTickCount();
            iConnection.Start( iStatus );
            ChangeStatus( EStartingInterface );
            iConnectAfterStartup = ETrue;
            SetActive();
            return;
            }
        default:
            {
            RDebug::Print( _L("ConnTest: Invalid selection"));
            return;
            }
        }

    ChangeStatus( EStartingInterface );
    //specify if a connection is needed after interface startup(
    iConnectAfterStartup = aConnect;
    SetActive();
    
    }

// ---------------------------------------------------------
// CSocketsEngine::StartInterfaceL(TBool aConnect)
// Start interface
// ---------------------------------------------------------
//
void CSocketsEngine::StartInterfaceL( TSocketsEngineStartType aStartType,
        TBool aConnect )
    {
    RDebug::Print( _L("ConnTest: StartInterfaceL( Start type = %d )"), aStartType );
    if (iEngineStatus == EConnected || iEngineStatus == EInterfaceUp)
        {
        RDebug::Print( _L("ConnTest: Selection cancelled!"));
        return;
        }

    SetSocketEngineConnType( aStartType );

    if (aStartType == EAskIap)
        {
        // Get IAP id
        TUint32 iapId;
        TInt popupOk = Utils::AskIapIdL( iapId );
        
        if (!popupOk)
            {
            RDebug::Print( _L("ConnTest: Selection cancelled!"));
            return;
            }
        RDebug::Print( _L("ConnTest: Selected IAP id=%d"), iapId );

        // Create overrides
        iPrefs.SetIapId( iapId );
        iPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        }
    else if (aStartType == ENetworkId)
        {
        // Get network id
        TUint32 nwkId;
        TInt popupOk = Utils::AskNetworkIdL( nwkId );
        
        if (!popupOk)
            {
            RDebug::Print( _L("ConnTest: Selection cancelled!"));
            return;
            }
        RDebug::Print( _L("ConnTest: Selected Network id=%d"), nwkId );

        // Create overrides
        iPrefs.SetNetId( nwkId );
        iPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        }

    // Start the timer after the dialogs
    StartTickCount();

    // ...And now there are few different kinds of iConnection.Start() functions to call...
    if (aStartType == EExtPrefs)
        {
        iPrefsList->AppendL( iExtPrefs );
        iConnection.Start( *iPrefsList, iStatus );
        iPrefsList->Remove( 0 );
        }
    else if (aStartType == EExtPrefsInternet)
        {
        TExtendedConnPref* tmpExtPrefs = new (ELeave) TExtendedConnPref;
        tmpExtPrefs->SetSnapPurpose( CMManager::ESnapPurposeInternet );
        iPrefsList->AppendL( tmpExtPrefs );
        iConnection.Start( *iPrefsList, iStatus );
        iPrefsList->Remove( 0 );
        delete tmpExtPrefs;
        }
    else if (aStartType == ENoPrefs)
        {
        iConnection.Start( iStatus );
        }
    else // (aStartType == EAskIap || aStartType == ENetworkId) - Other values aren't used.
        {
        iConnection.Start( iPrefs, iStatus );
        }

    ChangeStatus( EStartingInterface );
    iConnectAfterStartup = aConnect;
    SetActive();
    }

// ---------------------------------------------------------
// CSocketsEngine::RegisterToMobilityAPI(TBool aConnect)
// Register to mobility API
// ---------------------------------------------------------
//
void CSocketsEngine::RegisterToMobilityAPIL()
    {
    if (iEngineStatus == EConnected || iEngineStatus == EInterfaceUp)
        {
        if (!iIsRegisteredToMobAPI)
            {
            iConsole.PrintNotify( _L("RegisterToMobilityAPI\n"));
            //create the listener active object
            iMobility = CALRActiveObject::NewL( iConsole,
                    (MMobilityProtocolResp*)this );
            iMobility->OpenL( iConnection );
            iIsRegisteredToMobAPI = ETrue;
            }
        }
    else
        {
        iConsole.PrintNotify( _L("Could not register\n"));
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::UnRegisterToMobilityAPI(TBool aConnect)
// UnRegister from mobility API
// ---------------------------------------------------------
//
void CSocketsEngine::UnRegisterFromMobilityAPI()
    {
    if (iIsRegisteredToMobAPI && iMobility) //&& 
    //(iEngineStatus == EConnected || iEngineStatus == EInterfaceUp || iEngineStatus == EConnecting))
        {
        delete iMobility;
        iMobility = NULL;
        iConsole.PrintNotify( _L("Unregister MobilityAPI\n"));
        }
    iIsRegisteredToMobAPI = EFalse;
    }

// ---------------------------------------------------------
// CSocketsEngine::Error()
// ---------------------------------------------------------
// 
void CSocketsEngine::Error( TInt aError )
    {
    TBuf8<64> text;
    text.AppendFormat( _L8("MobilityError: %d\n"), aError );
    iConsole.PrintNotify( text );
    UnRegisterFromMobilityAPI();
    iRoaming = ERoamingOff;
    }

// ---------------------------------------------------------
// CSocketsEngine::PreferredCarrierAvailable()
// ---------------------------------------------------------
// 
void CSocketsEngine::PreferredCarrierAvailable( TAccessPointInfo aOldAP,
        TAccessPointInfo aNewAP,
        TBool aIsUpgrade,
        TBool aIsSeamless )
    {
    // silence warnings
    aOldAP = aOldAP;
    aIsUpgrade = aIsUpgrade;
    aIsSeamless = aIsSeamless;

    TBuf8<64> text;
    
    if (iRoaming == ERoamingAutomatic)
        {
        text.AppendFormat( _L8("Migrating to %i\n"), aNewAP.AccessPoint() );
        iMobility->MigrateToPreferredCarrier();
        iRoamingState = EIdle;
        }
    else if (iRoaming == ERoamingManual)
        {
        text.AppendFormat( _L8("Preferred IAP %i Available\n"), aNewAP.AccessPoint() );
        iRoamingState = EPendingPreferredCarrier;
        iConsole.PopupNotify( _L("Migrate or Ignore available"));
        }
    else
        {
        text.AppendFormat( _L8("Unexpected PreferrredCarrier %i\n"), aNewAP.AccessPoint() );
        iRoamingState = EIdle;
        iMobility->IgnorePreferredCarrier();
        }
    iConsole.PrintNotify( text );
    }
// ---------------------------------------------------------
// CSocketsEngine::NewCarrierActive()
// ---------------------------------------------------------
// 
void CSocketsEngine::NewCarrierActive( TAccessPointInfo aNewAP,
        TBool aIsSeamless )
    {
    // silence warning
    aIsSeamless = aIsSeamless;

    TBuf8<64> text;
    
    if (iRoaming == ERoamingAutomatic)
        {
        iRoamingState = EIdle;
        iMobility->NewCarrierAccepted();
        }
    else if (iRoaming == ERoamingManual)
        {
        text.AppendFormat( _L8("IAP %i pending accept\n"), aNewAP.AccessPoint() );
        iRoamingState = EPendingNewCarrierActive;
        iConsole.PopupNotify( _L("Accept or Reject available"));
        }
    else
        {
        iRoamingState = EIdle;
        text.AppendFormat( _L8("Unexpected NewCarrierActive %i\n"), aNewAP.AccessPoint() );
        iMobility->NewCarrierRejected();
        }
    
    // Clean up and refresh HTTP client for the new carrier
    delete iHttpClient;
    iHttpClient = NULL;
    
    TRAPD( err1, iHttpClient = CHttpClient::NewL( iConsole ) );
    if ( err1 != KErrNone )
        {
        User::Panic( KPanicConnTest, EConnTestHttpClientInitializationFailed );
        iHttpClient = NULL;
        }
    
    TRAPD( err2, iHttpClient->SetHttpConnectionInfoL( ETrue, iConnection, iSocketServ ) );
    if ( err2 != KErrNone )
        {
        User::Panic( KPanicConnTest, EConnTestHttpClientInitializationFailed );
        }
    
    iConsole.PrintNotify( text );
    }

// ---------------------------------------------------------
// CSocketsEngine::
// ---------------------------------------------------------
//
void CSocketsEngine::Migrate()
    {
    if (iRoaming != ERoamingManual)
        {
        User::Panic( KPanicSocketsEngine, EConnTestBadRoamingStatus );
        }
    iMobility->MigrateToPreferredCarrier();
    iConsole.PrintNotify( _L("Migration requested"));
    iRoamingState = EIdle;
    }

// ---------------------------------------------------------
// CSocketsEngine::
// ---------------------------------------------------------
//
void CSocketsEngine::Ignore()
    {
    if (iRoaming != ERoamingManual)
        {
        User::Panic( KPanicSocketsEngine, EConnTestBadRoamingStatus );
        }
    iMobility->IgnorePreferredCarrier();
    iConsole.PrintNotify( _L("Ignored new carrier"));
    iRoamingState = EIdle;
    }

// ---------------------------------------------------------
// CSocketsEngine::
// ---------------------------------------------------------
//
void CSocketsEngine::AcceptCarrier()
    {
    if (iRoaming != ERoamingManual)
        {
        User::Panic( KPanicSocketsEngine, EConnTestBadRoamingStatus );
        }
    iMobility->NewCarrierAccepted();
    iConsole.PrintNotify( _L("Accepted carrier"));
    iRoamingState = EIdle;
    }

// ---------------------------------------------------------
// CSocketsEngine::
// ---------------------------------------------------------
//
void CSocketsEngine::RejectCarrier()
    {
    if (iRoaming != ERoamingManual)
        {
        User::Panic( KPanicSocketsEngine, EConnTestBadRoamingStatus );
        }
    iMobility->NewCarrierRejected();
    iConsole.PrintNotify( _L("Rejected carrier"));
    iRoamingState = EIdle;
    }

// ---------------------------------------------------------
// CSocketsEngine::StartCloseInterfaceL()
// Start interface for testing RConnection::Close
// ---------------------------------------------------------
//
void CSocketsEngine::StartCloseInterfaceL()
    {
    if (iEngineStatus == EConnected || iEngineStatus == EInterfaceUp)
        {
        // Already started, simply return
        return;
        }

    // Get IAP id
    TUint32 iapId;
    TInt popupOk = Utils::AskIapIdL( iapId );
    
    if (!popupOk)
        {
        // Selection cancelled
        return;
        }
    RDebug::Print( _L("ConnTest: Selected IAP id=%d"), iapId );

    iCloseConnection = new (ELeave) RConnection;

    User::LeaveIfError( iCloseConnection->Open( iSocketServ ) );

    // Create overrides
    iPrefs.SetIapId( iapId );
    iPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );

    ChangeStatus( EStartingInterface );

    iCloseConnection->Start( iPrefs, iStatus );
    SetActive();

    iTempProgressNotifier = CProgressNotifier::NewL( *iCloseConnection, *this );
    iTempProgressNotifier->StartNotify();
    
    }

// ---------------------------------------------------------
// CSocketsEngine::StopInterfaceL()
// Shutdown interface
// ---------------------------------------------------------
//
void CSocketsEngine::StopInterfaceL()
    {
    // Note! Socket is not closed here, because in connected
    // state only Disconnect menu item is available. Therefore
    // interface cannot be stopped when connection is open.
    //
    // iProgressNotifier->Cancel() is called in ProgressNotifyReceived
    // method after KLinkLayerClosed has been received.
    //

    //unregister from mobility API if registered
    UnRegisterFromMobilityAPI();
    SetSocketEngineConnType( ETypeUnknown );
    
    if (iEngineStatus == EConnected || iEngineStatus == EInterfaceUp)
        {
        iConnection.Stop();
        ChangeStatus( EInterfaceDown );
        iHttpClient->SetHttpConnectionInfoL( ETrue, iConnection, iSocketServ );
        
        if (iCloseConnection)
            {
            // So if we are here, interface was started for testing RConnection::Close,
            // and the corresponding RConnection instance needs to be deleted.
            iCloseConnection->Close();
            delete iCloseConnection;
            iCloseConnection = NULL;
            }
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::CloseInterface()
// Close interface
// ---------------------------------------------------------
//
void CSocketsEngine::CloseInterface()
    {
    if (iCloseConnection)
        {
        iCloseConnection->Close();
        delete iCloseConnection;
        iCloseConnection = NULL;
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::ConnectL()
// Check whether the address is valid IP address. If it's not
// calls DNS to solve the name.
// ---------------------------------------------------------
//
void CSocketsEngine::ConnectL()
    {
    // Just in case (does nothing if interface has already been started)
    StartInterfaceL( EAskIap, ETrue );
    
    // Initiate connection process
    if ((iEngineStatus == EInterfaceUp) || (iEngineStatus == ETimedOut))
        {
        TInetAddr addr;
        if (addr.Input( iServerName ) == KErrNone)
            {
            // server name is already a valid ip address
            ConnectL( addr );
            }
        else // need to look up name using dns
            {
            // Initiate DNS
            User::LeaveIfError( iResolver.Open( iSocketServ, KAfInet,
                    KProtocolInetUdp, iConnection ) );
            // DNS request for name resolution
            StartTickCount();
            iResolver.GetByName( iServerName, iNameEntry, iStatus );

            ChangeStatus( ELookingUp );
            // Request time out
            iTimer->After( KTimeOut );
            SetActive();
            }
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::ConnectL()
// Connect to socket
// ---------------------------------------------------------
//                          
void CSocketsEngine::ConnectL( const TInetAddr& aAddr ) // <a name="ConnectL32">
    {
    
    // Initiate attempt to connect to a socket by IP address	
    if ((iEngineStatus == EInterfaceUp) || (iEngineStatus == ETimedOut))
        {
        // Open a TCP socket
        iSocket.Close(); // Just in case
        User::LeaveIfError( iSocket.Open( iSocketServ, KAfInet, iSocketType,
                iProtocol, iConnection ) );

        // Set up address information
        iAddress = aAddr;
        iAddress.SetPort( iPort );

        // Initiate socket connection
        StartTickCount();
        iSocket.Connect( iAddress, iStatus );
        ChangeStatus( EConnecting );
        SetActive();
        
        // Start a timeout
        iTimer->After( KTimeOut );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::Disconnect()
// Shutdown socket
// ---------------------------------------------------------
//
void CSocketsEngine::Disconnect()
    {
    if (iEngineStatus == EListening && iProtocol == KProtocolInetTcp)
        {
        iListeningSocket.CancelAccept();
        iListeningSocket.Close();
        // Remember to close the socket on reader RunL()
        }
    if (iEngineStatus == EConnected || iEngineStatus == EListening)
        {
        iSocketsRead->Cancel();
        iSocketsWrite->Cancel();

        // have to do this.
        iTimer->Cancel();

        // Use Close() instead of Shutdown(). This way we
        // can handle the situation, where server initiates
        // the closing process, and sends RST signal instead
        // of FIN. If server sends RST and Shutdown() is used,
        // engine stays in EDisconnecting state forever.
        iSocket.Close();
        
        ChangeStatus( EInterfaceUp );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::ListenL()
// Opens listening socket
// ---------------------------------------------------------
//
void CSocketsEngine::ListenL()
    {
    // Initiate attempt to connect to a socket by IP address	
    if ((iEngineStatus == EInterfaceUp) || (iEngineStatus == ETimedOut))
        {
        // Open a TCP socket
        iSocket.Close(); // Just in case
        iListeningSocket.Close();

        // Set up address information
        iAddress.SetFamily( 0 );
        iAddress.SetPort( iPort );
        switch (iProtocol)
            {
            case KProtocolInetTcp:
                User::LeaveIfError( iSocket.Open( iSocketServ ) );
                User::LeaveIfError( iListeningSocket.Open( iSocketServ,
                        KAfInet, iSocketType, iProtocol, iConnection ) );
                // Bind a port
                iListeningSocket.Bind( iAddress );
                // Listens a socket
                iListeningSocket.Listen( 1 ); // Only one connection allowed
                // Accepts one connection
                iListeningSocket.Accept( iSocket, iStatus );
                ChangeStatus( EListening );
                SetActive();
                break;
            case KProtocolInetUdp:
                User::LeaveIfError( iSocket.Open( iSocketServ, KAfInet,
                        iSocketType, iProtocol, iConnection ) );
                iSocket.Bind( iAddress );
                ChangeStatus( EListening );
                iStatus = KErrNone;
                RunL(); // ugly.
                break;
            default:
                User::Leave( KErrNotSupported );
                break;
            }
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::Write()
// Write data to socket
// ---------------------------------------------------------
//
void CSocketsEngine::WriteL( const TDesC8& aData )
    {
    RDebug::Print( _L("ConnTest: Sending data, length = %d"),aData.Length() );

    iSocketsRead->SetPerformance( EFalse );
    
    // Write data to socket
    if (iEngineStatus == EConnected)
        {
        iSocketsWrite->IssueWriteL( aData, &iAddress, iProtocol );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::Write()
// Write data to socket aCount times
// ---------------------------------------------------------
//
void CSocketsEngine::WriteFloodL( const TDesC8& aData, TInt aCount )
    {
    // Write data to socket
    if (iEngineStatus == EConnected)
        {
        iThroughputStartTime.UniversalTime();
        iSocketsWrite->IssueWriteL( aData, &iAddress, iProtocol, aCount );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::SendHttpRequestOverSocketL(TBool aHasBody)
// Write HTTP request to socket
// ---------------------------------------------------------
//
void CSocketsEngine::SendHttpRequestOverSocketL( TBool aHasBody,
        TBool aDoPerformance )
    {
    iSocketsRead->SetPerformance( aDoPerformance );

    TBuf8<256> req;
    
    if (aHasBody)
        {
        // Send HTTP POST
        req.Copy( _L8("POST "));

        // Add '/' if it is not included in the given page name
        if (!((TChar)iSettingData->iHttpPage[0] == '/'))
            {
            req.Append( _L8("/"));
            }

        req.Append( iSettingData->iHttpPage );
        req.Append( _L8(" HTTP/1.1\r\n"));
        req.Append( _L8("Host: "));
        req.Append( iSettingData->iServerName );
        req.Append( _L8(":"));
        req.AppendNum( iSettingData->iPort );
        req.AppendFormat( _L8("\r\nContent-Length: %d"), iSettingData->iPacketSize
                * iSettingData->iPackets );
        req.Append( _L8("\r\n\r\n"));
        
        iSocketsWrite->IssueWriteL( req, &iAddress, iProtocol,
                iSettingData->iPacketSize, iSettingData->iPackets );
        }
    else
        {
        // Send HTTP GET
        req.Copy( _L8("GET "));

        // Add '/' if it is not included in the given page name
        if (!((TChar)iSettingData->iHttpPage[0] == '/'))
            {
            req.Append( _L8("/"));
            }

        req.Append( iSettingData->iHttpPage );
        req.Append( _L8(" HTTP/1.1\r\n"));
        req.Append( _L8("Host: "));
        req.Append( iSettingData->iServerName );
        req.Append( _L8(":"));
        req.AppendNum( iSettingData->iPort );
        req.Append( _L8("\r\n\r\n"));

        StartTickCount();
        iSocketsWrite->IssueWriteL( req, &iAddress, iProtocol );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::Read()
// Read data from socket
// ---------------------------------------------------------
//
void CSocketsEngine::Read()
    {
    // Initiate read of data from socket
    if ((iEngineStatus == EConnected) && (!iSocketsRead->IsActive()))
        {
        iSocketsRead->Start( &iAddress, iProtocol );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::RunL()
// 
// ---------------------------------------------------------
//
void CSocketsEngine::RunL()
    {
    // Active object request complete handler.
    // iEngineStatus flags what request was made, so its
    // completion can be handled appropriately
    iTimer->Cancel(); // Cancel TimeOut timer before completion
    TBuf<64> text( _L(""));
    TBuf<64> errorText( _L("") );
    
    switch (iEngineStatus)
        {
        case EInterfaceDown:
            // Just in case, if status is changed in ProgressNotifyReceived
            // method before this method is called
            break;
        case EStartingInterface:
            text.Format( _L("RConnection::Start()"));
            StopTickCount( text );
            // Interface startup request
            if (iStatus == KErrNone)
                {
                // Interface started successfully
                ChangeStatus( EInterfaceUp );

                if (iCloseConnection)
                    {
                    iHttpClient->SetHttpConnectionInfoL( EFalse,
                            *iCloseConnection, iSocketServ );
                    }
                else
                    {
                    iHttpClient->SetHttpConnectionInfoL( EFalse, iConnection,
                            iSocketServ );
                    }

                if ( ( iSockEngineStartType == ESnap ||
                       iSockEngineStartType == EExtPrefs ||
                       iSockEngineStartType == EExtPrefsInternet ) &&
                     ( iRoaming == ERoamingAutomatic || iRoaming == ERoamingManual ) )
                    {
                    RegisterToMobilityAPIL();
                    }
                
                if (iConnectAfterStartup)
                    {
                    ConnectL();
                    }
                }
            else
                {
                errorText.Append(_L("<CSocketsEngine> Startup failed"));
                ChangeStatus( EInterfaceDown );
                SetSocketEngineConnType( ETypeUnknown );
                }
            break;
            
        case EConnecting:
            // IP connection request
            text.Format( _L("RSocket::Connect()"));
            StopTickCount( text );
            if (iStatus == KErrNone)
            // Connection completed successfully
                {
                ChangeStatus( EConnected );
                Read(); //Start CSocketsRead Active object
                }
            else
                {
                errorText.Append(_L("<CSocketsEngine> Conn. failed"));
                ChangeStatus( EInterfaceUp );
                }
            break;
            
        case ELookingUp:
            text.Format( _L("RHostResolver::GetByName()"));
            StopTickCount( text );
            iResolver.Close();
            if (iStatus == KErrNone)
                {
                // DNS look up successful
                iNameRecord = iNameEntry();
                // Extract domain name and IP address from name record
                PrintTextToConsole( _L("\r\nDomain name = "));
                PrintTextToConsole( iNameRecord.iName );
                TBuf<64> ipAddr;
                TInetAddr::Cast( iNameRecord.iAddr ).Output( ipAddr );
                iConsole.PrintNotify( _L("\r\nIP address = "));
                PrintTextToConsole( ipAddr );
                PrintTextToConsole( _L("\r\n"));
                // And connect to the IP address
                ChangeStatus( EInterfaceUp );
                ConnectL( TInetAddr::Cast( iNameRecord.iAddr ) );
                }
            else
                {
                // DNS lookup failed
                errorText.Append(_L("<CSocketsEngine> DNS lookup failed"));
                ChangeStatus( EInterfaceUp );
                }
            break;
        case EListening:
            // Listening socket accept returned
            if (iStatus == KErrNone && !iSocketsRead->IsActive())
            // Connection established
                {
                //ChangeStatus(EConnected); Might as well be in listening state.
                // This how we can keep accepting several connections
                // Initiate read of data from socket
                iSocketsRead->StartRAWRead( &iAddress, iProtocol );
                }
            else
                {
                errorText.Append(_L("<CSocketsEngine> DNS lookup failed"));
                iListeningSocket.Close();
                iSocket.Close();
                ChangeStatus( EInterfaceUp );
                }
            
            break;
        case EDisconnecting:
            if (iStatus == KErrNone)
                {
                iSocket.Close();
                ChangeStatus( EInterfaceUp );
                }
            break;
            
        default:
            // Ignore the state check here, because it might happen that state
            // has already been altered in ProgressNotifyReceived method.
            break;
        }
    
    // ErrorNotify starts the scheduler, don't add any code that refers to
    // class variables after it (they might have changed!)
    if (errorText.Length() > 0)
        {
        iConsole.ErrorNotify( errorText, iStatus.Int() );
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::DoCancel()
// Cancel ongoing operations
// ---------------------------------------------------------
//
void CSocketsEngine::DoCancel()
    {
    iTimer->Cancel();
    
    // Cancel ongoing operation
    switch (iEngineStatus)
        {
        case EStartingInterface:

            if (iCloseConnection)
                {
                iCloseConnection->Close();
                delete iCloseConnection;
                iCloseConnection = NULL;
                }
            else
                {
                iConnection.Stop();
                }
            
            UnRegisterFromMobilityAPI();
            SetSocketEngineConnType( ETypeUnknown );
            ChangeStatus( EInterfaceDown );
            
            break;
        case EConnecting:
            iSocket.CancelConnect();
            iSocket.Close();
            ChangeStatus( EInterfaceUp );
            break;
        case ELookingUp:
            // Cancel look up attempt
            iResolver.Cancel();
            iResolver.Close();
            ChangeStatus( EInterfaceUp );
            break;
        case EDisconnecting:
            ChangeStatus( EInterfaceUp );
            iSocket.Close();
            break;
        case EListening:
            ChangeStatus( EInterfaceUp );
            iListeningSocket.CancelAccept();
            iListeningSocket.Close();
            iSocket.Close();
            break;
        default:
            User::Panic( KPanicSocketsEngine, EConnTestBadStatus );
            break;
        }
    }

// ---------------------------------------------------------
// CSocketsEngine::ChangeStatus(TSocketsEngineState aNewStatus)
// Set new status for socket engine and update the status
// line in console window.
// ---------------------------------------------------------
//
void CSocketsEngine::ChangeStatus( TSocketsEngineState aNewStatus )
    {
    // Update the status (and the status display)
    switch (aNewStatus)
        {
        case EInterfaceDown:
            iConsole.SetStatus( _L("Interface stopped"));
            break;
        case EStartingInterface:
            iConsole.SetStatus( _L("Starting interface..."));
            break;
        case EInterfaceUp:
            iConsole.SetStatus( _L("Interface up"));
            break;
        case EConnecting:
            iConsole.SetStatus( _L("Connecting..."));
            break;
        case EConnected:
            iConsole.SetStatus( _L("Connected"));
            break;
        case EListening:
            iConsole.SetStatus( _L("Listening"));
            break;
        case ETimedOut:
            iConsole.SetStatus( _L("Timed out"));
            break;
        case ELookingUp:
            iConsole.SetStatus( _L("Looking up..."));
            break;
        case ELookUpFailed:
            iConsole.SetStatus( _L("Look up failed"));
            break;
        case EConnectFailed:
            iConsole.SetStatus( _L("Failed"));
            break;
        case EDisconnecting:
            iConsole.SetStatus( _L("Disconnecting..."));
            break;
        default:
            User::Panic( KPanicSocketsEngine, EConnTestBadStatus );
            break;
        }
    iEngineStatus = aNewStatus;
    }

// ---------------------------------------------------------
// CSocketsEngine::PrintTextToConsole(const TDesC& aDes)
// Helper function for printing notifications, text
// formatting added.
// ---------------------------------------------------------
//
void CSocketsEngine::PrintTextToConsole( const TDesC& aDes )
    {
    // Print some text on the console
    iConsole.PrintNotify( aDes ); // Try if this works on real hw
    }

// ---------------------------------------------------------
// CSocketsEngine::SetConnectionData(const CSettingData* aData)
// Set connection information.
// ---------------------------------------------------------
//
void CSocketsEngine::SetConnectionData( const CSettingData* aData )
    {
    iServerName.Copy( aData->iServerName );
    iPort = aData->iPort;

    switch (aData->iProtocol)
        {
        case 0:
            iProtocol = KProtocolInetTcp;
            iSocketType = KSockStream;
            break;
        case 1:
            iProtocol = KProtocolInetUdp;
            iSocketType = KSockDatagram;
            break;
        default:
            iProtocol = KProtocolInetTcp;
            iSocketType = KSockStream;
            break;
        }

    iRoaming = (TRoamingSetting)aData->iRoaming;

    iHttpPage = aData->iHttpPage;
    
    iSettingData = (CSettingData*)aData;
    }

// ---------------------------------------------------------
// CSocketsEngine::SetCustomPrefs(const CCustomPrefsData* aData)
// Set custom preferences information.
// ---------------------------------------------------------
//
void CSocketsEngine::SetCustomPrefsData( const CCustomPrefsData* aData )
    {
    iCustomPrefsData = (CCustomPrefsData*)aData;
    iExtPrefs->SetSnapPurpose( (CMManager::TSnapPurpose)aData->iSnapPurpose );
    iExtPrefs->SetSnapId( aData->iSnapId );
    iExtPrefs->SetIapId( aData->iIapId );
    iExtPrefs->SetBearerSet( aData->iBearerSet );
    iExtPrefs->SetNoteBehaviour( aData->iNoteBehaviour );
    iExtPrefs->SetDisconnectDialog( aData->iDisconnectDialog );
    iExtPrefs->SetConnSelectionDialog( aData->iConnSelectionDialog );
    iExtPrefs->SetForcedRoaming( aData->iForcedRoaming );
    }

// ---------------------------------------------------------
// CSocketsEngine::ConnectionInfo()
// Display information about current connection and sockets
// ---------------------------------------------------------
//
void CSocketsEngine::ConnectionInfoL()
    {
    TInt result;
    TBuf<40> val;
    TBuf<40> network;
    TBuf<20> query;
    TName name;
    TUint connCount;
    TUint i, j;
    TConnectionEnumArg args;
    TUint32 networkId;
    RApaLsSession appSess;
    TApaAppInfo appInfo;
    
    User::LeaveIfError( appSess.Connect() );
    CleanupClosePushL( appSess );

    HBufC* buf = HBufC::NewLC( 2048 );
    TPtr infoBuf = buf->Des();
    
    infoBuf.AppendFormat( _L("===============\n"));
    infoBuf.AppendFormat( _L("Connection info:\n"));

    // Connection name.
    result = iConnection.Name( name );
    User::LeaveIfError( result );
    RDebug::Print( _L("ConnTest: Connection name: %S"), &name );

    // Get connection info
    result = iConnection.EnumerateConnections( connCount );
    User::LeaveIfError( result );
    RDebug::Print( _L("ConnTest: Number of connections: %d"), connCount );
    infoBuf.AppendFormat( _L("Connections: %d\n"), connCount );

    if (connCount == 0)
        {
        infoBuf.AppendFormat( _L("---------------\n"));
        PrintTextToConsole( infoBuf );
        CleanupStack::PopAndDestroy( buf ); // buf
        CleanupStack::PopAndDestroy( &appSess );
        return;
        }

    //
    // Get active connection info
    //
    infoBuf.AppendFormat( _L("Active connection:\n"));

    // IAP name
    query.Format( _L("%s\\%s"), IAP, COMMDB_NAME);
    result = iConnection.GetDesSetting(query, val);
    if(result == KErrNone)
        {
        RDebug::Print(_L("ConnTest: IAP/Name value: %S"), &val);
        infoBuf.AppendFormat(_L("-IAP: %S\n"), &val);
        }

    // Network id
            query.Format(_L("%s\\%s"), IAP, IAP_NETWORK);
            result = iConnection.GetIntSetting(query, networkId);
            if(result == KErrNone)
                {
                // Get the corresponding network name for this id,
            // it is user friendlier.
            RDebug::Print(_L("ConnTest: NetworkId: %d"), networkId);

            // Network name
            CCommsDatabase* TheDb;

            TheDb = CCommsDatabase::NewL();
            CleanupStack::PushL(TheDb);

            CCommsDbTableView* view = TheDb->OpenViewMatchingUintLC( TPtrC(NETWORK),
                    TPtrC(COMMDB_ID),
                    networkId );
            result = view->GotoFirstRecord();

            if(result == KErrNone)
                {
                view->ReadTextL(TPtrC(COMMDB_NAME), network);
                RDebug::Print(_L("ConnTest: Network name: %S"), &network);
                infoBuf.AppendFormat(_L("-net: %S\n"), &network);
                }
            CleanupStack::PopAndDestroy(view);
            CleanupStack::PopAndDestroy(TheDb);
            }

        // Note! The following doesn't work, because NETWORK table
        // is not among those tables where information can be
        // fetched this way. (Perhaps in future.)
        /*    
         // Network name
         query.Format(_L("%s\\%s"), NETWORK, COMMDB_NAME);
         result = iConnection.GetDesSetting(query, network);
         RDebug::Print(_L("IAP/Name value: %S"), &val);
         infoBuf.AppendFormat(_L("Network name: %S\n"), &network);
         
         // Network id
         query.Format(_L("%s\\%s"), NETWORK, COMMDB_ID);
         result = iConnection.GetIntSetting(query, networkId);
         RDebug::Print(_L("IAP/Name value: %d"), networkId);
         infoBuf.AppendFormat(_L("Network id: %d\n"), networkId);
         */

        // Print transferred data
        TPckg<TUint> uplinkVolume(0);
        TPckg<TUint> downlinkVolume(0);
        TRequestStatus status;
        iConnection.DataTransferredRequest(uplinkVolume, downlinkVolume, status);
        User::WaitForRequest(status);
        if(status.Int() == KErrNone)
            {
            infoBuf.AppendFormat(_L("-up=%d, down=%d\n"), uplinkVolume(), downlinkVolume());
            }

        for(i = 1; i <= connCount; i++)
            {
            infoBuf.AppendFormat(_L("\nConnection %d:\n"), i);
            TPckgBuf<TConnectionInfo> connInfo;
            result = iConnection.GetConnectionInfo(i, connInfo);
            User::LeaveIfError(result);
            RDebug::Print(_L("ConnTest: IapId: %d, NetId: %d"), connInfo().iIapId, connInfo().iNetId);
            infoBuf.AppendFormat(_L(" -IapId=%d, NetId=%d\n"), connInfo().iIapId, connInfo().iNetId);

            // Get info about clients using this connection
            args.iIndex = i;
            TConnEnumArgBuf enumBuf(args);

            TConnectionGetClientInfoArg clientInfoArg;
            clientInfoArg.iIndex = 1;
            TConnGetClientInfoArgBuf clientInfoBuf(clientInfoArg);

            TConnectionClientInfo clientInfo;

            result = iConnection.Control(KCOLConnection, KCoEnumerateConnectionClients, enumBuf);
            User::LeaveIfError(result);
            infoBuf.AppendFormat(_L(" -clients: %d\n"), enumBuf().iCount);

            for(j = 1; j <= enumBuf().iCount; j++)
                {
                clientInfoBuf().iIndex = j;
                result = iConnection.Control( KCOLConnection,
                        KCoGetConnectionClientInfo,
                        clientInfoBuf );
                User::LeaveIfError(result);
                clientInfo = clientInfoBuf().iClientInfo;

                TInt id = clientInfo.iProcessId;
                RDebug::Print(_L("ConnTest: ProcessId: %d"), id);

                // Get application  name
#ifndef __WINS__
            result = appSess.GetAppInfo(appInfo, clientInfo.iUid);
            if(result == KErrNone)
                {
                infoBuf.AppendFormat(_L("   client %d=%S\n"), j, &(appInfo.iCaption));
                }
            else
                {
                // Servers don't have caption name (e.g. Connection Monitor server)
                infoBuf.AppendFormat(_L("   client %d=%x\n"), j, clientInfo.iUid);
                }
#else
            infoBuf.AppendFormat(_L("   client %d=N/A in WINS\n"), j);
#endif        
            }

        // Get info about sockets using this connection
        TConnectionGetSocketInfoArg socketInfoArg;
        socketInfoArg.iIndex = i;
        TConnGetSocketInfoArgBuf socketInfoBuf(socketInfoArg);
        TConnectionSocketInfo socketInfo;

        result = iConnection.Control( KCOLConnection,
                KCoEnumerateConnectionSockets,
                enumBuf);
        User::LeaveIfError(result);
        infoBuf.AppendFormat(_L(" -sockets: %d\n"), enumBuf().iCount);

        for(j = 1; j <= enumBuf().iCount; j++)
            {
            socketInfoBuf().iIndex = j;
            result = iConnection.Control( KCOLConnection,
                    KCoGetConnectionSocketInfo,
                    socketInfoBuf );
            User::LeaveIfError(result);
            socketInfo = socketInfoBuf().iSocketInfo;

            TUint id = socketInfo.iAddressFamily;
            TBuf<5> prot;
            socketInfo.iProtocol == KProtocolInetTcp ? prot.Copy(_L("tcp")) : prot.Copy(_L("udp"));

            TSockAddr sa = socketInfo.iSourceAddress;
            TSockAddr da = socketInfo.iDestinationAddress;

            TInetAddr& source = TInetAddr::Cast(sa);
            TInetAddr& dest = TInetAddr::Cast(da);

            TBuf<20> a1;
            source.Output(a1);

            TBuf<20> a2;
            dest.Output(a2);

            RDebug::Print(_L("ConnTest: ProcessId: %d"), id);
            infoBuf.AppendFormat(_L("  %d. socket: %S\n   %S->\n   %S\n"), j, &prot, &a1, &a2);
            }
        } // for (i...  
    infoBuf.AppendFormat(_L("---------------\n"));
    PrintTextToConsole(infoBuf);
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(&appSess);
    }

// ---------------------------------------------------------
// CSocketsEngine::SendHttpFrameworkRequestL()
// Send HTTP request
// ---------------------------------------------------------
//
void CSocketsEngine::SendHttpFrameworkRequestL( TBool aHasBody,
        TBool aDoPerformance,
        TBool aIsSecure )
    {
    iHttpClient->SetPerformance( aDoPerformance );
    iHttpClient->InvokeHttpMethodL( iSettingData, aHasBody, aIsSecure );
    }

// ---------------------------------------------------------
// CSocketsEngine::ProgressNotifyReceivedL(TInt aStage)
// Progress notification has been received, display the
// stage in output window.
// ---------------------------------------------------------
//
void CSocketsEngine::ProgressNotifyReceivedL( TInt aStage, TInt aError )
    {
    TBuf8<64> text;
    text.AppendFormat( _L8("Progress: %d, %d"), aStage, aError );

    TBuf8<16> error;
    error.Format( _L8(", %d"), aError );

    switch (aStage)
        {
        case KConnectionUninitialised: // 0
            iConsole.PrintNotify( _L8("Connection uninit"));
            iConsole.PrintNotify( error );
            break;
        case KStartingSelection: // 1000
            iConsole.PrintNotify( _L8("Starting selection"));
            iConsole.PrintNotify( error );
            break;
        case KFinishedSelection: // 2000
            iConsole.PrintNotify( _L8("Finished selection"));
            iConsole.PrintNotify( error );
            
            if (iCloseConnection)
                {
                // We are starting RConnection instance for testing RConnection::Close.
                // At this point we can attach the monitoring RConnection instance to
                // interface, and delete the temporary progress notifier.
                //
                // We may loose few progress notifications at start up, that's small
                // prize compared to advantages: we get correct notifications when
                // interface goes down, which is the essential part of this test.

                TBuf<20> query;
                TUint32 networkId;
                TInt result;
                TInt err( KErrNone );
                TConnectionInfo info;
                TPckg<TConnectionInfo> pckgInfo( info );
                
                // Network id
                query.Format( _L("%s\\%s"), IAP, IAP_NETWORK);
                result = iCloseConnection->GetIntSetting(query, networkId);
                if(result == KErrNone)
                    {
                    info.iIapId = iPrefs.IapId();
                    info.iNetId = networkId;
                    }
                err = iConnection.Attach( pckgInfo, RConnection::EAttachTypeMonitor );
                if ( err != KErrNone )
                    {
                    iConsole.ErrorNotify(_L("Attaching failed"), err);
                    }
                iTempProgressNotifier->Cancel();
                delete iTempProgressNotifier;
                iTempProgressNotifier = NULL;
                }
            break;
        case KConnectionFailure: // 2001
            iConsole.PrintNotify(_L8("Connection failure"));
            iConsole.PrintNotify(error);
            break;
        case KMinAgtProgress: // 2500
            iConsole.PrintNotify(_L8("Min agt progress"));
            iConsole.PrintNotify(error);
            break;
        case KConnectionOpen: // 3500
            iConsole.PrintNotify(_L8("Connection open"));
            iConsole.PrintNotify(error);
            break;
        case KConnectionClosed: // 4500
            iConsole.PrintNotify(_L8("Connection closed"));
            iConsole.PrintNotify(error);
            break;
        case KMaxAgtProgress: // 5500
            iConsole.PrintNotify(_L8("Max agent progress"));
            iConsole.PrintNotify(error);
            break;
        case KMinNifProgress: // 6000
            iConsole.PrintNotify(_L8("Min nif progress"));
            iConsole.PrintNotify(error);
            break;
        case KLinkLayerOpen: // 7000
            iConsole.PrintNotify(_L8("Link layer open"));
            iConsole.PrintNotify(error);
            break;
        case KLinkLayerClosed: // 8000
            iConsole.PrintNotify(_L8("Link layer closed"));
            iConsole.PrintNotify(error);
            iHttpClient->SetHttpConnectionInfoL(ETrue, iConnection, iSocketServ);
    
            switch (iEngineStatus)
                {
                case EStartingInterface:
                // This case will occur, if username/password prompt dialog
                // is used and cancel is selected in dialog, and when error
                // occurs during interface startup.
                break;
                case EInterfaceDown:
                // EInterfaceDown must be handled also, because this
                // state has been set in StopInterface method.
                break;
                case EListening:
                iListeningSocket.CancelAccept();
                iListeningSocket.Close();
                // FALLTHROUGH
                case ELookingUp:
                // Cancel everything depending on state.
                iResolver.Cancel();
                iResolver.Close();
                // FALLTHROUGH
                case EConnected:
                iSocketsRead->Cancel();
                iSocketsWrite->Cancel();
                // FALLTHROUGH
                case EConnecting:
                // This looks awful, but is required because of the fall through
                // and buggy implementation of the CancelConnect.
                if( EConnecting == iEngineStatus )
                    {
                    iSocket.CancelConnect();
                    }
                // FALLTHROUGH
                case EDisconnecting:
                iSocket.Close();
                // FALLTHROUGH
                case EInterfaceUp:
                if (iRoaming == ERoamingOff)
                    {
                    UnRegisterFromMobilityAPI();
                    SetSocketEngineConnType(ETypeUnknown);
                    ChangeStatus(EInterfaceDown);
                    }
                else
                    {
                    iConsole.PrintNotify(_L8("MobilitySession lost!\n"));
                    UnRegisterFromMobilityAPI();
                    SetSocketEngineConnType(ETypeUnknown);
                    ChangeStatus(EInterfaceDown);
                    }
                break;
                default:
                User::Panic(KPanicSocketsEngine, EConnTestBadStatus);
                break;
                }
    
            break;
        case KMaxNifProgress: // 9000
            iConsole.PrintNotify(_L8("Max nif progress\n"));
            break;
        default:
            iConsole.PrintNotify(text);
        }
    iConsole.PrintNotify(_L8("\f"));
    }

            // ---------------------------------------------------------
                // CSocketsEngine::ProgressNotifyError(TInt aStatus)
                // Some error has occurred while receiving progress
                // notifications.
                // ---------------------------------------------------------
                //
void CSocketsEngine::ProgressNotifyError( TInt aStatus )
    {
    iConsole.ErrorNotify( _L("<CProgressNotifier> Notify failed"), aStatus );
    }

// ---------------------------------------------------------
// CSocketsEngine::TimerExpired()
// Cancel ongoing operations
// ---------------------------------------------------------
//
void CSocketsEngine::TimerExpired()
    {
    Cancel();
    iConsole.ErrorNotify( _L("<CSocketsEngine> Timed out"), KErrTimedOut );
    }

// ---------------------------------------------------------
// CSocketsEngine::InterfaceInfoL()
// Print information about interfaces.
// ---------------------------------------------------------
//
void CSocketsEngine::InterfaceInfoL()
    {
    TBuf<128> t;
    TAutoClose<RSocketServ> ss;
    User::LeaveIfError( ss.iObj.Connect() );
    ss.PushL();

    TAutoClose<RSocket> sock;
    User::LeaveIfError( sock.iObj.Open( ss.iObj, _L("udp")) );
    sock.PushL();

    User::LeaveIfError(
            sock.iObj.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl ) );

    TProtocolDesc in;
    User::LeaveIfError( sock.iObj.Info( in ) );

    TPckgBuf<TSoInetInterfaceInfo> info, next;
    
    TInt res = sock.iObj.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, info );
    if (res != KErrNone)
        {
        User::Leave( res );
        }
    TInt count = 0;
    while (res == KErrNone)
        {
        res = sock.iObj.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, next );

        if (info().iName != _L("") && info().iName != _L("loop6") && info().iName != _L("loop4"))
            {
            t.Format( _L("Interface %d\n"),count++ );
            PrintTextToConsole( t );
            t.Format( _L("Name \"%S\"\n"), &info().iName );
            PrintTextToConsole( t );

            t.Format( _L("State "));
            switch (info().iState)
                {
                case EIfPending:
                    t.AppendFormat( _L("pending\n"));
                    break;
                case EIfUp:
                    t.AppendFormat( _L("up\n"));
                    break;
                case EIfBusy:
                    t.AppendFormat( _L("busy\n"));
                    break;
                default:
                    t.AppendFormat( _L("down\n"));
                    break;
                }

            t.AppendFormat( _L("Mtu %d\n"), info().iMtu );
            t.AppendFormat( _L("Speed Metric %d\n"), info().iSpeedMetric );

            t.Format( _L("Features:"));
            info().iFeatures & KIfIsLoopback ? t.AppendFormat( _L(" loopback")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfIsDialup ? t.AppendFormat( _L(" dialup")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfIsPointToPoint ? t.AppendFormat( _L(" pointtopoint")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfCanBroadcast ? t.AppendFormat( _L(" canbroadcast")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfCanMulticast ? t.AppendFormat( _L(" canmulticast")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfCanSetMTU ? t.AppendFormat( _L(" cansetmtu")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfHasHardwareAddr ? t.AppendFormat( _L(" hardwareaddr")) : t.AppendFormat( _L(""));
            info().iFeatures & KIfCanSetHardwareAddr ? t.AppendFormat( _L(" cansethardwareaddr")) : t.AppendFormat( _L(""));
            t.AppendFormat( _L("\n"));

            TName address;
            info().iAddress.Output( address );
            t.Format( _L("Addr: %S\n"), &address );

            if (info().iAddress.IsLinkLocal())
                {
                t.AppendFormat( _L("  -link local\n"));
                }
            else if (info().iAddress.IsSiteLocal())
                {
                t.AppendFormat( _L("  -site local\n"));
                }
            else
                {
                t.AppendFormat( _L("  -global\n"));
                }
            PrintTextToConsole( t );

            info().iBrdAddr.Output( address );
            info().iDefGate.Output( address );
            t.Format( _L("Gatew: %S\n"), &address );
            info().iNameSer1.Output( address );
            t.AppendFormat( _L("DNS 1: %S\n"), &address );
            info().iNameSer2.Output( address );
            t.AppendFormat( _L("DNS 2: %S\n"), &address );
            PrintTextToConsole( t );
            
            if (info().iHwAddr.Family() != KAFUnspec)
                {
                PrintTextToConsole( _L("Hardware address "));
                TUint j;
                for (j = sizeof(SSockAddr); j < sizeof(SSockAddr) + 6; ++j)
                    {
                    if (j < (TUint)info().iHwAddr.Length())
                        {
                        t.Format( _L("%02X"), info().iHwAddr[j] );
                        PrintTextToConsole( t );
                        }
                    else
                        {
                        PrintTextToConsole( _L("??"));
                        }
                    if (j < sizeof(SSockAddr) + 5)
                        {
                        PrintTextToConsole( _L("-"));
                        }
                    else
                        {
                        PrintTextToConsole( _L("\n"));
                        }
                    }
                }
            }
        if (res == KErrNone)
            {
            info = next;
            PrintTextToConsole( _L("\n"));
            }
        else
            {
            PrintTextToConsole( _L("\n"));
            }
        }
    t.Format( _L("Total %d interfaces\n"),count );
    PrintTextToConsole( t );

    sock.Pop();
    ss.Pop();
    }

// ---------------------------------------------------------
// CSocketsEngine::SetWLANQoS()
// Set QoS for WLAN
// ---------------------------------------------------------
//
void CSocketsEngine::SetWLANQoS( TInt aClass )
    {
    TInt opt;
    TInt err;
    TBuf<64> t;
    
    if (iEngineStatus != EConnected)
        {
        t.Copy( _L("\nConnect first!\n"));
        PrintTextToConsole( t );
        return;
        }

    err = iSocket.GetOpt( KSoIpTOS, KSolInetIp, opt );

    opt = opt >> 2;

    if (err == KErrNone)
        {
        t.Format( _L("Old TOS: %x\n"), opt );
        }
    else
        {
        t.Format( _L("Cannot get TOS opt\n"));
        }

    PrintTextToConsole( t );

    switch (aClass)
        {
        case 7:
            err = iSocket.SetOpt( KSoIpTOS, KSolInetIp, 0xE0 );
            break;
        case 5:
            err = iSocket.SetOpt( KSoIpTOS, KSolInetIp, 0xA0 );
            break;
        case 3:
            err = iSocket.SetOpt( KSoIpTOS, KSolInetIp, 0x60 );
            break;
        case 1:
            err = iSocket.SetOpt( KSoIpTOS, KSolInetIp, 0x20 );
            break;
        case 0:
            err = iSocket.SetOpt( KSoIpTOS, KSolInetIp, 0x0 );
            break;
        default:
            break;
        }

    err = iSocket.GetOpt( KSoIpTOS, KSolInetIp, opt );

    opt = opt >> 2;

    if (err == KErrNone)
        {
        t.Format( _L("New TOS: %x\n"), opt );
        }
    else
        {
        t.Format( _L("Cannot set TOS opt\n"));
        }
    
    PrintTextToConsole( t );
    
    }

// ---------------------------------------------------------
// CSocketsEngine::QoS1()
// 
// ---------------------------------------------------------
//
void CSocketsEngine::QoS1()
    {
    /*TQoSSelector selector;
     selector.setAddr(iSocket);
     
     RQoSPolicy policy;
     policy.Open(selector);
     
     CQosParameters* parameters = GetparametersL();
     policy.NotifyEvent(iObserver);
     policy.SetQoS(*parameters);
     
     policy.GetQoS();
     policy.Close();
     */
    }

// ---------------------------------------------------------
// CSocketsEngine::QoS2()
// 
// ---------------------------------------------------------
//
void CSocketsEngine::QoS2()
    {
    /*CQosParameters* parameters = GetparametersL();
     
     RQoSChannel channel;
     
     channel.Open(iSocket);
     channel.NotifyEvent(iObserver);
     channel.SetQoS(*parameters);
     
     channel.Join(iSocket2); 
     
     channel.Leave(ISocket);
     
     channel.Close();
     */
    }
// ----------------------------------------------------------------------------
// CSocketsEngine::SendDataL
// Start the sending of the data
// ----------------------------------------------------------------------------
//
void CSocketsEngine::SendDataL( const TBuf8<KSendDataSize> aData )
    {
    RDebug::Print( _L("ConnTest: SendDataL: size = %d"),aData.Length() );
    iSendCount = 0;
    iData->Des().Copy( aData );
    iData->Des().ZeroTerminate();
    iSocket.SetOpt( KSoUdpSynchronousSend, KSolInetUdp, 1 );

    iTroughputDataSize = 0;
    iThroughputStartTime.UniversalTime();
    
    DoSendDataL();
    }

// ----------------------------------------------------------------------------
// CSocketsEngine::DoSendDataL
// Do the actual sending of the data
// ----------------------------------------------------------------------------
//
void CSocketsEngine::DoSendDataL()
    {
    if (iSendCount++ < (iSettingData->iPackets))
        {
        // Delay is given in milliseconds, timer uses microseconds.
        if (iSettingData->iDelay != 0)
            {
            if (iSettingData->iProtocol == 1) //  1 == UDP here. Only way to send udp is raw data
                {
                RDebug::Print(
                        _L("ConnTest: DoSendDataL: inserting seq number %d"),iSendCount );
                TUint32* seqNumberPointer = (TUint32*)(iData->Des().Ptr());
                *seqNumberPointer = ByteOrder::Swap32( iSendCount ); // put sequence number into to the packet
                }
            WriteL( *iData );
            iSendTimer->After( (iSettingData->iDelay) * 1000 );
            }
        else
            {
            WriteFloodL( *iData, iSettingData->iPackets );
            }
        }
    }

// ---------------------------------------------------------
// Notifys when a packet has been sent
// aAmount is th amount of data that has been sent
// ---------------------------------------------------------
//
void CSocketsEngine::NotifySend( TInt aAmount )
    {
    if (iThroughputStartTime != 0)
        {
        
        iTroughputDataSize += aAmount;
        TInt size = iSettingData->iPacketSize;
        if (iSettingData->iPacketSize > KSendDataSize)
            {
            size = KSendDataSize;
            }
        // We add 1 for each packet since the create packet appends an extra \n
        // after each packet
        if (iTroughputDataSize == (iSettingData->iPackets * size))
        // + iSettingData->iPackets) )
            {
            TBuf8<256> text;
            Utils::CalculateThroughput( text, iThroughputStartTime,
                    iTroughputDataSize );
            iThroughputStartTime = 0;
            iConsole.PrintNotify( text );
            iTroughputDataSize = 0;
            }
        }
    }

// ---------------------------------------------------------
// Marks starting time into memory
//
// ---------------------------------------------------------
//
void inline CSocketsEngine::StartTickCount()
    {
    if (!iUseTTime)
        {
        iStartTime = User::FastCounter();
        }
    else
        {
        iStartTTime.UniversalTime();
        iStartTime = 1; // Non null. iStartTime is also used as flag.
        }
    
    }

// ---------------------------------------------------------
// Calculates time interval using tick count and prints it
//
// ---------------------------------------------------------
//
void CSocketsEngine::StopTickCount( const TDesC& aComponentName )
    {
    TBuf<128> text;
    if (!iUseTTime)
        {
        TUint32 currentTime = User::FastCounter();
        currentTime = currentTime - iStartTime;

        if (iStartTime == 0)
            {
            return; //do not print if start time hasn't been initialized.
            }
        iStartTime = 0;

        TInt freq = 0;
        TInt err = HAL::Get( HAL::EFastCounterFrequency, freq );
        if (err != KErrNone || freq == 0)
            {
            text.Format( _L("FastCounter error: %d \n"),err );
            }
        else
            {
            currentTime = (currentTime * 1000.0) / freq;
            text.Format( _L("%u ms by "), currentTime );
            text.Append( aComponentName );
            text.Append( _L("\n"));
            }
        }
    else
        {
        TTime currentTime;
        currentTime.UniversalTime();

        if (iStartTime == 0)
            {
            return; //do not print if start time hasn't been initialized.
            }
        TTimeIntervalMicroSeconds interval = currentTime.MicroSecondsFrom(
                iStartTTime );
        iStartTime = 0;

        TBuf<32> app;
        app.Format( _L("%u ms by "), interval.Int64() / 1000 );
        text.Append( app );
        text.Append( aComponentName );
        }
    
    PrintTextToConsole( text );
    }

// end of file
