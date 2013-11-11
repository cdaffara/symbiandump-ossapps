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
* Description:  Main test class for contactwidgethsplugin 
*
*/

#ifndef T_CONTACTWIDGETHSPLUGIN_H
#define T_CONTACTWIDGETHSPLUGIN_H

#include <QtTest/QtTest>

class ContactWidgetHsPlugin;

/**
* TestContactWidgetPlugin
*/
class TestContactWidgetPlugin : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void testCreateInstance();
    void testCreateInvalidInstance();

private:
    ContactWidgetHsPlugin* mContactWidgetPlugin;  	
};

#endif //T_CONTACTWIDGETHSPLUGIN_H
