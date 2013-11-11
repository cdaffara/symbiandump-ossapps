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

#ifndef UT_INFOWIDGETPREFERENCES_H
#define UT_INFOWIDGETPREFERENCES_H

#include <QtTest/QtTest>
#include <mockservice.h>

class InfoWidgetPreferences;

class UT_InfoWidgetPreferences : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetPreferences();
    ~UT_InfoWidgetPreferences();
    
private slots:

    void init();
    void cleanup();

    void t_storePreferences();
    void t_preference();
    void t_setPreference();
    void t_visibleItemCount();
    void t_isPreferenceSet();
    void t_preferences();
    void t_preferenceNames();
    
private:
    
    InfoWidgetPreferences *m_infoWidgetPreferences;

};

#endif // UT_INFOWIDGETPREFERENCES_H
