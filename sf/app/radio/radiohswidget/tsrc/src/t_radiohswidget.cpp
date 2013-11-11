/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <QVariant>

// User Includes
#include "t_radiohswidget.h"
#include "xqsettingsmanager.h"
#include "xqapplicationmanager.h"
#include "xqaiwrequest.h"
#include "xqaiwinterfacedescriptor.h"
#include "radioservicedef.h"
#include "radiologger.h"

// Constants
/**
* From ProfileEngineSDKCRKeys.h
* The id of the currently active profile, integer value.
* Possible values:
*
* 0 = General profile (default value)<br>
* 1 = Silent profile<br>
* 2 = Meeting profile<br>
* 3 = Outdoor profile<br>
* 4 = Pager profile<br>
* 5 = Off-line profile<br>
* 6 = Drive profile<br>
* 30-49 = User-created profiles<br>
**/
const int PROFILE_GENERAL(0); 
const int PROFILE_SILENT(1); 
const int PROFILE_MEETING(2); 
const int PROFILE_OUTDOOR(3); 
const int PROFILE_PAGER(4); 
const int PROFILE_OFFLINE(5); 
const int PROFILE_DRIVE(6); 
const int PROFILE_USER_CREATED(35);

/** Requests are not embedded. */
const bool EMBEDDED_REQUEST(false);

/** Last request should be answered with empty reply. */
int EMPTY_REPLY(-5);

/** Test values for favorite station count. */
const int FAVORITE_STATION_COUNT_INVALID_TOO_LOW1(-56);
const int FAVORITE_STATION_COUNT_INVALID_TOO_LOW2(-1);
const int FAVORITE_STATION_COUNT_VALID_LOWER_BOUNDARY(0);
const int FAVORITE_STATION_COUNT_VALID1(1);
const int FAVORITE_STATION_COUNT_VALID2(2);
const int FAVORITE_STATION_COUNT_VALID3(3);
const int FAVORITE_STATION_COUNT_VALID4(9);
const int FAVORITE_STATION_COUNT_VALID5(78);
const int FAVORITE_STATION_COUNT_VALID_UPPER_BOUNDARY(100);
const int FAVORITE_STATION_COUNT_INVALID_TOO_HIGH1(101);
const int FAVORITE_STATION_COUNT_INVALID_TOO_HIGH2(1256);

