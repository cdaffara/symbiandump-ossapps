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
* Description:  Test class for Communication Launcher widget
*
*/

#include "t_contactwidgeths.h"
#include "contactwidgeths.h"
#include "commlauncherwidget.h"

#include <HbEffect>
#include <HbDocumentLoader>

// Note: these are case sensitive
const QString callButtonName     = "ButtonCall";
const QString emailButtonName    = "ButtonEmail";
const QString messageButtonName  = "ButtonMessage";
const QString mycardButtonName   = "ButtonMycard";

// effect aliases
const QString appearTL = "appear_tl";
const QString appearTR = "appear_tr";
const QString appearBL = "appear_bl";
const QString appearBR = "appear_br"; 

// Docml file
const QString commLauncherDocml = ":/commlauncherwidget.docml";

/*!
  \class TestContactWidget
  \brief test class for CommLauncherWidget.
*/

/*!
    Initialize test case.
    Called before each testfunction is executed.
*/
void TestContactWidget::init()
{
    // For comm.launcher tests
    if (mTestPhase == 2) {
        QContact dummyContact;
        QContactLocalId dummyLocalId = 1;
        QContactId dummyId;
        dummyId.setLocalId(dummyLocalId);
        dummyContact.setId(dummyId);
        if (mCommLauncherWidget) {
			delete mCommLauncherWidget;
        }
        mCommLauncherWidget = new CommLauncherWidget;
        mCommLauncherWidget->setContact(dummyContact);
        QVERIFY(mCommLauncherWidget);
    }
}

/*!
    Cleanup test case.
    Called after every testfunction.
*/
void TestContactWidget::cleanup()
{
    // For comm.launcher tests
    if (mTestPhase == 2) {	
        delete mCommLauncherWidget;
        mCommLauncherWidget = 0;
    }
}

/*!
    switches to comm.launcher tests
*/
void TestContactWidget::setTestPhase()
{
    mTestPhase = 2; // Causes CommLauncher init test code to be run
    qDebug() << "CommLauncher TestCases begin ----------";
    QVERIFY(true); 
}

/*!
    Test creating a button for making a call
*/
void TestContactWidget::testCreateCallButton()
{
    const QString callIconName = "qtg_large_active_call";
    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    bool result = false;
    documentLoader->load(commLauncherDocml, &result);
    QVERIFY(result);
    HbPushButton *button = 
        mCommLauncherWidget->createButton(callIconName, callButtonName, documentLoader);
    QVERIFY(button);
    delete documentLoader;
}
  
/*!
    Test creating a button for sending a message
*/
void TestContactWidget::testCreateSendMsgButton()
{
    const QString msgIconName = "qtg_large_message";
    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    bool result = false;
    documentLoader->load(commLauncherDocml, &result);
    QVERIFY(result);
    HbPushButton *button = 
        mCommLauncherWidget->createButton(msgIconName, messageButtonName, documentLoader);
    QVERIFY(button); 
    delete documentLoader;
}

/*!
    Test creating a button for sending an email
*/
void TestContactWidget::testCreateEmailButton()
{
    const QString emailIconName = "qtg_large_email";
    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    bool result = false;
    documentLoader->load(commLauncherDocml, &result);
    QVERIFY(result);
    HbPushButton *button = 
        mCommLauncherWidget->createButton(emailIconName, emailButtonName, documentLoader);
    QVERIFY(button);
    delete documentLoader;
}

/*!
    Test creating a button for opening phonebook
*/
void TestContactWidget::testCreatePhonebookButton()
{
    const QString phonebookIconName = "qtg_large_mycard";
    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    bool result = false;
    documentLoader->load(commLauncherDocml, &result);
    QVERIFY(result);
    HbPushButton *button = 
        mCommLauncherWidget->createButton(phonebookIconName, mycardButtonName, documentLoader);
    QVERIFY(button);
    delete documentLoader;
}

/*!
    Test that creating a button with invalid icon retuns a blank button
*/
void TestContactWidget::testCreateInvalidButton()
{
    const QString invalidIconName = ":/invalid.svg";
    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    bool result = false;
    documentLoader->load(commLauncherDocml, &result);
    QVERIFY(result);
    HbPushButton *button = 
        mCommLauncherWidget->createButton(invalidIconName, invalidIconName, documentLoader);    
    QVERIFY(!button);
    delete documentLoader;
}

/*!
 *  Test that transition effects can added and used.
 *  
 *  This does NOT test whether the effects in .fxml files are correct.
 */
