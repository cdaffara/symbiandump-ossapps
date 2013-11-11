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

#include "MemSpyViewHeapTrackingResults.h"

// System includes
#include <hal.h>
#ifdef __EPOC32__
#include <e32rom.h>
#endif

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerenums.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspyui.rsg>
#include <memspysession.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyContainer.h"
#include "MemSpyViewHeapTracking.h"
#include "MemSpyContainerObserver.h"


_LIT( KCycle, "Cycle" );
_LIT( KTime, "Time" );
_LIT( KMemoryFree, "Memory Free" );
_LIT( KMemoryChange, "Memory Change (vs. Last Cycle)" );
_LIT( KNumOfChanges, "Number of Changes" );

CMemSpyViewHeapTrackingResults::CMemSpyViewHeapTrackingResults( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    //iEngine.HelperSysMemTracker().SetObserver( this );
    }


CMemSpyViewHeapTrackingResults::~CMemSpyViewHeapTrackingResults()
    {
    //iEngine.HelperSysMemTracker().RemoveObserver( this );
    }


void CMemSpyViewHeapTrackingResults::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Results Summary" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Make sure the correct item is selected
    const TInt cycleNumber = ((TInt) aSelectionRune);    
    TInt maxCycle = iMemSpySession.GetSwmtCyclesCount();    
    if  ( cycleNumber > 0 && cycleNumber <= maxCycle )
        {
        iListBox->SetCurrentItemIndex( cycleNumber - 1 );
        HandleListBoxItemSelectedL( cycleNumber );
        }
    }


void CMemSpyViewHeapTrackingResults::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewHeapTrackingResults::ViewType() const
    {
    return EMemSpyViewTypeHeapTrackingResults;
    }


CMemSpyViewBase* CMemSpyViewHeapTrackingResults::PrepareParentViewL()
    {
    CMemSpyViewHeapTracking* parent = new(ELeave) CMemSpyViewHeapTracking( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    //
   // iEngine.HelperSysMemTracker().RemoveObserver( this );
    //
    return parent;
    }


CMemSpyViewBase* CMemSpyViewHeapTrackingResults::PrepareChildViewL()
    {
    const TInt index = iListBox->CurrentItemIndex();
    //
    CMemSpyViewBase* child = NULL;
    
    if  ( index >= 0 )
        {
        //const CMemSpyEngineHelperSysMemTrackerCycle* cycle = iEngine.HelperSysMemTracker().CompletedCycles()[ index ];
		const CMemSpyApiMemoryTrackingCycle* cycle = iCycles[ index ];
        //
        child = new(ELeave) CMemSpyViewHeapTrackingResultsCycleInfo( iMemSpySession, iObserver, *cycle );
        CleanupStack::PushL( child );
        child->ConstructL( Rect(), *Parent() );
        CleanupStack::Pop( child );
        //
        //iEngine.HelperSysMemTracker().RemoveObserver( this );
        }
    
    //
    return child;
    }


void CMemSpyViewHeapTrackingResults::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
            
    iMemSpySession.GetMemoryTrackingCyclesL( iCycles );
    
    CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( iCycles.Count());
    
    for ( TInt i = 0; i < iCycles.Count(); i++ )
     	{
 		HBufC* tempName = HBufC::NewL( iCycles[i]->Caption().Length() + 16 );
     	CleanupStack::PushL( tempName );
     	TPtr tempNamePtr( tempName->Des() );     
     	
     	TBuf<KMaxName> temp;
     	temp.Copy(iCycles[i]->Caption());  	
     	tempNamePtr.Append( temp );
     	model->AppendL( tempNamePtr );
     	
     	CleanupStack::PopAndDestroy( tempName ); 
     	}    
        
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewHeapTrackingResults::HandleCyclesResetL()
    {
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewHeapTrackingResults::HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }


void CMemSpyViewHeapTrackingResults::HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
	TInt count = iMemSpySession.GetSwmtCyclesCount();    
	SetListBoxCurrentItemIndexL( count - 1 );
    CMemSpyViewBase::RefreshL();
    }
































