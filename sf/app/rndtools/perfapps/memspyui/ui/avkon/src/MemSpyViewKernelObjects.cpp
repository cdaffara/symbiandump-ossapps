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

#include "MemSpyViewKernelObjects.h"

// System includes
#include <eikspane.h>
#include <aknnavide.h> 
#include <akntabgrp.h>
#include <eikclbd.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <apgwgnam.h>
#include <aknmessagequerydialog.h>
#include <aknwaitdialog.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyenginehelperserver.h>
#include <memspy/engine/memspyenginehelpercondvar.h>

#include <memspysession.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewKernel.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyViewKernelContainers.h"

const TInt KMaxInfoLength = 128;

/*
CMemSpyViewKernelObjects::CMemSpyViewKernelObjects( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, TMemSpyDriverContainerType aObjectType )
:   CMemSpyViewBase( aEngine, aObserver ),
    iObjectType( aObjectType ),
    iCurrItemIndex( KErrNotFound )
    {
    }
*/
CMemSpyViewKernelObjects::CMemSpyViewKernelObjects( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TMemSpyDriverContainerType aObjectType )
:   CMemSpyViewBase( aSession, aObserver ),
    iObjectType( aObjectType ),
    iCurrItemIndex( KErrNotFound )
    {
    }


CMemSpyViewKernelObjects::~CMemSpyViewKernelObjects()
    {
    if ( iNavContainer && iNavDecorator )
        {
        iNavContainer->Pop( iNavDecorator );
        delete iNavDecorator;
        }
    delete iItems;
    delete iObjectList;
    
    if( iWaitDialog )
    	delete iWaitDialog;
    
    for (TInt i=0; i<iKernelObjectItems.Count(); i++)
        {
        delete iKernelObjectItems[i];
    }

    iKernelObjectItems.Close();
    }


void CMemSpyViewKernelObjects::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
	iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), ETrue);
	iWaitDialog->PrepareLC( R_MEMSPY_WAIT_NOTE );	
	iWaitDialog->RunLD();
	
    _LIT( KTitle, "Kernel Objects" );
    SetTitleL( KTitle );
    iItems = new(ELeave) CDesCArrayFlat(5);
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    CreateTabsL();
    
    iWaitDialog->ProcessFinishedL(); 
    }


CEikListBox* CMemSpyViewKernelObjects::ConstructListBoxL()
    {
    delete iListBox;
    iListBox = NULL;
    CAknSingleStyleListBox* listbox = new (ELeave) CAknSingleStyleListBox();
    iListBox = listbox;
    listbox->ConstructL( this, EAknListBoxSelectionList | EAknListBoxLoopScrolling );
    listbox->SetContainerWindowL( *this );
    listbox->CreateScrollBarFrameL( ETrue );
    SetListBoxModelL();
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listbox->SetListBoxObserver( this );
    listbox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    listbox->SetObserver( this );
    return listbox;
    }


void CMemSpyViewKernelObjects::RefreshL()
    {	
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();               
    }


TMemSpyViewType CMemSpyViewKernelObjects::ViewType() const
    {
    return EMemSpyViewTypeKernelObjects;
    }


CMemSpyViewBase* CMemSpyViewKernelObjects::PrepareParentViewL()
    {	
	if ( iNaviPane && iNavDecorator )
	   {
		iNaviPane->Pop( iNavDecorator );
		delete iNavDecorator;
		iNavDecorator = NULL;
	   }	
    //CMemSpyViewBase* parent = new(ELeave) CMemSpyViewKernelContainers( iEngine, iObserver );
    CMemSpyViewBase* parent = new(ELeave) CMemSpyViewKernelContainers( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewKernelObjects::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    if ( iListBox && 
         iListBox->Model()->NumberOfItems() && 
         iListBox->CurrentItemIndex() > KErrNotFound )
        {
        DetailsL();
        }
    return child;
    }


void CMemSpyViewKernelObjects::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == R_MEMSPY_MENUPANE )
       {
       if ( iObjectType > EMemSpyDriverContainerTypeProcess )
           {
           // Display Terminate, Switch to, End and Panic
           // commands only for Threads and processes
           aMenuPane->SetItemDimmed( EMemSpyCmdKernelObjects, ETrue );
           }
        }
    }


