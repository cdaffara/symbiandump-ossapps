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
* Description:  unit tests for the CpThemeListModel class from themeplugin
*
*/

#include <QtTest/QtTest>

#include "cpthemelistmodel.h"

class TestCpThemeListModel : public QObject
{
    Q_OBJECT

private slots:
    
    void testConstructor();
    void testRowCount();
    void testData();
    void testIndexOf();
};    


void TestCpThemeListModel::testConstructor()
{
    
    CpThemeListModel *obj = new CpThemeListModel();
    
    
    QVERIFY (obj != 0 );
    QVERIFY (obj->rowCount() > 0);
    
    delete obj;

}

void TestCpThemeListModel::testRowCount()
{
    CpThemeListModel *themeModel = new CpThemeListModel();
    QList<QPair<QString, QString> > allThemes = CpThemeUtil::availableThemes();
    QVERIFY(themeModel->rowCount() >= allThemes.size());
}

void TestCpThemeListModel::testData()
{
    CpThemeListModel *themeModel = new CpThemeListModel();
    QList<QPair<QString, QString> > allThemes = CpThemeUtil::availableThemes();
    
    QModelIndex index = themeModel->index(0,0, QModelIndex());
    
    //Check theme name (first) and theme path (second).  Skipping icons for now.
    QCOMPARE(themeModel->data(index, Qt::DisplayRole).toString(), allThemes.at(0).first);
    QCOMPARE(themeModel->data(index, CpThemeListModel::ItemDataRole).toString(), allThemes.at(0).second);
        
        
}

void TestCpThemeListModel::testIndexOf()
{
    CpThemeListModel *themeModel = new CpThemeListModel();
    QList<QPair<QString, QString> > allThemes = CpThemeUtil::availableThemes();
    
    CpThemeInfo *themeInfo = CpThemeUtil::buildThemeInfo(allThemes.at(1).second, allThemes.at(1).first);
       
    int index = themeModel->indexOf(*themeInfo);
       
     QVERIFY(index == 1);
    
}
QTEST_MAIN(TestCpThemeListModel)
#include "unittest_cpthemelistmodel.moc"

