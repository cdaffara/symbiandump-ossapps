/*
 * Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: The view class for ConnTest
* Implements command handling, menu initializing and view activation
*
*/

// INCLUDE FILES
#include <aknViewAppUi.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <AknGlobalNote.h>

#include <conntest.rsg>
#include "ConnTestView.h"
#include "ConnTestContainer.h"
#include "CustomPrefsItemList.h"
#include "CustomPrefsData.h"
#include "SettingItemList.h"
#include "SettingData.h"
#include "SocketsEngine.h"
#include "Utils.h"
#include <wlanmgmtclient.h>
#include "ConnTestAppUi.h"


// CONSTANTS


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CConnTestView::CConnTestView(const TUid aViewId)
// Constructor
// ---------------------------------------------------------
//
CConnTestView::CConnTestView(const TUid aViewId)
    : iViewId(aViewId), iWaiter( NULL )
    {
    RDebug::Print(_L("ConnTest: CConnTestView::CConnTestView( aViewId )"));
    }

// ---------------------------------------------------------
// CConnTestView::CConnTestView(const TUid aViewId,
//                              CSettingData* aSettings,
//                              CCustomPrefsData* aCustomPrefs)
// Constructor
// ---------------------------------------------------------
//
CConnTestView::CConnTestView(const TUid aViewId, CSettingData* aSettings, CCustomPrefsData* aCustomPrefs)
    : iViewId(aViewId), iSettingData(aSettings), iCustomPrefsData(aCustomPrefs), iWaiter( NULL )
    {
    RDebug::Print(_L("ConnTest: CConnTestView::CConnTestView( aViewId, aSettings, aCustomPrefs )"));
    }

// ---------------------------------------------------------
// CConnTestView::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CConnTestView::ConstructL()
    {
    RDebug::Print(_L("ConnTest: CConnTestView::ConstructL"));
    BaseConstructL( R_CONNTEST_VIEW1 );
    iSendData = HBufC8::NewL(KMaxSendBuffer);
    RDebug::Print(_L("ConnTest: CConnTestView::ConstructL iWlanMgmtClient"));
    TRAPD(err,iWlanMgmtClient = CWlanMgmtClient::NewL());
    if (err == KErrNone)
        {
        iSupportsWlan = ETrue;
        }
    else
        {
        iSupportsWlan = EFalse;
        }
    }

// ---------------------------------------------------------
// CConnTestView::~CConnTestView()
// Destructor
// ---------------------------------------------------------
//
CConnTestView::~CConnTestView()
    {
    RDebug::Print(_L("ConnTest: CConnTestView::~CConnTestView"));
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iSocketsEngine;
    delete iContainer;
    delete iSettingsListBox;
    delete iCustomPrefsListBox;
    //    delete iSettingData; owned now by CConnTestDocument
    if(iSendData)
        {
        delete iSendData;
        iSendData = NULL;   
        
        if ( iSupportsWlan && iWlanMgmtClient )
            {
            iWlanMgmtClient->CancelNotifications();
            delete iWlanMgmtClient;
            }           
        }
    }

// ---------------------------------------------------------
// TUid CConnTestView::Id()
// Return id of this view instance
// ---------------------------------------------------------
//
TUid CConnTestView::Id() const
    {
    //return KViewId;
    return iViewId;
    }

