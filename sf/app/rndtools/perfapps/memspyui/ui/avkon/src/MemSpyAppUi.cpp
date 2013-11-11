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

#include "MemSpyAppUi.h"

// System includes
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h> 
#include <aknradiobuttonsettingpage.h>
#include <eikaufty.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <barsread.h>
#include <f32file.h>
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelperthread.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyenginehelperfbserv.h>
#include <memspyengineclientinterface.h>
#include <memspy/driver/memspydriverenumerationsshared.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
//cigasto
#include <memspysession.h>

// User includes
#include "MemSpyContainer.h"
#include "MemSpyDocument.h"
#include "MemSpySettings.h"
#include "MemSpyDeviceWideOperationDialog.h"
#include "MemSpyViewRAMInfo.h" // for aknicon config check
#include "MemSpyExportBitmapsToMemoryCardDialog.h"
#include "MemSpy.hrh"
#include "viewcli.h"

// Constants
const TInt KMemSpyDefaultAutoCaptureTimerPeriod = 60;
const TUint32 KMemSpyEikSrvSID = 0x10003a4a;

/*
CMemSpyAppUi::CMemSpyAppUi( CMemSpyEngine& aEngine )
:   iEngine( aEngine ), iAutoCaptureTimerPeriod( KMemSpyDefaultAutoCaptureTimerPeriod ), iAutoCaptureOperationType( CMemSpyDeviceWideOperations::EPerEntityHeapInfo )
    {
    iEngine.SetObserver( this );
    }
*/
CMemSpyAppUi::CMemSpyAppUi( RMemSpySession &aSession )
:   iMemSpySession( aSession ), iAutoCaptureTimerPeriod( KMemSpyDefaultAutoCaptureTimerPeriod ), iAutoCaptureOperationType( CMemSpyDeviceWideOperations::EPerEntityHeapInfo )
    {
    //iEngine.SetObserver( this );
    }

CMemSpyAppUi::~CMemSpyAppUi()
    {
    RDebug::Printf( "[MemSpy] MemSpy is now closing." );
    //
    delete iAutoCaptureTimer;
    //
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    //
    //iEngine.SetObserver( NULL );
    SetViewServerTimeOutStatus( ETrue );
    }


void CMemSpyAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    //
    //iAppContainer = new (ELeave) CMemSpyContainer( iEngine, *this );
    iAppContainer = new (ELeave) CMemSpyContainer( iMemSpySession, *this );
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    iAppContainer->SetObserver( this );
    iAppContainer->SetFocus( ETrue );
    AddToStackL( iAppContainer );
    //
    iAutoCaptureTimer = CPeriodic::NewL( CActive::EPriorityIdle );
	//
	iEikonEnv->SetSystem( ETrue );
    //
    SetViewServerTimeOutStatus( EFalse );
    }


CMemSpyDocument& CMemSpyAppUi::MemSpyDocument()
    {
	CMemSpyDocument* doc = static_cast< CMemSpyDocument* >( Document() );
    return *doc;
    }


const CMemSpyDocument& CMemSpyAppUi::MemSpyDocument() const
    {
	const CMemSpyDocument* doc = static_cast< const CMemSpyDocument* >( Document() );
    return *doc;
    }


void CMemSpyAppUi::HandleStatusPaneSizeChange()
	{
	iAppContainer->SetRect( ClientRect() );
	} 


void CMemSpyAppUi::HandleControlEventL( CCoeControl* aControl, TCoeEvent /*aEventType*/ )
    {
    if  ( aControl == iAppContainer )
        {
        UpdateCBAL();
        }
    }


void CMemSpyAppUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    CMemSpyViewBase& view = iAppContainer->ActiveView();
    const TMemSpyViewType viewType = iAppContainer->ActiveViewType();
    //
    const TBool openableView = MemSpyViewTypeUtils::IsOpenableItem( viewType );
    const TBool exitable = MemSpyViewTypeUtils::IsExitableView( viewType );
    //
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DynInitMenuPaneL() - START - aResourceId: 0x%08x (%8d), iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d, viewType: %d, openableView: %d, exitable: %d", aResourceId, aResourceId, iAutoCaptureOperationType, iAutoCaptureTimer->IsActive(), viewType, openableView, exitable );
#endif
    //
    if  ( aResourceId == R_MEMSPY_MENUPANE )
        {
        const TInt count = aMenuPane->NumberOfItemsInPane();
        for( TInt i=0; i<count; i++ )
            {
            TBool dimItem = EFalse;
            CEikMenuPaneItem::SData& menuPaneMetaData = aMenuPane->ItemDataByIndexL( i );

            // If the command is inside the view-specific range then we'll hide it
            // unless the view and command id matches.
            const TBool isViewSpecific = ( menuPaneMetaData.iCommandId >= KMemSpyMenuCommandViewSpecific );
            if  ( isViewSpecific )
                {
                // Check whether this view requires this command item.
                dimItem = ( view.MenuCascadeCommandId() != menuPaneMetaData.iCommandId );
                }

            aMenuPane->SetItemDimmed( menuPaneMetaData.iCommandId, dimItem );
            }

        aMenuPane->SetItemDimmed( EMemSpyCmdOpen, !openableView );
        aMenuPane->SetItemDimmed( EAknSoftkeyBack, exitable );
        aMenuPane->SetItemDimmed( EAknCmdExit, !exitable );
        }
    else if ( aResourceId == R_MEMSPY_MENUPANE_TOOLS )
        {
        const TBool inOpenFilesView = ( viewType == EMemSpyViewTypeOpenFiles );
        aMenuPane->SetItemDimmed( EMemSpyCmdToolsListOpenFiles, !inOpenFilesView );
        }
    else if ( aResourceId == R_MEMSPY_MENUPANE_VIEW )
        {
        // Hide the refresh item when in the thread info container view
        aMenuPane->SetItemDimmed( EMemSpyCmdViewRefresh, viewType == EMemSpyViewTypeThreadInfoItemList );
        aMenuPane->SetItemDimmed( EMemSpyCmdViewOutputToSink, viewType == EMemSpyViewTypeThreadInfoItemList );
        }
    else if ( aResourceId == R_MEMSPY_MENUPANE_OUTPUT )
        {
        TMemSpySinkType currentSink;
        iMemSpySession.GetOutputSink( currentSink );
		aMenuPane->SetItemDimmed( EMemSpyCmdOutputToDebug, currentSink == ESinkTypeDebug );
        aMenuPane->SetItemDimmed( EMemSpyCmdOutputToFile, currentSink == ESinkTypeFile );                
        }
    else if ( aResourceId == R_MEMSPY_MENUPANE_AUTO_CAPTURE )
        {
        // Change the auto-capture toggle caption appropriately...
        TInt resId = R_MEMSPY_AUTO_CAPTURE_ENABLE;
        if  ( iAutoCaptureTimer->IsActive() )
            {
            resId = R_MEMSPY_AUTO_CAPTURE_DISABLE;
            }
        aMenuPane->SetItemTextL( EMemSpyCmdAutoCaptureToggle, resId );
        }

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DynInitMenuPaneL() - sending to view..." );
#endif

    view.DynInitMenuPaneL( aResourceId, aMenuPane );

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DynInitMenuPaneL() - END - aResourceId: 0x%08x (%8d), iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d, viewType: %d, openableView: %d, exitable: %d", aResourceId, aResourceId, iAutoCaptureOperationType, iAutoCaptureTimer->IsActive(), viewType, openableView, exitable );
#endif
    }


TKeyResponse CMemSpyAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }


