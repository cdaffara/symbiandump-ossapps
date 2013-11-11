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

#include "MemSpyViewRAMInfo.h"

// System includes
#include <hal.h>
#include <aknnotedialog.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperram.h>
#include <memspy/engine/memspyengineutils.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"






CMemSpyViewRAMInfo::CMemSpyViewRAMInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


void CMemSpyViewRAMInfo::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "RAM Info" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewRAMInfo::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewRAMInfo::ViewType() const
    {
    return EMemSpyViewTypeRAMInfo;
    }


CMemSpyViewBase* CMemSpyViewRAMInfo::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewRAMInfo::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    return child;
    }


void CMemSpyViewRAMInfo::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;

    TInt value;
    TMemSpySizeText size;
    TMemoryInfoV1Buf memInfoBuf;
    UserHal::MemoryInfo( memInfoBuf );
    const TMemoryInfoV1 memInfo( memInfoBuf() );

    // 1st item = Total RAM
    _LIT(KItem1Format, "\tTotal RAM\t\t%S");
    size = MemSpyUiUtils::FormatSizeText( memInfo.iTotalRamInBytes );
    item.Format( KItem1Format, &size );
    model->AppendL( item );

    // 2nd item = Max free RAM
    _LIT(KItem2Format, "\tMax Free RAM\t\t%S");
    size = MemSpyUiUtils::FormatSizeText( memInfo.iMaxFreeRamInBytes );
    item.Format( KItem2Format, &size );
    model->AppendL( item );

    // 3rd item = Free RAM
    _LIT(KItem3Format, "\tFree RAM\t\t%S (%S)");
    const TMemSpyPercentText freeRamPercentage( MemSpyEngineUtils::FormatPercentage( TReal( memInfo.iTotalRamInBytes ), TReal( memInfo.iFreeRamInBytes ) ) );
    size = MemSpyUiUtils::FormatSizeText( memInfo.iFreeRamInBytes );
    item.Format( KItem3Format, &size, &freeRamPercentage );
    model->AppendL( item );

    // 4th item = RAM disk info
    _LIT(KItem4Format, "\tRAM Disk Size\t\t%S");
    size = MemSpyUiUtils::FormatSizeText( memInfo.iInternalDiskRamInBytes );
    item.Format( KItem4Format, &size );
    model->AppendL( item );

    // 5th item = Max RAM disk size
    _LIT(KItem5Format, "\tMax RAM Disk Size\t\t%S");
    HAL::Get( HALData::EMaxRAMDriveSize, value );
    size = MemSpyUiUtils::FormatSizeText( value );
    item.Format( KItem5Format, &size );
    model->AppendL( item );

    // 6th item = RAM page size
    _LIT(KItem6Format, "\tMemory Page Size\t\t%S");
    HAL::Get( HALData::EMemoryPageSize, value );
    size = MemSpyUiUtils::FormatSizeText( value );
    item.Format( KItem6Format, &size );
    model->AppendL( item );

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


TBool CMemSpyViewRAMInfo::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    if  ( iEngine.HelperRAM().IsAknIconCacheConfigurable() )
        {
        switch ( aCommand )
            {
        case EMemSpyCmdRAMAvkonIconCacheDisabled:
            OnCmdSetIconCacheStatusL( EFalse );
		    break;
        case EMemSpyCmdRAMAvkonIconCacheEnabled:
            OnCmdSetIconCacheStatusL( ETrue );
		    break;

        default:
            handled = CMemSpyViewBase::HandleCommandL( aCommand );
            break;
            }
        }
    else
        {
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        }
    //
    return handled;
    }


void CMemSpyViewRAMInfo::OnCmdSetIconCacheStatusL( TBool aEnabled )
    {
    ASSERT( iEngine.HelperRAM().IsAknIconCacheConfigurable() );
    //
    const TInt64 savedAmount = iEngine.HelperRAM().SetAknIconCacheStatusL( aEnabled );
    if ( !aEnabled )
        {
        TBuf<128> buf;
        //
        if  ( savedAmount != 0 ) 
            {
            _LIT(KSavedAmountFormatBuffer, "Saved %S of RAM");
            const TMemSpySizeText savedAmountString( MemSpyEngineUtils::FormatSizeText( savedAmount, 0, EFalse ) );
            buf.Format( KSavedAmountFormatBuffer, &savedAmountString );
            }
        else
            {
            _LIT(KDidNotSaveAnyRAM, "Operation did not save any RAM");
            buf.Copy( KDidNotSaveAnyRAM );
            }

        // Show dialog
	    CAknNoteDialog* note = new( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone );
	    CleanupStack::PushL( note );
	    note->SetTextWrapping( ETrue );
	    note->PrepareLC( R_MEMSPY_RAM_MEMORY_DELTA_DIALOG );
	    note->SetTextL( buf );
	    CleanupStack::Pop( note );
	    note->RunLD();
        }
    //
    RefreshL();
    }
