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

#ifndef SETTINGITEMLIST_H
#define SETTINGITEMLIST_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CSettingData;

// CLASS DECLARATION

/**
* Control for settings.
*/
class CSettingItemList : public CAknSettingItemList 
{
public:
    CAknSettingItem* CreateSettingItemL( TInt identifier ); 
    void SetData(CSettingData* aData);
    
private:
    void SizeChanged();
    
private:
    CSettingData*   iData;
};


#endif

// End of File