void CMemSpyAppUi::HandleCommandL( TInt aCommand )
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::HandleCommandL() - START - aCommand: %8d, iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d", aCommand, iAutoCaptureOperationType, iAutoCaptureTimer->IsActive() );
#endif

    switch ( aCommand )
        {
    case EAknSoftkeyBack:
        OnCmdBackL();
        break;

    case EEikCmdExit:
    case EAknCmdExit:
    case EAknSoftkeyExit:
        OnCmdExitL();
        break;

    case EMemSpyCmdOpen:
        OnCmdOpenL();
        break;

    case EMemSpyCmdOutputToDebug:
        OnCmdOutputToDebugL();
		break;
    case EMemSpyCmdOutputToFile:
        OnCmdOutputToFileL();
		break;

    case EMemSpyCmdToolsAbout:
        OnCmdAboutL();
        break;
    //         
    case EMemSpyCmdPhoneInfoGeneralSummary:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityGeneralSummary );
    	OnCmdPhoneInformationOperationL( OutputPhoneInfo );
        break;        
    case EMemSpyCmdPhoneInfoGeneralDetailed:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityGeneralDetailed );
        OnCmdPhoneInformationOperationL( OutputDetailedPhoneInfo );
        break;
    /*
    case EMemSpyCmdPhoneInfoGeneralHandles:
        OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityGeneralHandles );
        break;
    case EMemSpyCmdPhoneInfoGeneralKernelContainers:
        OnCmdPhoneInformationOperationKernelContainersL();
        break;
    //
    */
    case EMemSpyCmdPhoneInfoHeapInfoSummary:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityHeapInfo );
    	OnCmdPhoneInformationOperationL (OutputHeapInfo );
        break;
    case EMemSpyCmdPhoneInfoHeapInfoCompact:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EEntireDeviceHeapInfoCompact );
    	OnCmdPhoneInformationOperationL( OutputCompactHeapInfo );
        break;
    case EMemSpyCmdPhoneInfoHeapCellListing:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityHeapCellListing );
        OnCmdPhoneInformationOperationL( OutputHeapCellListing );
        break;
        
    case EMemSpyCmdPhoneInfoHeapDump:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityHeapData );
    	OnCmdPhoneInformationOperationL( OutputHeapData );
        break;
    //		   		    		
    case EMemSpyCmdPhoneInfoStackInfo:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityStackInfo );
        OnCmdPhoneInformationOperationL( OutputStackInfo );
        break;
    case EMemSpyCmdPhoneInfoStackInfoCompact:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EEntireDeviceStackInfoCompact );
        OnCmdPhoneInformationOperationL( OutputCompactStackInfo );
        break;
    case EMemSpyCmdPhoneInfoStackDumpUser:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityStackDataUser );
    	OnCmdPhoneInformationOperationL( OutputUserStackData );
        break;
    case EMemSpyCmdPhoneInfoStackDumpKernel:
        //OnCmdPhoneInformationOperationL( CMemSpyDeviceWideOperations::EPerEntityStackDataKernel );
        OnCmdPhoneInformationOperationL( OutputKernelStackData );
        break;

    case EMemSpyCmdAutoCaptureToggle:
        OnCmdAutoCaptureToggleL();
        break;
    case EMemSpyCmdAutoCaptureRetryTime:
        OnCmdAutoCaptureRetryTimeL();
        break;
    case EMemSpyCmdAutoCaptureOperationType:
        OnCmdAutoCaptureOperationTypeL();
        break;

    default:
        iAppContainer->HandleCommandL( aCommand );
        break;
        }

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::HandleCommandL() - END - aCommand: %8d, iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d", aCommand, iAutoCaptureOperationType, iAutoCaptureTimer->IsActive() );
#endif
    }


void CMemSpyAppUi::HandleForegroundEventL( TBool aForeground)
    {
    // Refresh the container when coming to the foreground
    if  ( aForeground )
        {
        iAppContainer->OnCmdViewRefreshL();
        }
    }


void CMemSpyAppUi::HandleThreadSelectedL( const CMemSpyThread& /*aThread*/ )
    {
	MEikAppUiFactory* appUiFactory = (iEikonEnv)->AppUiFactory();
    appUiFactory->MenuBar()->TryDisplayMenuBarL();
    }


