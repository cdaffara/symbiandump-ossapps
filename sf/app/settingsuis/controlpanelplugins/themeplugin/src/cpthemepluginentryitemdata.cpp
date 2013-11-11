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

#include <hbdataform.h>
#include <hbiconitem.h>

#include <cpbasesettingview.h>
#include "cpthemecontrol.h"
#include "cpthemepluginentryitemdata.h"
#include <cpitemdatahelper.h>

/*!
    \class CpThemePluginEntryItemData
    \brief CpThemePluginEntryItemData is the entry item view for Theme Changer plugin.  It displays a title "Theme",
    the name of current theme and the preview icon of the current theme.

*/

/*!
    constructor.
*/
CpThemePluginEntryItemData::CpThemePluginEntryItemData(CpItemDataHelper &itemDataHelper,
                                            const QString &text,
                                            const QString &description,
                                            const HbIcon &icon,
                                            const HbDataFormModelItem *parent) :
                                        CpSettingFormEntryItemData(itemDataHelper,
                                                                   text,
                                                                   description,
                                                                   icon,
                                                                   parent)
{
    //Create a CpThemeControl. the themeControl takes care of displaying the themes
    //and letting user select a theme and apply a theme change.
    mThemeControl = new CpThemeControl();

    setDescription(mThemeControl->currentThemeName());

    //connect to signal to update the current theme name and icon whenever the theme changes.
    QObject::connect(mThemeControl, SIGNAL(themeUpdated(const QString&, const HbIcon&)), this, 
                            SLOT(themeUpdated(const QString&, const HbIcon&)));
}

/*!
    destructor.
*/
CpThemePluginEntryItemData::~CpThemePluginEntryItemData()
{
    delete mThemeControl;
    mThemeControl = 0;
}

/*!
    Slot used for updating entry item's description and icon with current theme name and its 
    preview icon.
 */
void CpThemePluginEntryItemData::themeUpdated(const QString& themeName, const HbIcon& icon)
{
	Q_UNUSED(icon);
    setDescription(themeName);
}

/*!
    Return the theme changer UI view.  Returns 0 if it can't create
    the view.
*/
CpBaseSettingView *CpThemePluginEntryItemData::createSettingView() const
{
    if(mThemeControl) {
        CpBaseSettingView *view = mThemeControl->themeListView();
        return view;
    }
    
    return 0;
}
