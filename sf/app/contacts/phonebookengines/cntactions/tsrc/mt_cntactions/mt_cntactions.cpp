/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "mt_cntactions.h"
#include <qcontactaction.h>

#include <QtTest/QtTest>

#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 1000; \
        QTest::qWait(10); \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)


void TestCntActions::initTestCase()
{
    int error = qRegisterMetaType<QContactAction::State>();
	//create manager
	m_manager = new QContactManager("symbian");
	    
}

void TestCntActions::cleanupTestCase()
{
    delete m_manager;
}

void TestCntActions::init()
{
    //delete all contacts from the database
    QList<QContactLocalId> contacts = m_manager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    m_manager->removeContacts(contacts, &errorMap);
}

void TestCntActions::cleanup()
{}

void TestCntActions::emptyContactNoActionSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
    
    //expected no actions found
    QList<QContactActionDescriptor> actions = contact.availableActions();
    QVERIFY(actions.count() == 0);
}

void TestCntActions::phonenumberCallSupport()
{
    QContact contact;
    
    //Add phonenumber to contact
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("555111222");
    contact.saveDetail(&number);
    m_manager->saveContact(&contact);
    
    //verify that the contact has a phonenumber
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() >  0);
    
    //get the actions
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    
    //verify that it includes the actiosn
    QVERIFY(actions.contains("call", Qt::CaseInsensitive));
    
    //verify that the action works
    QList<QContactActionDescriptor> callActionDescriptors = QContactAction::actionDescriptors("call", "symbian");
    QVERIFY(callActionDescriptors.count() == 1);
    QContactAction *callAction = QContactAction::action(callActionDescriptors.at(0));
    QVERIFY(callAction != 0);
    QContactAction::State state = callAction->state();
    QVERIFY(state != QContactAction::InactiveState);
    QVERIFY(callAction->isDetailSupported(numberList.at(0)) == true);
    QList<QContactDetail> supportedDetails = callAction->supportedDetails(contact);
    QVERIFY(supportedDetails.count() != 0);
    QVariantMap variantMap = callAction->metaData();
    QVERIFY(variantMap.count() == 0);
    variantMap = callAction->results();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyCallAction(callAction, SIGNAL(stateChanged(QContactAction::State)));
    callAction->invokeAction(contact, numberList.at(0));
    callAction->invokeAction(contact);
    QTRY_COMPARE(spyCallAction.count(), 2); // make sure the signal was emitted exactly one time
    delete callAction;
}

void TestCntActions::phonenumberNoCallSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
        
    //no actions expected
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    
    QVERIFY(actions.contains("call", Qt::CaseInsensitive) == false);
}

void TestCntActions::phonenumberMessageSupport()
{
    QContact contact;
    
    //Add phonenumber to contact
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("555111222");
    contact.saveDetail(&number);
    m_manager->saveContact(&contact);
    
    //verify that the contact has a phonenumber
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() >  0);
    
    //get the actions
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    
    //verify that it includes the actiosn
    QVERIFY(actions.contains("message", Qt::CaseInsensitive));
    
    QList<QContactActionDescriptor> messageActionDescriptors = QContactAction::actionDescriptors("message", "symbian");
    QVERIFY(messageActionDescriptors.count() == 1);
    QContactAction *messageAction = QContactAction::action(messageActionDescriptors.at(0));
    QVERIFY(messageAction != 0);
    QVERIFY(messageAction->isDetailSupported(numberList.at(0)) == true);
    QList<QContactDetail> supportedDetails = messageAction->supportedDetails(contact);
    QVERIFY(supportedDetails.count() != 0);
    QVariantMap variantMap = messageAction->metaData();
    QVERIFY(variantMap.count() == 0);
    variantMap = messageAction->results();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyMessageAction(messageAction, SIGNAL(stateChanged(QContactAction::State)));
    messageAction->invokeAction(contact, numberList.at(0));
    messageAction->invokeAction(contact);
    QTRY_COMPARE(spyMessageAction.count(), 2); // make sure the signal was emitted exactly one time
    delete messageAction;
}

void TestCntActions::phonenumberNoMessageSupport()
{
    QContactPhoneNumber faxNumber;
    faxNumber.setNumber("555111222");
    faxNumber.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);

    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors("message", "symbian");

    QContactAction* contactAction = QContactAction::action(actionDescriptors.first());
    bool isSupportDetail = contactAction->isDetailSupported(faxNumber);
    
    delete contactAction;

    QVERIFY(isSupportDetail == false);
}

