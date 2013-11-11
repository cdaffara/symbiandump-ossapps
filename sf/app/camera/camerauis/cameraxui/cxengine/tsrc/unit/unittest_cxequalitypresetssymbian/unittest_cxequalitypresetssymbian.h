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


#ifndef UNITTEST_CXEQUALITYPRESETSSYMBIAN_H
#define UNITTEST_CXEQUALITYPRESETSSYMBIAN_H

#include <QObject>

// forward declaration
class CxeQualityPresetsSymbian;
class CxeFakeSettings;

class UnitTestCxeQualityPresetsSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeQualityPresetsSymbian();
    ~UnitTestCxeQualityPresetsSymbian();

private slots:
    void init();
    void cleanup();

    void testImageQualityPresets();
    void testVideoQualityPresets();
    void testRecordingTimeAvailable();

private:
    //data members
    CxeQualityPresetsSymbian* mQualityPresets;
    CxeFakeSettings* mSettings;
};

#endif //UNITTEST_CXEQUALITYPRESETSSYMBIAN_H