void CMemSpyViewKernelObjects::SetListBoxModelL()
    {
    //_LIT( KLineFormatSpec, "\t%S" );
    _LIT( KTab, "\t" );
       
    for (TInt i=0; i<iKernelObjectItems.Count(); i++)
        {
        delete iKernelObjectItems[i];
        }
    iMemSpySession.GetKernelObjectItems( iKernelObjectItems, iObjectType );    
    
    CDesC16ArrayFlat* model = new (ELeave) CDesC16ArrayFlat( iKernelObjectItems.Count() + 1 ); //array for formated items, +1 added there for 0 case - creating 0-sized array panics
    CleanupStack::PushL( model );
        
    for ( TInt i = 0; i < iKernelObjectItems.Count(); i++ )
        {
        HBufC* tempName = HBufC::NewL( iKernelObjectItems[i]->Name().Length() + 16 );
        CleanupStack::PushL( tempName );
        TPtr tempNamePtr( tempName->Des() );
        tempNamePtr.Copy( KTab );
    	
        TBuf<KMaxName> temp;
        temp.Copy(iKernelObjectItems[i]->Name());    	
        tempNamePtr.Append( temp	 );
        model->AppendL( tempNamePtr );
            
        CleanupStack::PopAndDestroy( tempName ); 
        }
    
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    CleanupStack::Pop( model );
    }


TBool CMemSpyViewKernelObjects::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
        case EMemSpyCmdKernelObjectTerminate:
            {
            TRAPD( err, OnCmdTerminateL() );
            if ( err )
                {
                CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
                note->ExecuteLD( _L("Cannot terminate task") );
                }
            break;
            }
        case EMemSpyCmdKernelObjectSwitchTo:
            {
            TRAPD( err, OnCmdSwitchToL() );
            if ( err )
                {
                CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
                note->ExecuteLD( _L("Cannot bring to foreground") );
                }
            break;
            }
        case EMemSpyCmdKernelObjectEnd:
            {
            TRAPD( err, OnCmdEndL() );
            if ( err )
                {
                CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
                note->ExecuteLD( _L("Cannot end task") );
                }
            break;
            }
        case EMemSpyCmdKernelObjectPanic:
            {
            TRAPD( err, OnCmdPanicL() );
            if ( err )
                {
                CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote( ETrue );
                note->ExecuteLD( _L("Cannot panic task") );
                }
            break;
            }
        default:
            {
            handled = CMemSpyViewBase::HandleCommandL( aCommand );
            break;        
            }
        }
    //
    return handled;
    }


TKeyResponse CMemSpyViewKernelObjects::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {	
    TKeyResponse resp = EKeyWasNotConsumed;
    resp = CMemSpyViewBase::OfferKeyEventL( aKeyEvent, aType );    
    //}
    return resp;
    }

void CMemSpyViewKernelObjects::TabChangedL( TInt /*aIndex*/ )
    {
    }


void CMemSpyViewKernelObjects::OnCmdTerminateL()
    {
    switch ( iObjectType )
        {
        case EMemSpyDriverContainerTypeThread:
            {
            TBool err = EFalse;
            TBool doTerminate = ETrue;
            
            TThreadId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() ); //get id of the current thread
                        
            iMemSpySession.ThreadSystemPermanentOrCritical( id, err );
            
            if( err )
            	{
				CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
				doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
				
				if ( doTerminate )
					{
					iMemSpySession.EndThreadL( id, ETerminate );
					RefreshL();
					}
            	}                                  
            break;
            }        
        case EMemSpyDriverContainerTypeProcess:
            {
            TBool err = EFalse;
            TBool doTerminate = ETrue;
            TProcessId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() );
            iMemSpySession.ProcessSystemPermanentOrCritical( id, err );
            
            if( err )
            	{
				CAknQueryDialog* importDialog = CAknQueryDialog::NewL();	
                doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
                
                if( doTerminate )
                	{
                    iMemSpySession.EndProcessL( id, ETerminate );     
                    RefreshL();
                    }
            	}                     
            break;
            }
        default:
            {
            // Programming error
            __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
            }
        }
    }


