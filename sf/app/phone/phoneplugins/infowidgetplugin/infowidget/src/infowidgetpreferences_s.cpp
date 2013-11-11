/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "infowidgetpreferences.h"
#include "infowidgetlogging.h"

/*!
    InfoWidgetPreferences::InfoWidgetPreferences() 
*/
InfoWidgetPreferences::InfoWidgetPreferences(IHsWidgetPreferenceService &preferenceService, 
    QObject* parent)
    : QObject(parent)
{
    DPRINT << "DUMMY IMPLEMENTATION";

    m_options = InfoWidgetPreferences::DisplayActiveLine|
            InfoWidgetPreferences::DisplayMcn|
            InfoWidgetPreferences::DisplaySatText|
            InfoWidgetPreferences::DisplayHomeZone;
}

/*!
    InfoWidgetPreferences::~InfoWidgetPreferences() 
*/
InfoWidgetPreferences::~InfoWidgetPreferences()
{
    DPRINT << "DUMMY IMPLEMENTATION";
}

/*!
    InfoWidgetPreferences::loadPreferences() 
*/
void InfoWidgetPreferences::loadPreferences()
{
    DPRINT << "DUMMY IMPLEMENTATION";
}

/*!
    InfoWidgetPreferences::storePreferences() 
*/
void InfoWidgetPreferences::storePreferences()
{
    DPRINT << "DUMMY IMPLEMENTATION";
}

/*!
    InfoWidgetPreferences::preference() 
*/
QString InfoWidgetPreferences::preference(Option preferenceId)
{
    DPRINT << "DUMMY IMPLEMENTATION";
    DPRINT << ": preference id: " << static_cast<int>(preferenceId); 
    
    QString preferenceString;
    if (m_options.testFlag(preferenceId)) {
        preferenceString = DISPLAY_SETTING_ON; 
    } else {
        preferenceString = DISPLAY_SETTING_OFF;  
    } 
    
    return preferenceString; 
}

/*!
    InfoWidgetPreferences::setPreference() 
*/
void InfoWidgetPreferences::setPreference(Option preferenceId, 
        QString preferenceString)
{
    DPRINT << "DUMMY IMPLEMENTATION";
    DPRINT << ": preference id: " << static_cast<int>(preferenceId);
    DPRINT << ": preference string: " << preferenceString; 

    DPRINT << ": initial options: " << m_options;
    
    if (preferenceString.compare(DISPLAY_SETTING_ON) == 0) {
        m_options |= preferenceId; 
    } else {
        m_options &= ~preferenceId; 
    }
    
    DPRINT << ": modified options: " << m_options;
}

// End of File.

