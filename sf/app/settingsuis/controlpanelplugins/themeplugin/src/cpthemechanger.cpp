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
#include <QString>

#include "cpthemechanger.h"
#include "cpthemeutil.h"
#include "cpthemeinfo.h"

#include <hbinstance.h>
#include <restricted/hbthemeservices_r.h>

/*!
  \class CpThemeChanger

  \brief CpThemeChanger provides an interface for changing the current
  theme.
  This API is only for use with the control panel and its theme
  changing plugin.
*/

/*!
  Constructor.
*/
CpThemeChanger::CpThemeChanger(QObject* p) :
    QObject(p),
    mCurrentTheme(0)
{
    if(hbInstance->theme()) {
        connect(hbInstance->theme(),SIGNAL(changeFinished()), this, SLOT(changeFinished()));
    }
       
    setCurrentTheme();
}

/*!
    Returns a ThemeInfo object containing the current theme name and
    corresponding icons.

    If no repersentative icons exist, the HbIcon returned will be
    uninitialized.
*/
const CpThemeInfo* CpThemeChanger::currentTheme() const
{
    return mCurrentTheme;
}

/*!
 * Private helper function that gets the current theme from hbinstance and s
 * ets class' current theme.  
 */
void CpThemeChanger::setCurrentTheme()
{
    QString themeName = "";
    if (HbInstance::instance()) {
        HbTheme *hbTheme = HbInstance::instance()->theme();
        if(hbTheme) {
            themeName = hbTheme->name();
        }
         
    }
       
    if(mCurrentTheme && mCurrentTheme->name() == themeName) {
        return;
    } else {
        //buildThemeInfo creates new theme info.
        CpThemeInfo* tmpTheme = CpThemeUtil::buildThemeInfo(HbThemeServices::themePath(), themeName);
         
        //delete old value. set the new value.
        if(tmpTheme) {
            if(mCurrentTheme){
                delete mCurrentTheme;
            }
            mCurrentTheme = tmpTheme;
        }
    }
}


/*!
 Change a theme. Returns true on success, false otherwise.
 */
bool CpThemeChanger::changeTheme(const CpThemeInfo& newTheme)
{
    bool result = false;
    // Skip doing this if the request is for the current theme
    if (newTheme.name().isEmpty() || (mCurrentTheme && newTheme.name() == mCurrentTheme->name())) {
        return result;
    }

    QString themePath = newTheme.itemData();
    
    if(!themePath.isEmpty()) {
        HbThemeServices::setTheme(themePath);
        result = true;
    }
    return result;
}

/*!
 Destructor
 */
CpThemeChanger::~CpThemeChanger()
{
    delete mCurrentTheme;
    mCurrentTheme = 0;
}

/*!
 *  Slot to handle notification from theme server that theme change
 *  is done.  Notify the owner.
 */
void CpThemeChanger::changeFinished()
{
    setCurrentTheme();
    emit themeChangeFinished();
}

// End of file

