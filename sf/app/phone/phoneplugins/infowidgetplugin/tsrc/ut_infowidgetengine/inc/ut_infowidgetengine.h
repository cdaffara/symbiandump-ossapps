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

#ifndef UT_INFOWIDGETENGINE_H
#define UT_INFOWIDGETENGINE_H

#include <QtTest/QtTest>
#include <mockservice.h>

class InfoWidgetEngine;

class UT_InfoWidgetEngine : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetEngine();
    ~UT_InfoWidgetEngine();
    
private slots:

    void init();
    void cleanup();

    void t_modelData();
    void t_updateNetworkDataToModel();
    void t_updateSatDataToModel();
    void t_updateLineDataToModel();
    void t_handleNetworkError();
    void t_handleSatError(); 
    void t_handleLineError();
    
    void t_handlePreferencesChanged();
    void t_suspend();
    void t_resume();
    
private:
    
    InfoWidgetEngine *m_infoWidgetEngine;

};

#endif // UT_INFOWIDGETENGINE_H
