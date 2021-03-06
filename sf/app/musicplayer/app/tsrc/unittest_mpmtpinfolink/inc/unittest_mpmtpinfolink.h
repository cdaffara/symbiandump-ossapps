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
* Description: Unit test for MpMtpInfoLink
*
*/

#ifndef TESTMPMTPINFOLINK_H
#define TESTMPMTPINFOLINK_H

#include <QtTest/QtTest>

class QTranslator;
class MpMtpInfoLink;

class TestMpMtpInfoLink : public QObject
{
    Q_OBJECT

public:
    TestMpMtpInfoLink();
    virtual ~TestMpMtpInfoLink();

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructor();
    void testMousePressEvent();
    void testMouseReleaseEvent();
    void testSetMtpInfoText();

private:

    MpMtpInfoLink  *mTest;
    QTranslator    *mMpTranslator; // Own

};

#endif  // TESTMPMTPINFOLINK_H