// ---------------------------------------------------------
// CConnTestView::HandleCommandL(TInt aCommand)
// Command handler
// ---------------------------------------------------------
//
void CConnTestView::HandleCommandL(TInt aCommand)
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
            iSocketsEngine->Cancel();
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            
        case EAknSoftkeyOk:       
        case EAknSoftkeyBack:
            // Same actions for OK and Back keys. These are called
            // from settings or Ext Connection Preferences view.
            if (iActiveContainer == ESettingsContainer)
                {
                // Store settings and update the values
                // in socket engine.
                iSettingsListBox->StoreSettingsL();
                iSocketsEngine->SetConnectionData(iSettingData);
                }
            else // ECustomPrefsContainer
                {
                // Store the custom preferences and update the values
                // in socket engine.
                iCustomPrefsListBox->StoreSettingsL();
                iSocketsEngine->SetCustomPrefsData(iCustomPrefsData);                
                }
            SwapContainerL(EOutputContainer);
            break;
            
        case EConnTestCmdStartInterface:
            // Start the interface
            iSocketsEngine->StartInterfaceL(CSocketsEngine::EAskIap);
            break;
            
        case EConnTestCmdStartSnap:
            //start connection with snap
            iSocketsEngine->StartConnWithSnapL();
            break;
            
        case EConnTestCmdStartInterfaceExtConnPrefs:
            // Start the interface using Extended Connection Preferences
            RDebug::Print(_L("ConnTest: Start ExtConnPrefs"));
            iSocketsEngine->StartInterfaceL(CSocketsEngine::EExtPrefs);
            break;
            
        case EConnTestCmdStartInternet:
            // Start the interface using Extended Connection Preferences
            RDebug::Print(_L("ConnTest: Start Internet"));
            iSocketsEngine->StartInterfaceL(CSocketsEngine::EExtPrefsInternet);
            break;
                
        case EConnTestCmdStartInterfaceNoPrefs:
            // Start the interface
            iSocketsEngine->StartInterfaceL(CSocketsEngine::ENoPrefs);
            break;
            
        case EConnTestCmdStartByNetworkId:
            // Start the interface with network id
            iSocketsEngine->StartInterfaceL(CSocketsEngine::ENetworkId);
            break;
            
        case EConnTestCmdStopInterface:
            // Stop the interface with RConnection::Stop
            iSocketsEngine->StopInterfaceL();
            break;
            
        case EConnTestCmdStartForClose:
            // Stop the interface with RConnection::Close
            iSocketsEngine->StartCloseInterfaceL();
            break;
            
        case EConnTestCmdCloseInterface:
            // Stop the interface with RConnection::Close
            iSocketsEngine->CloseInterface();
            break;
            
        case EConnTestCmdConnectionInfo:
            // Print information about the active connections
            iSocketsEngine->ConnectionInfoL();
            break;
            
        case EConnTestCmdInterfaceInfo:
            // Print information about interfaces. Show wait note here,
            // because this usually takes a while.
            {            
            TInt permanentNoteId;
            CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
            CleanupStack::PushL(globalNote);
            globalNote->SetSoftkeys(R_AVKON_SOFTKEYS_EMPTY);
            permanentNoteId = globalNote->ShowNoteL( EAknGlobalWaitNote, _L("Fetching information...") );
            
            iSocketsEngine->InterfaceInfoL();
            
            globalNote->CancelNoteL(permanentNoteId);
            CleanupStack::PopAndDestroy(globalNote); // globalNote
            }
            break;
            
        case EConnTestCmdClearOutputWindow:
            // Clear the output window
            iContainer->ClearOutputWindow();
            break;
            
        case EConnTestCmdSaveOutputWindow:
            iContainer->SaveOutputWindowToFile();
            break;
            
        case EConnTestCmdSettings:
            // Display settings list
            SwapContainerL(ESettingsContainer);
            break;
            
        case EConnTestCmdSetCustomPrefs:
            // Display Custom Preferences list
            SwapContainerL(ECustomPrefsContainer);
            break;
            
        case EConnTestCmdMigrate:
            iSocketsEngine->Migrate();
            break;
        case EConnTestCmdIgnore:
            iSocketsEngine->Ignore();
            break;
        case EConnTestCmdAccept:
            iSocketsEngine->AcceptCarrier();
            break;
        case EConnTestCmdReject:
            iSocketsEngine->RejectCarrier();
            break;
        case EConnTestCmdConnect:
            // Connect the socket
            iSocketsEngine->ConnectL();
            break;
            
        case EConnTestCmdDisconnect:
            // Disconnect the socket but do not stop the interface
            iSocketsEngine->Disconnect();
            break;
            
        case EConnTestCmdListen:
            // Create TCP socket that listens and receives raw TCP data
            iSocketsEngine->ListenL();
            break;
            
        case EConnTestCmdSendHttpGet:
            {
            // Send HTTP Get
            iSocketsEngine->SendHttpRequestOverSocketL(EFalse, EFalse);
            }
            break;
            
        case EConnTestCmdSendPerformanceHttpGet:
            {
            // Send HTTP Get
            if (iSocketsEngine->GetSocketEngineState() == CSocketsEngine::EConnected)
                {
                iSocketsEngine->SendHttpRequestOverSocketL(EFalse, ETrue);
                }
            }
            break;
            
        case EConnTestCmdSendHttpPost:
            {
            // Send HTTP Post
            iSocketsEngine->SendHttpRequestOverSocketL(ETrue, ETrue);
            }
            break;
            
        case EConnTestCmdSendRawData:
            // Send datagrams
            CreateDataPacket();
            SendDataL();
            break;
            
        case EConnTestCmdHttpFrameworkGet:
            // Send HTTP request (for testing the HTTP framework)
            iSocketsEngine->SendHttpFrameworkRequestL(EFalse, EFalse, EFalse);
            break;
            
        case EConnTestCmdHttpFrameworkPost:
            // Send HTTP request (for testing the HTTP framework)
            iSocketsEngine->SendHttpFrameworkRequestL(ETrue, EFalse, EFalse);
            break;
            
        case EConnTestCmdHttpFrameworkSecureGet:
            // Send HTTP request (for testing the HTTP framework)
            iSocketsEngine->SendHttpFrameworkRequestL(EFalse, EFalse, ETrue);
            break;
            
        case EConnTestCmdHttpFrameworkPerformanceGet:
            // Send HTTP request (for testing the HTTP framework)
            iSocketsEngine->SendHttpFrameworkRequestL(EFalse, ETrue, EFalse);
            break;
            
        case EConnTestCmdHttpFrameworkPerformancePost:
            // Send HTTP request (for testing the HTTP framework)
            iSocketsEngine->SendHttpFrameworkRequestL(ETrue, ETrue, EFalse);
            break;
            
        case EConnTestCmdCancel:
            // Cancel ongoing operation
            iSocketsEngine->Cancel();
            break;
            
        case EConnTestCmdConnectionPrefsIap:
            // Write IapId value to ConnectionPreferences table
            TRAPD(err, Utils::SetConnectionPrefsIapL());
            if(err)
                {
                if(err == KErrNotFound)
                    {
                    iContainer->ErrorNotify(_L("Non-existing id"), err);
                    }
                else
                    {
                    iContainer->ErrorNotify(_L("Setting id failed"), err);
                    }
                }
            break;
            
        case EConnTestCmdTimeoutLastSocketActivity:
            // Set LastSocketActivity
            TRAPD(err2, Utils::SetTimeoutsL(EConnTestCmdTimeoutLastSocketActivity));
            if(err2)
                {
                iContainer->ErrorNotify(_L("Failed to set timer"), err2);
                }        
            break;
            
        case EConnTestCmdTimeoutLastSocketClosed:
            // Set LastSocketClosed
            TRAPD(err3, Utils::SetTimeoutsL(EConnTestCmdTimeoutLastSocketClosed));
            if(err3)
                {
                iContainer->ErrorNotify(_L("Failed to set timer"), err3);
                }        
            break;
            
        case EConnTestCmdTimeoutLastSessionClosed:
            // Set LastSessionClosed
            TRAP(err, Utils::SetTimeoutsL(EConnTestCmdTimeoutLastSessionClosed));
            if(err)
                {
                iContainer->ErrorNotify(_L("Failed to set timer"), err);
                }        
            break;
            
        case EConnTestCmdWLANQoSVoice:
            iSocketsEngine->SetWLANQoS(7);
            break;
            
        case EConnTestCmdWLANQoSVideo:
            iSocketsEngine->SetWLANQoS(5);
            break;
            
        case EConnTestCmdWLANQoSBestEffort:
            iSocketsEngine->SetWLANQoS(3);
            break;
            
        case EConnTestCmdWLANQoSBackground:
            iSocketsEngine->SetWLANQoS(1);
            break;
            
        case EConnTestCmdWLANQoSNone:
            iSocketsEngine->SetWLANQoS(0);
            break;
            
            
            //
            // WLAN COMMANDS
            // 
        case EConnTestWlanAvailableIaps:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            iWaiter = new (ELeave) CConnTestViewWaiter( iWlanMgmtClient, *this, EConnTestWaitTypeAvailableIaps );
            iWaiter->ConstructL();
            iWaiter->IssueRequest();
            }
            break;
            
        case EConnTestWlanScan:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            iWaiter = new (ELeave) CConnTestViewWaiter( iWlanMgmtClient, *this, EConnTestWaitTypeScan );
            iWaiter->ConstructL();
            iWaiter->IssueRequest();
            }
            break;
            
        case EConnTestWlanGetSsid:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            TWlanSsid ssid;
            TInt ret = iWlanMgmtClient->GetConnectionSsid( ssid );
            if( ret )
                {
                RDebug::Print( _L( "ConnTest: Error: %d" ), ret );
                break;
                }
            
            RDebug::Print( _L( "ConnTest: SSID = %S" ), &ssid );
            
            TBuf8<64> text;
            text.Append(_L8("SSID: "));
            text.Append(ssid);
            text.Append(_L8("\n"));
            iContainer->PrintNotify(text);
            }
            break;
            
        case EConnTestWlanGetBssid:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            TWlanBssid bssid;
            TInt ret = iWlanMgmtClient->GetConnectionBssid( bssid );
            if( ret )
                {
                RDebug::Print( _L( "ConnTest: Error: %d" ), ret );
                break;
                }
            
            RDebug::Print( _L( "ConnTest: BSSID = %S" ), &bssid );
            
            TBuf8<64> text;
            text.AppendFormat(_L8("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n"), bssid[0], bssid[1], bssid[2], bssid[3],bssid[4], bssid[5] );
            iContainer->PrintNotify(text);
            }
            break;
            
        case EConnTestWlanGetRssi:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            TInt32 rssi32;
            TInt ret = iWlanMgmtClient->GetConnectionSignalQuality( rssi32 );
            if( ret )
                {
                RDebug::Print( _L( "ConnTest: Error: %d" ), ret );
                break;
                }
            
            RDebug::Print( _L( "ConnTest: RSSI = %d" ), rssi32 );
            
            TInt8 rssi8 = static_cast<TInt8>(rssi32);
            TBuf8<64> text;
            text.AppendFormat(_L8("RSSI: %d\n"), rssi8);
            iContainer->PrintNotify(text);
            }
            break;
            
        case EConnTestWlanGetConnMode:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            TWlanConnectionMode connMode;
            TInt ret = iWlanMgmtClient->GetConnectionMode( connMode );
            if( ret )
                {
                RDebug::Print( _L( "ConnTest: Error: %d" ), ret );
                break;
                }
            
            RDebug::Print( _L( "ConnTest: ConnectionMode = %d" ), connMode );
            
            TBuf8<64> text;
            text.Append(_L8("Mode: "));
            switch( connMode )
                {
                case EWlanConnectionModeNotConnected:
                    text.Append(_L8("Not Connected"));
                    break;
                case EWlanConnectionModeInfrastructure:
                    text.Append(_L8("Infrastructure"));
                    break;
                case EWlanConnectionModeAdhoc:
                    text.Append(_L8("Ad hoc"));
                    break;
                case EWlanConnectionModeSecureInfra:
                    text.Append(_L8("SecureInfra"));
                    break;
                case EWlanConnectionModeSearching:
                    text.Append(_L8("Searching"));
                    break;
                }
            text.Append(_L8("\n"));
            iContainer->PrintNotify(text);
            }
            break;
            
        case EConnTestWlanGetSecurityMode:
            {
            if (!iSupportsWlan) { iContainer->PrintNotify(_L("WLAN not supported\n")); break; }
            TWlanConnectionSecurityMode securityMode;
            TInt ret = iWlanMgmtClient->GetConnectionSecurityMode( securityMode );
            if( ret )
                {
                RDebug::Print( _L( "ConnTest: Error: %d" ), ret );
                break;
                }
            
            RDebug::Print( _L( "ConnTest: SecurityMode = %d" ), securityMode );
            
            TBuf8<64> text;
            text.Append(_L8("Mode: "));
            switch( securityMode )
                {
                case EWlanConnectionSecurityOpen:
                    text.Append(_L8("Open"));
                    break;
                case EWlanConnectionSecurityWep:
                    text.Append(_L8("WEP"));
                    break;
                case EWlanConnectionSecurity802d1x:
                    text.Append(_L8("802.1X"));
                    break;
                case EWlanConnectionSecurityWpa:
                    text.Append(_L8("WPA"));
                    break;
                case EWlanConnectionSecurityWpaPsk:
                    text.Append(_L8("WPA-PSK"));
                    break;
                }
            text.Append(_L8("\n"));
            iContainer->PrintNotify(text);
            }
            break;
            
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CConnTestView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CConnTestView::SizeChanged()
    {
    if(iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    if(iSettingsListBox)
        {
        iSettingsListBox->SetRect(ClientRect());
        }
    }

// ---------------------------------------------------------
// CConnTestView::DoActivateL(...)
// Activation of this view instance.
// ---------------------------------------------------------
//
void CConnTestView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
                                 const TDesC8& /*aCustomMessage*/)
    {
    if(!iContainer)
        {
        RDebug::Print(_L("ConnTest: CConnTestView::DoActivateL iContainer"));
        iContainer = new (ELeave) CConnTestContainer;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        //AppUi()->AddToStackL( *this, iContainer );
        } 
    AppUi()->AddToStackL( *this, iContainer );
    
    // Connection data for this view
    if(!iSettingData)
        {
        // Note! Document class owns now settings, so that they can
        // be stored into file.
        //iSettingData = CSettingData::NewL(); owned now by CConnTestDocument
        }
    
    // Create socket engine with initial data
    if(!iSocketsEngine) 
        {
        RDebug::Print(_L("ConnTest: CConnTestView::DoActivateL iSocketsEngine"));
        iSocketsEngine = CSocketsEngine::NewL(*iContainer);
        iSocketsEngine->SetConnectionData(iSettingData);
        }
    
    // Create settings list
    if(!iSettingsListBox)
        {
        RDebug::Print(_L("ConnTest: CConnTestView::DoActivateL iSettingsListBox"));
        CreateSettingsListBoxL(R_CONNTEST_SETTING_LIST);
        }    

    // Create Custom Preferences list
    if(!iCustomPrefsListBox)
        {
        RDebug::Print(_L("ConnTest: CConnTestView::DoActivateL iCustomPrefsListBox"));
        CreateCustomPrefsListBoxL(R_CONNTEST_CUSTOM_PREFS_LIST);
        }
    
    iActiveContainer = EOutputContainer;
    iContainer->MakeVisible(ETrue);
    SizeChanged();
    
    if ( iSupportsWlan )
        {
        RDebug::Print(_L("ConnTest: CConnTestView::DoActivateL iWlanMgmtClient->ActivateNotificationsL"));
        iWlanMgmtClient->ActivateNotificationsL( *this );
        }        
    }

