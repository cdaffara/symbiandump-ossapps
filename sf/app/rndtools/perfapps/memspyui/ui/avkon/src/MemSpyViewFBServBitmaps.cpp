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

#include "MemSpyViewFBServBitmaps.h"

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
#include <memspy/engine/memspyenginehelperheap.h>

// User includes
#include "MemSpyUiUtils.h"
#include "MemSpyViewMainMenu.h"
#include "MemSpyViewThreads.h"
#include "MemSpyContainerObserver.h"
#include "MemSpyExportBitmapsToMemoryCardDialog.h"

// Constants
const TInt KMemSpyIdleResetListboxTimerPeriod = 500000;
const TInt KMemSpyImageSlideshowPeriod = 1000000;



CMemSpyViewFBServBase::CMemSpyViewFBServBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps )
:   CMemSpyViewBase( aEngine, aObserver ), iBitmaps( aBitmaps )
    {
    }


CMemSpyViewFBServBase::~CMemSpyViewFBServBase()
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyViewFBServBase::~CMemSpyViewFBServBase() - deleting bitmaps: 0x%08x", iBitmaps );
#endif
    delete iBitmaps;
    }


TBool CMemSpyViewFBServBase::HandleCommandL( TInt aCommand )
    {
    TBool handled = ETrue;
    //
    switch ( aCommand )
        {
	case EMemSpyCmdImagesSlideshow:
    	OnCmdSlideShowL();
        break;
#ifndef __WINS__
	case EMemSpyCmdImagesSaveAllToMemoryCard:
        OnCmdExportToMemoryCardL();
        break;
#endif
	case EMemSpyCmdImagesListing:
        OnCmdImageListingL();
        break;

    default:
        handled = CMemSpyViewBase::HandleCommandL( aCommand );
        break;
        }
    //
    return handled;
    }


void CMemSpyViewFBServBase::OnCmdSlideShowL()
    {
    TInt index = 0;
    CMemSpyViewFBServSlideshow::NewLD( *iBitmaps, index );
    
    // Select the item that was last displayed
    const TInt count = iBitmaps->Count();
    if  ( iListBox && index >= 0 && index < count )
        {
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        DrawDeferred();
        }
    }


void CMemSpyViewFBServBase::OnCmdExportToMemoryCardL()
    {
    // Deletes itself
    CMemSpyExportBitmapsToMemoryCardDialog::NewL( iCoeEnv->FsSession(), *iBitmaps );
    }


void CMemSpyViewFBServBase::OnCmdImageListingL()
    {
    // Begin a new data stream
    _LIT( KMemSpyContext, "Bitmap List - " );
    _LIT( KMemSpyFolder, "Bitmaps" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

    // Set prefix for overall listing
    _LIT(KOverallPrefix, "Bitmap List - ");
    iEngine.Sink().OutputPrefixSetLC( KOverallPrefix );

    // Create header
    CMemSpyEngineFbServBitmap::OutputDataColumnsL( iEngine );
    
    // List items
    const TInt count = iBitmaps->Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyEngineFbServBitmap& bitmap = iBitmaps->At( i );
        //
        bitmap.OutputDataL( iEngine );
        }

    // Tidy up
    CleanupStack::PopAndDestroy(); // prefix

    // End data stream
    iEngine.Sink().DataStreamEndL();
    }






























CMemSpyViewFBServBitmaps::CMemSpyViewFBServBitmaps( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewFBServBase( aEngine, aObserver, NULL )
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyViewFBServBitmaps::CMemSpyViewFBServBitmaps(1) - iBitmaps is: 0x%08x", iBitmaps );
#endif
    }


CMemSpyViewFBServBitmaps::CMemSpyViewFBServBitmaps( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps )
:   CMemSpyViewFBServBase( aEngine, aObserver, aBitmaps )
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyViewFBServBitmaps::CMemSpyViewFBServBitmaps(2) - iBitmaps is: 0x%08x", iBitmaps );
#endif
    }


CMemSpyViewFBServBitmaps::~CMemSpyViewFBServBitmaps()
    {
    delete iIdleResetListboxTimer;
    iBitmapHandles.Close();
    }


