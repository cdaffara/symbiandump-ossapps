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
  \class InfoWidgetPreferences
  \brief Preference store for widget  
         display etc. options   
*/


/*!
    InfoWidgetPreferences::InfoWidgetPreferences() 
*/
InfoWidgetPreferences::InfoWidgetPreferences(QObject *parent): 
    QObject(parent)
{
    DPRINT;
}

/*!
    InfoWidgetPreferences::~InfoWidgetPreferences() 
*/
InfoWidgetPreferences::~InfoWidgetPreferences()
{
    DPRINT;
}

/*!
    InfoWidgetPreferences::storePreferences()
    
    Store acceptable preference set  
*/
bool InfoWidgetPreferences::storePreferences()
{
    DPRINT;
    bool changed(false); 
    
    if (validate() && 
        m_validatedOptions != m_options){
            m_validatedOptions = m_options;
            changed = true;
            emit preferencesChanged(m_validatedOptions);
        }
    else if (visibleItemCount() <= 0) { 
        DPRINT << ": invalid options, restoring initial options";
        restorePreferences();
    }

    return changed; 
}

/*!
    InfoWidgetPreferences::restorePreferences()
    
    Restores last acceptable preference set 
*/
void InfoWidgetPreferences::restorePreferences()
{
    DPRINT;
    m_options = m_validatedOptions; 
}

/*!
    InfoWidgetPreferences::preference() 
*/
QString InfoWidgetPreferences::preference(Option preferenceId) const
{
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
    InfoWidgetPreferences::isPreferenceSet()
*/
bool InfoWidgetPreferences::isPreferenceSet(Option preferenceId) const
{
    DPRINT << ": preference id: " << static_cast<int>(preferenceId); 
    return m_options.testFlag(preferenceId); 
}

/*!
    InfoWidgetPreferences::preferences()
*/
InfoWidgetPreferences::Options InfoWidgetPreferences::preferences() const
{
    return m_options; 
}

/*!
    InfoWidgetPreferences::setPreference() 
*/
void InfoWidgetPreferences::setPreference(Option preferenceId, 
    const QString& preferenceString)
{
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

/*!
    InfoWidgetPreferences::visibleItemCount() 
*/
int InfoWidgetPreferences::visibleItemCount() 
{
    DPRINT << ": IN";
    
    int visibleItems = 0;
    if (m_options.testFlag(DisplayHomeZone)){
        visibleItems++; 
    }
    if (m_options.testFlag(DisplayMcn)){
        visibleItems++; 
    }
    if (m_options.testFlag(DisplayActiveLine)){
        visibleItems++; 
    }
    if (m_options.testFlag(DisplaySatText)){
        visibleItems++; 
    }
    if (m_options.testFlag(DisplaySpn)){
        visibleItems++; 
    }
    
    DPRINT << ": visible item count: " << visibleItems;
    return visibleItems; 
}

/*!
    InfoWidgetPreferences::validate() 
*/
bool InfoWidgetPreferences::validate() 
{
    return visibleItemCount() > 0; 
}

/*!
   InfoWidgetPreferences::preferenceNames()
   
   Convenience function for getting all preference names
*/
QStringList InfoWidgetPreferences::preferenceNames()
{
    QStringList preferenceList; 
    preferenceList << "spnDisplay" << "homeZoneDisplay" << 
            "activeLineDisplay" << "satDisplay" << "mcnDisplay";
    return preferenceList; 
}


// End of File. 

