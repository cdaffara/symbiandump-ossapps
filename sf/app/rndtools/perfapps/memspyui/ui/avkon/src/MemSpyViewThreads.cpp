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

#include "MemSpyViewThreads.h"

// System includes
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

// User includes
#include "MemSpyContainerObserver.h"
#include "MemSpyViewProcesses.h"
#include "MemSpyViewThreadInfoItemList.h"



CMemSpyViewThreads::CMemSpyViewThreads( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aId, TThreadId aThreadId )
:   CMemSpyViewBase( aSession, aObserver ), iParentProcessId( aId ), iCurrentThreadId( aThreadId )
    {    
    }


CMemSpyViewThreads::~CMemSpyViewThreads()
    {
    iThreads.ResetAndDestroy();
    }

 
void CMemSpyViewThreads::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    TInt error = KErrNone;
    RArray<CMemSpyApiThread*> threads;
	TRAP( error, iMemSpySession.GetThreadsL( iParentProcessId, threads ) );
	
	if( error == KErrNone )
	    {
	    CleanupClosePushL( threads );
        for (TInt i=0; i<threads.Count(); i++)
            {
            iThreads.Append( threads[i] );
            }
        CleanupStack::PopAndDestroy( &threads );
        
	    _LIT( KTitle, "Threads" );
	    SetTitleL( KTitle );
	    //
	    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
	    //    
	    if( iCurrentThreadId > 0 )
	        {		
	        for( TInt i = 0; i < iThreads.Count() ; i++ )
	            {
	            if( iThreads[i]->Id() == iCurrentThreadId )
	                {
	                const TInt index = i;
	                if  ( index >= 0 && index < iListBox->Model()->NumberOfItems() )
	                    {
	                    iListBox->SetCurrentItemIndex( index );
	                    HandleListBoxItemSelectedL( index );
	                    }
	                }			
	            }
	        }
	    else if( iThreads.Count() > 0 )
	        {
	        iListBox->SetCurrentItemIndex( 0 );
	        HandleListBoxItemSelectedL( 0 );
	        }
	    }
    }


TProcessId CMemSpyViewThreads::Process() const
    {
	return iParentProcessId;
    }


TThreadId CMemSpyViewThreads::CurrentThread()
    { 
	return iCurrentThreadId;
    }


void CMemSpyViewThreads::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewThreads::ViewType() const
    {
    return EMemSpyViewTypeThreads;
    }


CMemSpyViewBase* CMemSpyViewThreads::PrepareParentViewL()
    {
    CMemSpyViewProcesses* parent = new(ELeave) CMemSpyViewProcesses( iMemSpySession, iObserver, iParentProcessId );
    CleanupStack::PushL( parent );
    //parent->ConstructL( Rect(), *Parent(), &iParentProcessId );
    parent->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewThreads::PrepareChildViewL()
    {	
    CMemSpyViewThreadInfoItemList* child = new(ELeave) CMemSpyViewThreadInfoItemList( iMemSpySession, iObserver, iParentProcessId, iCurrentThreadId );
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent(), EMemSpyThreadInfoItemTypeFirst );
    CleanupStack::Pop( child );
    return child;    
    }


void CMemSpyViewThreads::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if  ( aResourceId == R_MEMSPY_MENUPANE )
        {
		/*
        CMemSpyThread& thread = CurrentThread();
        aMenuPane->SetItemDimmed( EMemSpyCmdThread, thread.IsDead() );
        */
		//aMenuPane->SetItemDimmed( EMemSpyCmdThread, (iThreads[iListBox->CurrentItemIndex()]->ExitType() !=  EExitPending ));
        }
    }


