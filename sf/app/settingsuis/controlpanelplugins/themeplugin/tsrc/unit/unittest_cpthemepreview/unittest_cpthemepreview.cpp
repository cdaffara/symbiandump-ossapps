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
* Description:  unit tests for the CpThemePreview class from themeplugin
*
*/

#include <QtTest/QtTest>

#include "cpthemepreview.h"
#include "cpthemechanger.h"
#include "cpthemeutil.h"

class TestCpThemePreview : public QObject
{
    Q_OBJECT

private slots:
    
    void testAll();
    
};

void TestCpThemePreview::testAll()
{
    QList<QPair<QString, QString> > themes = CpThemeUtil::availableThemes();
    
    QPair<QString, QString> pair;
    
    pair = themes.at(0);
    
    QString name = pair.first;
    QString themePath = pair.second;
    CpThemeInfo* themeInfo = CpThemeUtil::buildThemeInfo(themePath, name);
    
    CpThemePreview *obj = new CpThemePreview(*themeInfo);
    
    
    QVERIFY (obj != 0 );
    QCOMPARE(obj->themeName(), name);
    QCOMPARE(obj->themeIcon(), themeInfo->icon());

    delete themeInfo;
    
    pair = themes.at(1);
    name = pair.first;
    themePath = pair.second;
    themeInfo = CpThemeUtil::buildThemeInfo(themePath, name);
    
    obj->setThemeInfo(*themeInfo);
    
    QCOMPARE(obj->themeName(), name);
    QCOMPARE(obj->themeIcon(), themeInfo->icon());
        

    delete obj;

}

QTEST_MAIN(TestCpThemePreview)
#include "unittest_cpthemepreview.moc"

