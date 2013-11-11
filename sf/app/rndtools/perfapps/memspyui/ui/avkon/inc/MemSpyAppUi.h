/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYAPPUI_H
#define MEMSPYAPPUI_H

// System includes
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <coecobs.h>

// User includes
#include "MemSpyDeviceWideOperationDialog.h"
#include "MemSpyContainerObserver.h"
#include <memspy/engine/memspyengineobserver.h>
#include "MemSpySettings.h"

// Classes referenced
class CEikMenuBar;
class CMemSpyEngine;
class CMemSpyContainer;
class CMemSpyDocument;

//cigasto
class RMemSpySession;


class CMemSpyAppUi : public CAknAppUi, public MMemSpyContainerObserver, public MCoeControlObserver, public MMemSpyEngineObserver//, public MMemSpyDeviceWideOperationDialogObserver
    {
public:
    //CMemSpyAppUi( CMemSpyEngine& aEngine );
	CMemSpyAppUi( RMemSpySession& aMemSpySession );
    ~CMemSpyAppUi();
    void ConstructL();

public: // API
    CMemSpyDocument& MemSpyDocument();
    const CMemSpyDocument& MemSpyDocument() const;
    inline CMemSpyContainer& Container() { return *iAppContainer; }

private: // From Avkon
    void HandleStatusPaneSizeChange();

private: // From MCoeControlObserver
    void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

private: // From MEikMenuObserver
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // From CEikAppUi
	void HandleForegroundEventL(TBool aForeground);
    void HandleCommandL(TInt aCommand);
    TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private: // From MMemSpyContainerObserver
    void HandleThreadSelectedL( const CMemSpyThread& aThread );

private: // From MMemSpyEngineObserver
    void HandleMemSpyEngineEventL( MMemSpyEngineObserver::TEvent aEvent, TAny* aContext );

private: // From MMemSpyDeviceWideOperationDialogObserver
    void DWOperationStarted();
    void DWOperationCancelled();
    void DWOperationCompleted();

private: // Internal methods
    void UpdateCBAL();
    void InitiateMemSpyClientServerOperationL( TInt aOpCode );
    void SetViewServerTimeOutStatus( TBool aEnabled );
    
private: // Command handlers
    void OnCmdBackL();
    void OnCmdExitL();
    void OnCmdOpenL();
    void OnCmdAboutL();

    // Output
    void OnCmdOutputToDebugL();
    void OnCmdOutputToFileL();
 
    // Phone-information
    //void OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::TOperation aOp );
    void OnCmdPhoneInformationOperationL( TDeviceWideOperation aOperation );
    void OnCmdPhoneInformationOperationKernelContainersL();
    
    // Automatic capture
    void OnCmdAutoCaptureToggleL();
    void OnCmdAutoCaptureRetryTimeL();
    void OnCmdAutoCaptureOperationTypeL();

    // Misc
    void OnCmdToolsBitmapsSaveToMemoryCardL();
    void OnCmdToolsSendToBackgroundL();
    void OnCmdToolsBringToForegroundL();

private: // Auto capture related
    static TInt AutoCaptureCallBack( TAny* aSelf );
    void AutoCaptureCallBackL();

private: // Member data
    //CMemSpyEngine& iEngine;
    RMemSpySession& iMemSpySession;
    CMemSpyContainer* iAppContainer;
    CMemSpySettings* iSettings;
    
    // For phone information dialog
    TBool iRunningDeviceWideOperation;
    
    // For auto-capture
    TInt iAutoCaptureTimerPeriod;
    CMemSpyDeviceWideOperations::TOperation iAutoCaptureOperationType;
    CPeriodic* iAutoCaptureTimer;
    };



#endif