void CMemSpyAppUi::HandleMemSpyEngineEventL( MMemSpyEngineObserver::TEvent aEvent, TAny* aContext )
    {
    if  ( aEvent == MMemSpyEngineObserver::EHandleThreadsOrProcessesChanged )
        {
        // Get the original container
        const CMemSpyEngineObjectContainer& oldContainer = *reinterpret_cast< CMemSpyEngineObjectContainer* >( aContext );
        (void) oldContainer;

        // Refresh the container
        iAppContainer->OnCmdViewRefreshL();
        }
    else if ( aEvent == MMemSpyEngineObserver::EHandleClientServerOperationRequest )
        {
        const TInt function = reinterpret_cast<TInt>( aContext );
        InitiateMemSpyClientServerOperationL( function );
        }
    }


void CMemSpyAppUi::DWOperationStarted()
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DWOperationStarted() - START - iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif

    iRunningDeviceWideOperation = ETrue;

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DWOperationStarted() - END - iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif
    }


void CMemSpyAppUi::DWOperationCancelled()
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DWOperationCancelled() - START - iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif

    iAutoCaptureTimer->Cancel();

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DWOperationCancelled() - END - iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif
    }


void CMemSpyAppUi::DWOperationCompleted()
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DWOperationCompleted() - START - iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif

    iRunningDeviceWideOperation = EFalse;

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::DWOperationCompleted() - END - iAutoCaptureTimer is active: %d", iAutoCaptureTimer->IsActive() );
#endif
    }


void CMemSpyAppUi::UpdateCBAL()
    {
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    const TMemSpyViewType viewType = iAppContainer->ActiveViewType();
    //
    if  ( MemSpyViewTypeUtils::IsExitableView( viewType ) )
        {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_EXIT );
        }
    else
        {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
        }

    cba->MakeVisible( ETrue );
    cba->DrawNow();
    }


void CMemSpyAppUi::InitiateMemSpyClientServerOperationL( TInt aOpCode )
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::InitiateMemSpyClientServerOperationL() - START - aOpCode: %d", aOpCode );
#endif

    switch( aOpCode )
        {
    case EMemSpyClientServerOpSummaryInfo:
        HandleCommandL( EMemSpyCmdPhoneInfoGeneralSummary );
        break;
    case EMemSpyClientServerOpSummaryInfoDetailed:
        HandleCommandL( EMemSpyCmdPhoneInfoGeneralDetailed );
        break;
    //
    case EMemSpyClientServerOpHeapInfo:
        HandleCommandL( EMemSpyCmdPhoneInfoHeapInfoSummary );
        break;
    case EMemSpyClientServerOpHeapCellListing:
        HandleCommandL( EMemSpyCmdPhoneInfoHeapCellListing );
        break;
    case EMemSpyClientServerOpHeapData:
        HandleCommandL( EMemSpyCmdPhoneInfoHeapDump );
        break;
    //
    case EMemSpyClientServerOpStackInfo:
        HandleCommandL( EMemSpyCmdPhoneInfoStackInfo );
        break;
    case EMemSpyClientServerOpStackDataUser:
        HandleCommandL( EMemSpyCmdPhoneInfoStackDumpUser );
        break;
    case EMemSpyClientServerOpStackDataKernel:
        HandleCommandL( EMemSpyCmdPhoneInfoStackDumpKernel );
        break;
    //
    case EMemSpyClientServerOpBitmapsSave:
        OnCmdToolsBitmapsSaveToMemoryCardL();
        break;
    case EMemSpyClientServerOpSendToBackground:
        OnCmdToolsSendToBackgroundL();
        break;
    case EMemSpyClientServerOpBringToForeground:
        OnCmdToolsBringToForegroundL();
        break;
    case EMemSpyClientServerOpExit:
        OnCmdExitL();
        break;
    //
    default:
        User::Leave( KErrNotSupported );
        break;
        }

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::InitiateMemSpyClientServerOperationL() - END - aOpCode: %d", aOpCode );
#endif
    }