/** Test values for textual information. */
const QString TEST_FREQUENCY("99.9");
const QString TEST_NAME("Radio Test");
const QString TEST_GENRE("Test GenRe");
const QString TEST_RADIO_TEXT("Test RDS");
const QString TEST_DYNAMIC_PSNAME("Dynamic PS Name test");
const QString TEST_EMPTY_STRING("");
const QString TEST_INVALID_STRING;
/** This test string contains 1000 characters. */
const QString TEST_TOO_LONG_STRING("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

/** Test values for service notification types. */
const int RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_LOW1(-357);
const int RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_LOW2(-1);
const int RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_LOW3(0);
const int RADIO_SERVICE_NOTIFICATION_ID_VALID_LOWER_BOUNDARY(1);
const int RADIO_SERVICE_NOTIFICATION_ID_VALID_UPPER_BOUNDARY(8);
const int RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_HIGH1(9);
const int RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_HIGH2(57982);
const int RADIO_STATUS_INVALID_TOO_LOW1(-581);
const int RADIO_STATUS_INVALID_TOO_LOW2(-1);
const int RADIO_STATUS_VALID_LOWER_BOUNDARY(0);
const int RADIO_STATUS_VALID_UPPER_BOUNDARY(5);
const int RADIO_STATUS_INVALID_TOO_HIGH1(6);
const int RADIO_STATUS_INVALID_TOO_HIGH2(456);

/** Mock returns for settings manager stub. */
QVariant settings_manager_mock_return_profile;
QVariant settings_manager_mock_return_2nd_profile;
bool settings_manager_mock_bool_return;

/** These request are defined in stub xqapplicationmanager.cpp. */
extern XQAiwRequest *application_manager_mock_refresh_request;
extern XQAiwRequest *application_manager_mock_monitor_request;

/** List for request mocks. */
QVariantList request_mocks;
/** Amount of requests send. */
int send_count;


/*!
    \class TestRadioHsWidget
    \brief Unit testing for FM Radio hs widget.

    TestRadioHsWidget implements needed functions for the FM Radio home screen
    widget unit tests.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Main function.
    
    There's been problems with QTEST_MAIN macro, so main function is defined
    here.
*/
int main(int /* argc*/, char *argv[])
{
    LOG_METHOD_ENTER;
    TestRadioHsWidget tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiohswidget.txt";
    
    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

/*!
    Constructor.
 */
TestRadioHsWidget::TestRadioHsWidget()
{
    LOG_METHOD_ENTER;
}


/*!
    Destructor.
 */
TestRadioHsWidget::~TestRadioHsWidget()
{
    LOG_METHOD_ENTER;
}

/*!
    Called before each testfunction is executed.
 */
void TestRadioHsWidget::init()
{
    LOG_METHOD_ENTER;
    settings_manager_mock_return_profile = PROFILE_GENERAL;
    request_mocks.clear();
    mSendInfos.clear();
    send_count = -1;
    mRadioHsWidgetStub.data()->reset();
}

/*!
    Called after every testfunction.
 */
void TestRadioHsWidget::cleanup()
{
    LOG_METHOD_ENTER;
}

/*!
    Called before the first testfunction is executed
 */
void TestRadioHsWidget::initTestCase()
{
    LOG_METHOD_ENTER;
    mRadioHsWidgetStub.reset(new RadioHsWidget());
    QVERIFY(mRadioHsWidgetStub);
    mProfileReader.reset(new RadioHsWidgetProfileReader(mRadioHsWidgetStub.data()));
    QVERIFY(mProfileReader);
    mServiceClient.reset(new RadioHsWidgetRadioServiceClient(mRadioHsWidgetStub.data()));
    QVERIFY(mServiceClient);
}

/*!
    Called after the last testfunction was executed.
 */
void TestRadioHsWidget::cleanupTestCase()
{
    LOG_METHOD_ENTER;
}

/*!
    Tests RadioHsWidgetProfileReader and it's isCurrentProfileOffline()
    function.
 */
void TestRadioHsWidget::offlineModeTest()
{
    LOG_METHOD;
    settings_manager_mock_return_profile = PROFILE_GENERAL;
    bool isOffline = mProfileReader.data()->isCurrentProfileOffline();
    QVERIFY2(( isOffline == false ), "FmRadioHsWidget:RadioHsWidget profile is offline");
    settings_manager_mock_return_profile = PROFILE_OFFLINE;
    isOffline = mProfileReader.data()->isCurrentProfileOffline();
    QVERIFY2(( isOffline == true ), "FmRadioHsWidget:RadioHsWidget profile is not offline");
    settings_manager_mock_return_profile = PROFILE_USER_CREATED;
    isOffline = mProfileReader.data()->isCurrentProfileOffline();
    QVERIFY2(( isOffline == false ), "FmRadioHsWidget:RadioHsWidget profile is offline");
}

/*!
    Tests RadioHsWidgetRadioServiceClient class and it's ability to receive
    and forward valid information.
 */
void TestRadioHsWidget::validReplies()
{
    LOG_METHOD;
    // Replies that should be ok.
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID_LOWER_BOUNDARY));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID1));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID2));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID3));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID4));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID5));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_VALID_UPPER_BOUNDARY));
    createSendInfo(RadioServiceNotification::CurrentIsFavorite, QVariant(true));
    createSendInfo(RadioServiceNotification::CurrentIsFavorite, QVariant(false));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RadioStatus::UnSpecified));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RadioStatus::Playing));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RadioStatus::Muted));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RadioStatus::Seeking));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RadioStatus::NoAntenna));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RadioStatus::PoweringOff));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RADIO_STATUS_VALID_LOWER_BOUNDARY));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RADIO_STATUS_VALID_UPPER_BOUNDARY));
    createSendInfo(RadioServiceNotification::Frequency, QVariant(TEST_FREQUENCY));
    createSendInfo(RadioServiceNotification::Name, QVariant(TEST_NAME));
    createSendInfo(RadioServiceNotification::Name, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RadioServiceNotification::Genre, QVariant(TEST_GENRE));
    createSendInfo(RadioServiceNotification::RadioText, QVariant(TEST_RADIO_TEXT));
    createSendInfo(RadioServiceNotification::DynamicPS, QVariant(TEST_DYNAMIC_PSNAME));
    // Empty reply should be last to put RServiceClient to make an request, which is not replied. 
    createSendInfo(EMPTY_REPLY, QVariant(EMPTY_REPLY));

    send_count = mSendInfos.count();
    
    mServiceClient.data()->startMonitoring(FmRadio::VisibiltyToBackground);
    mServiceClient.data()->stopMonitoring();
    
    int receivedCount = mRadioHsWidgetStub.data()->receivedInformationCount();
    QVERIFY2((receivedCount == send_count - 1), "Send and received counts doesn't match");
    
    ReceivedInfo temp;
    for (int i = 0; i < send_count - 1; i++) {
        temp = mRadioHsWidgetStub.data()->receivedInformation();
        QVERIFY(temp.informationType == mSendInfos.at(i).informationType);
        verifyReceivedInformation(temp.informationType, temp.information, mSendInfos.at(i).information);
    }
}

/*!
    Tests RadioHsWidgetRadioServiceClient class and it's ability to receive
    and forward invalid information.
 */
