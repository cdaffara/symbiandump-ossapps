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
* Description: This class is test case for backupsettings and restoresettings
*
*/

#include <QtTest/QtTest>

#include "fmbackupsettings.h"
#include "fmrestoresettings.h"
#include "fmbkupengine.h"
#include <QStringList>

#include <hbglobal.h>
#include <QTranslator>
#include <QLocale>

class TestSettings: public QObject
{
    Q_OBJECT

private slots:
	// init test strings those are used to verify string fetch feature of FmBackupSettings.
	void initTestCase();

    void testTestSettingsContentAll();
    void testTestSettingsContentOne();
    void testTestSettingsContentTwo();

    void testContentToString();
    void testContentToString_data();

    void testSchedulingToString();
    void testSchedulingToString_data();

    void testWeekdayToString();
    void testWeekdayToString_data();

    void testFmBackupEntry();

    void testFmRestoreInfo();
    void testFmRestoreEntry();

    void cleanupTestCase();       // Finalize test data

private:
    FmBackupSettings *settings;
	QString mFileManagerBackupWeekdayMonday;
	QString mFileManagerBackupWeekdayTuesday;
	QString mFileManagerBackupWeekdayWednesday;
	QString mFileManagerBackupWeekdayThursday;
	QString mFileManagerBackupWeekdayFirday;
	QString mFileManagerBackupWeekdaySaturday;
	QString mFileManagerBackupWeekdaySunday;

	QString mFileManagerBackupScheduleNever;
	QString mFileManagerBackupScheduleDaily;
	QString mFileManagerBackupScheduleWeekly;

	QString mFileManagerBackupSettingsTitleContents;
	QString mFileManagerBackupSettingsTitleScheduling;
	QString mFileManagerBackupSettingsTitleWeekday;
	QString mFileManagerBackupSettingsTitleTime;
	QString mFileManagerBackupSettingsTitleTargetDrive;

	QString mFileManagerBackupSettingsContentAll;
	QString mFileManagerBackupSettingsContentSettings;
	QString mFileManagerBackupSettingsContentMessages;
	QString mFileManagerBackupSettingsContentContacts;
	QString mFileManagerBackupSettingsContentCalendar;
	QString mFileManagerBackupSettingsContentBookmarks;
	QString mFileManagerBackupSettingsContentUserFiles;
};

void TestSettings::initTestCase()
{
	// install translator
	QTranslator translator;
    QString lang = QLocale::system().name(); 
    QString path = "z:/resource/qt/translations/"; 
    translator.load( path + "filemanager_" + lang );
    QCoreApplication::installTranslator(&translator);

	// init localized string
	mFileManagerBackupWeekdayMonday = hbTrId( "Monday" );
	mFileManagerBackupWeekdayTuesday = hbTrId( "Tuesday" );
	mFileManagerBackupWeekdayWednesday = hbTrId( "Wednesday" );
	mFileManagerBackupWeekdayThursday = hbTrId( "Thursday" );
	mFileManagerBackupWeekdayFirday = hbTrId( "Friday" );
	mFileManagerBackupWeekdaySaturday = hbTrId( "Saturday" );
	mFileManagerBackupWeekdaySunday = hbTrId( "Sunday" );

	mFileManagerBackupScheduleNever = hbTrId( "Never" );
	mFileManagerBackupScheduleDaily = hbTrId( "Daily" );
	mFileManagerBackupScheduleWeekly = hbTrId( "Weekly" );

	mFileManagerBackupSettingsTitleContents = hbTrId( "Backup Contents" );
	mFileManagerBackupSettingsTitleScheduling = hbTrId( "Backup scheduling" );
	mFileManagerBackupSettingsTitleWeekday = hbTrId( "Weekday" );
	mFileManagerBackupSettingsTitleTime = hbTrId( "Time" );
	mFileManagerBackupSettingsTitleTargetDrive = hbTrId( "Backup destination" );

	mFileManagerBackupSettingsContentAll = hbTrId( "All" );
	mFileManagerBackupSettingsContentSettings = hbTrId( "Settings" );
	mFileManagerBackupSettingsContentMessages = hbTrId( "Messages" );
	mFileManagerBackupSettingsContentContacts = hbTrId( "Contacts" );
	mFileManagerBackupSettingsContentCalendar = hbTrId( "Calendar" );
	mFileManagerBackupSettingsContentBookmarks = hbTrId( "Bookmarks" );
	mFileManagerBackupSettingsContentUserFiles = hbTrId( "Files" );
}