CMemSpyViewHeapTrackingResultsCycleInfo::CMemSpyViewHeapTrackingResultsCycleInfo( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, const CMemSpyApiMemoryTrackingCycle& aCycle )
:   CMemSpyViewBase( aSession, aObserver ), iCycle( aCycle )
    {
    //iEngine.HelperSysMemTracker().SetObserver( this );
    }


CMemSpyViewHeapTrackingResultsCycleInfo::~CMemSpyViewHeapTrackingResultsCycleInfo()
    {
    //iEngine.HelperSysMemTracker().RemoveObserver( this );
    }


void CMemSpyViewHeapTrackingResultsCycleInfo::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitleFormat, "Cycle Summary [%04d]" );
    TBuf<30> buf;
    buf.Format( KTitleFormat, iCycle.CycleNumber() );
    SetTitleL( buf );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Make sure the correct item is selected
    const TInt index = ((TInt) aSelectionRune);
    iListBox->SetCurrentItemIndex( index );
    HandleListBoxItemSelectedL( index );
    }


void CMemSpyViewHeapTrackingResultsCycleInfo::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewHeapTrackingResultsCycleInfo::ViewType() const
    {
    return EMemSpyViewTypeHeapTrackingResultsCycleInfo;
    }


CMemSpyViewBase* CMemSpyViewHeapTrackingResultsCycleInfo::PrepareParentViewL()
    {
    CMemSpyViewHeapTrackingResults* parent = new(ELeave) CMemSpyViewHeapTrackingResults( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) iCycle.CycleNumber() );
    CleanupStack::Pop( parent );
    //
    //iEngine.HelperSysMemTracker().RemoveObserver( this );
    //
    return parent;
    }


void CMemSpyViewHeapTrackingResultsCycleInfo::SetListBoxModelL()
    {
	CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( 6 );
			
	HBufC* buf = HBufC::NewL( 64 );
	TPtr pBuf( buf->Des() );
	
	TBuf<4> cycleBuf;
	_LIT( KCycleFormat, "%d");
	cycleBuf.Format( KCycleFormat, iCycle.CycleNumber() );
	
	pBuf.Zero();	
	pBuf.Append( _L("\t") );
	pBuf.Append( KCycle );
	pBuf.Append( _L("\t\t") );
	pBuf.Append( cycleBuf );	
	model->AppendL( pBuf );
	
	//time:
	TBuf<128> timeBuf;
    const TDateTime dt = iCycle.Time().DateTime();
    //
    _LIT( KTimeFormatSpec, "%04d%02d%02d %02d:%02d:%02d" );
    timeBuf.Format( KTimeFormatSpec, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second() );	    
    pBuf.Zero();
    pBuf.Append( _L("\t"));
    pBuf.Append( KTime );
    pBuf.Append( _L("\t\t") );        
    pBuf.Append( timeBuf );         
	model->AppendL( pBuf );
	
	TBuf<16> memoryFreeBuf;	
	memoryFreeBuf.Format( KCycleFormat, iCycle.FreeMemory() );
	pBuf.Zero();	
	pBuf.Append( _L("\t") );
	pBuf.Append( KMemoryFree );
	pBuf.Append( _L("\t\t") );
	pBuf.Append( memoryFreeBuf );
	model->AppendL( pBuf );
	
	memoryFreeBuf.Format( KCycleFormat, iCycle.MemoryDelta() );
	pBuf.Zero();	
	pBuf.Append( _L("\t") );
	pBuf.Append( KMemoryChange );
	pBuf.Append( _L("\t\t") );
	pBuf.Append( memoryFreeBuf );
	model->AppendL( pBuf );
	
	cycleBuf.Format( KCycleFormat, iCycle.ChangeCount() );
	pBuf.Zero();	
	pBuf.Append( _L("\t") );
	pBuf.Append( KNumOfChanges );
	pBuf.Append( _L("\t\t") );
	pBuf.Append( cycleBuf );
	model->AppendL( pBuf );	
	
	CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
	listbox->Model()->SetItemTextArray( model );
	listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewHeapTrackingResultsCycleInfo::HandleCyclesResetL()
    {
    Container().NavigateToParentViewL();
    }


void CMemSpyViewHeapTrackingResultsCycleInfo::HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }


