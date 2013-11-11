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

#include "MemSpyViewKernelHeap.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyengineoutputlist.h>

#include <memspysession.h>
#include <memspy/api/memspyapiheap.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewKernel.h"
#include "MemSpyContainerObserver.h"

// Constants
const TInt KMemSpyBufferSize = 20;

// Literal constants
_LIT(KMemSpyHexFormat, "0x%08x");
_LIT(KMemSpyUnsignedFormat, "%u");
_LIT(KMemSpyDecFormat, "%d");
_LIT(KMemSpyLongFormat, "%Ld");
_LIT(KMemSpyCaptionYes, "Yes");
_LIT(KMemSpyCaptionNo, "No");


CMemSpyViewKernelHeap::CMemSpyViewKernelHeap( RMemSpySession& aSession, MMemSpyViewObserver& aObserver ) //cigasto: remember to uncomment from MMP!
:   CMemSpyViewBase( aSession, aObserver )
    {
    }

CMemSpyViewKernelHeap::~CMemSpyViewKernelHeap()
    {
    }


void CMemSpyViewKernelHeap::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Kernel Heap" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewKernelHeap::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewKernelHeap::ViewType() const
    {
    return EMemSpyViewTypeKernelHeap;
    }


CMemSpyViewBase* CMemSpyViewKernelHeap::PrepareParentViewL()
    {
    CMemSpyViewKernel* parent = new(ELeave) CMemSpyViewKernel( iMemSpySession, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewKernelHeap::PrepareChildViewL()
    {	
    CMemSpyViewBase* child = NULL;
    return child;    
    }


void CMemSpyViewKernelHeap::SetListBoxModelL()
    {	
	CMemSpyApiHeap* heap = iMemSpySession.GetHeap();
	User::LeaveIfNull( heap );
	CleanupStack::PushL( heap );
		
	CDesCArrayFlat* model = FormatModelLC( heap );	
				
	CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
	listbox->Model()->SetItemTextArray( model );
	listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
	
	CleanupStack::Pop( model );
	CleanupStack::PopAndDestroy( heap );
	}


TBool CMemSpyViewKernelHeap::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdKernelHeapDump:
        OnCmdDumpKernelHeapL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewKernelHeap::OnCmdDumpKernelHeapL()
    {	
    iMemSpySession.DumpKernelHeap();
    }


//CDesCArrayFlat* CMemSpyViewKernelHeap::FormatModel( RArray<CMemSpyApiHeap*> &aHeap )
CDesCArrayFlat* CMemSpyViewKernelHeap::FormatModelLC( CMemSpyApiHeap* aHeap )
	{
	CDesCArrayFlat* model = new (ELeave) CDesC16ArrayFlat( 2 );
	CleanupStack::PushL( model );
	
	if (aHeap) 
	    {
        _LIT( KItem0, "Heap type" );		   	  
        _LIT( KItem1, "Heap size" );	  
        _LIT( KItem8b, "Heap base address" );	        	       
        _LIT( KItem1b, "Shared" );	        
        _LIT( KItem2, "Chunk size" );
        _LIT( KItem3, "Alloc. count" );
        _LIT( KItem4, "Free. count" );
        _LIT( KItem5, "Biggest alloc." );
        _LIT( KItem6, "Biggest free" );
        _LIT( KItem6a, "Total alloc." );
        _LIT( KItem6b, "Total free" );
        _LIT( KItem7, "Slack free space" );
        _LIT( KItem8a, "Fragmentation" );
        _LIT( KItem13, "Header size (A)" );
        _LIT( KItem14, "Header size (F)" );
        _LIT( KItem9a, "Overhead (alloc)" );
        _LIT( KItem9b, "Overhead (free)" );
        _LIT( KItem9c, "Overhead (total)" );
        _LIT( KItem9d, "Overhead" );
        _LIT( KItem10, "Min. length" );
        _LIT( KItem11, "Max. length" );
        _LIT( KItem12, "Debug Allocator Library" );
         
        HBufC* hItem = FormatItemLC( KItem0, aHeap->Type() );
        TPtr pItem(hItem->Des());
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();	
        
        hItem = FormatItemLC( KItem1, aHeap->Size() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();	
        
        TUint address( aHeap->BaseAddress() );	
        hItem = FormatItemLC( KItem8b, address );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        if(aHeap->Shared()) //Yes / No value formatting		
            hItem = FormatItemLC( KItem1b, KMemSpyCaptionYes );		
        else		
            hItem = FormatItemLC( KItem1b, KMemSpyCaptionNo );	
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem2, aHeap->ChunkSize() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem3, aHeap->AllocationsCount() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem4, aHeap->FreeCount() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem5, aHeap->BiggestAllocation() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem6, aHeap->BiggestFree() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem6a, aHeap->TotalAllocations() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem6b, aHeap->TotalFree() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem7, aHeap->SlackFreeSpace() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        TReal iOneHundred( aHeap->Size() );
        TReal iValue( aHeap->Fragmentation() );	
        hItem = FormatPercentageItemLC( KItem8a, iOneHundred, iValue );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem13, aHeap->HeaderSizeA() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem14, aHeap->HeaderSizeF() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem9a, aHeap->AllocationOverhead() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem9b, aHeap->FreeOverhead() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem9c, aHeap->TotalOverhead() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        TReal iOverhead( aHeap->Overhead() );	
        hItem = FormatPercentageItemLC( KItem9d, iOneHundred, iOverhead );	
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        hItem = FormatItemLC( KItem10, aHeap->MinLength() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
            
        hItem = FormatItemLC( KItem11, aHeap->MaxLength() );
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
        
        if( aHeap->DebugAllocatorLibrary() )		
            hItem = FormatItemLC( KItem12, KMemSpyCaptionYes );		
        else
            hItem = FormatItemLC( KItem12, KMemSpyCaptionNo );	
        pItem = hItem->Des();
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();
	    }
	else
	    {
        HBufC* hItem = FormatItemLC( _L("Not Found"), _L("") );
        TPtr pItem(hItem->Des());
        model->AppendL( pItem );
        CleanupStack::PopAndDestroy( hItem );
        pItem.Zero();	    
	    }
	
	return model;
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, const TDesC& aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	pRetBuf.Append( _L("\t\t") );
	pRetBuf.Append( aValue );
	return retBuf;
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, TInt aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
    
	TBuf<KMemSpyBufferSize> val;
    val.Format( KMemSpyDecFormat, aValue );
    
    pRetBuf.Append( _L("\t") );
    pRetBuf.Append( aCaption );
    pRetBuf.Append( _L("\t\t") );
    pRetBuf.Append( val );    
	
	return retBuf;
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, TUint aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
    
	TBuf<KMemSpyBufferSize> val;
    //val.Format( KMemSpyDecFormat, aValue );
	val.Format( KMemSpyHexFormat, aValue );
    
    pRetBuf.Append( _L("\t") );
    pRetBuf.Append( aCaption );
    pRetBuf.Append( _L("\t\t") );
    pRetBuf.Append( val );    
	
	return retBuf;	
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, const TInt64& aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
	    
	TBuf<KMemSpyBufferSize> val;
	val.Format( KMemSpyLongFormat, aValue );
	    
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	pRetBuf.Append( _L("\t\t") );
	pRetBuf.Append( val );    
		
	return retBuf;	
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, TAny* aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
		    
	TBuf<KMemSpyBufferSize> val;
	val.Format( KMemSpyHexFormat, aValue );
		    
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	pRetBuf.Append( _L("\t\t") );
	pRetBuf.Append( val );    
			
	return retBuf;	
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, TUint* aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
		    
	TBuf<KMemSpyBufferSize> val;
	val.Format( KMemSpyHexFormat, aValue );
		    
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	pRetBuf.Append( _L("\t\t") );
	pRetBuf.Append( val );    
			
	return retBuf;	
	}

HBufC* CMemSpyViewKernelHeap::FormatItemLC( const TDesC& aCaption, TUint8* aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
		    
	TBuf<KMemSpyBufferSize> val;
	val.Format( KMemSpyHexFormat, aValue );
		    
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	pRetBuf.Append( _L("\t\t") );
	pRetBuf.Append( val );    
			
	return retBuf;	
	}

HBufC* CMemSpyViewKernelHeap::FormatPercentageItemLC( const TDesC& aCaption, TReal aOneHundredPercentValue, TReal aValue )
	{
	HBufC* retBuf = HBufC::NewLC( KMaxName );	//buffer for formatted item
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
	
    const TReal value = (TInt) (( aValue / aOneHundredPercentValue) * 100.0); // percentage value calculation    
    _LIT(KPercentFormat, "%3.2f %%");
    TMemSpyPercentText val;
    val.Format( KPercentFormat, value );
    
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	pRetBuf.Append( _L("\t\t") );
	pRetBuf.Append( val );    
			
	return retBuf;
	}
