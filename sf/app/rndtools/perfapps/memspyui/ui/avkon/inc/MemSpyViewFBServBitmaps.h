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

#ifndef MEMSPYVIEWFBSERVBITMAPS_H
#define MEMSPYVIEWFBSERVBITMAPS_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>
#include <eikimage.h>
#include <AknDialog.h>

// User includes
#include "MemSpyViewBase.h"

// Engine includes
#include <memspy/engine/memspyenginehelperfbserv.h>



class CMemSpyViewFBServBase : public CMemSpyViewBase
    {
public:
    CMemSpyViewFBServBase( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps );
    ~CMemSpyViewFBServBase();

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_IMAGES; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdImages; }

protected: // Command handlers
    void OnCmdSlideShowL();
    void OnCmdExportToMemoryCardL();
    void OnCmdImageListingL();

protected: // Data members
    CMemSpyEngineFbServBitmapArray* iBitmaps;
    };





class CMemSpyViewFBServBitmaps : public CMemSpyViewFBServBase, public MMemSpyEngineFbSerbBitmapArrayObserver
    {
public:
    CMemSpyViewFBServBitmaps( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    CMemSpyViewFBServBitmaps( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps );
    ~CMemSpyViewFBServBitmaps();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From MMemSpyEngineFbSerbBitmapArrayObserver
    void HandleFbServBitmapArrayEventL( TEvent aEvent );

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );
    void HandleListBoxItemSelectedL( TInt aIndex );

private: // Idle timer update
    static TInt IdleUpdateListBoxModel( TAny* aSelf );
    void DoIdleUpdateListBoxModelL();

private: // Data members
    RArray<TInt> iBitmapHandles;
    CMemSpyEngineFbServBitmap* iCurrentBitmap;
    CPeriodic* iIdleResetListboxTimer;
    };



class CMemSpyViewFBServBitmapInfo : public CMemSpyViewFBServBase
    {
public:
    CMemSpyViewFBServBitmapInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps, CMemSpyEngineFbServBitmap& aSelectedBitmap );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aIndex );

private: // Data members
    CMemSpyEngineFbServBitmap& iBitmapObject;
    };




class CMemSpyViewFBServBitmapViewer : public CMemSpyViewFBServBase
    {
public:
    CMemSpyViewFBServBitmapViewer( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyEngineFbServBitmapArray* aBitmaps, CMemSpyEngineFbServBitmap& aSelectedBitmap );
    ~CMemSpyViewFBServBitmapViewer();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    CEikListBox* ConstructListBoxL();

private: // From CCoeControl
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // Data members
    CMemSpyEngineFbServBitmap& iBitmapObject;
    CEikImage* iImage;
    TRect iBorderRect;
    };





class CMemSpyViewFBServSlideshow : public CAknDialog
    {
public:
    static void NewLD( CMemSpyEngineFbServBitmapArray& aBitmaps, TInt& aIndex );
    ~CMemSpyViewFBServSlideshow();

private:
    CMemSpyViewFBServSlideshow( CMemSpyEngineFbServBitmapArray& aBitmaps, TInt& aIndex );
    void ConstructL();

private:
    void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);

private: // Idle timer update
    static TInt IdleUpdate( TAny* aSelf );
    void ShowNextImageL();

private: // Data members
    CMemSpyEngineFbServBitmapArray& iBitmaps;
    TInt& iIndex;
    CPeriodic* iTimer;
    };




#endif
