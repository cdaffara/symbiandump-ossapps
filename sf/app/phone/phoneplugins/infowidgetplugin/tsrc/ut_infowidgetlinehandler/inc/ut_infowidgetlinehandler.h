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

#ifndef UT_INFOWIDGETLINEHANDLER_H
#define UT_INFOWIDGETLINEHANDLER_H

#include <QtTest/QtTest>
#include <mockservice.h>

class InfoWidgetLineHandler;

class UT_InfoWidgetLineHandler : public QObject, public MockService
{
    Q_OBJECT

public:    
    
    UT_InfoWidgetLineHandler();
    ~UT_InfoWidgetLineHandler();
    
private slots:

    void init();
    void cleanup();

    void t_activeLine(); 
    
private:
    
    InfoWidgetLineHandler *m_lineHandler;

};

#endif // UT_INFOWIDGETLINEHANDLER_H