void CMemSpyAppUi::SetViewServerTimeOutStatus( TBool aEnabled )
    {
	/* TODO
	iMemSpySession.SetServerTimeOutStatus( KMemSpyEikSrvSID, aEnabled );
	*/
	/*
    TRAP_IGNORE(
        iEngine.HelperProcess().ImpersonateL( KMemSpyEikSrvSID );
        //
        CVwsSessionWrapper* ses = CVwsSessionWrapper::NewLC();
        ses->EnableServerEventTimeOut( aEnabled );
        CleanupStack::PopAndDestroy( ses );
        //
        iEngine.HelperProcess().ImpersonateL( 0 );
        );
    */	
    }


void CMemSpyAppUi::OnCmdBackL()
    {
    const TMemSpyViewType viewType = iAppContainer->ActiveViewType();
    //
    if  ( !MemSpyViewTypeUtils::IsExitableView( viewType ) )
        {
        // Should go back one view
        iAppContainer->NavigateToParentViewL();
        }
    }


void CMemSpyAppUi::OnCmdExitL()
    {
    Exit();
    }


void CMemSpyAppUi::OnCmdOpenL()
    {
    const TMemSpyViewType viewType = iAppContainer->ActiveViewType();
    //
    if  ( MemSpyViewTypeUtils::IsOpenableItem( viewType ) )
        {
        // Should go back one view
        iAppContainer->NavigateToChildViewL();
        }
    }


void CMemSpyAppUi::OnCmdPhoneInformationOperationL( TDeviceWideOperation aOperation )
    {	
	//
	//CMemSpyDeviceWideOperationDialog::ExecuteLD( iMemSpySession, aOperation );
	CMemSpyDwoTracker* tracker = CMemSpyDeviceWideOperationDialog::CreateDeviceWideOperation( iMemSpySession, aOperation );
	tracker->Start();		
	/*
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::OnCmdPhoneInformationOperationL() - START - aOp: %d, iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", aOp, iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif

    if  ( !iRunningDeviceWideOperation )
        {        
		CMemSpyDeviceWideOperationDialog::ExecuteLD( iMemSpySession, *this, aOp );
        }

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::OnCmdPhoneInformationOperationL() - END - aOp: %d, iRunningDeviceWideOperation: %d, iAutoCaptureTimer is active: %d", aOp, iRunningDeviceWideOperation, iAutoCaptureTimer->IsActive() );
#endif
	*/
    }


void CMemSpyAppUi::OnCmdPhoneInformationOperationKernelContainersL()
    {
	/* TODO
	iMemSpySession.OutputKernelObjects();
	*/
	/*
    CMemSpyEngineGenericKernelObjectContainer* list = iEngine.HelperKernelContainers().ObjectsAllL();
    CleanupStack::PushL( list );
    iEngine.HelperKernelContainers().OutputL( *list );
    CleanupStack::PopAndDestroy( list );
    */	
    }


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


void CMemSpyAppUi::OnCmdAutoCaptureToggleL()
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::OnCmdAutoCaptureToggleL() - START - iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d", iAutoCaptureOperationType, iAutoCaptureTimer->IsActive() );
#endif

    if  ( iAutoCaptureTimer->IsActive() )
        {
        iAutoCaptureTimer->Cancel();
        }
    else
        {
        iAutoCaptureTimer->Cancel();
        //
        const TTimeIntervalMicroSeconds32 interval( iAutoCaptureTimerPeriod * 1000000 );
        iAutoCaptureTimer->Start( 0, interval, TCallBack(AutoCaptureCallBack, this) );
        }

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::OnCmdAutoCaptureToggleL() - START - iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d", iAutoCaptureOperationType, iAutoCaptureTimer->IsActive() );
#endif
    }