TBool CMemSpyViewThreads::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdThreadEndKill:
        OnCmdEndKillL();
		break;
    case EMemSpyCmdThreadEndTerminate:
        OnCmdEndTerminateL();
		break;
    case EMemSpyCmdThreadEndPanic:
        OnCmdEndPanicL();
		break;
	case EMemSpyCmdThreadSetPriorityAbsoluteVeryLow:
	case EMemSpyCmdThreadSetPriorityAbsoluteLowNormal:
	case EMemSpyCmdThreadSetPriorityAbsoluteLow:
	case EMemSpyCmdThreadSetPriorityAbsoluteBackgroundNormal:
	case EMemSpyCmdThreadSetPriorityAbsoluteBackground:
	case EMemSpyCmdThreadSetPriorityAbsoluteForegroundNormal:
	case EMemSpyCmdThreadSetPriorityAbsoluteForeground:
	case EMemSpyCmdThreadSetPriorityAbsoluteHighNormal:
	case EMemSpyCmdThreadSetPriorityAbsoluteHigh:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime1:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime2:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime3:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime4:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime5:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime6:
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime7: 
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime8:
        OnCmdSetPriorityL( aCommand );
        break;

    case EMemSpyCmdThreadInfoHandles:
        OnCmdInfoHandlesL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewThreads::OnCmdEndKillL()
    {
    TBool system = ETrue;
    TBool doTerminate = ETrue;
    TInt err = KErrNone;
    
    iMemSpySession.ThreadSystemPermanentOrCritical( iCurrentThreadId, system );
    
    if ( system )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
        }
    //
    if  ( doTerminate )
        {
        TRAP( err, iMemSpySession.EndThreadL( iCurrentThreadId, EKill ) );
        if ( err == KErrNone )
            RefreshL();
        }    
    }


void CMemSpyViewThreads::OnCmdEndTerminateL()
    {
	TBool system = ETrue;
    TBool doTerminate = ETrue;
    TInt err = KErrNone;
    
    iMemSpySession.ThreadSystemPermanentOrCritical( iCurrentThreadId, system );
    
    if ( system )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
        }
    //
    if  ( doTerminate )
        {
        TRAP( err, iMemSpySession.EndThreadL( iCurrentThreadId, ETerminate ) );
        if( err == KErrNone )
            RefreshL();        
        }    
    }


void CMemSpyViewThreads::OnCmdEndPanicL()
    {
    TBool system = ETrue;
    TBool doTerminate = ETrue;
    TInt err = KErrNone;
    
    iMemSpySession.ThreadSystemPermanentOrCritical( iCurrentThreadId, system );
    
    if ( system )
        {
        CAknQueryDialog* importDialog = CAknQueryDialog::NewL();
        doTerminate = ( importDialog->ExecuteLD( R_MEMSPY_PANIC_SYSTEM_CRITICAL_THREAD_OR_PROCESS ) );
        }
    //
    if  ( doTerminate )
        {
        TRAP( err, iMemSpySession.EndThreadL( iCurrentThreadId, EPanic ) );
        if( err == KErrNone )            
            RefreshL();
        }    
    }


void CMemSpyViewThreads::OnCmdSetPriorityL( TInt aCommand )
    {	
    TThreadPriority pri = EPriorityAbsoluteBackground;
    switch( aCommand )
        {
	case EMemSpyCmdThreadSetPriorityAbsoluteVeryLow:
        pri = EPriorityAbsoluteVeryLow;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteLowNormal:
        pri = EPriorityAbsoluteLowNormal;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteLow:
        pri = EPriorityAbsoluteLow;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteBackgroundNormal:
        pri = EPriorityAbsoluteBackgroundNormal;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteBackground:
        pri = EPriorityAbsoluteBackground;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteForegroundNormal:
        pri = EPriorityAbsoluteForegroundNormal;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteForeground:
        pri = EPriorityAbsoluteForeground;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteHighNormal:
        pri = EPriorityAbsoluteHighNormal;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteHigh:
        pri = EPriorityAbsoluteHigh;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime1:
        pri = EPriorityAbsoluteRealTime1;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime2:
        pri = EPriorityAbsoluteRealTime2;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime3:
        pri = EPriorityAbsoluteRealTime3;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime4:
        pri = EPriorityAbsoluteRealTime4;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime5:
        pri = EPriorityAbsoluteRealTime5;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime6:
        pri = EPriorityAbsoluteRealTime6;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime7: 
        pri = EPriorityAbsoluteRealTime7;
        break;
	case EMemSpyCmdThreadSetPriorityAbsoluteRealTime8:
        pri = EPriorityAbsoluteRealTime8;
        break;
        }

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreads::OnCmdSetPriorityL() - pri: %d", pri );
#endif
    TInt err = KErrNone;
    TRAP( err , iMemSpySession.SetThreadPriorityL( iCurrentThreadId, pri ) );
    if( err == KErrNone )
        {
        RefreshL();
        }
    else
        {
        #ifdef _DEBUG
            RDebug::Printf( "CMemSpyViewThreads::OnCmdSetPriorityL() - set priority failed" );
        #endif
        }

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyViewThreads::OnCmdSetPriorityL() - END" );
#endif
	
    }


