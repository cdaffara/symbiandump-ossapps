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

#include "MemSpyViewProcesses.h"

// System includes
#include <aknsfld.h>
#include <AknQueryDialog.h>
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspysession.h>
#include <memspy/engine/memspyengineutils.h>

// User includes
#include "MemSpyViewThreads.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyUiUtils.h"

// Constants
const TInt KMemSpyMaxSearchTextLength = 30;
const TInt KMemSpyMaxSearchTextLengthWithWildcards = KMemSpyMaxSearchTextLength + 4;


CMemSpyViewProcesses::CMemSpyViewProcesses( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    }

CMemSpyViewProcesses::CMemSpyViewProcesses( RMemSpySession& aEngine, MMemSpyViewObserver& aObserver, TProcessId aId )
:   CMemSpyViewBase( aEngine, aObserver ), iCurrentProcessId( aId )
    {
    }

CMemSpyViewProcesses::~CMemSpyViewProcesses()
    {	
	iProcesses.ResetAndDestroy();
    delete iMatcherBuffer;
    }


//void CMemSpyViewProcesses::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
void CMemSpyViewProcesses::ConstructL( const TRect& aRect, CCoeControl& aContainer, TProcessId aSelectionRune )
    {
    RArray<CMemSpyApiProcess*> processes;
	iMemSpySession.GetProcessesL( processes ); // get processes array;
	CleanupClosePushL( processes );
	iProcesses.ResetAndDestroy();
	for (TInt i=0; i<processes.Count(); i++)
	    {
	    iProcesses.Append(processes[i]);
	    }
	CleanupStack::PopAndDestroy( &processes );
	
    _LIT( KTitle, "Processes\n& Threads" );
    SetTitleL( KTitle );
    //
    //CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    CMemSpyViewBase::ConstructL( aRect, aContainer );
    //
    iMatcherBuffer = HBufC::NewL( KMemSpyMaxSearchTextLengthWithWildcards );
    //
    /* TODO: to restore the Search field functionality
    iSearchField = CAknSearchField::NewL( *this, CAknSearchField::ESearch, NULL, KMemSpyMaxSearchTextLength );

    iSearchField->SetObserver( this );
    iSearchField->SetFocus( ETrue );
    iSearchField->SetComponentsToInheritVisibility( ETrue );
    */
    //    
    if  ( iCurrentProcessId > 0 )
        {
		TInt index = 0;
	//	TProcessId selectedItem = aSelectionRune; //static_cast< TProcessId >( *aSelectionRune );
		for( TInt i=0; i<iProcesses.Count();i++)
			{
			if( iProcesses[i]->Id() == iCurrentProcessId )
				{
				index = i;
				}
			}					        
        
        if  ( index >= 0 && index < iListBox->Model()->NumberOfItems() )
            {
            iListBox->SetCurrentItemIndex( index );
            HandleListBoxItemSelectedL( index );
            }            
        }
    else if ( iProcesses.Count() > 0 )
        {
        iListBox->SetCurrentItemIndex( 0 );
        HandleListBoxItemSelectedL( 0 );
        }
    //
    SizeChanged();
    ActivateL();
    }


CMemSpyProcess& CMemSpyViewProcesses::CurrentProcess() const
    {
	/*
    __ASSERT_ALWAYS( iCurrentProcess != NULL, User::Invariant() );
    return *iCurrentProcess;
    */	
    }


void CMemSpyViewProcesses::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewProcesses::ViewType() const
    {
    return EMemSpyViewTypeProcesses;
    }


CMemSpyViewBase* CMemSpyViewProcesses::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewProcesses::PrepareChildViewL()
    {	
    CMemSpyViewThreads* child = new(ELeave) CMemSpyViewThreads( iMemSpySession, iObserver, iProcesses[iListBox->CurrentItemIndex()]->Id(), 0 );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    
	return 0;
    }