void TestContactWidget::testTransitionEffects()
{
    // add Effects
    bool b11 = HbEffect::add(mCommLauncherWidget, QString(":/friend_expand_tl.fxml"), appearTL );
    bool b12 = HbEffect::add(mCommLauncherWidget, QString(":/friend_expand_bl.fxml"), appearBL );
    bool b13 = HbEffect::add(mCommLauncherWidget, QString(":/friend_expand_tr.fxml"), appearTR );
    bool b14 = HbEffect::add(mCommLauncherWidget, QString(":/friend_expand_br.fxml"), appearBR );       
    bool b2 =  HbEffect::add(mCommLauncherWidget, QString(":/friend_minimize.fxml"),  "hide" );
    QVERIFY(b11);
    QVERIFY(b12);
    QVERIFY(b13);
    QVERIFY(b14);    
    QVERIFY(b2);
    // display them
    bool b31 = HbEffect::start(mCommLauncherWidget, appearTR);
    bool b32 = HbEffect::start(mCommLauncherWidget, appearBR);  
    bool b33 = HbEffect::start(mCommLauncherWidget, appearTL);  
    bool b34 = HbEffect::start(mCommLauncherWidget, appearBL);      
    bool b4 =  HbEffect::start(mCommLauncherWidget, "hide");
    QVERIFY(b31);
    QVERIFY(b32);
    QVERIFY(b33);
    QVERIFY(b34);    
    QVERIFY(b4);
    // Correct Appear-effect is selected depending on FriendWidget/CommsLauncher positions
    QPointF friendPos   = QPointF(100,100);
    QPointF launcherPos;
    launcherPos = QPointF(120,80);
    mCommLauncherWidget->selectAppearEffect(friendPos, launcherPos);
    QVERIFY(mCommLauncherWidget->mAppearEffect == appearTR );
    launcherPos = QPointF(120,200);
    mCommLauncherWidget->selectAppearEffect(friendPos, launcherPos);
    QVERIFY(mCommLauncherWidget->mAppearEffect == appearBR );
    launcherPos = QPointF(20,80);
    mCommLauncherWidget->selectAppearEffect(friendPos, launcherPos);
    QVERIFY(mCommLauncherWidget->mAppearEffect == appearTL );
    launcherPos = QPointF(20,200);
    mCommLauncherWidget->selectAppearEffect(friendPos, launcherPos);
    QVERIFY(mCommLauncherWidget->mAppearEffect == appearBL );     
}

/*!
    Test that the ui is created correctly when contact has number
*/
void TestContactWidget::testCreateUiWithNumber()
{
    //Create contact for commLauncher 
    QString secondName = "Launcher";  
    QString lastName = "Number";
    QString number1 = "22334400";
    QString number2 = "0";
    QString avatar2 = "";    
    QContactLocalId id = 0; 
    utilAddContact(secondName, lastName, number1, number2, avatar2, id);
    QContact con = utilGetContact(id);
    mCommLauncherWidget->setContact(con);
    
    mCommLauncherWidget->createUI();
    // UI has buttons atleast for phonebook, call and messaging
    QVERIFY(mCommLauncherWidget->mButtonCount > 2);
}

/*!
    Test that the ui is created correctly when contact has no number
*/
void TestContactWidget::testCreateUiWithoutNumber()
{
    //Create contact for commLauncher 
    QString secondName = "Launcher";  
    QString lastName = "NoNumber";
    QString number1 = "";
    QString number2 = "";
    QString avatar2 = "";    
    QContactLocalId id = 0; 
    utilAddContact(secondName, lastName, number1, number2, avatar2, id);
    QContact con = utilGetContact(id);
    mCommLauncherWidget->setContact(con);
    
    mCommLauncherWidget->createUI();
    // UI doesn't have buttons for call and messaging
    QVERIFY(mCommLauncherWidget->mButtonCount < 3);
}

/*!
    Test that the width for launcher is calculated correctly
*/
void TestContactWidget::testCommLauncherWidth()
{
    int width = mCommLauncherWidget->commLauncherWidth();
    QVERIFY(width > 0);
}

/*!
    Test that signal is emited from popupAboutToClose
*/
void TestContactWidget::testPopupAboutToClose()
{
    QSignalSpy spy(mCommLauncherWidget, SIGNAL(launcherClosed()));
    mCommLauncherWidget->popupAboutToClose();
    QCOMPARE(spy.count(), 1);
}

/*
    Create test contact with avatar.
    TODO This is here just because can't add avatar to contact in wk20.
*/
void TestContactWidget::testLast()
{
	mContactWidget->createContactManager();	
    QVERIFY(mContactWidget->mContactManager);
    
    // -Delete all contacts
    QVERIFY(utilDeleteContacts());      
    
    QContactLocalId id = 0; 
    // TODO since avatar handling will change from 9.2 to 10.1, 
    // testing cases where contact has avatar is not done yet. 
     QString firstName = "first_av";  
     QString lastName  = "Last1";
     QString number1   = "11223344";
     QString number2   = "+3585012345";
     QString avatar1   = "z:/private/20022F35/import/widgetregistry/2002C353/test1_avatar.jpg"; 
     utilAddContact( firstName,  lastName,  number1, number2, avatar1, id); 
    
     QVERIFY(1);
}
