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
#include <QDate>
#include <QTest>
#include <QDebug>

#include "unittest_cxefilenamegeneratorsymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxesettings.h"
#include "cxefakesettings.h"

namespace
{
    static const char* FILENAME_SUFFIX = "CxTest";
}

UnitTestCxeFilenameGeneratorSymbian::UnitTestCxeFilenameGeneratorSymbian()
    : mFileNameGenerator(NULL), mFakeSettings(NULL), mMode(Cxe::ImageMode)
{
}

UnitTestCxeFilenameGeneratorSymbian::~UnitTestCxeFilenameGeneratorSymbian()
{
    delete mFileNameGenerator;
    delete mFakeSettings;
}

// Run before each individual test case
void UnitTestCxeFilenameGeneratorSymbian::init()
{
    mFakeSettings = new CxeFakeSettings();

    mFakeSettings->set(CxeSettingIds::FNAME_MONTH_FOLDER, QDate::currentDate().toString("yyyyMM"));
    mFakeSettings->set(CxeSettingIds::FNAME_IMAGE_COUNTER, 0);
    mFakeSettings->set(CxeSettingIds::FNAME_VIDEO_COUNTER, 0);
    mFakeSettings->set(CxeSettingIds::FNAME_FOLDER_SUFFIX, FILENAME_SUFFIX);

    mFileNameGenerator = new CxeFilenameGeneratorSymbian(*mFakeSettings, mMode);
    mFileNameGenerator->init(mMode);
    }

// Run after each individual test case
void UnitTestCxeFilenameGeneratorSymbian::cleanup()
{
    delete mFileNameGenerator;
    mFileNameGenerator = NULL;
    delete mFakeSettings;
    mFakeSettings = NULL;
}

void UnitTestCxeFilenameGeneratorSymbian::testStartNewImageFilenameSequence()
{
    mFileNameGenerator->startNewImageFilenameSequence();
}

void UnitTestCxeFilenameGeneratorSymbian::testNextImageFilenameInSequence()
{
    mFileNameGenerator->startNewImageFilenameSequence();
    QString filename;
    QString ext(".jpg");
    CxeError::Id error = mFileNameGenerator->nextImageFilenameInSequence(filename, ext);
    filename = removeDrive(filename);
    QString reference(generateSequenceImageFileName(0, 0));

    qDebug() << "UnitTestCxeFilenameGeneratorSymbian - Check generated sequence filename (root path removed)..";
    qDebug() << "UnitTestCxeFilenameGeneratorSymbian - Generated: [" << filename << "]";
    qDebug() << "UnitTestCxeFilenameGeneratorSymbian - Reference: [" << reference << "]";
    QVERIFY(QString::compare(removeDrive(filename), reference, Qt::CaseInsensitive) == 0);
}

void UnitTestCxeFilenameGeneratorSymbian::testGenerateFilename()
{
    QString filename;
    QString ext(".jpg");
    CxeError::Id error = mFileNameGenerator->generateFilename(filename, ext);
    filename = removeDrive(filename);
    QString reference(generateImageFileName(0));
    qDebug() << "UnitTestCxeFilenameGeneratorSymbian - Check generated filename (root path removed)..";
    qDebug() << "UnitTestCxeFilenameGeneratorSymbian - Generated: [" << filename << "]";
    qDebug() << "UnitTestCxeFilenameGeneratorSymbian - Reference: [" << reference << "]";
    QVERIFY(QString::compare(filename, generateImageFileName(0), Qt::CaseInsensitive) == 0);
}

QString UnitTestCxeFilenameGeneratorSymbian::generateImageFileName(int counter)
{
    // get the file name suffix
    //QString fileNameSuffix;
    //qobject_cast<CxeSettings*>(mFakeSettings)->get(CxeSettingIds::FNAME_FOLDER_SUFFIX, fileNameSuffix);
    //QString fileNameSuffix("");
    //QString cleanedFileNameSuffix = fnFormat.arg(fileNameSuffix);

    QString monthName = QDate::currentDate().toString("yyyyMM");
    QString filename;
    //filename.sprintf("\\Images\\Camera\\%s\\%sA0\\%04d%s.jpg", monthName.toAscii().data(), monthName.toAscii().data(), counter, fileNameSuffix);
    filename.sprintf("\\Images\\Camera\\%s\\%sA0\\%04d.jpg", monthName.toAscii().data(), monthName.toAscii().data(), counter);
    return filename;
}

QString UnitTestCxeFilenameGeneratorSymbian::generateSequenceImageFileName(int counter, int burstIndex)
{
    Q_UNUSED(burstIndex);

    // For now, burst image names are created like normal unique names
    QString filename = generateImageFileName(counter);
    // get the file name suffix
    /*QString fileNameSuffix;
    qobject_cast<CxeSettings*>(mFakeSettings)->get(CxeSettingIds::FNAME_FOLDER_SUFFIX, fileNameSuffix);

    QString monthName = QDate::currentDate().toString("yyyyMM");
    QString filename;
    filename.sprintf("\\Images\\Camera\\%s\\%sA0\\%04d%s(%02d).jpg", monthName.toAscii().data(), monthName.toAscii().data(), counter, fileNameSuffix, burstIndex);
    */
    return filename;
}

QString UnitTestCxeFilenameGeneratorSymbian::removeDrive(const QString &filename)
{
    // File name generator may return paths that point to C, E, or F drives.
    // In order to simplify path verification, all drive letters are removed.

    QString res = filename;
    res.remove(QRegExp("^c:\\\\data", Qt::CaseInsensitive));
    res.remove(QRegExp("^e:", Qt::CaseInsensitive));
    res.remove(QRegExp("^f:", Qt::CaseInsensitive));
    return res;
}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeFilenameGeneratorSymbian);
