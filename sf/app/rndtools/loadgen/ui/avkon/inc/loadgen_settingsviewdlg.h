/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOADGEN_SETTINGSVIEWDLG_H
#define LOADGEN_SETTINGSVIEWDLG_H

//  INCLUDES
#include <AknDialog.h>
#include <eiklbo.h>
#include <AknTabObserver.h> 
#include <akntabgrp.h>

//  FORWARD DECLARATIONS
class CAknSettingItemArray;
class CAknSettingStyleListBox;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknTabGroup; 
class TLoadGenSettings;


//  CLASS DEFINITIONS

class CLoadGenSettingsViewDlg : public CAknDialog, public MEikListBoxObserver, public MAknTabObserver
    {
public:
    static CLoadGenSettingsViewDlg* NewL(TLoadGenSettings& aSettings);
    virtual ~CLoadGenSettingsViewDlg();

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
    CLoadGenSettingsViewDlg(TLoadGenSettings& aSettings);
    void ConstructL();        

private: // Data
    CAknSettingItemArray*               iSettingItemArray;
    CAknSettingStyleListBox*            iListBox;
    CAknNavigationControlContainer*     iNaviContainer;
    CAknNavigationDecorator*            iDecoratedTabGroup;
    CAknTabGroup*                       iTabGroup;
    TLoadGenSettings&               iSettings;
    };


#endif

// End of File