void CMemSpyViewFBServBitmaps::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Font & Bitmap Server" );
    SetTitleL( KTitle );

    if  ( iBitmaps == NULL )
        {
        // Prepare bitmap handles
        iEngine.HelperFbServ().GetArrayOfBitmapHandlesL( iBitmapHandles );

        // Create bitmap array - construction occurs asynchronously
        iBitmaps = CMemSpyEngineFbServBitmapArray::NewL( CActive::EPriorityIdle, iBitmapHandles, *this );
        }

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Create idle listbox refresh timer
    if  ( iBitmaps->Count() == 0 )
        {
        iIdleResetListboxTimer = CPeriodic::NewL( CActive::EPriorityLow );
        }
    else
        {
        if  ( aSelectionRune != NULL )
            {
            iCurrentBitmap = reinterpret_cast< CMemSpyEngineFbServBitmap* >( aSelectionRune );
            const TInt index = iBitmaps->BitmapIndexByHandle( iCurrentBitmap->Handle() );
            iListBox->SetCurrentItemIndex( index );
            HandleListBoxItemSelectedL( index );
            }

        iListBox->DrawDeferred();
        }
    }


void CMemSpyViewFBServBitmaps::RefreshL()
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyViewFBServBitmaps::RefreshL() - iBitmaps->Count(): %d", iBitmaps->Count());
#endif

    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewFBServBitmaps::ViewType() const
    {
    return EMemSpyViewTypeFBServBitmaps;
    }


CMemSpyViewBase* CMemSpyViewFBServBitmaps::PrepareParentViewL()
    {
    CMemSpyViewMainMenu* parent = new(ELeave) CMemSpyViewMainMenu( iEngine, iObserver );
    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), (TAny*) ViewType() );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewFBServBitmaps::PrepareChildViewL()
    {
    CMemSpyViewBase* child = new(ELeave) CMemSpyViewFBServBitmapInfo( iEngine, iObserver, iBitmaps, *iCurrentBitmap );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iBitmaps = NULL;

    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewFBServBitmaps::HandleFbServBitmapArrayEventL( TEvent aEvent )
    {
    if  ( aEvent == EBitmapItemsCreated )
        {
        }
    else if ( aEvent == EBitmapArrayConstructionComplete )
        {
        }
    //
    iIdleResetListboxTimer->Cancel();
    iIdleResetListboxTimer->Start( KMemSpyIdleResetListboxTimerPeriod/2, KMemSpyIdleResetListboxTimerPeriod, TCallBack( IdleUpdateListBoxModel, this ) );
    }


void CMemSpyViewFBServBitmaps::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( iBitmaps );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewFBServBitmaps::HandleListBoxItemActionedL( TInt aIndex )
    {
    const TInt count = iBitmaps->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineFbServBitmap& bitmap = iBitmaps->At( aIndex );
        iCurrentBitmap = &bitmap;
        }
    else
        {
        iCurrentBitmap = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemActioned );
    }


void CMemSpyViewFBServBitmaps::HandleListBoxItemSelectedL( TInt aIndex )
    {
    const TInt count = iBitmaps->Count();
    if  ( aIndex >= 0 && aIndex < count )
        {
        CMemSpyEngineFbServBitmap& bitmap = iBitmaps->At( aIndex );
        iCurrentBitmap = &bitmap;
        }
    else
        {
        iCurrentBitmap = NULL;
        }

    // Notify observer about item selection
    ReportEventL( MMemSpyViewObserver::EEventItemSelected );
    }


TInt CMemSpyViewFBServBitmaps::IdleUpdateListBoxModel( TAny* aSelf )
    {
    CMemSpyViewFBServBitmaps* self = reinterpret_cast< CMemSpyViewFBServBitmaps* >( aSelf );
    TRAP_IGNORE( self->DoIdleUpdateListBoxModelL() );
    return EFalse;
    }


void CMemSpyViewFBServBitmaps::DoIdleUpdateListBoxModelL()
    {
    // Try to maintain current item selection if at all possible.
    TInt handle = 0;
    if  ( iCurrentBitmap )
        {
        handle = iCurrentBitmap->Handle();
        iCurrentBitmap = NULL;
        }
   
    // Update list box & model
    iListBox->HandleItemAdditionL();
    iListBox->DrawDeferred();
    
    // Try to select previous item if it is still available
    const TInt index = iBitmaps->BitmapIndexByHandle( handle );
    if  ( index >= 0 && index < iBitmaps->Count() )
        {
        iListBox->SetCurrentItemIndex( index );
        HandleListBoxItemSelectedL( index );
        }

    iIdleResetListboxTimer->Cancel();
    }

































