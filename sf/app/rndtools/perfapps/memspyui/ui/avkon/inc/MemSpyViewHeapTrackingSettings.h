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

#ifndef MEMSPYVIEWHEAPTRACKINGSETTINGS_H
#define MEMSPYVIEWHEAPTRACKINGSETTINGS_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>
#include <aknsettingitemlist.h> 
#include <akncheckboxsettingpage.h>
#include <AknQueryDialog.h> 


// User includes
#include "MemSpyViewBase.h"

// Classes referenced
class RMemSpySession;


class CMemSpyViewHeapTrackingSettings : public CMemSpyViewBase
    {
public:
    //CMemSpyViewHeapTrackingSettings( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
	CMemSpyViewHeapTrackingSettings( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewHeapTrackingSettings();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();
    void GetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    void SetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Internal methods
    static void PrepareItemBufL( TDes& aBuf, const TDesC& aCaption, TBool aStatus, TBool aHeapCategorySelected );
    };




class CSWMTCategorySelectionCheckBoxSettingItem : public CAknSettingItem
    {
public:
    CSWMTCategorySelectionCheckBoxSettingItem( TInt aIdentifier, TInt& aCategories );
    virtual ~CSWMTCategorySelectionCheckBoxSettingItem();

public:
    CSelectionItemList* ItemArray() const;
    
protected:
    void CompleteConstructionL();
    void StoreL();
    void LoadL();
    void HandleSettingPageEventL( CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType );

private:
    void AddNewItemToArrayL(const TDesC& aLabel); 
    
private:
    CSelectionItemList*     iItemArray;
    HBufC*                  iSettingText;
    TInt&                   iExtCategories;
    };






class CMemSpySWMTCategorySelectionCheckBoxSettingPage : public CAknCheckBoxSettingPage
    {
    public:
        CMemSpySWMTCategorySelectionCheckBoxSettingPage( TInt aResourceID, CSelectionItemList* aItemArray );
    public:     // New functions
        void UpdateCba();
    };
    





class CMemSpyAllowEmptyDataDialog : public CAknTextQueryDialog
    {
    public:
        CMemSpyAllowEmptyDataDialog( TDes& aText, const TTone& aTone = ENoTone );
    protected: // from CAknTextQueryDialog
        void UpdateLeftSoftKeyL();
    };

#endif
