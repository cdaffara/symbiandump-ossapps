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

#ifndef PLUGINATTRIBUTES_H_
#define PLUGINATTRIBUTES_H_

/*
 * 
 * PluginAttributes class definition, internal settings format
 *  
 */
#include <qobject.h>
#include <qvariant.h>
#include <qstring.h>

class HbDataFormModelItem;

/*
 * 
 * TSettingItem class definition, internal settings format
 *  
 */
class SettingItem
{
public:
    enum
    {
        SettingItemTypeInt = 0, SettingItemTypeBool, SettingItemTypeHex, SettingItemTypeText
    };

public:
    QString mSettingText;
    int mType;
    QString mValue;
    QString mSettingDescription;
    QString mUIText;
};

class PluginAttributes// : public QObject
{
    //Q_OBJECT
public:

    // default constructor
    PluginAttributes();

    ~PluginAttributes();
public:
    int mUid;
    QString mShortName; // name of the plugin, short name
    QString mName; // name of the plugin, long name
    QString mDescription; // plugin description, info about HW/SW dependencies etc.
    int mSampleRate; // sample rate of the plugin
    bool mEnabled; // enabled for profiling
    bool mIsHidden; // hidden, i.e. no start/stop controls
    int mItemCount; // plugin specific setting item count


    // plugin specific settings, plugin implementation dependent
    SettingItem mSettingItem1;
    SettingItem mSettingItem2;
    SettingItem mSettingItem3;
    SettingItem mSettingItem4;
    SettingItem mSettingItem5;
    SettingItem mSettingItem6;

    HbDataFormModelItem *mFormItemSettingItem1;
    HbDataFormModelItem *mFormItemSettingItem2;
    HbDataFormModelItem *mFormItemSettingItem3;
    HbDataFormModelItem *mFormItemSettingItem4;
    HbDataFormModelItem *mFormItemSettingItem5;
    HbDataFormModelItem *mFormItemSettingItem6;
    HbDataFormModelItem *mFormItemSampleRate;

};

#endif /* PLUGINATTRIBUTES_H_ */
