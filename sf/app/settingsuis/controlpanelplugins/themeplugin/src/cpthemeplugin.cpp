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
#include "cpthemeplugin.h"
#include "cpthemepluginentryitemdata.h"

/*!
    \class CpThemePlugin
    \brief CpThemePlugin is a control panel plugin that allows a user to view available themes and
           change the current theme of the phone.

*/

/*!
    constructor.
*/
CpThemePlugin::CpThemePlugin()
{
}

/*!
    destructor.
*/
CpThemePlugin::~CpThemePlugin()
{
}

/*!
    create the control panel entry item data for Theme settins.
*/

QList<CpSettingFormItemData*> CpThemePlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{

    //create a cpthemepluginentryitemdata with default values and return it.
    CpSettingFormEntryItemData *entryItem = new CpThemePluginEntryItemData(
        itemDataHelper,
        hbTrId("txt_cp_dblist_theme"),
        QString(),
        HbIcon("qtg_large_personalization"));
        return QList<CpSettingFormItemData*>() << entryItem;
}

Q_EXPORT_PLUGIN2(cpthemeplugin, CpThemePlugin);