void CMemSpyViewKernelObjects::OnCmdSwitchToL()
    {	
	TBool brought = EFalse;
	/*
    TInt wgCount;
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );
    User::LeaveIfError( wgCount = wsSession.NumWindowGroups() );
    RArray<RWsSession::TWindowGroupChainInfo> wgArray;
    CleanupClosePushL( wgArray );
    User::LeaveIfError( wsSession.WindowGroupList( &wgArray ) );
    TApaTask task( wsSession );
    TBool brought( EFalse );
    TInt wgId( KErrNotFound );
    TThreadId threadId;
    */
    switch ( iObjectType )
        {    	    	    	
        case EMemSpyDriverContainerTypeThread:
            {
            TThreadId currentThreadId( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() );
            TInt error = iMemSpySession.SwitchToThread( currentThreadId, brought );
            /*
            TThreadId currentThreadId( iObjectList->At( iListBox->CurrentItemIndex() ).iId );            
            
            // loop trough all window groups and see if a thread id matches
            while( !brought && wgCount-- )
                {
                wgId = wgArray[wgCount].iId;
                User::LeaveIfError( wsSession.GetWindowGroupClientThreadId( wgId, threadId ) );
                if ( threadId == currentThreadId )
                    {
                    CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( wsSession, wgId );
                    task.SetWgId( wgId );
                    if ( !wgName->Hidden() && task.Exists() )
                        {
                        task.BringToForeground();
                        brought = ETrue;                        
                        }
                    CleanupStack::PopAndDestroy( wgName );
                    }
                }
                */                
            break;
            }
        case EMemSpyDriverContainerTypeProcess:
            {                        
            TProcessId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() );
            TInt error = iMemSpySession.SwitchToProcess( id, brought  );
            /*
            //CMemSpyEngineObjectContainer& container = iEngine.Container();
            //TProcessId id( iObjectList->At( iListBox->CurrentItemIndex() ).iId );
            //CMemSpyProcess& process = container.ProcessByIdL( id );
            
            TProcessId id( iKernelObjectItems[iListBox->CurrentItemIndex()]->Id() );
            
            
            // loop trough threads in a process
            for ( TInt i = 0; i < process.MdcaCount(); i++ )
                {
                TInt wgCountLocal = wgCount;
                
                // loop trough all window groups and see if a thread id matches
                while( !brought && wgCountLocal-- )
                    {
                    wgId = wgArray[wgCountLocal].iId;
                    User::LeaveIfError( wsSession.GetWindowGroupClientThreadId( wgId, threadId ) );
                    if ( threadId == process.At( i ).Id() )
                        {
                        CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( wsSession, wgId );
                        task.SetWgId( wgId );
                        if ( !wgName->Hidden() && task.Exists() )
                            {
                            task.BringToForeground();
                            brought = ETrue;                        
                            }
                        CleanupStack::PopAndDestroy( wgName );
                        }
                    }
                }
			*/
            break;
            }
        default:
            {
            // Programming error
            __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
            }
        }
    if ( !brought )
        {
        // Error handling in HandleCommandL
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( 2 ); //wgArray,wsSession    
    }


