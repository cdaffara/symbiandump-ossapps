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

#ifndef INFOWIDGETPREFERENCES_H
#define INFOWIDGETPREFERENCES_H

#include <QObject>
#include <QStringList>
#include <QFlags>

// Preference string values
const char DISPLAY_SETTING_ON[]    = "On";
const char DISPLAY_SETTING_OFF[]     = "Off";

// Class declaration
class InfoWidgetPreferences : public QObject
{
    Q_OBJECT

public: 
    enum DisplaySetting {
        DisplayOff = 0, 
        DisplayOn    
    }; 
    
    enum Option {
        DisplayHomeZone = 0x1,
        DisplayMcn = 0x2,
        DisplayActiveLine = 0x4, 
        DisplaySatText = 0x8,
        DisplaySpn = 0x10
    };
    Q_DECLARE_FLAGS(Options, Option)

public:
    InfoWidgetPreferences(QObject *parent = NULL);
    ~InfoWidgetPreferences();

    bool storePreferences();
    void restorePreferences();
    QStringList preferenceNames();
            
    bool isPreferenceSet(Option preferenceId) const;
    QString preference(Option preferenceId) const;
    InfoWidgetPreferences::Options preferences() const;
    void setPreference(Option preferenceId, const QString &preferenceString);
     
    bool validate(); 
    int visibleItemCount();

signals:
    void preferencesChanged(InfoWidgetPreferences::Options options);  
		
private:
    Q_DISABLE_COPY(InfoWidgetPreferences)

    InfoWidgetPreferences::Options m_options; 
    InfoWidgetPreferences::Options m_validatedOptions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InfoWidgetPreferences::Options)

#endif // INFOWIDGETPREFERENCES_H

