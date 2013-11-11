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

#include "MemSpyViewROMInfo.h"

// System includes
#include <hal.h>
#ifdef __EPOC32__
#include <e32rom.h>
#endif

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"

// Remap of E32 DP constants, needed so MemSpy can still build on v9.1
enum TMemSpyDemandPagingKernelConfigFlags
    {
    EMemSpyKernelConfigPagingPolicyMask = 3<<5,
    EMemSpyKernelConfigPagingPolicyNoPaging = 0<<5,
    EMemSpyKernelConfigPagingPolicyAlwaysPage = 1<<5,
    EMemSpyKernelConfigPagingPolicyDefaultUnpaged = 2<<5,
    EMemSpyKernelConfigPagingPolicyDefaultPaged = 3<<5,
    };



CMemSpyViewROMInfo::CMemSpyViewROMInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


void CMemSpyViewROMInfo::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "ROM Info" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


void CMemSpyViewROMInfo::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewROMInfo::ViewType() const
    {
    return EMemSpyViewTypeROMInfo;
    }


CMemSpyViewBase* CMemSpyViewROMInfo::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewROMInfo::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    return child;
    }


void CMemSpyViewROMInfo::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;

    TInt value = 0;
    TMemSpySizeText valueBuf;
    TMemSpySizeText valueBuf2;
    
    // 1st item = Total RAM
    _LIT(KItem1Format, "\tROM Size (HAL)\t\t%S");
    HAL::Get( HALData::EMemoryROM, value );
    valueBuf = MemSpyUiUtils::FormatSizeText( value );
    item.Format( KItem1Format, &valueBuf );
    model->AppendL( item );

