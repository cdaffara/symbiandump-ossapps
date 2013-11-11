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
* Description: Unit test for MusicServices
*
*/

#ifndef TESTMUSICSERVICES_H
#define TESTMUSICSERVICES_H

#include <QtTest/QtTest>

/* in our unit test usually this goes on the CPP file, but in this case we need it here to get
 the observer interface, and also the mpxcollectionopenutility stub before the tested class so 
 we get the stub instead of the normal mpxcollectionopenutility.*/
//#include "stub/inc/mpxcollectionopenutility.h"


class MusicServices;


class TestMusicServices : public QObject
{
    Q_OBJECT

public:

    TestMusicServices();
    virtual ~TestMusicServices();
    
public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructor();
    void testCurrentService();
    void testSetCurrentService();

private:

    MusicServices  *mTest;

};

#endif  // TESTmusicservices_H