// ---------------------------------------------------------
// CConnTestView::DoDeactivate()
// Deactivation of this view instance. Controls are not 
// deleted, so that old information in status and output
// windows stays visible.
// ---------------------------------------------------------
//
void CConnTestView::DoDeactivate()
    {
    if ( iSupportsWlan )
        {
        iWlanMgmtClient->CancelNotifications();
        }        
    
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    iContainer->MakeVisible(EFalse);
    
    if (iSettingsListBox)
        {
        AppUi()->RemoveFromStack(iSettingsListBox);
        }
    }


// ----------------------------------------------------------------------------
// CConnTestView::SwapContainerL
// Switch the display between main view and settings list.
// ----------------------------------------------------------------------------
//
void CConnTestView::SwapContainerL(TConnTestContainer aActiveContainer)
    {
    CAknTitlePane* titlePane = STATIC_CAST(
        CAknTitlePane*,
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
    if (iActiveContainer == aActiveContainer)
        {
        return;
        }
    
    // Remove the old container...
    switch (iActiveContainer)
        {
        case EOutputContainer:
            {
            iContainer->MakeVisible(EFalse);
            AppUi()->RemoveFromStack( iContainer );
            break;
            }
        case ESettingsContainer:
            {
            AppUi()->RemoveFromStack(iSettingsListBox);
            iSettingsListBox->MakeVisible(EFalse);
            break;
            }
        default: // ECustomPrefsContainer:
            {
            AppUi()->RemoveFromStack(iCustomPrefsListBox);
            iCustomPrefsListBox->MakeVisible(EFalse);
            break;
            }
        }
    
    // And add the new container
    switch (aActiveContainer)
        {
        case EOutputContainer:
            {
            Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
            AppUi()->AddToStackL( *this, iContainer );
            iContainer->MakeVisible(ETrue);
            titlePane->SetTextToDefaultL();
            
            Cba()->DrawNow();
            break;
            }
        case ESettingsContainer:
            {
            Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_OK_BACK);
            AppUi()->AddToStackL( *this, iSettingsListBox );
            iSettingsListBox->MakeVisible(ETrue);
            titlePane->SetTextL( _L("Settings") );
            break;
            }
        default: // ECustomPrefsContainer:
            {
            Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_OK_BACK);
            AppUi()->AddToStackL( *this, iCustomPrefsListBox );
            iCustomPrefsListBox->MakeVisible(ETrue);
            titlePane->SetTextL( _L("Custom Preferences") );
            break;
            }
        }
    
    iActiveContainer = aActiveContainer;
    }