void CMemSpyAppUi::OnCmdAutoCaptureRetryTimeL()
    {
    CAknNumberQueryDialog* dialog = new(ELeave) CAknNumberQueryDialog( iAutoCaptureTimerPeriod );
    dialog->ExecuteLD( R_MEMSPY_DEVICE_WIDE_SETTINGS );
    }


void CMemSpyAppUi::OnCmdAutoCaptureOperationTypeL()
    {
    CDesCArrayFlat* items = iCoeEnv->ReadDesC16ArrayResourceL( R_MEMSPY_AUTO_CAPTURE_OPERATION_TYPES );
    CleanupStack::PushL(items);
    //
    TInt currentItem = iAutoCaptureOperationType;
    //
    CAknRadioButtonSettingPage* dialog = new(ELeave) CAknRadioButtonSettingPage( R_MEMSPY_AUTO_CAPTURE_OPERATION_TYPES_SETTINGS_PAGE, currentItem, items );
    if  ( dialog->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        iAutoCaptureOperationType = static_cast< CMemSpyDeviceWideOperations::TOperation >( currentItem );
        }
    //
    CleanupStack::PopAndDestroy( items ); 
    }


TInt CMemSpyAppUi::AutoCaptureCallBack( TAny* aSelf )
    {
    CMemSpyAppUi* self = reinterpret_cast< CMemSpyAppUi* >( aSelf );
    TRAPD( err, self->AutoCaptureCallBackL() );
    //
    const TBool callMeBackAgain = ( err == KErrNone );
    return callMeBackAgain;
    }


void CMemSpyAppUi::AutoCaptureCallBackL()
    {
#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::AutoCaptureCallBackL() - START - iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d", iAutoCaptureOperationType, iAutoCaptureTimer->IsActive() );
#endif

   // OnCmdPhoneInformationOperationL( iAutoCaptureOperationType );

#ifdef _DEBUG
    RDebug::Printf( "[MemSpy] CMemSpyAppUi::AutoCaptureCallBackL() - END - iAutoCaptureOperationType: %d, iAutoCaptureTimer is active: %d", iAutoCaptureOperationType, iAutoCaptureTimer->IsActive() );
#endif
    }





















void CMemSpyAppUi::OnCmdOutputToDebugL()
    {
	iMemSpySession.SwitchOutputSinkL( ESinkTypeDebug ); //set in engine
	MemSpyDocument().Settings().SetSinkType( ESinkTypeDebug ); //set in settings
    }


void CMemSpyAppUi::OnCmdOutputToFileL()
    {
	iMemSpySession.SwitchOutputSinkL( ESinkTypeFile ); //set in engine
	MemSpyDocument().Settings().SetSinkType( ESinkTypeFile ); //set in settings
    }











void CMemSpyAppUi::OnCmdAboutL()
    {
    CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog;
    dialog->ExecuteLD( R_MEMSPYUI_ABOUT_DIALOG );
    }












void CMemSpyAppUi::OnCmdToolsBitmapsSaveToMemoryCardL()
    {
	/* TODO
    // Get bitmap handles
    RArray<TInt> handles;
    CleanupClosePushL( handles );
    iEngine.HelperFbServ().GetArrayOfBitmapHandlesL( handles );

    // Build bitmap list
    CMemSpyEngineFbServBitmapArray* array = CMemSpyEngineFbServBitmapArray::NewL( handles );
    CleanupStack::PopAndDestroy( &handles );
    CleanupStack::PushL( array );

    // Export...
    CMemSpyExportBitmapsToMemoryCardDialog* dialog = CMemSpyExportBitmapsToMemoryCardDialog::NewL( iCoeEnv->FsSession(), *array );
    
    // All done
    CleanupStack::PopAndDestroy( array );
    */
    }


void CMemSpyAppUi::OnCmdToolsSendToBackgroundL()
    {
	/*
    TApaTask task( iCoeEnv->WsSession() );
    task.SendToBackground();
    */
    }


void CMemSpyAppUi::OnCmdToolsBringToForegroundL()
    {
    iCoeEnv->BringOwnerToFront();
    }