void CMemSpyViewProcesses::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == MenuCascadeResourceId() )
        {
        TInt index = iListBox->CurrentItemIndex( );
        const TBool hide = iProcesses[index]->IsDead() || iProcesses[index]->ThreadCount() == 0;
        
        //aMenuPane->SetItemDimmed( EMemSpyCmdProcessInfo, hide ); //to be added into API
        aMenuPane->SetItemDimmed( EMemSpyCmdProcessEnd, hide );
        }
    }


TBool CMemSpyViewProcesses::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    // Sorting
    case EMemSpyCmdProcessSortById:
        OnCmdSortByIdL();
		break;
    case EMemSpyCmdProcessSortByName:
        OnCmdSortByNameL();
		break;
    case EMemSpyCmdProcessSortByThreadCount:
        OnCmdSortByThreadCountL();
		break;
    case EMemSpyCmdProcessSortByCodeSegs:
        OnCmdSortByCodeSegsL();
		break;
    case EMemSpyCmdProcessSortByHeapUsage:
        OnCmdSortByHeapUsageL();
		break;
    case EMemSpyCmdProcessSortByStackUsage:
        OnCmdSortByStackUsageL();
		break;
    // End
    case EMemSpyCmdProcessEndTerminate:
        OnCmdEndTerminateL();
		break;
    case EMemSpyCmdProcessEndKill:
        OnCmdEndKillL();
		break;
    case EMemSpyCmdProcessEndPanic:
        OnCmdEndPanicL();
		break;
    // Info
    case EMemSpyCmdProcessInfoSummary:
        OnCmdInfoSummaryL();
		break;
    case EMemSpyCmdProcessInfoHandles:
        OnCmdInfoHandlesL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewProcesses::OnCmdSortByIdL()
    {	
    //iEngine.Container().SortById();
    RefreshL();    
    }


void CMemSpyViewProcesses::OnCmdSortByNameL()
    {	
    //iEngine.Container().SortByName();
    RefreshL();    
    }


void CMemSpyViewProcesses::OnCmdSortByThreadCountL()
    {	
    //iEngine.Container().SortByThreadCount();
    RefreshL();    
    }


void CMemSpyViewProcesses::OnCmdSortByCodeSegsL()
    {	
    //iEngine.Container().SortByCodeSegs();
    RefreshL();
    }


void CMemSpyViewProcesses::OnCmdSortByHeapUsageL()
    {	
    //iEngine.Container().SortByHeapUsage();
    RefreshL();    
    }


void CMemSpyViewProcesses::OnCmdSortByStackUsageL()
    {
   // iEngine.Container().SortByStackUsage();
    RefreshL();
    }


void CMemSpyViewProcesses::OnCmdInfoSummaryL()
    {
    //CMemSpyProcess& process = CurrentProcess();
    //iEngine.HelperProcess().OutputProcessInfoL( process );
    }


void CMemSpyViewProcesses::OnCmdInfoHandlesL()
    {
	/*
    CMemSpyProcess& process = CurrentProcess();
    //
    const TInt threadCount = process.Count();
    for( TInt i=0; i<threadCount; i++ )
        {
        CMemSpyThread& thread = process.At( i );
        thread.InfoContainerForceSyncronousConstructionL().PrintL();
        }
        */
    }


void CMemSpyViewProcesses::OnCmdEndTerminateL()
    {
    TBool system = ETrue;
    TBool doTerminate = ETrue;
    TInt err = KErrNone;
    
    iMemSpySession.ProcessSystemPermanentOrCritical( iCurrentProcessId, system );
    
    if ( system )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
        }
    
    if ( doTerminate )
        {        
        TRAP( err, iMemSpySession.EndProcessL( iCurrentProcessId, ETerminate ) );
        if( err == KErrNone )
            RefreshL();
        }        
    }


void CMemSpyViewProcesses::OnCmdEndPanicL()
    {
    TBool system = ETrue;
    TBool doTerminate = ETrue;
    TInt err = KErrNone;
        
    iMemSpySession.ProcessSystemPermanentOrCritical( iCurrentProcessId, system );
        
    if ( system )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
        }
        
    if ( doTerminate )
        {
        TRAP( err, iMemSpySession.EndProcessL( iCurrentProcessId, EPanic ) );
        if( err == KErrNone )            
            RefreshL();
        }               
    }