void CMemSpyViewKernelObjects::OnCmdEndL()
    {
    switch ( iObjectType )
        {
        case EMemSpyDriverContainerTypeThread:
            {
            TBool err = EFalse;
            TBool doTerminate = ETrue;
            
            TThreadId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() ); //get id of the current thread                                   
            iMemSpySession.ThreadSystemPermanentOrCritical( id, err );
            
            if( err )
            	{
				CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
				doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
                        				
				if ( doTerminate )
					{
					iMemSpySession.EndThreadL( id, EKill );
					RefreshL();
					}
            	}            

            
            /*
            CMemSpyEngineObjectContainer& container = iEngine.Container();
            TThreadId id( iObjectList->At( iListBox->CurrentItemIndex() ).iId );
            
            // Try to find the thread in question...
            CMemSpyProcess* process = NULL;
            CMemSpyThread* thread = NULL; 
            User::LeaveIfError( container.ProcessAndThreadByThreadId( id, process, thread ) );

            if ( thread )
                {                
                //
                if  ( thread->IsSystemPermanent() || thread->IsSystemCritical() )
                    {
                    CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
                    doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
                    }
                //
                if  ( doTerminate )
                    {
                    thread->KillL();
                    }                
                }
            
            RefreshL();
            */
            break;
            }
        case EMemSpyDriverContainerTypeProcess:
            {
            TBool err = EFalse;
            TBool doTerminate = ETrue;
            TProcessId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() );
            iMemSpySession.ProcessSystemPermanentOrCritical( id, err );
            
            if( err )
            	{
				CAknQueryDialog* importDialog = CAknQueryDialog::NewL();	
				doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
				
				if( doTerminate )
					{
					iMemSpySession.EndProcessL( id, EKill);     
					RefreshL();
					}
            	}
            
            /*
            TBool doTerminate = ETrue;
            // Obtain the process that corresponds to the selected item
            CMemSpyEngineObjectContainer& container = iEngine.Container();
            TProcessId id( iObjectList->At( iListBox->CurrentItemIndex() ).iId );
            CMemSpyProcess& process = container.ProcessByIdL( id );            

            if  ( process.IsSystemPermanent() || process.IsSystemCritical() )
                {
                CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
                doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
                }
            if  ( doTerminate )
                {
                process.KillL();
                RefreshL();
                }
			*/
            break;
            }
        default:
            {
            // Programming error
            __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
            }
        }    
    }


void CMemSpyViewKernelObjects::OnCmdPanicL()
    {
    switch ( iObjectType )
        {
        case EMemSpyDriverContainerTypeThread:
            {
            TBool err = EFalse;
            TBool doTerminate = ETrue;
            
            TThreadId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() ); //get id of the current thread                                   
            iMemSpySession.ThreadSystemPermanentOrCritical( id, err );
            
            if( err )
            	{
				CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
				doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
                        				
				if ( doTerminate )
					{
					iMemSpySession.EndThreadL( id, EPanic );
					RefreshL();
					}
            	}
            
            break;
            }
        case EMemSpyDriverContainerTypeProcess:
            {
            TBool err = EFalse;
            TBool doTerminate = ETrue;
            TProcessId id( iKernelObjectItems[ iListBox->CurrentItemIndex() ]->Id() );
            iMemSpySession.ProcessSystemPermanentOrCritical( id, err );
            
            if( err )
            	{
				CAknQueryDialog* importDialog = CAknQueryDialog::NewL();	
				doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
                           
				if( doTerminate )
					{
					iMemSpySession.EndProcessL( id, EPanic );     
					RefreshL();
					}
            	}
        
            break;
            }
        default:
            {
            // Programming error
            __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
            }
        }
    }


void CMemSpyViewKernelObjects::CreateTabsL()
    {
	TMemSpyDriverContainerType type = iObjectType;
	const TPtrC pType( MemSpyUiUtils::TypeAsString( type ) );
	
	CEikStatusPane* statusPane = static_cast<CAknAppUi*> ( iEikonEnv->EikAppUi() )->StatusPane();
	TUid uid;
	uid.iUid = EEikStatusPaneUidNavi;
	iNaviPane = ( CAknNavigationControlContainer * ) statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );	
	iNavDecorator=iNaviPane->CreateMessageLabelL( pType );
	iNaviPane->PushL(*iNavDecorator); 		
    }


