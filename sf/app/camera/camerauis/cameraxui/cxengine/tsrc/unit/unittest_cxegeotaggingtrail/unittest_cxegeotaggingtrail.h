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
#ifndef UNITTEST_CXEGEOTAGGINGTRAIL_H
#define UNITTEST_CXEGEOTAGGINGTRAIL_H

#include <QObject>

// forward declartion
class CxeFakeSettings;
class CxeGeoTaggingTrail;

// class defining unit test cases for CxeGeoTaggingTrail engine class

class UnitTestCxeGeoTaggingTrail : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeGeoTaggingTrail();
    ~UnitTestCxeGeoTaggingTrail();

private slots:
    void init();
    void cleanup();
    void testStart();
    void testStop();
    void testState();


private:
    // data
    CxeGeoTaggingTrail *mTrail;
    CxeFakeSettings *mSettings;
};

#endif // UNITTEST_CXEGEOTAGGINGTRAIL_H