void CMemSpyViewProcesses::OnCmdEndKillL()
    {
    TBool system = ETrue;
    TBool doTerminate = ETrue;
    TInt err = KErrNone;
         
    iMemSpySession.ProcessSystemPermanentOrCritical( iCurrentProcessId, system );
         
    if ( system )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
        }
         
    if ( doTerminate )
        {
        TRAP( err, iMemSpySession.EndProcessL( iCurrentProcessId, EKill ) )
        if( err == KErrNone )
            RefreshL();
        }               
    }




void CMemSpyViewProcesses::SetListBoxModelL()
    {	
    RArray<CMemSpyApiProcess*> processes;
    iMemSpySession.GetProcessesL( processes ); // get processes array;
    CleanupClosePushL( processes );
    iProcesses.ResetAndDestroy();
    for (TInt i=0; i<processes.Count(); i++)
        {
        iProcesses.Append( processes[i] );
        }
    
    CDesCArrayFlat* model = FormatModelLC( processes ); //TODO Format model method with advanced formatting
		
	CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    CleanupStack::Pop( model );
    CleanupStack::PopAndDestroy( &processes );
    }


void CMemSpyViewProcesses::HandleListBoxItemActionedL( TInt /*aIndex*/ )
    {
    // Notify observer about an item being 'fired'
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewProcesses::HandleListBoxItemSelectedL( TInt aIndex )
    {
    iCurrentProcessId = iProcesses[aIndex]->Id();
    /*
    if  ( iCurrentProcess )
        {
        iCurrentProcess->Close();
        }

    // Obtain the process that corresponds to the selected item
    CMemSpyEngineObjectContainer& container = iEngine.Container();
    CMemSpyProcess& process = container.At( aIndex );
    iCurrentProcess = &process;

    // Notify observer about item selection     
     */
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }


void CMemSpyViewProcesses::SizeChanged()
    {
    const TRect rect( Rect() );
    CMemSpyViewBase::SizeChanged();
    /*
    if  ( iListBox && iSearchField )
        {
        const TInt lafIndex_H = 1;
        AknLayoutUtils::LayoutControl( iListBox, rect, AknLayout::list_gen_pane( lafIndex_H ) );
        AknLayoutUtils::LayoutControl( iSearchField, rect, AknLayout::find_pane() );
        }
    else
        {
        CMemSpyViewBase::SizeChanged();
        }
    */

/*
    // Search field
    TSize searchFieldSize( iSearchField->MinimumSize() ); // BALLS: search field doesn't implement minimum size correctly?!?!?
    searchFieldSize.SetWidth( rect.Width() );
    const TRect searchFieldRect( TPoint( rect.iTl.iX, rect.iBr.iY ), searchFieldSize );
    iSearchField->SetRect( searchFieldRect );

    // Listbox
    TSize listBoxSize( rect.Size() - TSize( 0, searchFieldSize.iHeight ) );
    iListBox->SetRect( rect.iTl, listBoxSize );
*/
    }


TInt CMemSpyViewProcesses::CountComponentControls() const
    {
    TInt count = 0;
    //
    if  ( iListBox != NULL )
        {
        ++count;
        }
    /*if  ( iSearchField != NULL )
        {
        ++count;
        }*/
    //
    return count;
    }


CCoeControl* CMemSpyViewProcesses::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* ret = iListBox;
    //
    if  ( aIndex == 1 )
        {
        //ret = iSearchField;
        }
    //
    return ret;
    }