void CMemSpyViewKernelObjects::DetailsL()
    {
    HBufC* messageBuf = HBufC::NewLC( 40960 );    
    TPtr messagePtr = messageBuf->Des();

    //const TMemSpyDriverHandleInfoGeneric& selectedObject = iObjectList->At( iListBox->CurrentItemIndex() );
    //iKernelObjectItems[iListBox->CurrentItemIndex];

    // General attributes:
    TFullName name;
    name.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameDetail() );
    AppendFormatString( messagePtr, _L("Name: %S\n"), &name );
    name.Zero();
    name.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->Name() );
    AppendFormatString( messagePtr, _L("FullName: %S\n"), &name );
    name.Zero();
    AppendFormatString( messagePtr, _L("AccessCount: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AccessCount() );
    AppendFormatString( messagePtr, _L("UniqueID: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->UniqueID() );
    AppendFormatString( messagePtr, _L("Protection: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Protection() );
    AppendFormatString( messagePtr, _L("OwnrAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfKernelOwner() );
    AppendFormatString( messagePtr, _L("KernelAddr: 0x%08X"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Handle() );
    
    // Object type specific attributes:
    switch ( iKernelObjectItems[iListBox->CurrentItemIndex()]->Type() )
        {
        case EMemSpyDriverContainerTypeUnknown:
            {
            break;
            }
        case EMemSpyDriverContainerTypeThread:
            {
            AppendFormatString( messagePtr, _L("\nOwnrPrAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningProcess() );
            AppendFormatString( messagePtr, _L("ThreadID: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Id() );
            AppendFormatString( messagePtr, _L("Priority: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Priority() );
            TFullName procName;
            procName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameOfOwner() );
            AppendFormatString( messagePtr, _L("Proc: %S"), &procName );
            procName.Zero();
            break;
            }
        case EMemSpyDriverContainerTypeProcess:
            {
            AppendFormatString( messagePtr, _L("\nOwnrPrAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningProcess() );
            AppendFormatString( messagePtr, _L("CreatorId: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->CreatorId() );
            AppendFormatString( messagePtr, _L("Attributes: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Attributes() );
            AppendFormatString( messagePtr, _L("StckChnk: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfDataBssStackChunk() );
            AppendFormatString( messagePtr, _L("ProcessID: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Id() );
            AppendFormatString( messagePtr, _L("Priority: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Priority() );
            AppendFormatString( messagePtr, _L("SecurityZone: %d"), iKernelObjectItems[iListBox->CurrentItemIndex()]->SecurityZone() );
            /* TODO: to solve process details
            CMemSpyEngineObjectContainer& container = iEngine.Container();
            TProcessId id( iKernelObjectItems[iListBox->CurrentItemIndex().iId );
            TRAP_IGNORE(
                CMemSpyProcess& process = container.ProcessByIdL( id );
                AppendFormatString( messagePtr, _L("\nSID: 0x%08X\n"), process.SID() );
                AppendFormatString( messagePtr, _L("VID: 0x%08X\n"), process.VID() );
                AppendFormatString( messagePtr, _L("UID1: 0x%08X\n"), process.UIDs()[0].iUid  );
                AppendFormatString( messagePtr, _L("UID2: 0x%08X\n"), process.UIDs()[1].iUid  );
                AppendFormatString( messagePtr, _L("UID3: 0x%08X\n"), process.UIDs()[2].iUid );
                AppendFormatString( messagePtr, _L("Caps: 0x%08X%08X"), process.Capabilities().iCaps[0], process.Capabilities().iCaps[1]);
                );
            */
            break;
            }
        case EMemSpyDriverContainerTypeChunk:
            {
            AppendFormatString( messagePtr, _L("\nOwnrPrAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningProcess() );
            AppendFormatString( messagePtr, _L("Size: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Size() );
            AppendFormatString( messagePtr, _L("MaxSize: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->MaxSize() );
            AppendFormatString( messagePtr, _L("Bottom: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Bottom() );
            AppendFormatString( messagePtr, _L("Top: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Top() );
            AppendFormatString( messagePtr, _L("Attr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Attributes() );
            AppendFormatString( messagePtr, _L("Start: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->StartPos() );
            AppendFormatString( messagePtr, _L("CntrlID: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->ControllingOwner() );
            AppendFormatString( messagePtr, _L("Restrictions: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Restrictions() );
            AppendFormatString( messagePtr, _L("MapAttr: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->MapAttr() );
            AppendFormatString( messagePtr, _L("Type: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->ChunkType() );
            TFullName procName;
            procName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameOfOwner() );
            AppendFormatString( messagePtr, _L("Proc: %S"), &procName );
            procName.Zero();
            // OwnerAddr iAddressOfKernelOwner already listed
            // ProcAddr iAddressOfOwningProcess already listed
            break;
            }
        case EMemSpyDriverContainerTypeLibrary:
            {
            AppendFormatString( messagePtr, _L("\nMapCount: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->MapCount() );
            AppendFormatString( messagePtr, _L("State: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->State() );
            AppendFormatString( messagePtr, _L("CodeSeg: 0x%08X"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfCodeSeg() );
            break;
            }
        case EMemSpyDriverContainerTypeSemaphore:
            {
            AppendFormatString( messagePtr, _L("\nCount: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Count() );
            AppendFormatString( messagePtr, _L("Resetting: %u"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Resetting() );
            break;
            }
        case EMemSpyDriverContainerTypeMutex:
            {
            AppendFormatString( messagePtr, _L("\nHoldCount: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Count() );
            AppendFormatString( messagePtr, _L("WaitCount: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->WaitCount() );
            AppendFormatString( messagePtr, _L("Resetting: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Resetting() );
            AppendFormatString( messagePtr, _L("Order: %u"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Order() );
            break;
            }
        case EMemSpyDriverContainerTypeTimer:
            {
            AppendFormatString( messagePtr, _L("\nState: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->TimerState() );
            AppendFormatString( messagePtr, _L("Type: %d"), iKernelObjectItems[iListBox->CurrentItemIndex()]->TimerType() );
            break;
            }
        case EMemSpyDriverContainerTypeServer:
            {
            // Owner == ThrdAddr
            AppendFormatString( messagePtr, _L("\nThrdAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningThread() );
            TFullName thrName;
            thrName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameOfOwner() );
            AppendFormatString( messagePtr, _L("Thr: %S\n"), &thrName );
            thrName.Zero();
            AppendFormatString( messagePtr, _L("Type: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->SessionType() );
            TBuf8<55>buf;
            /*
            RArray<TMemSpyDriverServerSessionInfo> sessions;
            CleanupClosePushL( sessions );            
            iEngine.HelperServer().GetServerSessionsL( iKernelObjectItems[iListBox->CurrentItemIndex(), sessions );
            const TInt count = sessions.Count();
            for ( TInt i = 0; i < count; i++ )
                {
                const TMemSpyDriverServerSessionInfo& session = sessions[ i ];
                AppendFormatString( messagePtr, _L("SessAddr: 0x%08X\n"), session.iAddress );
                TFullName sessName;
                sessName.Copy( session.iName );
                AppendFormatString( messagePtr, _L("Sess: %S\n"), &sessName );
                }
            CleanupStack::PopAndDestroy( &sessions );
            */
            break;
            }
        case EMemSpyDriverContainerTypeSession:
            {
            // Server == SvrAddr
            AppendFormatString( messagePtr, _L("\nServer: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfServer() );
            TFullName srvName;
            srvName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->Name() );
            AppendFormatString( messagePtr, _L("Srv: %S\n"), &srvName );
            srvName.Zero();
            // Session: not supported
            AppendFormatString( messagePtr, _L("AccCount: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->TotalAccessCount() );
            AppendFormatString( messagePtr, _L("SesType: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->SessionType() );
            AppendFormatString( messagePtr, _L("SvrType: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->SvrSessionType() );
            AppendFormatString( messagePtr, _L("MsgCount: %d\n"),  iKernelObjectItems[iListBox->CurrentItemIndex()]->MsgCount() );
            AppendFormatString( messagePtr, _L("MsgLimit: %d"), iKernelObjectItems[iListBox->CurrentItemIndex()]->MsgLimit() );
            break;
            }
        case EMemSpyDriverContainerTypeLogicalDevice:
            {
            AppendFormatString( messagePtr, 
                                _L("\nVersion: %d.%d.%d\n"), 
                                /*
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version.iMajor, TODO: To solve this
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version.iMinor,
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version.iBuild );
                                */
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version(),
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version(),
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version() );
            AppendFormatString( messagePtr, _L("ParseMask: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->ParseMask() );
            AppendFormatString( messagePtr, _L("UnitsMask: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->UnitsMask() );
            AppendFormatString( messagePtr, _L("Open channels: %d"), iKernelObjectItems[iListBox->CurrentItemIndex()]->OpenChannels() );
            break;
            }
        case EMemSpyDriverContainerTypePhysicalDevice:
            {
            AppendFormatString( messagePtr, 
                                _L("\nVersion: %d.%d.%d\n"), 
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version(),
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version(),
                                iKernelObjectItems[iListBox->CurrentItemIndex()]->Version() );
            AppendFormatString( messagePtr, _L("UnitsMask: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->UnitsMask() );
            AppendFormatString( messagePtr, _L("CodeSeg: 0x%08X"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfCodeSeg() );
            break;
            }
        case EMemSpyDriverContainerTypeLogicalChannel:
            {
            // No other details
            break;
            }
        case EMemSpyDriverContainerTypeChangeNotifier:
            {
            AppendFormatString( messagePtr, _L("\nChanges: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Changes() );
            // Thread == ThrdAddr
            AppendFormatString( messagePtr, _L("ThrdAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningThread() );
            TFullName thrName;
            thrName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameOfOwner() );
            AppendFormatString( messagePtr, _L("Thr: %S"), &thrName );
            thrName.Zero();
            break;
            }
        case EMemSpyDriverContainerTypeUndertaker:
            {
            // Thread == ThrdAddr
            AppendFormatString( messagePtr, _L("\nThrdAddr: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningThread() );
            TFullName thrName;
            thrName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameOfOwner() );
            AppendFormatString( messagePtr, _L("Thr: %S"), &thrName );
            thrName.Zero();
            break;
            }
        case EMemSpyDriverContainerTypeMsgQueue:
            {
            // No other details
            break;
            }
        case EMemSpyDriverContainerTypePropertyRef:
            {
            /*
            Not listing details here, as propertyRef is not listed in TaskMgr.
            Following propertyRef attributes are available at engine side. 
            
            IsReady
            Type
            Category
            Key
            RefCount
            ThreadId
            CreatorSID
            */
            break;
            }
        case EMemSpyDriverContainerTypeCondVar:
            {
            AppendFormatString( messagePtr, _L("\nResetting: %u\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->Resetting() );
            // Using iAddressOfOwningThread for mutex
            AppendFormatString( messagePtr, _L("Mutex: 0x%08X\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->AddressOfOwningThread() );
            // Using iNameOfOwner for mutex
            TFullName mtxName;
            mtxName.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->NameOfOwner() );
            AppendFormatString( messagePtr, _L("Mtx: %S\n"), &mtxName );
            mtxName.Zero();
            AppendFormatString( messagePtr, _L("WaitCount: %d\n"), iKernelObjectItems[iListBox->CurrentItemIndex()]->WaitCount() );
            /*
            RArray<TMemSpyDriverCondVarSuspendedThreadInfo> threads;
            CleanupClosePushL( threads );            
            iEngine.HelperCondVar().GetCondVarSuspendedThreadsL( iKernelObjectItems[iListBox->CurrentItemIndex(), threads );
            const TInt count = threads.Count();
            for ( TInt i = 0; i < count; i++ )
                {
                const TMemSpyDriverCondVarSuspendedThreadInfo& thr = threads[ i ];
                AppendFormatString( messagePtr, _L("SuspThrdAddr: 0x%08X\n"), thr.iAddress );
                TFullName thrName;
                thrName.Copy( thr.iName );
                AppendFormatString( messagePtr, _L("Thr: %S\n"), &thrName );
                }
            CleanupStack::PopAndDestroy( &threads );
            */
            break;
            }
        default:
            {
            // Programming error
            __ASSERT_ALWAYS( EFalse, User::Panic( _L("MemSpy-View"), 0 ) );
            }
        }
    
    CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog( CAknQueryDialog::ENoTone );
    dialog->PrepareLC( R_MEMSPY_KERNEL_OBJECT_DETAILS_DIALOG );
    TFileName headerText;
    headerText.Copy( iKernelObjectItems[iListBox->CurrentItemIndex()]->Name() );
    dialog->SetHeaderTextL( headerText );
    dialog->SetMessageTextL(messagePtr);
    dialog->RunLD();
    CleanupStack::PopAndDestroy( messageBuf );
    }


void CMemSpyViewKernelObjects::AppendFormatString( TPtr& aPtr, TRefByValue<const TDesC> aFmt, ... )
    {
    TBuf<KMaxInfoLength> infoString;
    VA_LIST list;
    VA_START ( list, aFmt );
    infoString.FormatList( aFmt, list );
    aPtr.Append( infoString );
    }

