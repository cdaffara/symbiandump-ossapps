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
#ifndef CPTHEMEINFO_H_
#define CPTHEMEINFO_H_


#include <QString>

#include <hbicon.h>

class CpThemeInfo {

public:
    
    
    enum ThemeListItemType {
                 ThemeListItemType_default = 0,
                 ThemeListItemType_URL,
                 ThemeListItemType_APP
             };
    
     CpThemeInfo();
     ~CpThemeInfo();
     
     QString name() const;
     void setName(const QString& newName);
     
     ThemeListItemType itemType() const;
     void setItemType(ThemeListItemType type);
     
     QString itemData() const;
     void setItemData(const QString& data);
     
     HbIcon icon() const;
     void setIcon(const HbIcon& newIcon);
     
     HbIcon portraitPreviewIcon() const;
     void setPortraitPreviewIcon(const HbIcon& newIcon);
     
     HbIcon landscapePreviewIcon() const;
     void setLandscapePreviewIcon(const HbIcon& newIcon);
     
     bool operator < (const CpThemeInfo &other) const   {            
         return mName.toCaseFolded().localeAwareCompare(other.mName.toCaseFolded()) < 0;
     }
     bool operator == (const CpThemeInfo &other) const {
         return mName.localeAwareCompare(other.mName) == 0;
     }
     
private:
     CpThemeInfo::ThemeListItemType mItemType;
     QString mItemData;
     QString mName;
     HbIcon mIcon;
     HbIcon mPortraitPreviewIcon;
     HbIcon mLandscapePreviewIcon;

};    
Q_DECLARE_METATYPE(CpThemeInfo::ThemeListItemType)

   
#endif /* CPTHEMEINFO_H_ */
