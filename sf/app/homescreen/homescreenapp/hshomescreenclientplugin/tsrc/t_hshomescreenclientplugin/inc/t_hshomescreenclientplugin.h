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
* Description:  Test class for hshomescreenclientplugin
*
*/


#include <QtTest/QtTest>

#include "hshomescreenclientplugin.h"


/**
* @test Test class for hshomescreenclientplugin.
*/
class HsHomescreenClientPluginTest : public QObject
{
	Q_OBJECT

private slots:

    void cleanupTestCase();
    void initTestCase();
    
	/**
	 * HsHomescreenClient
	 */
     void testConstruction();
     void testAddWidget();
     void testAddInvalidWidget();
     void testSetWallpaper();
     void testSetWallpaper2();
     void testOnRequestError();

signals:
     void onRequestCompletedTest(const QVariant &result);
         
private:
     
};

