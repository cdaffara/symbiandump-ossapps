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
* Description: Unit Test for mpsettingsview
*
*/

#ifndef UNITTEST_MPSETTINGSVIEW_H_
#define UNITTEST_MPSETTINGSVIEW_H_

#include <QtTest/QtTest>
#include <QObject>

//Forward Declaration
class MpSettingsView;

class TestMpSettingsView : public QObject
{
    Q_OBJECT

public:

    TestMpSettingsView();
    virtual ~TestMpSettingsView();
    

signals:
    void back ();    
    void albumArtChanged();
 
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:

    void testInitializeView();

    void testActivateView();
    void testDeactivateView();
    void testBack();

private:
    MpSettingsView *mTest;
    
};

#endif /* UNITTEST_MPSETTINGSVIEW_H_ */