void TestSettings::cleanupTestCase()
{
    QCoreApplication::processEvents();
}

void TestSettings::testTestSettingsContentAll()
{
    FmBkupEngine *backupEngine = new FmBkupEngine( this );
    settings = new FmBackupSettings( backupEngine );
    QTime time = QTime::currentTime();
    settings->setContent(FmBackupSettings::EFileManagerBackupContentAll);
    settings->setScheduling(FmBackupSettings::EFileManagerBackupScheduleWeekly);
    settings->setWeekday(FmBackupSettings::EFileManagerBackupWeekdayMonday);
    settings->setTime(time);
    settings->setTargetDrive("C:");

    settings->save();
    delete settings;
    

    settings = new FmBackupSettings( backupEngine );
    settings->load();

    QVERIFY(settings->content() == FmBackupSettings::EFileManagerBackupContentAll);
    QVERIFY(settings->scheduling() == FmBackupSettings::EFileManagerBackupScheduleWeekly);
    QVERIFY(settings->weekday() == FmBackupSettings::EFileManagerBackupWeekdayMonday);
    QVERIFY( settings->time().toString() == time.toString() );
    QVERIFY(settings->targetDrive() == "C:");

    QList< FmBackupEntry* > backupEntryList = settings->backupEntryList();
    
    for( int i = 0; i< backupEntryList.count(); i++ )
    {
        FmBackupEntry* entry = backupEntryList[i];
        switch( entry->type() )
        {
        case FmBackupEntry::EContents:
            {
            QVERIFY( entry->title() == mFileManagerBackupSettingsTitleContents );
            break;
            }
        case FmBackupEntry::EScheduling:
            {
            QVERIFY( entry->title() == mFileManagerBackupSettingsTitleScheduling );
            break;
            }
        case FmBackupEntry::EWeekday:
            {
            QVERIFY( entry->title() == mFileManagerBackupSettingsTitleWeekday );
            break;
            }
        case FmBackupEntry::ETime:
            {
            QVERIFY( entry->title() == mFileManagerBackupSettingsTitleTime );
            break;
            }
        case FmBackupEntry::ETarget:
            {
            QVERIFY( entry->title() == mFileManagerBackupSettingsTitleTargetDrive );
            break;
            }

        }
        qDebug( entry->title().toUtf8().data() );
        qDebug( entry->tips().toUtf8().data() );
    }
    delete backupEngine;
}


void TestSettings::testTestSettingsContentOne()
{
    FmBkupEngine *backupEngine = new FmBkupEngine( this );
    settings = new FmBackupSettings( backupEngine );
    QTime time = QTime::currentTime();
    settings->setContent(FmBackupSettings::EFileManagerBackupContentSettings | 
        FmBackupSettings::EFileManagerBackupContentMessages);
    settings->setScheduling(FmBackupSettings::EFileManagerBackupScheduleWeekly);
    settings->setWeekday(FmBackupSettings::EFileManagerBackupWeekdayWednesday);
    settings->setTime(time);
    settings->setTargetDrive("E:");

    settings->save();
    delete settings;

    settings = new FmBackupSettings( backupEngine );
    settings->load();

    QVERIFY(settings->content() == quint32(FmBackupSettings::EFileManagerBackupContentSettings |
        FmBackupSettings::EFileManagerBackupContentMessages));
    QVERIFY(settings->scheduling() == FmBackupSettings::EFileManagerBackupScheduleWeekly);
    QVERIFY(settings->weekday() == FmBackupSettings::EFileManagerBackupWeekdayWednesday);
    QVERIFY( settings->time().toString() == time.toString() );
    QVERIFY(settings->targetDrive() == "E:");
    delete backupEngine;
}

