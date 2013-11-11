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

#include <restricted/hbthemeservices_r.h>
#include <QList>
#include <QStringList>
#include <QPair>
#include "cpthemeutil.h"
#include "cpthemeinfo.h"

class TestCpThemeUtil : public QObject
{
    Q_OBJECT

private slots:
    
    void testMultiple();
    void testAvailableThemes(); 
    void testThemeDirectories();

};

void TestCpThemeUtil::testMultiple()
{
    QList<QPair<QString, QString> > themes = HbThemeServices::availableThemes();
    
    QPair<QString, QString> pair;
    
    pair = themes.at(0);
    
    QString name = pair.first;
    QString themePath = pair.second;
    
    QList<CpThemeInfo> themeList = CpThemeUtil::buildThemeList();
    
    QCOMPARE(name, themeList.at(0).name());
    QCOMPARE(themePath, themeList.at(0).itemData());
    
    CpThemeInfo *themeInfo = CpThemeUtil::buildThemeInfo(themePath, name);
    QCOMPARE(name, themeInfo->name());
    QCOMPARE(themePath, themeInfo->itemData());
    
    
    //null test, making sure no crash happens.
    QString defaultThemePath = CpThemeUtil::defaultTheme();
    
    delete themeInfo;
    themeInfo = 0;
    
}
void TestCpThemeUtil::testAvailableThemes()
{
    QList<QPair<QString, QString> > themes = HbThemeServices::availableThemes();
    QList<QPair<QString, QString> > utilThemes = CpThemeUtil::availableThemes();
    
    for(int i = 0; i < themes.size(); i++) {
        QPair<QString, QString>pair1;
        QPair<QString, QString>pair2;
        
        pair1 = themes.at(i);
        pair2 = utilThemes.at(i);
        QCOMPARE(pair1.first, pair2.first);
        QCOMPARE(pair1.second, pair2.second);
    }
}

void TestCpThemeUtil::testThemeDirectories()
{
    QList<CpThemeInfo> themeList = CpThemeUtil::buildThemeList();
    QStringList themeDirs = CpThemeUtil::themeDirectories(themeList);
    QVERIFY(themeList.size() > 0 && themeDirs.size() > 0);
}

QTEST_MAIN(TestCpThemeUtil)
#include "unittest_cpthemeutil.moc"

