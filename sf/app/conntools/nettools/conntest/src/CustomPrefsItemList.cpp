/*
 * Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: CCustomPrefsItemList implemets the Extended Connection Preferences setting
 * list for Conntest.
 *
 */

// INCLUDE FILES
#include <cmapplicationsettingsui.h>
#include <extendedconnpref.h>

#include "CustomPrefsItemList.h"
#include "CustomPrefsData.h"
#include "conntest.hrh"
#include "Utils.h"
#include "cmapplicationsettingsui.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CCustomPrefsItemList::CreateSettingItemL
// Inherited from CAknSettingItem. Updates descriptor array for list box.
// ----------------------------------------------------------------------------
//
CAknSettingItem* CCustomPrefsItemList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknSettingItem* customPrefsItem = NULL;
    
    switch (aIdentifier)
        {
        case EConnTestExtConnPrefsItemSnapPurpose:
            customPrefsItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                    aIdentifier, iData->iSnapPurpose );
            break;
        case EConnTestExtConnPrefsItemSnapId:
            customPrefsItem = new (ELeave) CAknIntegerEdwinSettingItem(
                    aIdentifier, iData->iSnapId );
            break;
        case EConnTestExtConnPrefsItemIapId:
            customPrefsItem = new (ELeave) CAknIntegerEdwinSettingItem(
                    aIdentifier, iData->iIapId );
            break;
        case EConnTestExtConnPrefsItemBearerSet:
            customPrefsItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                    aIdentifier, iData->iBearerSet );
            break;
        case EConnTestExtConnPrefsItemNoteBehaviour:
            customPrefsItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                    aIdentifier, iData->iNoteBehaviour );
            break;
        case EConnTestExtConnPrefsItemDisconnectDialog:
            customPrefsItem = new (ELeave) CAknBinaryPopupSettingItem(
                    aIdentifier, iData->iDisconnectDialog );
            break;
        case EConnTestExtConnPrefsItemConnSelectionDialog:
            customPrefsItem = new (ELeave) CAknBinaryPopupSettingItem(
                    aIdentifier, iData->iConnSelectionDialog );
            break;
        case EConnTestExtConnPrefsItemForcedRoaming:
            customPrefsItem = new (ELeave) CAknBinaryPopupSettingItem(
                    aIdentifier, iData->iForcedRoaming );
            break;
        default:
            break;
        }
    return customPrefsItem;
    }

// ----------------------------------------------------------------------------
// CCustomPrefsItemList::SetData
// 
// ----------------------------------------------------------------------------
//
void CCustomPrefsItemList::SetData( CCustomPrefsData* aData )
    {
    iData = aData;
    }

// ----------------------------------------------------------------------------
// CCustomPrefsItemList::SizeChanged
// Size changed handler
// ----------------------------------------------------------------------------
//
void CCustomPrefsItemList::SizeChanged()
    {
    if (ListBox())
        {
        ListBox()->SetRect( Rect() );
        }
    }

// ---------------------------------------------------------
// CCustomPrefsItemList::SelectSnapL()
// Selects a SNAP
// ---------------------------------------------------------
//
void CCustomPrefsItemList::SelectSnapL()
    {
    // Start the SNAP selection dialog
    CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL( settings );
    TCmSettingSelection selection;
    TUint listItems = CMManager::EShowDestinations;
    TBearerFilterArray filterArray;
    
    settings->RunApplicationSettingsL( selection, listItems, filterArray );

    CleanupStack::PopAndDestroy( settings );
    
    switch (selection.iResult)
        {
        case CMManager::EDestination:
            {
            iData->iSnapId = selection.iId;
            break;
            }
        default:
            {
            iData->iSnapId = 0;
            RDebug::Print( _L("ConnTest: Invalid selection") );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CCustomPrefsItemList::SelectIapL()
// Selects an IAP
// ---------------------------------------------------------
//
void CCustomPrefsItemList::SelectIapL()
    {
    // IAP selection menu
    TUint32 iapId;
    TInt popupOk = Utils::AskIapIdL( iapId );
    
    if (popupOk)
        {
        RDebug::Print( _L("ConnTest: Selected IAP id=%d"), iapId );
        iData->iIapId = iapId;
        }
    else
        {
        RDebug::Print( _L("ConnTest: Invalid selection") );
        iData->iIapId = 0;
        }
    }

// ---------------------------------------------------------
// CCustomPrefsItemList::EditItemL()
// Edits a menu item
// ---------------------------------------------------------
//
void CCustomPrefsItemList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    if (aIndex == EConnTestExtConnPrefsItemSnapId)
        {
        // SNAP selection menu
        SelectSnapL();
        // Update the selected value into the CustomPrefsItemList
        CCustomPrefsSelectedIntegerItem* snapItem =
                (CCustomPrefsSelectedIntegerItem*)(*SettingItemArray())[aIndex];
        snapItem->EditSelectedIntegerItemL( iData->iSnapId );
        }
    else if (aIndex == EConnTestExtConnPrefsItemIapId)
        {
        // IAP selection menu
        SelectIapL();
        // Update the selected value into the CustomPrefsItemList
        CCustomPrefsSelectedIntegerItem* iapItem =
                (CCustomPrefsSelectedIntegerItem*)(*SettingItemArray())[aIndex];
        iapItem->EditSelectedIntegerItemL( iData->iIapId );
        }
    else
        {
        // The rest are handled "traditionally", via the base class
        CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
        }
    }

// ---------------------------------------------------------
// CCustomPrefsSelectedIntegerItem::EditSelectedIntegerItemL()
// Edits a SNAP or IAP item
// ---------------------------------------------------------
//
void CCustomPrefsItemList::CCustomPrefsSelectedIntegerItem::EditSelectedIntegerItemL( TInt aId )
    {
    this->SetExternalValue( aId );
    this->LoadL();
    this->UpdateListBoxTextL();
    }

// End of File