void TestRadioHsWidget::invalidReplies()
{
    LOG_METHOD;
    QList<int> skipSendInfo;
    // Replies that should not be ok.
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_INVALID_TOO_LOW1));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_INVALID_TOO_LOW2));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_INVALID_TOO_HIGH1));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant(FAVORITE_STATION_COUNT_INVALID_TOO_HIGH2));
    createSendInfo(RadioServiceNotification::FavoriteCount, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::CurrentIsFavorite, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RADIO_STATUS_INVALID_TOO_LOW1));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RADIO_STATUS_INVALID_TOO_LOW1));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RADIO_STATUS_INVALID_TOO_HIGH1));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant(RADIO_STATUS_INVALID_TOO_HIGH2));
    createSendInfo(RadioServiceNotification::RadioStatus, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::Frequency, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::Name, QVariant(TEST_INVALID_STRING));
    createSendInfo(RadioServiceNotification::Name, QVariant(TEST_TOO_LONG_STRING));
    createSendInfo(RadioServiceNotification::Name, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::Genre, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::RadioText, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    createSendInfo(RadioServiceNotification::DynamicPS, QVariant());
    skipSendInfo.append(request_mocks.count() - 1);
    // Replies that should generate a warning.
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_VALID_LOWER_BOUNDARY, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_VALID_UPPER_BOUNDARY, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_LOW1, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_LOW2, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_LOW3, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_HIGH1, QVariant(TEST_EMPTY_STRING));
    createSendInfo(RADIO_SERVICE_NOTIFICATION_ID_INVALID_TOO_HIGH2, QVariant(TEST_EMPTY_STRING));
    // Empty reply should be last to put RServiceClient to make an request, which is not replied. 
    createSendInfo(EMPTY_REPLY, QVariant(EMPTY_REPLY));
    
    send_count = mSendInfos.count();
    
    mServiceClient.data()->startMonitoring(FmRadio::VisibiltyToBackground);
    mServiceClient.data()->stopMonitoring();
    
    int receivedCount = mRadioHsWidgetStub.data()->receivedInformationCount();
    int skippedCount = skipSendInfo.count();
    QVERIFY2((receivedCount == send_count - skippedCount - 1), "Send and received counts doesn't match");
    
    ReceivedInfo temp;
    for (int i = 0; i < send_count - 1; i++) {
        if (skipSendInfo.contains(i)) {
            // Skip verification for information that was filtered in
            // RadioHsWidgetRadioServiceClient.
            continue;
        }
        temp = mRadioHsWidgetStub.data()->receivedInformation();
        QVERIFY(temp.informationType == mSendInfos.at(i).informationType);
        verifyReceivedInformation(temp.informationType, temp.information, mSendInfos.at(i).information);
    }
}

/*!
    Helper function to store send information and append it to a request
    mocks. 
 */
void TestRadioHsWidget::createSendInfo(int informationType, const QVariant information)
{
    SendInfo info;
    QVariantList notificationList;
    
    info.informationType = informationType;
    info.information = information;
    info.data = QVariant(mServiceClient.data()->
        createRadioNotificationData(
            info.informationType,
            info.information
            ));
    
    mSendInfos.append(info);

    notificationList.append(info.data);
    request_mocks.append(QVariant(notificationList));
}

/*!
    Helper function to verify receved information.
 */
void TestRadioHsWidget::verifyReceivedInformation(int informationType,
    const QVariant &receivedInformation, const QVariant &sendInformation)
{
    switch (informationType) {
    case RadioServiceNotification::CurrentIsFavorite:
        QVERIFY(receivedInformation.canConvert(QVariant::Bool));
        QCOMPARE(receivedInformation.toBool(), sendInformation.toBool());
        break;
    case RadioServiceNotification::FavoriteCount:
        // Fall through because of similar handling
    case RadioServiceNotification::RadioStatus:
        QVERIFY(receivedInformation.canConvert(QVariant::Int));
        QCOMPARE(receivedInformation.toInt(), sendInformation.toInt());
        break;
    case RadioServiceNotification::Frequency:
        // Fall through because of similar type of information and thus
        // similar handling
    case RadioServiceNotification::Name:
        // Fall through because of similar type of information and thus
        // similar handling
    case RadioServiceNotification::Genre:
        // Fall through because of similar type of information and thus
        // similar handling
    case RadioServiceNotification::RadioText:
        // Fall through because of similar type of information and thus
        // similar handling
    case RadioServiceNotification::DynamicPS:
        QVERIFY(receivedInformation.canConvert(QVariant::String));
        QCOMPARE(receivedInformation.toString(), sendInformation.toString());
        break;
    default:
        QWARN("Default case, shouldn't be here except if specifically testing RadioServiceNotification id out of bounds.");
        QVERIFY(receivedInformation.canConvert(QVariant::String));
        QCOMPARE(receivedInformation.toString(), sendInformation.toString());
        break;
    }
}

//QTEST_MAIN(t_radiohswidget)
