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
#ifndef UNITTEST_CXEFILENAMEGENERATORSYMBIAN_H
#define UNITTEST_CXEFILENAMEGENERATORSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeFilenameGeneratorSymbian;
class CxeFakeSettings;

class UnitTestCxeFilenameGeneratorSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeFilenameGeneratorSymbian();
    ~UnitTestCxeFilenameGeneratorSymbian();

private slots:
    void init();
    void cleanup();

    void testStartNewImageFilenameSequence();
    void testNextImageFilenameInSequence();
    void testGenerateFilename();

private:
    // Helper methods for test cases
    QString generateImageFileName(int counter);
    QString generateSequenceImageFileName(int counter, int burstIndex);
    QString removeDrive(const QString &filename);

private:
    CxeFilenameGeneratorSymbian *mFileNameGenerator; // This will be recreated for each test
    CxeFakeSettings *mFakeSettings;                  // This will be recreated for each test
    Cxe::CameraMode mMode;
};

#endif // UNITTEST_CXEFILENAMEGENERATORSYMBIAN_H
