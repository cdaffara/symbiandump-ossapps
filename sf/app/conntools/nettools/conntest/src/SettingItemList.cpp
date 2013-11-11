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
* Description: CSettingItemList implemets the settings list for Conntest
*
*/

// INCLUDE FILES
#include "SettingItemList.h"
#include "SettingData.h"
#include "conntest.hrh"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CSettingItemList::CreateSettingItemL
// Updates descripter array for list box
// ----------------------------------------------------------------------------
//
CAknSettingItem* CSettingItemList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknSettingItem* settingItem = NULL;

    switch (aIdentifier)
        {
        case EConnTestIpAddress:
            settingItem = new (ELeave) 
            CAknTextSettingItem(aIdentifier, iData->iServerName );
            break;
        case EConnTestPort:
            settingItem = new (ELeave) 
            CAknIntegerEdwinSettingItem(aIdentifier, iData->iPort );
            break;
        case EConnTestHttpPage:
            settingItem = new (ELeave) 
            CAknTextSettingItem(aIdentifier, iData->iHttpPage);
            break;
        case EConnTestProtocol:
            settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                    aIdentifier, iData->iProtocol );
            break;
        case EConnTestPacketSize:
            settingItem = new (ELeave) 
            CAknIntegerEdwinSettingItem(aIdentifier, iData->iPacketSize );
            break;
        case EConnTestPackets:
            settingItem = new (ELeave) 
            CAknIntegerEdwinSettingItem(aIdentifier, iData->iPackets );
            break;
        case EConnTestDelay:
            settingItem = new (ELeave) 
            CAknIntegerEdwinSettingItem(aIdentifier, iData->iDelay );
            break;
        case EConnTestRoaming:
            settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                    aIdentifier, iData->iRoaming );
            break;
        default:
            break;
        }
    return settingItem;
    }

// ----------------------------------------------------------------------------
// CSettingItemList::SetData
// 
// ----------------------------------------------------------------------------
//
void CSettingItemList::SetData(CSettingData* aData)
    {
    iData = aData;
    }

// ----------------------------------------------------------------------------
// CSettingItemList::SizeChanged
// Size changed handler
// ----------------------------------------------------------------------------
//
void CSettingItemList::SizeChanged()
    {
    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }

// End of File