void CMemSpyViewThreads::OnCmdInfoHandlesL()
    {
	iMemSpySession.OutputThreadInfoHandlesL( iCurrentThreadId );	
    }


void CMemSpyViewThreads::SetListBoxModelL()
    {
    RArray<CMemSpyApiThread*> threads;
    iMemSpySession.GetThreadsL( iParentProcessId, threads );
    CleanupClosePushL( threads );
    iThreads.ResetAndDestroy();
    for (TInt i=0; i<threads.Count(); i++)
        {
        iThreads.Append( threads[i] );
        }
    CleanupStack::PopAndDestroy( &threads );
    
    CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( iThreads.Count() ); //array for formated items
    CleanupStack::PushL( model );
    
	_LIT( KTab, "\t" );
	_LIT( KTTab, "\t\t" );
			
	for( TInt i=0; i < iThreads.Count(); i++ )
	    {
	    HBufC* tempName = HBufC::NewL( iThreads[i]->Name().Length() + 16 );
	    CleanupStack::PushL( tempName );
	    TPtr tempNamePtr( tempName->Des() );
	    tempNamePtr.Copy( KTab );
	    tempNamePtr.Append( iThreads[i]->Name() );
	    tempNamePtr.Append( KTTab );
	    
	    AppendPriority( tempNamePtr, iThreads[i]->ThreadPriority() );
	    
	    model->AppendL( tempNamePtr );
	    	
	    CleanupStack::PopAndDestroy( tempName ); 
	    }			
	
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );    
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    CleanupStack::Pop( model );
    }


void CMemSpyViewThreads::HandleListBoxItemActionedL( TInt /*aIndex*/ )
    {
    // Notify observer about an item being 'fired'
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewThreads::HandleListBoxItemSelectedL( TInt aIndex )
	{	
	iCurrentThreadId = iThreads[ aIndex ]->Id();
	
    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }


void CMemSpyViewThreads::AppendPriority( TDes& aDes, TThreadPriority aPriority )
    {
    switch( aPriority )
        {
    case EPriorityNull:
        aDes += _L("[Null]");
        break;
    case EPriorityMuchLess:
        aDes += _L("[Much Less]");
        break;
    case EPriorityLess:
        aDes += _L("[Less]");
        break;
    case EPriorityNormal:
        aDes += _L("[Normal]");
        break;
    case EPriorityMore:
        aDes += _L("[More]");
        break;
    case EPriorityMuchMore:
        aDes += _L("[Much More]");
        break;
    case EPriorityRealTime:
        aDes += _L("[Real Time]");
        break;

    // Absolute values
    case EPriorityAbsoluteVeryLow:
        aDes += _L("[Abs Very Low]");
        break;
    case EPriorityAbsoluteLowNormal:
        aDes += _L("[Abs Low Norm]");
        break;
    case EPriorityAbsoluteLow:
        aDes += _L("[Abs Low]");
        break;
    case EPriorityAbsoluteBackground:
        aDes += _L("[Abs Bgnd]");
        break;
    case EPriorityAbsoluteBackgroundNormal:
        aDes += _L("[Abs Bgnd Norm]");
        break;
    case EPriorityAbsoluteForeground:
        aDes += _L("[Abs Fgnd]");
        break;
    case EPriorityAbsoluteForegroundNormal:
        aDes += _L("[Abs Fgnd Norm]");
        break;
    case EPriorityAbsoluteHigh:
        aDes += _L("[Abs High]");
        break;
    case EPriorityAbsoluteHighNormal:
        aDes += _L("[Abs High Norm]");
        break;
    case EPriorityAbsoluteRealTime1:
    case EPriorityAbsoluteRealTime2:
    case EPriorityAbsoluteRealTime3:
    case EPriorityAbsoluteRealTime4:
    case EPriorityAbsoluteRealTime5:
    case EPriorityAbsoluteRealTime6:
    case EPriorityAbsoluteRealTime7:
    case EPriorityAbsoluteRealTime8:
        aDes.AppendFormat( _L("[Abs RT %d]"), ( aPriority - EPriorityAbsoluteRealTime1 ) + 1 );
        break;
    default:
        aDes += _L("[Unknown Pri.]");
        break;
        }
    }