// ----------------------------------------------------------------------------
// CConnTestView::CreateSettingsListBoxL
// Create the setting item list, but don't show it.
// ----------------------------------------------------------------------------
//
void CConnTestView::CreateSettingsListBoxL(TInt aResourceId)
    {
    iSettingsListBox = new (ELeave) CSettingItemList;
    iSettingsListBox->SetData(iSettingData);
    iSettingsListBox->SetMopParent(this);
    iSettingsListBox->ConstructFromResourceL(aResourceId);
    iSettingsListBox->MakeVisible(EFalse);
    iSettingsListBox->SetRect(ClientRect());
    iSettingsListBox->ActivateL();
    }

// ----------------------------------------------------------------------------
// CConnTestView::CreateCustomPrefsListBoxL
// Create the Custom Prefs item list, but don't show it.
// ----------------------------------------------------------------------------
//
void CConnTestView::CreateCustomPrefsListBoxL(TInt aResourceId)
    {
    iCustomPrefsListBox = new (ELeave) CCustomPrefsItemList;
    iCustomPrefsListBox->SetData(iCustomPrefsData);
    iCustomPrefsListBox->SetMopParent(this);
    iCustomPrefsListBox->ConstructFromResourceL(aResourceId);
    iCustomPrefsListBox->MakeVisible(EFalse);
    iCustomPrefsListBox->SetRect(ClientRect());
    iCustomPrefsListBox->ActivateL();
    }