CMemSpyViewFBServBitmapInfo::CMemSpyViewFBServBitmapInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps, CMemSpyEngineFbServBitmap& aSelectedBitmap )
:   CMemSpyViewFBServBase( aEngine, aObserver, aBitmaps ), iBitmapObject( aSelectedBitmap )
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyViewFBServBitmapInfo::CMemSpyViewFBServBitmapInfo() - iBitmaps is: 0x%08x", iBitmaps );
#endif
    }


void CMemSpyViewFBServBitmapInfo::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Bitmap Details" );
    SetTitleL( KTitle );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    
    if  ( iBitmaps->Count() )
        {
        iListBox->SetCurrentItemIndex( 0 );
        HandleListBoxItemSelectedL( 0 );
        }
    }


void CMemSpyViewFBServBitmapInfo::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewFBServBitmapInfo::ViewType() const
    {
    return EMemSpyViewTypeFBServBitmapInfo;
    }


CMemSpyViewBase* CMemSpyViewFBServBitmapInfo::PrepareParentViewL()
    {
    CMemSpyViewFBServBitmaps* parent = new(ELeave) CMemSpyViewFBServBitmaps( iEngine, iObserver, iBitmaps );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iBitmaps = NULL;

    CleanupStack::PushL( parent );
    parent->ConstructL( Rect(), *Parent(), &iBitmapObject );
    CleanupStack::Pop( parent );
    return parent;
    }


CMemSpyViewBase* CMemSpyViewFBServBitmapInfo::PrepareChildViewL()
    {
    CMemSpyViewFBServBitmapViewer* child = new(ELeave) CMemSpyViewFBServBitmapViewer( iEngine, iObserver, iBitmaps, iBitmapObject );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iBitmaps = NULL;

    CleanupStack::PushL( child );
    CEikAppUi* appUi = static_cast< CEikAppUi* >( iEikonEnv->AppUi() );
    const TRect rect( TPoint( 0, 0 ), appUi->ClientRect().Size() );
    child->ConstructL( rect, *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewFBServBitmapInfo::SetListBoxModelL()
    {
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( &iBitmapObject );
    listbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }


void CMemSpyViewFBServBitmapInfo::HandleListBoxItemActionedL( TInt aIndex )
    {
    // Only report the event if its the first item that was actioned
    if  ( aIndex == 0 )
        {
        ReportEventL( MMemSpyViewObserver::EEventItemActioned );
        }
    }






























CMemSpyViewFBServBitmapViewer::CMemSpyViewFBServBitmapViewer( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps, CMemSpyEngineFbServBitmap& aSelectedBitmap )
:   CMemSpyViewFBServBase( aEngine, aObserver, aBitmaps ), iBitmapObject( aSelectedBitmap )
    {
#ifdef _DEBUG
    RDebug::Printf("CMemSpyViewFBServBitmapViewer::CMemSpyViewFBServBitmapViewer() - iBitmaps is: 0x%08x", iBitmaps );
#endif
    }


CMemSpyViewFBServBitmapViewer::~CMemSpyViewFBServBitmapViewer()
    {
    delete iImage;
    }


void CMemSpyViewFBServBitmapViewer::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "Bitmap Preview" );
    SetTitleL( KTitle );

    // Finish construction
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );

    // Prepare image
    iImage = new(ELeave) CEikImage();
    iImage->SetContainerWindowL( *this );
    iImage->SetPicture( &iBitmapObject.Bitmap(), NULL );
    iImage->SetPictureOwnedExternally( ETrue );
    SizeChanged();
    }


void CMemSpyViewFBServBitmapViewer::RefreshL()
    {
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewFBServBitmapViewer::ViewType() const
    {
    return EMemSpyViewTypeFBServBitmapViewer;
    }


CMemSpyViewBase* CMemSpyViewFBServBitmapViewer::PrepareParentViewL()
    {
    CMemSpyViewFBServBitmapInfo* parent = new(ELeave) CMemSpyViewFBServBitmapInfo( iEngine, iObserver, iBitmaps, iBitmapObject );

    // Ownership is transferred to child. Cleanupstack guarantees it will be destroyed now.
    // This object is about to die in any case.
    iBitmaps = NULL;

    CleanupStack::PushL( parent );
    CEikAppUi* appUi = static_cast< CEikAppUi* >( iEikonEnv->AppUi() );
    const TRect rect( TPoint( 0, 0 ), appUi->ClientRect().Size() );
    parent->ConstructL( rect, *Parent() );
    CleanupStack::Pop( parent );
    return parent;
    }


void CMemSpyViewFBServBitmapViewer::SetListBoxModelL()
    {
    }


CEikListBox* CMemSpyViewFBServBitmapViewer::ConstructListBoxL()
    {
    return NULL;
    }


void CMemSpyViewFBServBitmapViewer::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    //
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( Rect() );
    //
    gc.SetBrushColor( KRgbBlack );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( iBorderRect );
    }
    

