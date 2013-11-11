/*
 * Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef CUSTOMPREFSITEMLIST_H
#define CUSTOMPREFSITEMLIST_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CCustomPrefsData;

// CLASS DECLARATION

/**
 * Control for setting the (Extended) Custom Preferences.
 */
class CCustomPrefsItemList : public CAknSettingItemList
    {
public:
    CAknSettingItem* CreateSettingItemL( TInt identifier );
    void SetData( CCustomPrefsData* aData );

    void EditItemL( TInt aIndex, TBool aCalledFromMenu );

private:
    /**
     * Size change handler
     */
    void SizeChanged();

    /**
     * Selects a SNAP
     */
    void SelectSnapL();

    /**
     * Selects a IAP
     */
    void SelectIapL();

private:
    CCustomPrefsData* iData;

    /**
     * Control for selecting the Custom Preferences list's SNAP or IAP ID.
     */
    class CCustomPrefsSelectedIntegerItem : public CAknIntegerSettingItem
        {
    public:
        void EditSelectedIntegerItemL( TInt aId );
        };
    };

#endif

// End of File