// ----------------------------------------------------------------------------
// CConnTestView::CreateDataPacket
// Write data to internal buffer, which is sent to socket. Maximum
// size of the buffer is KSendDataSize bytes.
// ----------------------------------------------------------------------------
//
void CConnTestView::CreateDataPacket()
    {
    RDebug::Print(_L("ConnTest: CConnTestView::CreateDataPacket: packet size = %d, SendDataSize = %d"),iSettingData->iPacketSize,KSendDataSize);
    RDebug::Print(_L("ConnTest: CConnTestView::CreateDataPacket: buffer maxsize = %d"),iSendData->Des().MaxLength());
    
    iSendData->Des().FillZ();
    iSendData->Des().Zero(); 
    TInt length = iSettingData->iPacketSize;
    if(length > iSendData->Des().MaxLength())
        {
        TBuf<128> tt;
        tt.Format(
            _L("There was not enough space in the buffer for the data, so truncating to %d"),
            iSendData->Des().MaxLength()
            );
        iContainer->PrintNotify(tt);
        length = iSendData->Des().MaxLength();
        iSettingData->iPacketSize = length;
        }
    iSendData->Des().Fill('a', iSettingData->iPacketSize - 1);
    iSendData->Des().Append(_L8("\n"));
    RDebug::Print(_L("ConnTest: CConnTestView::CreateDataPacket: created size = %d"),iSendData->Length());
    }


