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
* Contributors: XENT-MV
*
* Description:  unit tests for the CpThemePluginEntryItemData class from themeplugin
*
*/

#include <QtTest/QtTest>
#include <QObject>

#include <cpitemdatahelper.h>

#include "cpthemepluginentryitemdata.h"

class TestCpThemePluginEntryItemData : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor();
};

void TestCpThemePluginEntryItemData::testConstructor()
{
    CpItemDataHelper helper;

    CpThemePluginEntryItemData *obj = new CpThemePluginEntryItemData(helper);
    
    QVERIFY(obj != NULL);

    delete obj;
}


QTEST_MAIN(TestCpThemePluginEntryItemData)
#include "unittest_cpthemepluginentryitemdata.moc"

