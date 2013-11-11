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


#ifndef PERFMON_SETTINGSVIEWDLG_H
#define PERFMON_SETTINGSVIEWDLG_H

//  INCLUDES
#include <AknDialog.h>
#include <eiklbo.h>
#include <AknTabObserver.h> 
#include <akntabgrp.h>
#include <aknsettingitemlist.h> 
#include <akncheckboxsettingpage.h> 

#include "perfmon_model.h"


//  FORWARD DECLARATIONS
class CAknSettingItemArray;
class CAknSettingStyleListBox;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknTabGroup; 
class TPerfMonSettings;
class CSelectionItemList;


//  CLASS DEFINITIONS

class CPerfMonSettingsViewDlg : public CAknDialog, public MEikListBoxObserver, public MAknTabObserver
    {
public:
    static CPerfMonSettingsViewDlg* NewL(TPerfMonSettings& aSettings);
    virtual ~CPerfMonSettingsViewDlg();

public: // From MEikListBoxObserver
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

public: // From MAknTabObserver
    void TabChangedL(TInt aIndex); 

public: // From CAknDialog
    void ProcessCommandL(TInt aCommandId);

protected: // From CEikDialog
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    void PreLayoutDynInitL();
    TBool OkToExitL(TInt aButtonId);    

private: // New methods
    void ShowSettingPageL(TBool aCalledFromMenu);
    void SetVisibilitiesOfSettingItemsL();
    void UpdateListBoxL();
    void AddSettingItemL(TInt aId, TInt aTitleResource, TInt aSettingPageResource, TInt aAssociatedResource, TInt aOrdinal);

private: // Constructors
    CPerfMonSettingsViewDlg(TPerfMonSettings& aSettings);
    void ConstructL();        

private: // Data
    CAknSettingItemArray*               iSettingItemArray;
    CAknSettingStyleListBox*            iListBox;
    CAknNavigationControlContainer*     iNaviContainer;
    CAknNavigationDecorator*            iDecoratedTabGroup;
    CAknTabGroup*                       iTabGroup;
    TPerfMonSettings&               iSettings;
    };


class CSourceSelectionCheckBoxSettingItem : public CAknSettingItem
    {
public:
    CSourceSelectionCheckBoxSettingItem( TInt aIdentifier, TPerfMonSources& aMemoryInUse );
	virtual ~CSourceSelectionCheckBoxSettingItem();

protected:
	void CompleteConstructionL();
	void StoreL();
	void LoadL();
	const TDesC& SettingTextL();
	void EditItemL( TBool aCalledFromMenu );
    void HandleSettingPageEventL( CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType );

private:
    void AddNewItemToArrayL(const TDesC& aLabel); 

private:
    CSelectionItemList*     iItemArray;
    HBufC*                  iSettingText;
    TPerfMonSources&        iExternalSources;
    };


class CSourceSelectionCheckBoxSettingPage : public CAknCheckBoxSettingPage
	{
	public:
        CSourceSelectionCheckBoxSettingPage( TInt aResourceID, CSelectionItemList* aItemArray );
	public:		// New functions
		void UpdateCba();
	};
	

#endif

// End of File