void TestCntActions::phonenumberVideoCallSupport()
{
    QContact contact;
    
    //Add phonenumber to contact
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("555111222");
    contact.saveDetail(&number);
    m_manager->saveContact(&contact);
    
    //verify that the contact has a phonenumber
    QList<QContactPhoneNumber> numberList = contact.details<QContactPhoneNumber>();
    QVERIFY(numberList.count() >  0);
    
    //get the actions
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    
    //verify that it includes the actiosn
    QVERIFY(actions.contains("videocall", Qt::CaseInsensitive));
    
    //Test Video Call action
    QList<QContactActionDescriptor> videoCallActionDescriptors = QContactAction::actionDescriptors("videocall", "symbian");
    QVERIFY(videoCallActionDescriptors.count() == 1);
    QContactAction *videoCallAction = QContactAction::action(videoCallActionDescriptors.at(0));
    QVERIFY(videoCallAction != 0);
    QVERIFY(videoCallAction->isDetailSupported(numberList.at(0)) == true);
    QList<QContactDetail> supportedDetails = videoCallAction->supportedDetails(contact);
    QVERIFY(supportedDetails.count() != 0);
    QVariantMap variantMap = videoCallAction->metaData();
    QVERIFY(variantMap.count() == 0);
    variantMap = videoCallAction->results();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyVideoCallAction(videoCallAction, SIGNAL(stateChanged(QContactAction::State)));
    videoCallAction->invokeAction(contact, numberList.at(0));
    videoCallAction->invokeAction(contact);
    QTRY_COMPARE(spyVideoCallAction.count(), 2); // make sure the signal was emitted exactly one time
    delete videoCallAction;
}

void TestCntActions::phonenumberNoVideoCallSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
    
    //expected no actions found
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    QVERIFY(actions.contains("videocall", Qt::CaseInsensitive) == false);
}

void TestCntActions::emailSupport()
{
    QContact contact;
    QContactEmailAddress email;
    email.setEmailAddress("test@test.com");
    contact.saveDetail(&email);
    m_manager->saveContact(&contact);
    
    //one number exist in contact
    QList<QContactEmailAddress> emailList = contact.details<QContactEmailAddress>();
    QVERIFY(emailList.count() == 1);
    
    //one action expected
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    QVERIFY(actions.count() == 1);   
    QVERIFY(actions.contains("email", Qt::CaseInsensitive));
    
    //pick first number for the actions
    QContactEmailAddress emailAddress = contact.detail<QContactEmailAddress>();
        
    //Test Email action
    QList<QContactActionDescriptor> emailActionDescriptors = QContactAction::actionDescriptors("email", "symbian");
    QVERIFY(emailActionDescriptors.count() == 1);
    QContactAction *emailAction = QContactAction::action(emailActionDescriptors.at(0));
    QVERIFY(emailAction != 0);
    QVERIFY(emailAction->isDetailSupported(emailList.at(0)) == true);
    QList<QContactDetail> supportedDetails = emailAction->supportedDetails(contact);
    QVERIFY(supportedDetails.count() != 0);
    QVariantMap variantMap = emailAction->metaData();
    QVERIFY(variantMap.count() == 0);
    variantMap = emailAction->results();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyEmailAction(emailAction, SIGNAL(stateChanged(QContactAction::State)));
    emailAction->invokeAction(contact, emailAddress);
    emailAction->invokeAction(contact);
    QTRY_COMPARE(spyEmailAction.count(), 2); // make sure the signal was emitted exactly one time
    delete emailAction;
}

void TestCntActions::noEmailSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
    
    //expected no actions found
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    QVERIFY(actions.contains("email", Qt::CaseInsensitive) == false);
}

void TestCntActions::urlSupport()
{
    QContact contact;
    QContactUrl url;
    url.setUrl("www.test.com");
    contact.saveDetail(&url);
    m_manager->saveContact(&contact);
    
    //one number exist in contact
    QList<QContactUrl> urlList = contact.details<QContactUrl>();
    QVERIFY(urlList.count() == 1);
    
    //no actions expected
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    QVERIFY(actions.count() == 1);   
    QVERIFY(actions.contains("url", Qt::CaseInsensitive));
    
    //pick first number for the actions
    QContactUrl urlAddress = contact.detail<QContactUrl>();
        
    //Test Email action
    QList<QContactActionDescriptor> urlActionDescriptors = QContactAction::actionDescriptors("url", "symbian");
    QVERIFY(urlActionDescriptors.count() == 1);
    QContactAction *urlAction = QContactAction::action(urlActionDescriptors.at(0));
    QVERIFY(urlAction != 0);
    QVERIFY(urlAction->isDetailSupported(urlList.at(0)) == true);
    QList<QContactDetail> supportedDetails = urlAction->supportedDetails(contact);
    QVERIFY(supportedDetails.count() != 0);
    QVariantMap variantMap = urlAction->metaData();
    QVERIFY(variantMap.count() == 0);
    variantMap = urlAction->results();
    QVERIFY(variantMap.count() == 0);
    QSignalSpy spyUrlAction(urlAction, SIGNAL(stateChanged(QContactAction::State)));
    urlAction->invokeAction(contact, urlAddress);
    urlAction->invokeAction(contact);
    QTRY_COMPARE(spyUrlAction.count(), 2); // make sure the signal was emitted exactly once each time
    delete urlAction;
}

void TestCntActions::noUrlSupport()
{
    QContact contact;
    m_manager->saveContact(&contact);
    
    //expected no actions found
    QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
    QStringList actions;
    for (int i = 0;i < actionDescriptors.count();i++)
    {
        QString action = actionDescriptors.at(i).actionName();
        actions << action;
    }
    QVERIFY(actions.contains("url", Qt::CaseInsensitive) == false);
}

//QTEST_MAIN(TestCntActions);
