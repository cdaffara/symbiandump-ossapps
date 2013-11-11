/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef UNITTEST_CXEFEATUREMANAGER_H
#define UNITTEST_CXEFEATUREMANAGER_H

#include <QObject>

class CxeFeatureManagerImp;
class CxeFakeSettings;

class UnitTestCxeFeatureManager : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeFeatureManager();
    ~UnitTestCxeFeatureManager();

private slots:
    void init();
    void cleanup();
    
    void testIsFeatureSupported();
    void testconfiguredValues();
    
private:
    CxeFeatureManagerImp *mFeatureManager;
    CxeFakeSettings *mFakeSettings;
};

#endif // UNITTEST_CXEFEATUREMANAGER_H
