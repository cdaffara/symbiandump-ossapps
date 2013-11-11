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
#ifndef UNITTEST_CXESETTINGSCENREPSTORE_H
#define UNITTEST_CXESETTINGSCENREPSTORE_H

#include <QObject>

// forward declartion
class CxeSettingsCenRepStore;

// class defining unit test cases for CxeSettingsCenRepStore engine class

class UnitTestCxeSettingsCenrepStore : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeSettingsCenrepStore();
    ~UnitTestCxeSettingsCenrepStore();

private slots:
    void init();
    void cleanup();
    void testSettingValues();
    void testloadSettings();
    void testReset();

private:
    // Helper methods for test cases
    
private:
    // data
    CxeSettingsCenRepStore *mSettingsCenrepStore;

};

#endif // UNITTEST_CXESETTINGSCENREPSTORE_H