// ----------------------------------------------------------------------------
// CConnTestView::SendDataL
// The engine does the real work
// ----------------------------------------------------------------------------
//
void CConnTestView::SendDataL()
    {
    iSocketsEngine->SendDataL(*iSendData);
    }

// ------------------------------------------------------------------------------
// CConnTestView::IsReadyForHTTPGet()
// This function is used to check if a HTTP get can be performed
// on an already existing connection
// ------------------------------------------------------------------------------
//
TBool CConnTestView::IsReadyForHTTPGet()
    {
    TBool res = EFalse;
    if (iSocketsEngine && iSocketsEngine->GetSocketEngineState() == CSocketsEngine::EConnected )
        {
        res = ETrue;
        }
    return res;
    }

// ------------------------------------------------------------------------------
// CConnTestView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
// This function is called by the EIKON framework just before it displays
// a menu pane. Menu is adjusted according to socket engine state.
// ------------------------------------------------------------------------------
//
void CConnTestView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
    {   
    RDebug::Print(_L("ConnTest: DynInitMenuPaneL: resourceId=%x"), aResourceId);
    
    switch(aResourceId)
        {
        case R_CONNTEST_VIEW1_MENU:
            {
            CSocketsEngine::TSocketsEngineState state = iSocketsEngine->GetSocketEngineState();
            CSocketsEngine::TRoamingUIState roamingState = iSocketsEngine->GetSocketEngineRoamingState();
            
            switch(state)
                {
                case CSocketsEngine::EInterfaceDown:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, EFalse);
                    break;
                case CSocketsEngine::EStartingInterface:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, ETrue);     
                    break;
                case CSocketsEngine::EInterfaceUp:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, EFalse);     
                    break;
                case CSocketsEngine::EConnected:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, EFalse);                
                    break;
                case CSocketsEngine::EConnecting:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);                
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, ETrue);                
                    break;
                case CSocketsEngine::EListening:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, ETrue);
                    break;               
                case CSocketsEngine::EDisconnecting:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, ETrue);                
                    break;
                case CSocketsEngine::ELookingUp:
                    aMenuPane->SetItemDimmed(EConnTestCmdCancel, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdStartInternet, ETrue);                
                    aMenuPane->SetItemDimmed(EConnTestCmdStopInterface, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdConnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdDisconnect, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdListen, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdSendRawData, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdHttpFrameworkGet, ETrue);                
                    break;
                default:
                    break;
                }
            switch (roamingState)
                {
                case CSocketsEngine::EPendingPreferredCarrier:
                    aMenuPane->SetItemDimmed(EConnTestCmdMigrate, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdIgnore, EFalse);
                    aMenuPane->SetItemDimmed(EConnTestCmdAccept, ETrue);                
                    aMenuPane->SetItemDimmed(EConnTestCmdReject, ETrue);
                    break;
                case CSocketsEngine::EPendingNewCarrierActive:
                    aMenuPane->SetItemDimmed(EConnTestCmdMigrate, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdIgnore, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdAccept, EFalse);                
                    aMenuPane->SetItemDimmed(EConnTestCmdReject, EFalse);
                    break;
                default:
                    aMenuPane->SetItemDimmed(EConnTestCmdMigrate, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdIgnore, ETrue);
                    aMenuPane->SetItemDimmed(EConnTestCmdAccept, ETrue);                
                    aMenuPane->SetItemDimmed(EConnTestCmdReject, ETrue);
                    break;
                }
            }
            break;
        case R_CONNTEST_SUB_MENU_IPDUMP:
            {
            
            // Additional menupane handling, which doesn't depend on the state of the socket engine
            CConnTestAppUi* appui = (CConnTestAppUi *)AppUi();
            if(appui->IsLogging())
                {
                aMenuPane->SetItemDimmed(EConnTestCmdStartIpLog, ETrue);     
                }
            else
                {
                aMenuPane->SetItemDimmed(EConnTestCmdStopIpLog, ETrue);     
                }
            }
            break;   
            
            //enable mobility API submenupoints when connection is started with a snap
        case R_CONNTEST_SUB_MENU_ADVANCED:
            {
            
            CSocketsEngine::TSocketsEngineStartType connType = iSocketsEngine->GetSocketEngineConnType();
            
            
            break;
            
            }
        default:
            break;         
        }
    }