void TestSettings::testTestSettingsContentTwo()
{
    FmBkupEngine *backupEngine = new FmBkupEngine( this );
    settings = new FmBackupSettings( backupEngine );
    QTime time = QTime::currentTime();
    settings->setContent(FmBackupSettings::EFileManagerBackupContentSettings | 
        FmBackupSettings::EFileManagerBackupContentMessages);
    settings->setScheduling(FmBackupSettings::EFileManagerBackupScheduleWeekly);
    settings->setWeekday(FmBackupSettings::EFileManagerBackupWeekdayWednesday);
    settings->setTime(time);
    settings->setTargetDrive("E:");

    settings->save();
    delete settings;

    settings = new FmBackupSettings( backupEngine );
    settings->load();

    QVERIFY(settings->content() == quint32( FmBackupSettings::EFileManagerBackupContentSettings |
        FmBackupSettings::EFileManagerBackupContentMessages ) );
    QVERIFY(settings->scheduling() == FmBackupSettings::EFileManagerBackupScheduleWeekly);
    QVERIFY(settings->weekday() == FmBackupSettings::EFileManagerBackupWeekdayWednesday);
    QVERIFY( settings->time().toString() == time.toString() );
    QVERIFY(settings->targetDrive() == "E:");
    delete backupEngine;
}

void TestSettings::testContentToString()
{
    QFETCH(quint32, param);
    QFETCH(QString, value);

    QCOMPARE(FmBackupSettings::contentToString( param ), value);
}

void TestSettings::testContentToString_data()
{
    QTest::addColumn<quint32>("param");
    QTest::addColumn<QString>("value");

    QTest::newRow("contentToString_0") << (quint32)FmBackupSettings::EFileManagerBackupContentAll << mFileManagerBackupSettingsContentAll;
    QTest::newRow("contentToString_1") << (quint32)FmBackupSettings::EFileManagerBackupContentSettings << mFileManagerBackupSettingsContentSettings;
    QTest::newRow("contentToString_2") << (quint32)FmBackupSettings::EFileManagerBackupContentMessages << mFileManagerBackupSettingsContentMessages;
    QTest::newRow("contentToString_3") << (quint32)FmBackupSettings::EFileManagerBackupContentContacts << mFileManagerBackupSettingsContentContacts;
    QTest::newRow("contentToString_4") << (quint32)FmBackupSettings::EFileManagerBackupContentCalendar << mFileManagerBackupSettingsContentCalendar;
    QTest::newRow("contentToString_5") << (quint32)FmBackupSettings::EFileManagerBackupContentBookmarks << mFileManagerBackupSettingsContentBookmarks;
    QTest::newRow("contentToString_6") << (quint32)FmBackupSettings::EFileManagerBackupContentUserFiles << mFileManagerBackupSettingsContentUserFiles;
}

void TestSettings::testSchedulingToString()
{
    QFETCH(int, param);
    QFETCH(QString, value);
    FmBkupEngine *backupEngine = new FmBkupEngine( this );
    settings = new FmBackupSettings( backupEngine );
    QCOMPARE(settings->schedulingToString( (FmBackupSettings::TFileManagerBackupSchedule)param ), value);
}

void TestSettings::testSchedulingToString_data()
{
    QTest::addColumn<int>("param");
    QTest::addColumn<QString>("value");

    QTest::newRow("schedulingToString_0") << (int)FmBackupSettings::EFileManagerBackupScheduleNever << mFileManagerBackupScheduleNever;
    QTest::newRow("schedulingToString_1") << (int)FmBackupSettings::EFileManagerBackupScheduleDaily << mFileManagerBackupScheduleDaily;
    QTest::newRow("schedulingToString_2") << (int)FmBackupSettings::EFileManagerBackupScheduleWeekly << mFileManagerBackupScheduleWeekly;
}

