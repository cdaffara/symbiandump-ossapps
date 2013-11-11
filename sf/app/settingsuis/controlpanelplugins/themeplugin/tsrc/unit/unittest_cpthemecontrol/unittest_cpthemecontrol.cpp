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
* Description:  unit tests for the CpThemeControl class from themeplugin
*
*/

#include <QtTest/QtTest>
#include <QSignalSpy>

#include <hbinstance.h>
#include <hbtheme.h>

#include "cpthemechanger.h"
#include "cpthemecontrol.h"
#include "cpthemeinfo.h"
#include "cpthemeutil.h"

class TestCpThemeControl : public QObject
{
    Q_OBJECT

private slots:

    void testConstructor();
    void testThemeListView();
    void testCurrentThemeName();
    void testCurrentThemeIcon();
   
private:
    CpThemeChanger* mThemeChanger;
};



// verify that the constructor works and that the
// defaults are sane.
void TestCpThemeControl::testConstructor()
{
    CpThemeControl * control = new CpThemeControl();

    QVERIFY(control !=0 );
    QVERIFY(!control->currentThemeName().isEmpty());
    QVERIFY(!control->currentThemeIcon().iconName().isEmpty());

    delete control;
}

// verify that the themeListView doesn't return NULL (or crash)
void TestCpThemeControl::testThemeListView()
{
    CpThemeControl control;

    QVERIFY(control.themeListView() != 0);
}

// test that we get back a non-empty QString
void TestCpThemeControl::testCurrentThemeName()
{
    CpThemeControl control;

    QVERIFY( (control.currentThemeName() == hbInstance->theme()->name())  
         || (control.currentThemeName() == QString("hbdefault")));
}

// test that we get a non-empty string for current theme icon
void TestCpThemeControl::testCurrentThemeIcon()
{
    CpThemeControl control;

    QVERIFY(!control.currentThemeIcon().isNull() && !control.currentThemeIcon().iconName().isEmpty());
}

    



QTEST_MAIN(TestCpThemeControl)
#include "unittest_cpthemecontrol.moc"
