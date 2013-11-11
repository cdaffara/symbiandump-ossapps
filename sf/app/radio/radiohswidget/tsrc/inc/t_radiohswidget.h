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


#ifndef T_RADIOHSWIDGET_H_
#define T_RADIOHSWIDGET_H_


// System includes
#include <QtTest/QtTest>

// User includes
#include "radiohswidget.h"
#include "radiohswidgetprofilereader.h"
#include "radiohswidgetradioserviceclient.h"

/*!
    \struct SendInfo
    \brief Helper to store send information on unit tests.
*/
struct SendInfo {
    /*! Stores the type of information send. */
    int informationType;
    /*! Stores the information to send. */
    QVariant information;
    /*! Stores the actual data of information send. */
    QVariant data;
};


class TestRadioHsWidget : public QObject
{
    Q_OBJECT    
    
public:

    TestRadioHsWidget();
    ~TestRadioHsWidget();    

public slots:
    
private slots:
    // test framework called slots 
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();
    void offlineModeTest();
    void validReplies();
    void invalidReplies();
    
private:
    void createSendInfo(int informationType, const QVariant information);
    void verifyReceivedInformation(int informationType,
        const QVariant &receivedInformation, const QVariant &sendInformation);
    
private: // Data
    /*! Stores RadioHsWidget stub. */
    QScopedPointer<RadioHsWidget> mRadioHsWidgetStub;
    /*! Stores RadioHsWidgetProfileReader. */
    QScopedPointer<RadioHsWidgetProfileReader> mProfileReader;
    /*! Stores RadioHsWidgetRadioServiceClient. */
    QScopedPointer<RadioHsWidgetRadioServiceClient> mServiceClient;
    
    /*! Stores refresh request mock. */
    //XQAiwRequest *application_manager_mock_refresh_request;
    
    /*! Stores send information. */
    QList<SendInfo> mSendInfos;
};

#endif /* T_RADIOHSWIDGET_H_ */