#ifdef __EPOC32__
    TBuf<128> buf2;
    _LIT( KTimeFormatSpecLocal, "%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
	const TUint main_start = UserSvr::RomHeaderAddress();
    TRomHeader* romHeader = (TRomHeader*) main_start;

    _LIT(KItem2Format, "\tROM Size (Header)\t\t%S");
    valueBuf = MemSpyUiUtils::FormatSizeText( TInt64( romHeader->iRomSize ) );
    item.Format( KItem2Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem2aFormat, "\tROM Page Size\t\t%S");
    HAL::Get( HALData::EMemoryPageSize, value );
    valueBuf = MemSpyUiUtils::FormatSizeText( value );
    item.Format( KItem2aFormat, &valueBuf );
    model->AppendL( item );

    _LIT(KItem10Format, "\tVersion\t\tv%2d.%02d.%06d");
    item.Format( KItem10Format, romHeader->iVersion.iMajor, romHeader->iVersion.iMinor, romHeader->iVersion.iBuild );
    model->AppendL( item );

    _LIT(KItem3Format, "\tTimestamp\t\t%S");
    TTime time( romHeader->iTime );
    time.FormatL( buf2, KTimeFormatSpecLocal );
    item.Format( KItem3Format, &buf2 );
    model->AppendL( item );

    _LIT(KItem4Format, "\tAddress Range\t\t%S - %S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iRomBase );
    MemSpyUiUtils::FormatHex( valueBuf2, (romHeader->iRomBase + romHeader->iRomSize ));
    item.Format( KItem4Format, &valueBuf, &valueBuf2 );
    model->AppendL( item );

    _LIT(KItem5Format, "\tPrimary File Address\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iPrimaryFile );
    item.Format( KItem5Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem6Format, "\tSecondary File Address\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iSecondaryFile );
    item.Format( KItem6Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem7Format, "\tChecksum\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iCheckSum );
    item.Format( KItem7Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem8Format, "\tKernel Config Flags\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iKernelConfigFlags );
    item.Format( KItem8Format, &valueBuf );
    model->AppendL( item );

    if  ( romHeader->iKernelConfigFlags & EKernelConfigIpcV1Available )
        {
        _LIT(KItem8aFormat, "\tIPC v1 Available\t\tYes");
        item.Copy( KItem8aFormat );
        model->AppendL( item );
        }
    if  ( romHeader->iKernelConfigFlags & EKernelConfigPlatSecEnforcement )
        {
        _LIT(KItem8bFormat, "\tPlatSec Enforcement\t\tYes");
        item.Copy( KItem8bFormat );
        model->AppendL( item );
        }
    if  ( romHeader->iKernelConfigFlags & EKernelConfigPlatSecDiagnostics )
        {
        _LIT(KItem8cFormat, "\tPlatSec Diagnostics\t\tYes");
        item.Copy( KItem8cFormat );
        model->AppendL( item );
        }
    if  ( romHeader->iKernelConfigFlags & EKernelConfigPlatSecProcessIsolation )
        {
        _LIT(KItem8dFormat, "\tPlatSec Process Isolation\t\tYes");
        item.Copy( KItem8dFormat );
        model->AppendL( item );
        }
    if  ( romHeader->iKernelConfigFlags & EKernelConfigPlatSecEnforceSysBin )
        {
        _LIT(KItem8eFormat, "\tEnforce \\Sys\\Bin\\\t\tYes");
        item.Copy( KItem8eFormat );
        model->AppendL( item );
        }

    const TUint32 pagingPolicy = ( romHeader->iKernelConfigFlags & EMemSpyKernelConfigPagingPolicyMask );
#ifdef _DEBUG
    RDebug::Printf( "pagingPolicy: %u (0x%08x)", pagingPolicy, pagingPolicy );
#endif
    _LIT(KItem8eFormat, "\tDemand Paging\t\t%S");
    //
    if  ( pagingPolicy & EMemSpyKernelConfigPagingPolicyNoPaging )
        {
        _LIT( KDemandPagingPolicyNoPaging, "No Paging" );
        item.Format( KItem8eFormat, &KDemandPagingPolicyNoPaging );
        }
    else if ( pagingPolicy & EMemSpyKernelConfigPagingPolicyAlwaysPage )
        {
        _LIT( KDemandPagingPolicyAlwaysPage, "Always Paging" );
        item.Format( KItem8eFormat, &KDemandPagingPolicyAlwaysPage);
        }
    else if ( pagingPolicy & EMemSpyKernelConfigPagingPolicyDefaultUnpaged )
        {
        _LIT( KDemandPagingPolicyDefaultUnpaged, "Default Unpaged" );
        item.Format( KItem8eFormat, &KDemandPagingPolicyDefaultUnpaged);
        }
    else if ( pagingPolicy & EMemSpyKernelConfigPagingPolicyDefaultPaged )
        {
        _LIT( KDemandPagingPolicyDefaultPaged, "Default Paged" );
        item.Format( KItem8eFormat, &KDemandPagingPolicyDefaultPaged );
        }
    //
    model->AppendL( item );

    _LIT(KItem9Format, "\tSection Header Address\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iRomSectionHeader );
    item.Format( KItem9Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem11Format, "\tCompressed Size\t\t%S");
    valueBuf = MemSpyUiUtils::FormatSizeText( TInt64( romHeader->iCompressedSize ) );
    item.Format( KItem11Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem12Format, "\tUncompressed Size\t\t%S");
    valueBuf = MemSpyUiUtils::FormatSizeText( TInt64( romHeader->iUncompressedSize ) );
    item.Format( KItem12Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem13Format, "\tDisabled Caps. #1\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iDisabledCapabilities[0] );
    item.Format( KItem13Format, &valueBuf );
    model->AppendL( item );

    _LIT(KItem14Format, "\tDisabled Caps. #2\t\t%S");
    MemSpyUiUtils::FormatHex( valueBuf, romHeader->iDisabledCapabilities[1] );
    item.Format( KItem14Format, &valueBuf );
    model->AppendL( item );

    for(TInt i=0; i<TRomHeader::KNumTraceMask; i++)
        {
        _LIT(KItem15Format, "\tTrace Mask #%d\t\t%S");
        MemSpyUiUtils::FormatHex( valueBuf, romHeader->iTraceMask[i] );
        item.Format( KItem15Format, i+1, &valueBuf );
        model->AppendL( item );
        }

#endif

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