TKeyResponse CMemSpyViewProcesses::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse resp = EKeyWasNotConsumed;
    //
    if  ( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
        case EKeyUpArrow:
        case EKeyDownArrow:
        case EKeyEnter:
        case EKeyOK:
            if  ( iListBox )
                {
                resp = iListBox->OfferKeyEventL( aKeyEvent, aType );
                }
            break;
        default:
            break;
            }
        //
        if  ( resp == EKeyWasNotConsumed )
            {
            // Do we need to show the find field?
            //resp = iSearchField->OfferKeyEventL( aKeyEvent, aType );
            }
        }
    //
    return resp;
    }


void CMemSpyViewProcesses::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
    {
    CMemSpyViewBase::HandleControlEventL( aControl, aEventType );
    //
    if  ( aEventType == MCoeControlObserver::EEventStateChanged )
        {
        /*
        if  ( aControl == iSearchField )
            {
            SelectListBoxItemByFindTextL();
            }
            */
        }
    }


void CMemSpyViewProcesses::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    if  ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    /*if  ( iSearchField )
        {
        iSearchField->SetFocus( IsFocused() );
        }
        */
    }


void CMemSpyViewProcesses::SelectListBoxItemByFindTextL()
    {
	/*
    _LIT( KMemSpyWildcardCharacter, "*" );
    
    TPtr pBuffer( iMatcherBuffer->Des() );
    iSearchField->GetSearchText( pBuffer );
    //pBuffer.Insert( 0, KMemSpyWildcardCharacter );
    pBuffer.Append( KMemSpyWildcardCharacter );
    
    CMemSpyEngineObjectContainer& container = iEngine.Container();
    const TInt count = container.Count();
    //
    TInt index = 0;
    while( index < count )
        {
        CMemSpyProcess& process = container.At( index );
        const TPtrC processName( process.Name() );
        //
        if  ( processName.MatchF( pBuffer ) >= 0 )
            {
            HandleListBoxItemSelectedL( index );
            iListBox->ScrollToMakeItemVisible( index );
            iListBox->SetCurrentItemIndexAndDraw( index );
            return;
            }
            
        ++index;
        }
        */
    }

//Model formating methods
//TODO: to be debbuged

CDesCArrayFlat* CMemSpyViewProcesses::FormatModelLC( const RArray<CMemSpyApiProcess*> &aProcesses )
	{		
    CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( iProcesses.Count() + 16 ); //array for formated items
    CleanupStack::PushL( model );
    
	_LIT( KMemSpyProcessNameFormatSpecBasicName, " \t%S\t\t%8x, " );
	
	for( TInt i=0; i < aProcesses.Count(); i++ )
		{
		HBufC* tempName = HBufC::NewL( KMaxFullName ); //aProcesses[i]->Name().Length() + 16
	    CleanupStack::PushL( tempName );
	    TPtr tempNamePtr( tempName->Des() );	    
	    tempNamePtr.Copy( aProcesses[i]->Name() );
		
		TBuf<10> priority;
		TProcessPriority tempPriority = aProcesses[i]->Priority();
		MemSpyUiUtils::AppendPriority( priority, tempPriority );
		
	    // Convert the full name to the format we want in the UI
	    TBuf<KMaxFullName + 60> name;
	    TMemSpyTruncateOverflow overflow; //included from EngineUtils TODO: to consider if this is needed to be removed or left there
	    TUint32 tempSID =  aProcesses[i]->SID();
	    name.AppendFormat( KMemSpyProcessNameFormatSpecBasicName, &overflow, tempName, tempSID );	//tempNamePtr
	    
	    if( aProcesses[i]->ExitType() != EExitPending ) // instead of IsDead() method
	    	{
			MemSpyUiUtils::AppendExitInfo( name, aProcesses[i]->ExitType(), aProcesses[i]->ExitReason(), aProcesses[i]->ExitCategory() );
	    	}
	    else
	        {
			_LIT( KMemSpyProcessNameFormatSpecAlive, "%2d thr, %S" );
			name.AppendFormat( KMemSpyProcessNameFormatSpecAlive, &overflow, aProcesses[i]->ThreadCount(), &priority );
	        }
							
    	model->AppendL( name );
    	
    	CleanupStack::PopAndDestroy( tempName ); 
		}	
	
	return model;
	}