void TestSettings::testWeekdayToString()
{
    QFETCH(int, param);
    QFETCH(QString, value);

    QCOMPARE(FmBackupSettings::weekdayToString( (FmBackupSettings::TFileManagerBackupWeekday)param ), value);
}

void TestSettings::testWeekdayToString_data()
{

    QTest::addColumn<int>("param");
    QTest::addColumn<QString>("value");

    QTest::newRow("weekdayToString_0") << (int)FmBackupSettings::EFileManagerBackupWeekdayMonday << mFileManagerBackupWeekdayMonday;
    QTest::newRow("weekdayToString_1") << (int)FmBackupSettings::EFileManagerBackupWeekdayTuesday << mFileManagerBackupWeekdayTuesday;
    QTest::newRow("weekdayToString_2") << (int)FmBackupSettings::EFileManagerBackupWeekdayWednesday << mFileManagerBackupWeekdayWednesday;
    QTest::newRow("weekdayToString_3") << (int)FmBackupSettings::EFileManagerBackupWeekdayThursday << mFileManagerBackupWeekdayThursday;
    QTest::newRow("weekdayToString_4") << (int)FmBackupSettings::EFileManagerBackupWeekdayFriday << mFileManagerBackupWeekdayFirday;
    QTest::newRow("weekdayToString_5") << (int)FmBackupSettings::EFileManagerBackupWeekdaySaturday << mFileManagerBackupWeekdaySaturday;
    QTest::newRow("weekdayToString_6") << (int)FmBackupSettings::EFileManagerBackupWeekdaySunday << mFileManagerBackupWeekdaySunday;
}


void TestSettings::testFmBackupEntry()
{
    FmBackupEntry backupEntry( "title","tips", FmBackupEntry::EContents );
    QVERIFY( backupEntry.title() == "title" );
    QVERIFY( backupEntry.tips() == "tips" );
    QVERIFY( backupEntry.type() == FmBackupEntry::EContents );

    FmBackupEntry backupEntryCopy( backupEntry );
    QVERIFY( backupEntryCopy.title() == "title" );
    QVERIFY( backupEntryCopy.tips() == "tips" );
    QVERIFY( backupEntryCopy.type() == FmBackupEntry::EContents );

}

void TestSettings::testFmRestoreInfo()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    FmRestoreInfo restoreInfo((quint32)FmBackupSettings::EFileManagerBackupContentSettings, dateTime, "E:" );
    QVERIFY( restoreInfo.content() == (quint32)FmBackupSettings::EFileManagerBackupContentSettings );
    QVERIFY( restoreInfo.dateTime().toString() == dateTime.toString() );
    QVERIFY( restoreInfo.drive() == "E:" );

    FmRestoreInfo restoreInfoCopy( restoreInfo );
    QVERIFY( restoreInfoCopy.content() == restoreInfo.content() );
    QVERIFY( restoreInfoCopy.dateTime().toString() == restoreInfo.dateTime().toString() );
    QVERIFY( restoreInfoCopy.drive() == restoreInfo.drive() );

    FmRestoreInfo restoreInfoNext = restoreInfo;
    QVERIFY( restoreInfoNext.content() == restoreInfo.content() );
    QVERIFY( restoreInfoNext.dateTime().toString() == restoreInfo.dateTime().toString() );
    QVERIFY( restoreInfoNext.drive() == restoreInfo.drive() );
}

void TestSettings::testFmRestoreEntry()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    FmRestoreInfo restoreInfo((quint32)FmBackupSettings::EFileManagerBackupContentSettings, dateTime, "E:" );

    FmRestoreEntry restoreEntry( "displayText", restoreInfo );
    QVERIFY( restoreEntry.text() == "displayText");
    QVERIFY( restoreEntry.restoreInfo().content() == (quint32)FmBackupSettings::EFileManagerBackupContentSettings );
    QVERIFY( restoreEntry.restoreInfo().dateTime().toString() == dateTime.toString() );
    QVERIFY( restoreEntry.restoreInfo().drive() == "E:" );
}

QTEST_MAIN(TestSettings)
#include "unit_backuprestoresettings.moc"