void CMemSpyViewFBServBitmapViewer::SizeChanged()
    {
    TRect rect( Rect() );
    TSize size( rect.Size() );
    //
    TSize bitmapSize( iBitmapObject.Bitmap().SizeInPixels() );

    // Try to center image, if possible.
    if  ( bitmapSize.iWidth < size.iWidth )
        {
        const TInt offset = ( size.iWidth - bitmapSize.iWidth) / 2;
        rect.Move( offset, 0 );
        size.iWidth = bitmapSize.iWidth;
        }
    if  ( bitmapSize.iHeight < size.iHeight )
        {
        const TInt offset = ( size.iHeight - bitmapSize.iHeight) / 2;
        rect.Move( 0, offset );
        size.iHeight = bitmapSize.iHeight;
        }
    //
    rect.SetSize( size );
    if  ( iImage )
        {
        iImage->SetRect( rect );
        }
    iBorderRect = rect;
    iBorderRect.Grow( 2, 2 );
    }


TInt CMemSpyViewFBServBitmapViewer::CountComponentControls() const
    {
    TInt count = 0;
    //
    if  ( iImage )
        {
        ++count;
        }
    //
    return count;
    }


CCoeControl* CMemSpyViewFBServBitmapViewer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iImage;
    }

















CMemSpyViewFBServSlideshow::CMemSpyViewFBServSlideshow( CMemSpyEngineFbServBitmapArray& aBitmaps, TInt& aIndex )
:   iBitmaps( aBitmaps ), iIndex( aIndex )
    {
    }


CMemSpyViewFBServSlideshow::~CMemSpyViewFBServSlideshow()
    {
    delete iTimer;
    }


void CMemSpyViewFBServSlideshow::ConstructL()
    {
    iTimer = CPeriodic::NewL( CActive::EPriorityLow );
    iTimer->Start( KMemSpyImageSlideshowPeriod, KMemSpyImageSlideshowPeriod, TCallBack( IdleUpdate, this ) );
    }


void CMemSpyViewFBServSlideshow::NewLD( CMemSpyEngineFbServBitmapArray& aBitmaps, TInt& aIndex )
    {
    CMemSpyViewFBServSlideshow* self = new(ELeave) CMemSpyViewFBServSlideshow( aBitmaps, aIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    //
    self->ExecuteLD( R_MEMSPY_IMAGE_SLIDESHOW );
    }


void CMemSpyViewFBServSlideshow::PreLayoutDynInitL()
    {
    ShowNextImageL();
    }


TBool CMemSpyViewFBServSlideshow::OkToExitL( TInt /*aButtonId*/ )
    {
    return ETrue;
    }


TInt CMemSpyViewFBServSlideshow::IdleUpdate( TAny* aSelf )
    {
    CMemSpyViewFBServSlideshow* self = reinterpret_cast< CMemSpyViewFBServSlideshow* >( aSelf );
    TRAP_IGNORE( self->ShowNextImageL() );
    return EFalse;
    }


void CMemSpyViewFBServSlideshow::ShowNextImageL()
    {
	CEikImage* imageControl = static_cast< CEikImage* >( Control(EMemSpyCtrlIdSlideShowImage) );

    // Get the right image
    const TInt imageCount = iBitmaps.Count();
    if  ( iIndex < imageCount )
        {
        CMemSpyEngineFbServBitmap& bitmap = iBitmaps.At( iIndex );
        imageControl->SetPicture( &bitmap.Bitmap(), NULL );
        imageControl->SetPictureOwnedExternally( ETrue );
        
        // Prepare title
        const TSize size( bitmap.Bitmap().SizeInPixels() );
        _LIT(KTitleFormat, "%3d x %3d");
        TBuf<128> title;
        title.Format( KTitleFormat, size.iWidth, size.iHeight );
        SetTitleL( title );
        //
        User::ResetInactivityTime();
        //
        Layout();
        //
        ++iIndex;
        }
    else
        {
        // Dismiss myself.
        delete this;
        }
    }

