/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef CONNTESTVIEW_H
#define CONNTESTVIEW_H

// INCLUDES
#include <aknview.h>
#include "conntest.hrh"
#include <wlanmgmtclient.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CSocketsEngine;
class CConnTestContainer;
class CSettingItemList;
class CSettingData;
class CCustomPrefsItemList;
class CCustomPrefsData;
class CWlanScanInfo;
class CConnTestView;

// CLASS DECLARATION

enum TConnTestWaitType
    {
    EConnTestWaitTypeAvailableIaps,
    EConnTestWaitTypeScan
    };

class CWlanMgmtClient;

class CConnTestViewWaiter : public CActive
    {
    public:
        CConnTestViewWaiter( CWlanMgmtClient* aWlanMgmtClient, CConnTestView& aCallback, TConnTestWaitType aType );
        void ConstructL();
        virtual ~CConnTestViewWaiter();
        
        void IssueRequest();
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();

    private:
        TConnTestWaitType iType;
        CConnTestView& iCallback;
        CWlanMgmtClient* iMgmtClient;
        RArray<TUint> iAvailableIaps;
        CWlanScanInfo* iScanInfo;
    };

/**
*  CConnTestView view class.
* 
*/
class CConnTestView : public CAknView, public MWlanMgmtNotifications
{
public: // Constructors and destructor
    
    /**
    * Symbian default constructor.
    */
    void ConstructL();
    
    /**
    * Constructor
    * @aViewId Id for this view
    */
    CConnTestView(const TUid aViewId);

    /**
    * Constructor
    * @aViewId Id for this view
    * @aSettings Connection settings for the socket engine of this view
    */
    CConnTestView(const TUid aViewId, CSettingData* aSettings, CCustomPrefsData* aCustomPrefs);

    /**
    * Destructor.
    */
    ~CConnTestView();
    
public: // New functions
    
     /**
    * Returns if the iSocketsEngine has an open connection
    * to be used in HTTP GET
    */
    TBool IsReadyForHTTPGet();
    
    /**
    * Handles the view size changes
    */
    void SizeChanged();
       
    void RequestCompleted( TInt aStatus, RArray<TUint>& aAvailableIaps );
    
    void RequestCompleted( TInt aStatus, CWlanScanInfo& aScanInfo );

public: // Functions from base classes
    
    /**
    * From CAknView
    */
    TUid Id() const;
    
    /**
    * From CAknView
    */
    void HandleCommandL(TInt aCommand);
    
    /**
    * From ?base_class ?member_description
    */
    void HandleClientRectChange();

private: // new

    //
    // WlanMgmtClient notifications
    //
    //
    void ConnectionStateChanged( TWlanConnectionMode /* aNewState */ );
    void BssidChanged( TWlanBssid& /* aNewBSSID */ );
    void BssLost();
    void BssRegained();
    void NewNetworksDetected();
    void OldNetworksLost();
    void TransmitPowerChanged( TUint /* aPower */ );
    void RssChanged(
        TWlanRssClass aRssClass,
        TUint aRss );

    /**
    * Method for sending data over connected socket. 
    */
    void SendDataL();

private:
    
    /**
    * From CAknView
    */
    void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
        const TDesC8& aCustomMessage);
    
    /**
    * From CAknView
    */
    void DoDeactivate();
   
    /**
    * Adjusts the visible items in menu pane accordding to
    * socket engine state.
    * @aResourceId Resource ID identifying the menu pane to initialise
    * @aMenuPane The in-memory representation of the menu pane
    */
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    
    /**
    * Swaps container window between the main window and settings list.
    * @param aActiveContainer Becomes container
    */
    void SwapContainerL(TConnTestContainer aActiveContainer);
    
    /**
    * Creates settings item list
    * @param aResourceId for ListBox initialization
    */
    void CreateSettingsListBoxL(TInt aResourceId);
    
    /**
    * Creates Custom Preferences item list
    * @param aResourceId for ListBox initialization
    */
    void CreateCustomPrefsListBoxL(TInt aResourceId);
    
    /**
    * Create the data packet that is sent over connected socket.
    * Packet size can be set in settings page.
    */
    void CreateDataPacket();    

private: // Data
    TUid                    iViewId;
    CConnTestContainer*     iContainer;   
    CSocketsEngine*         iSocketsEngine;    
    CSettingItemList*       iSettingsListBox;
    CSettingData*           iSettingData;
    CCustomPrefsItemList*   iCustomPrefsListBox;
    CCustomPrefsData*       iCustomPrefsData;
    TConnTestContainer      iActiveContainer;
    HBufC8*                 iSendData;
    TBool                   iSupportsWlan;
    CConnTestViewWaiter*    iWaiter;
    CWlanMgmtClient*        iWlanMgmtClient;
};

#endif

// End of File
