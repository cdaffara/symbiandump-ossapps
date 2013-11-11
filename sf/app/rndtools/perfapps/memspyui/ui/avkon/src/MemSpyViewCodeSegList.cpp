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

#include "MemSpyViewCodeSegList.h"

// System includes
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyViewThreads.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyExportBitmapsToMemoryCardDialog.h"



CMemSpyViewCodeSegBase::CMemSpyViewCodeSegBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineCodeSegList* aList )
:   CMemSpyViewBase( aEngine, aObserver ), iList( aList )
    {
    }


CMemSpyViewCodeSegBase::~CMemSpyViewCodeSegBase()
    {
    delete iList;
    }


TBool CMemSpyViewCodeSegBase::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
    case EMemSpyCmdCodeSegSortByName:
        iList->SortByFileNameL();
        CMemSpyViewBase::RefreshL();
        break;
    case EMemSpyCmdCodeSegSortByCodeSize:
        iList->SortByCodeSizeL();
        CMemSpyViewBase::RefreshL();
        break;
    case EMemSpyCmdCodeSegSortByDataSize:
        iList->SortByDataSizeL();
        CMemSpyViewBase::RefreshL();
        break;
    case EMemSpyCmdCodeSegSortByUid:
        iList->SortByUidsL();
        CMemSpyViewBase::RefreshL();
        break;
    case EMemSpyCmdCodeSegListing:
        OnCmdCodeSegmentListingL();
        break;
    case EMemSpyCmdCodeSegShowItemsAll:
        OnCmdShowItemsAllL();
        break;
    case EMemSpyCmdCodeSegShowItemsGlobalData:
        OnCmdShowItemsGlobalDataL();
        break;

    case EMemSpyCmdCodeSegShowCapsWithTCBProcess:
    case EMemSpyCmdCodeSegShowCapsWithTCBAll:              
        OnCmdFilterByCapabilityL( ECapabilityTCB, aCommand == EMemSpyCmdCodeSegShowCapsWithTCBAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithCommDDProcess:
    case EMemSpyCmdCodeSegShowCapsWithCommDDAll:
        OnCmdFilterByCapabilityL( ECapabilityCommDD, aCommand == EMemSpyCmdCodeSegShowCapsWithCommDDAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithPowerMgmtProcess:
    case EMemSpyCmdCodeSegShowCapsWithPowerMgmtAll:
        OnCmdFilterByCapabilityL( ECapabilityPowerMgmt, aCommand == EMemSpyCmdCodeSegShowCapsWithPowerMgmtAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithMultimediaDDProcess:
    case EMemSpyCmdCodeSegShowCapsWithMultimediaDDAll:
        OnCmdFilterByCapabilityL( ECapabilityMultimediaDD, aCommand == EMemSpyCmdCodeSegShowCapsWithMultimediaDDAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithReadDeviceDataProcess:
    case EMemSpyCmdCodeSegShowCapsWithReadDeviceDataAll:
        OnCmdFilterByCapabilityL( ECapabilityReadDeviceData, aCommand == EMemSpyCmdCodeSegShowCapsWithReadDeviceDataAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithWriteDeviceDataProcess:
    case EMemSpyCmdCodeSegShowCapsWithWriteDeviceDataAll:
        OnCmdFilterByCapabilityL( ECapabilityWriteDeviceData, aCommand == EMemSpyCmdCodeSegShowCapsWithWriteDeviceDataAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithDRMProcess:
    case EMemSpyCmdCodeSegShowCapsWithDRMAll:
        OnCmdFilterByCapabilityL( ECapabilityDRM, aCommand == EMemSpyCmdCodeSegShowCapsWithDRMAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithTrustedUIProcess:
    case EMemSpyCmdCodeSegShowCapsWithTrustedUIAll:
        OnCmdFilterByCapabilityL( ECapabilityTrustedUI, aCommand == EMemSpyCmdCodeSegShowCapsWithTrustedUIAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithProtServProcess:
    case EMemSpyCmdCodeSegShowCapsWithProtServAll:
        OnCmdFilterByCapabilityL( ECapabilityProtServ, aCommand == EMemSpyCmdCodeSegShowCapsWithProtServAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithDiskAdminProcess:
    case EMemSpyCmdCodeSegShowCapsWithDiskAdminAll:
        OnCmdFilterByCapabilityL( ECapabilityDiskAdmin, aCommand == EMemSpyCmdCodeSegShowCapsWithDiskAdminAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithNetworkControlProcess:
    case EMemSpyCmdCodeSegShowCapsWithNetworkControlAll:
        OnCmdFilterByCapabilityL( ECapabilityNetworkControl, aCommand == EMemSpyCmdCodeSegShowCapsWithNetworkControlAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithAllFilesProcess:
    case EMemSpyCmdCodeSegShowCapsWithAllFilesAll:
        OnCmdFilterByCapabilityL( ECapabilityAllFiles, aCommand == EMemSpyCmdCodeSegShowCapsWithAllFilesAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithSwEventProcess:
    case EMemSpyCmdCodeSegShowCapsWithSwEventAll:
        OnCmdFilterByCapabilityL( ECapabilitySwEvent, aCommand == EMemSpyCmdCodeSegShowCapsWithSwEventAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithNetworkServicesProcess:
    case EMemSpyCmdCodeSegShowCapsWithNetworkServicesAll:
        OnCmdFilterByCapabilityL( ECapabilityNetworkServices, aCommand == EMemSpyCmdCodeSegShowCapsWithNetworkServicesAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithLocalServicesProcess:
    case EMemSpyCmdCodeSegShowCapsWithLocalServicesAll:
        OnCmdFilterByCapabilityL( ECapabilityLocalServices, aCommand == EMemSpyCmdCodeSegShowCapsWithLocalServicesAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithReadUserDataProcess:
    case EMemSpyCmdCodeSegShowCapsWithReadUserDataAll:
        OnCmdFilterByCapabilityL( ECapabilityReadUserData, aCommand == EMemSpyCmdCodeSegShowCapsWithReadUserDataAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithWriteUserDataProcess:
    case EMemSpyCmdCodeSegShowCapsWithWriteUserDataAll:
        OnCmdFilterByCapabilityL( ECapabilityWriteUserData, aCommand == EMemSpyCmdCodeSegShowCapsWithWriteUserDataAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithLocationProcess:
    case EMemSpyCmdCodeSegShowCapsWithLocationAll:
        OnCmdFilterByCapabilityL( ECapabilityLocation, aCommand == EMemSpyCmdCodeSegShowCapsWithLocationAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithSurroundingsDDProcess:
    case EMemSpyCmdCodeSegShowCapsWithSurroundingsDDAll:
        OnCmdFilterByCapabilityL( ECapabilitySurroundingsDD, aCommand == EMemSpyCmdCodeSegShowCapsWithSurroundingsDDAll );
        break;
    case EMemSpyCmdCodeSegShowCapsWithUserEnvProcess:
    case EMemSpyCmdCodeSegShowCapsWithUserEnvAll:
        OnCmdFilterByCapabilityL( ECapabilityUserEnvironment, aCommand == EMemSpyCmdCodeSegShowCapsWithUserEnvAll );
        break;
    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewCodeSegBase::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewCodeSegBase::OnCmdCodeSegmentListingL()
    {
    // Begin a new data stream
    _LIT( KMemSpyContext, "CodeSeg List - " );
    _LIT( KMemSpyFolder, "CodeSegs" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

    // Set prefix for overall listing
    _LIT(KOverallPrefix, "CodeSeg List - ");
    iEngine.Sink().OutputPrefixSetLC( KOverallPrefix );

    // Create header
    CMemSpyEngineCodeSegList::OutputDataColumnsL( iEngine );
    
    // List items
    const TInt count = iList->Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyEngineCodeSegEntry& entry = iList->At( i );
        //
        entry.OutputDataL( iEngine.HelperCodeSegment() );
        }

    // Tidy up
    CleanupStack::PopAndDestroy(); // prefix

    // End data stream
    iEngine.Sink().DataStreamEndL();
    }


void CMemSpyViewCodeSegBase::OnCmdShowItemsAllL()
    {
    iList->ShowAllL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewCodeSegBase::OnCmdShowItemsGlobalDataL()
    {
    iList->ShowOnlyEntriesWithGlobalDataL();
    CMemSpyViewBase::RefreshL();
    }


void CMemSpyViewCodeSegBase::OnCmdFilterByCapabilityL( TCapability aCapability, TBool aAllBinaries )
    {
    TMemSpyViewCodeSegFilter filterLogic( aCapability, aAllBinaries );
    TMemSpyEngineVisibiltyFunction<CMemSpyEngineCodeSegEntry> filterFunction( TMemSpyViewCodeSegFilter::FilterItem, &filterLogic );
    iList->ShowL( filterFunction );
    CMemSpyViewBase::RefreshL();
    }





























CMemSpyViewCodeSegList::CMemSpyViewCodeSegList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewCodeSegBase( aEngine, aObserver, NULL )
    {
    }


CMemSpyViewCodeSegList::CMemSpyViewCodeSegList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineCodeSegList& aList )
:   CMemSpyViewCodeSegBase( aEngine, aObserver, &aList )
    {
    }


void CMemSpyViewCodeSegList::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Code Segments" );
    SetTitleL( KTitle );

    if  ( iList == NULL )
        {
        // Prepare code segment list
        iList = iEngine.HelperCodeSegment().CodeSegmentListL();
        iList->SortByCodeSizeL();
        }

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    if  ( aSelectionRune != NULL )
        {
        iCurrentCodeSegment = reinterpret_cast< CMemSpyEngineCodeSegEntry* >( aSelectionRune );
        const TInt index = iList->ItemIndex( *iCurrentCodeSegment );
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        }

    iListBox->DrawDeferred();
    }


TMemSpyViewType CMemSpyViewCodeSegList::ViewType() const
    {
    return EMemSpyViewTypeCodeSegmentList;
    }


CMemSpyViewBase* CMemSpyViewCodeSegList::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewCodeSegList::PrepareChildViewL()
    {
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewCodeSegDetails( iEngine, iObserver, *iList, *iCurrentCodeSegment );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iList = NULL;

    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewCodeSegList::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( iList );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewCodeSegList::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iList->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineCodeSegEntry& codeSeg = iList->At( aIndex );
        iCurrentCodeSegment = &codeSeg;
        }
    else
        {
        iCurrentCodeSegment = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewCodeSegList::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iList->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineCodeSegEntry& codeSeg = iList->At( aIndex );
        iCurrentCodeSegment = &codeSeg;
        }
    else
        {
        iCurrentCodeSegment = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }



































CMemSpyViewCodeSegDetails::CMemSpyViewCodeSegDetails( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineCodeSegList& aList, CMemSpyEngineCodeSegEntry& aSelectedCodeSegment )
:   CMemSpyViewCodeSegBase( aEngine, aObserver, &aList ), iCodeSegment( aSelectedCodeSegment )
    {
    }


void CMemSpyViewCodeSegDetails::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Code Segment Details" );
    SetTitleL( KTitle );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewCodeSegDetails::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewCodeSegDetails::ViewType() const
    {
    return EMemSpyViewTypeCodeSegmentDetails;
    }


CMemSpyViewBase* CMemSpyViewCodeSegDetails::PrepareParentViewL()
    {
    CMemSpyViewCodeSegList* parent = new(ELeave) CMemSpyViewCodeSegList( iEngine, iObserver, *iList );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iList = NULL;

    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), &iCodeSegment );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewCodeSegDetails::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iCodeSegment );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }





















TBool TMemSpyViewCodeSegFilter::FilterItem( const CMemSpyEngineCodeSegEntry*& aItem, TAny* aRune )
    {
    TMemSpyViewCodeSegFilter* self = reinterpret_cast< TMemSpyViewCodeSegFilter* >( aRune );

    TBool match = aItem->HasCapability( self->iCapability );
    if  ( match && self->iAllBinaries == EFalse )
        {
        // We're only looking for EXE's...
        TParsePtrC parser( aItem->FileName() );
        const TPtrC pExtension( parser.Ext() );
        _LIT(KProcessExtension, ".exe");
        //
        match = ( pExtension.CompareF( KProcessExtension ) == 0 );
        }
    //
    return match;
    }



