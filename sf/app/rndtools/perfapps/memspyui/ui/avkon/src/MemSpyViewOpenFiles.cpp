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

#include "MemSpyViewOpenFiles.h"

// System includes
#include <AknIconArray.h>
#include <eikdef.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <gulicon.h>
#include <avkon.mbg>

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
#include "MemSpyViewThreads.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyContainerObserver.h"




CMemSpyViewOpenFiles::CMemSpyViewOpenFiles( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }


CMemSpyViewOpenFiles::~CMemSpyViewOpenFiles()
    {
    iThreadIds.Close();
    iFileNames.Close();
    }


void CMemSpyViewOpenFiles::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Open Files" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    }


CEikListBox* CMemSpyViewOpenFiles::ConstructListBoxL()
    {
    delete iListBox;
    iListBox = NULL;
    CAknDoubleGraphicStyleListBox* listbox = new (ELeave) CAknDoubleGraphicStyleListBox();
    iListBox = listbox;
    //
    listbox->ConstructL( this, EAknListBoxSelectionList | EAknListBoxLoopScrolling );
    listbox->SetContainerWindowL( *this );
    listbox->CreateScrollBarFrameL( ETrue );
    SetListBoxModelL();
    listbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listbox->SetListBoxObserver( this );
    listbox->SetObserver( this );

    // Create icon array
    CAknIconArray* iconArray = new (ELeave) CAknIconArray(1);
    CleanupStack::PushL( iconArray );

    // Create bitmap
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    const TInt error1 = bitmap->Create( TSize(10,10), EColor16M );
    User::LeaveIfError( error1 );

    // Create mask
    CFbsBitmap* mask = new(ELeave) CFbsBitmap();
    CleanupStack::PushL( mask );
    const TInt error2 = mask->Create( TSize(10,10), EColor16M );
    User::LeaveIfError( error2 );

    // Create icon & transfer bitmap
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2, bitmap );
    CleanupStack::PushL( icon );

    // Transfer icon to array
    iconArray->AppendL( icon );
    CleanupStack::Pop( icon );
    
    // Give icon array to listbox
    static_cast<CEikFormattedCellListBox*>(iListBox)->ItemDrawer()->FormattedCellData()->SetIconArrayL(iconArray);
    CleanupStack::Pop( iconArray );
    //
    return listbox;
    }


void CMemSpyViewOpenFiles::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewOpenFiles::ViewType() const
    {
    return EMemSpyViewTypeOpenFiles;
    }


TBool CMemSpyViewOpenFiles::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
	case EMemSpyCmdToolsListOpenFiles:
        OnCmdListOpenFilesL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewOpenFiles::OnCmdListOpenFilesL()
    {
    iEngine.ListOpenFilesL();
    }


CMemSpyViewBase* CMemSpyViewOpenFiles::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewOpenFiles::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;

    // First, try to find the selected thread
    if  ( iActionedThreadId )
        {
        // Try to create a view of the thread in question
        CMemSpyProcess* process = NULL;
        CMemSpyThread* thread = NULL;
        const TInt error = iEngine.Container().ProcessAndThreadByThreadId( *iActionedThreadId, process, thread );
        if  ( error == KErrNone && thread != NULL )
            {
            child = new(ELeave) CMemSpyViewThreads( iEngine, iObserver, thread->Process() );
            CleanupStack::PushL( child );
            child->ConstructL( Rect(), *Parent(), thread );
            CleanupStack::Pop( child );
            }
        }
    //
    return child;
    }


void CMemSpyViewOpenFiles::SetListBoxModelL()
    {
    _LIT(KLineFormatSpec, "%d\t%S\t%S %S");

    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 50> item;
    TMemSpySizeText valueBuf;
    TBuf<64> timeBuf;

    iFileNames.Reset();
    iThreadIds.Reset();
    iActionedThreadId = NULL;

    RFs& fsSession = iCoeEnv->FsSession();
    TOpenFileScan scanner( fsSession );

    CFileList* list = NULL;
    scanner.NextL( list );

    TIdentityRelation<TEntry> comparer( CompareTEntryObjects );

    while( list != NULL )
        {
        CleanupStack::PushL( list );
    
        const TInt entryCount = list->Count();
        for(TInt i=0; i<entryCount; i++)
            {
            const TEntry& entry = (*list)[ i ];

            // Check for duplicates
            const TInt foundIndex = iFileNames.Find( entry, comparer );
            if  ( foundIndex == KErrNotFound )
                {
                // Get time and size format strings
                valueBuf = MemSpyUiUtils::FormatSizeText( entry.iSize );
                MemSpyUiUtils::FormatTimeL( timeBuf, entry.iModified );

                // Get just file name
                TParsePtrC parser( entry.iName );
                const TPtrC pJustName( parser.NameAndExt() );

                // Create item 
                item.Format( KLineFormatSpec, 0, &pJustName, &valueBuf, &timeBuf );
                model->AppendL( item );
                iThreadIds.AppendL( scanner.ThreadId() );
                iFileNames.AppendL( entry );
                }
            }

        CleanupStack::PopAndDestroy( list );
        list = NULL;
        scanner.NextL( list );
        }

    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


void CMemSpyViewOpenFiles::HandleListBoxItemActionedL( TInt aCurrentIndex )
    {
    if  ( aCurrentIndex >= 0 && aCurrentIndex < iThreadIds.Count() )
        {
        iActionedThreadId = &iThreadIds[ aCurrentIndex ];
        }
    else
        {
        iActionedThreadId = NULL;
        }

    // Notify observer about an item being 'fired'
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


TBool CMemSpyViewOpenFiles::CompareTEntryObjects( const TEntry& aLeft, const TEntry& aRight )
    {
    return ( aLeft.iName.CompareF( aRight.iName ) == 0 );
    }