// ------------------------------------------------------------------------------
// CConnTestView::RequestCompleted()
// ------------------------------------------------------------------------------
//
void CConnTestView::RequestCompleted( TInt aStatus, RArray<TUint>& aAvailableIaps )
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::RequestCompleted(aAvailableIaps)" ) );
    
    if ( aStatus != KErrNone )
        {
        RDebug::Print( _L( "ConnTest: client->RequestCompleted failed with %d" ), aStatus );
        return;                
        }
    
    RDebug::Print( _L( "ConnTest: ivailableIaps.Count() = %u" ), aAvailableIaps.Count() );
    
    TBuf<100> msg;
    msg.AppendNum( aAvailableIaps.Count() );
    msg.Append( _L( " WLAN IAP(s) found\n\n" ) );
    for ( TUint idx( 0 ); idx < (TUint)aAvailableIaps.Count(); ++idx )
        {
        msg.AppendFormat( _L("%02u "), aAvailableIaps[idx] );
        }
    msg.Append( _L( "\n" ) );
    iContainer->ErrorNotify( msg, KErrNone );
    
    delete iWaiter;
    iWaiter = NULL;
    }

// ------------------------------------------------------------------------------
// CConnTestView::RequestCompleted()
// ------------------------------------------------------------------------------
//
void CConnTestView::RequestCompleted( TInt aStatus, CWlanScanInfo& aScanInfo )
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::RequestCompleted(aScanInfo)" ) );
    
    if ( aStatus != KErrNone )
        {
        RDebug::Print( _L( "ConnTest: client->RequestCompleted failed with %d" ), aStatus );
        return;                
        }
    
    TUint count( 0 );
    
    aScanInfo.First();
    while (!aScanInfo.IsDone())
        {
        ++count;
        aScanInfo.Next();
        }
    
    TBuf<100> msg;
    msg.AppendNum( count );
    msg.Append( _L( " WLAN AP(s) found\n\n" ) );
    iContainer->ErrorNotify( msg, KErrNone );                 
    
    delete iWaiter;
    iWaiter = NULL;
    }

// ------------------------------------------------------------------------------
// CConnTestView::ConnectionStateChanged()
// ------------------------------------------------------------------------------
//
void CConnTestView::ConnectionStateChanged( TWlanConnectionMode aNewState )
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::ConnectionStateChanged" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.Append(_L8("Wlan State: "));
    switch( aNewState )
        {
        case EWlanConnectionModeNotConnected:
            text.Append(_L8("Not Connected"));
            break;
        case EWlanConnectionModeInfrastructure:
            text.Append(_L8("Infrastructure"));
            break;
        case EWlanConnectionModeAdhoc:
            text.Append(_L8("Ad hoc"));
            break;
        case EWlanConnectionModeSecureInfra:
            text.Append(_L8("SecureInfra"));
            break;
        case EWlanConnectionModeSearching:
            text.Append(_L8("Searching"));
            break;
        }
    text.Append(_L8("\n"));
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::BssidChanged()
// ------------------------------------------------------------------------------
//
void CConnTestView::BssidChanged( TWlanBssid& aNewBSSID )
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::BssidChanged" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.AppendFormat(_L8("Wlan BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n"), aNewBSSID[0], aNewBSSID[1], aNewBSSID[2], aNewBSSID[3], aNewBSSID[4], aNewBSSID[5] );
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::BssLost()
// ------------------------------------------------------------------------------
//
void CConnTestView::BssLost()
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::BssLost" ) );    
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.AppendFormat(_L8("Wlan: BssLost\n"));
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::BssRegained()
// ------------------------------------------------------------------------------
//
void CConnTestView::BssRegained()
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::BssRegained" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.AppendFormat(_L8("Wlan: BssRegained\n"));
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::NewNetworksDetected()
// ------------------------------------------------------------------------------
//
void CConnTestView::NewNetworksDetected()
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::NewNetworksDetected" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.AppendFormat(_L8("Wlan: NewNetworksDetected\n"));
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::OldNetworksLost()
// ------------------------------------------------------------------------------
//
void CConnTestView::OldNetworksLost()
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::OldNetworksLost" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.AppendFormat(_L8("Wlan: OldNetworksLost\n"));
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::TransmitPowerChanged()
// ------------------------------------------------------------------------------
//
void CConnTestView::TransmitPowerChanged( TUint aPower )
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::TransmitPowerChanged" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    text.AppendFormat(_L8("Wlan: TxPower: %d\n"), aPower);
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestView::RssChanged()
// ------------------------------------------------------------------------------
//
void CConnTestView::RssChanged( TWlanRssClass aRssClass,
                                TUint aRss )
    {
    RDebug::Print( _L( "ConnTest: CConnTestView::RssChanged" ) );
    
    TBuf8<64> text;
    TTime currentTime(0);
    currentTime.HomeTime();
    TDateTime time = currentTime.DateTime();
    text.AppendFormat(_L8("%02u:%02u:%02u "),
        time.Hour(), time.Minute(), time.Second() );
    if ( aRssClass == EWlanRssClassNormal )
        {
        text.AppendFormat(_L8("Wlan RSS: -%ddBm (good)\f"), aRss);
        }
    else if ( aRssClass == EWlanRssClassWeak )
        {
        text.AppendFormat(_L8("Wlan RSS: -%ddBm (weak)\f"), aRss);
        }
    else
        {
        text.AppendFormat(_L8("Wlan RSS: -%ddBm (unknown)\f"), aRss);
        }
    iContainer->PrintNotify(text);
    }

