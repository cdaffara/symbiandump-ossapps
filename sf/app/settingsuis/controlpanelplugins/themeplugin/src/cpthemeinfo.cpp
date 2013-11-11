/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

/*!
 * A simple class to represent theme information. This information includes:
 *  1. Theme Name
 *  2. Theme Preview Thumbnail
 *  3. Theme Preview landscape and portrait icon
 *  4. Theme list type: either an item representing a theme, or a link (e.g OviStore).
 *  5. Item type data. Represents additional information (e.g for link type, the URL).
 */
#include <hbicon.h>

#include "cpthemeinfo.h"

 CpThemeInfo::CpThemeInfo()
{
}

 
 CpThemeInfo::~CpThemeInfo()
{
}
      
QString CpThemeInfo::name() const
{
    return mName;
}

void CpThemeInfo::setName(const QString& newName)
{
    mName = newName;
}
      
CpThemeInfo::ThemeListItemType CpThemeInfo::itemType() const
{
    return mItemType;
}

void CpThemeInfo::setItemType(CpThemeInfo::ThemeListItemType type)
{
    mItemType = type;
}

      
QString CpThemeInfo::itemData() const
{
    return mItemData;
}

void CpThemeInfo::setItemData(const QString& data)
{
    mItemData = data;
}

      
HbIcon CpThemeInfo::icon() const
{
    return mIcon;
}
  
void CpThemeInfo::setIcon(const HbIcon& newIcon)
{
    mIcon = newIcon;
}

      
HbIcon CpThemeInfo::portraitPreviewIcon() const
{
    return mPortraitPreviewIcon;
}

void CpThemeInfo::setPortraitPreviewIcon(const HbIcon& newIcon)
{
    mPortraitPreviewIcon = newIcon;
}

      
HbIcon CpThemeInfo::landscapePreviewIcon() const
{
    return mLandscapePreviewIcon;
}

void CpThemeInfo::setLandscapePreviewIcon(const HbIcon& newIcon)
{
    mLandscapePreviewIcon = newIcon;
}