void CMemSpyViewHeapTrackingResultsCycleInfo::HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }








































/*



CMemSpyViewHeapTrackingResultsChangeDescriptor::CMemSpyViewHeapTrackingResultsChangeDescriptor( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, const CMemSpyEngineHelperSysMemTrackerCycle& aCycle, const CMemSpyEngineHelperSysMemTrackerCycleChange& aChangeDescriptor, TInt aIndex )
:   CMemSpyViewBase( aEngine, aObserver ), iCycle( aCycle ), iChangeDescriptor( aChangeDescriptor ), iIndex( aIndex )
    {
    iEngine.HelperSysMemTracker().SetObserver( this );
    }


CMemSpyViewHeapTrackingResultsChangeDescriptor::~CMemSpyViewHeapTrackingResultsChangeDescriptor()
    {
    iEngine.HelperSysMemTracker().RemoveObserver( this );
    }


void CMemSpyViewHeapTrackingResultsChangeDescriptor::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    TBuf<30> buf;
    if  ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeHeapUser )
        {
        _LIT( KTitleFormat, "User Heap Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeHeapKernel )
        {
        _LIT( KTitleFormat, "Kernel Heap Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeChunk )
        {
        _LIT( KTitleFormat, "Chunk Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeRamDrive )
        {
        _LIT( KTitleFormat, "RAM Drive Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeStack )
        {
        _LIT( KTitleFormat, "Stack Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeGlobalData )
        {
        _LIT( KTitleFormat, "Global Data Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeCode )
        {
        _LIT( KTitleFormat, "RAM-Loaded Code Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeHandle )
        {
        _LIT( KTitleFormat, "Handle Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeOpenFile )
        {
        _LIT( KTitleFormat, "Open File Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeDiskSpace )
        {
        _LIT( KTitleFormat, "Disk Space Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    else if ( iChangeDescriptor.Type() == EMemSpyEngineSysMemTrackerTypeBitmap )
        {
        _LIT( KTitleFormat, "Bitmap Change [%04d]" );
        buf.Format( KTitleFormat, iCycle.CycleNumber() );
        }
    //
    SetTitleL( buf );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewHeapTrackingResultsChangeDescriptor::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewHeapTrackingResultsChangeDescriptor::ViewType() const
    {
    return EMemSpyViewTypeHeapTrackingResultsCycleChangeDescriptor;
    }


CMemSpyViewBase* CMemSpyViewHeapTrackingResultsChangeDescriptor::PrepareParentViewL()
    {
    CMemSpyViewHeapTrackingResultsCycleInfo* parent = new(ELeave) CMemSpyViewHeapTrackingResultsCycleInfo( iEngine, iObserver, iCycle );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) iIndex );
    CleanupStack::Pop( parent );
    //
    iEngine.HelperSysMemTracker().RemoveObserver( this );
    //
    return parent;
    }


void CMemSpyViewHeapTrackingResultsChangeDescriptor::SetListBoxModelL()
    {
    MDesCArray& model = const_cast< MDesCArray& >( static_cast< const MDesCArray&>( iChangeDescriptor ) );
    //
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &model );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewHeapTrackingResultsChangeDescriptor::HandleCyclesResetL()
    {
    CMemSpyViewHeapTrackingResults* mainResultsMenu = new(ELeave) CMemSpyViewHeapTrackingResults( iEngine, iObserver );
    CleanupStack::PushL( mainResultsMenu );
    mainResultsMenu->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( mainResultsMenu );
    //
    Container().SetNewActiveViewL( mainResultsMenu );
    }


void CMemSpyViewHeapTrackingResultsChangeDescriptor::HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    }


void CMemSpyViewHeapTrackingResultsChangeDescriptor::HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    }
*/