// ------------------------------------------------------------------------------
// CConnTestViewWaiter::CConnTestViewWaiter()
// ------------------------------------------------------------------------------
//
CConnTestViewWaiter::CConnTestViewWaiter( CWlanMgmtClient* aWlanMgmtClient, 
                                          CConnTestView& aCallback, 
                                          TConnTestWaitType aType )
    :   CActive( CActive::EPriorityStandard ),
        iType( aType ),
        iCallback( aCallback ),
        iMgmtClient( aWlanMgmtClient ),
        iScanInfo( NULL )
    {
    RDebug::Print( _L( "ConnTest: CConnTestViewWaiter::CConnTestViewWaiter()" ) );    
    CActiveScheduler::Add( this );        
    }
    
// ------------------------------------------------------------------------------
// CConnTestViewWaiter::ConstructL()
// ------------------------------------------------------------------------------
//
void CConnTestViewWaiter::ConstructL()
    {
    iScanInfo = CWlanScanInfo::NewL();
    }     

// ------------------------------------------------------------------------------
// CConnTestViewWaiter::~CConnTestViewWaiter()
// ------------------------------------------------------------------------------
//
CConnTestViewWaiter::~CConnTestViewWaiter()
    {
    RDebug::Print( _L( "ConnTest: CConnTestViewWaiter::~CConnTestViewWaiter()" ) );
    Cancel();
    iAvailableIaps.Close();
    delete iScanInfo;
    }

// ------------------------------------------------------------------------------
// CConnTestViewWaiter::RunL()
// ------------------------------------------------------------------------------
//
void CConnTestViewWaiter::RunL()
    {
    RDebug::Print( _L( "ConnTest: CConnTestViewWaiter::RunL()" ) );

    if ( iType == EConnTestWaitTypeAvailableIaps )
        {
        iCallback.RequestCompleted( iStatus.Int(), iAvailableIaps );
        }
    else // EConnTestWaitTypeScan
        {
        iCallback.RequestCompleted( iStatus.Int(), *iScanInfo );
        }
    }

// ------------------------------------------------------------------------------
// CConnTestViewWaiter::RunError()
// ------------------------------------------------------------------------------
//
TInt CConnTestViewWaiter::RunError( TInt /* aError */ )
    {
    RDebug::Print( _L( "ConnTest: CConnTestViewWaiter::RunError()" ) );
    return KErrNone;
    }

// ------------------------------------------------------------------------------
// CConnTestViewWaiter::DoCancel()
// ------------------------------------------------------------------------------
//
void CConnTestViewWaiter::DoCancel()
    {
    RDebug::Print( _L( "ConnTest: CConnTestViewWaiter::DoCancel()" ) );
    }

// ------------------------------------------------------------------------------
// CConnTestViewWaiter::IssueRequest()
// ------------------------------------------------------------------------------
//
void CConnTestViewWaiter::IssueRequest()
    {
    RDebug::Print( _L( "ConnTest: CConnTestViewWaiter::IssueRequest()" ) );
    if ( iType == EConnTestWaitTypeAvailableIaps )
        {
        iMgmtClient->GetAvailableIaps( iStatus, iAvailableIaps );    
        }
    else // EConnTestWaitTypeScan
        {
        iMgmtClient->GetScanResults( iStatus, *iScanInfo );
        }
    SetActive();
    }

// End of File

