/*!
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
* Description:  Unit tests for PhoneNoteController.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <hbapplication.h>
#include <QSignalSpy>
//#include <hbglobal_p.h>
#include "phonenotecontroller.h"
#include "phoneresourceids.h"
#include "phoneui.hrh"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamquery.h"
#include "phoneresourceadapter.h"
#include "phoneconstants.h"
#include "phoneuitestmain.h"


class TestPhoneNoteController : public QObject
{
    Q_OBJECT
public:
    TestPhoneNoteController();
    virtual ~TestPhoneNoteController();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testShowGlobalNoteDefault ();
    void testShowGlobalInfoNoteWithResourceId ();
    void testShowGlobalWarningNoteWithText ();
    void testShowGlobalNoteWithTextAndResourceId ();
    void testShowGlobalNoteTwoTimes ();
    void testShowGlobalNoteThreeTimes ();
    void testShowNote();
    void testShowQuery();
    void testShowClobalWaitNote();
    void testShowIndicationDialogDefault ();
    void testShowIndicationDialogWithResourceId ();
    void testShowIndicationDialogWithText ();
    void testShowIndicationDialogTextAndResourceId ();
    void testShowIndicationDialogTwoTimes ();
    void testShowIndicationDialogThreeTimes ();
    
private:
    PhoneNoteController *m_noteController; // class under test
};

TestPhoneNoteController::TestPhoneNoteController ()
{
}

TestPhoneNoteController::~TestPhoneNoteController ()
{
}

void TestPhoneNoteController::initTestCase ()
{
    m_noteController = new PhoneNoteController();
}

void TestPhoneNoteController::cleanupTestCase ()
{
    delete m_noteController;
}

void TestPhoneNoteController::init ()
{
}

void TestPhoneNoteController::cleanup ()
{
}

void TestPhoneNoteController::testShowGlobalNoteDefault ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetTimeout(0);
    m_noteController->showGlobalNote(&globalNoteParam);
    QTest::qWait(2500);
    
    globalNoteParam.SetTimeout(KPhoneNoteNoTimeout);
    m_noteController->showGlobalNote(&globalNoteParam);
    QTest::qWait(2500);
}

void TestPhoneNoteController::testShowGlobalInfoNoteWithResourceId ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetTextResourceId(R_NOTETEXT_NO_ANSWER);
    globalNoteParam.SetType( EPhoneMessageBoxInformation );
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowGlobalWarningNoteWithText ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetType( EPhoneMessageBoxWarning );
    globalNoteParam.SetText(_L("Test note 1"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
}

void TestPhoneNoteController::testShowGlobalNoteWithTextAndResourceId ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetTextResourceId(R_PHONE_TEXT_COLP_CONNECTED);
    globalNoteParam.SetType( EPhoneMessageBoxInformation );
    globalNoteParam.SetText(_L("Test number"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowGlobalNoteTwoTimes ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    //globalNoteParam.SetTextResourceId(R_NOTETEXT_NO_ANSWER);
    globalNoteParam.SetType( EPhoneMessageBoxInformation );
    globalNoteParam.SetText(_L("Test note 2"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    globalNoteParam.SetText(_L("Test note 3"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(10000);

}

void TestPhoneNoteController::testShowGlobalNoteThreeTimes ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    //globalNoteParam.SetTextResourceId(R_NOTETEXT_NO_ANSWER);
    globalNoteParam.SetType( EPhoneMessageBoxInformation );
    globalNoteParam.SetText(_L("Test note 4"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    globalNoteParam.SetText(_L("Test note 5"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    globalNoteParam.SetText(_L("Test note 6"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowNote()
{
    TPhoneCmdParamNote noteParam;
    
    m_noteController->showNote(&noteParam);
    m_noteController->removeDtmfNote();
    m_noteController->removeNote();
    
    noteParam.SetType(EPhoneNoteDtmfSending);
    m_noteController->showNote(&noteParam);
    
    noteParam.SetResourceId(R_PHONEUI_SENDING_DTMF_WAIT_NOTE);
    noteParam.SetText(_L("Sending:\n123p456"));
    
    m_noteController->showNote(&noteParam);
    QTest::qWait(5000);
    
    noteParam.SetText(_L("Sending:\n123p456"));
    m_noteController->showNote(&noteParam);
    
    QTest::qWait(5000);
    
    m_noteController->removeNote();
    
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowQuery()
{
    TPhoneCmdParamQuery queryParam;    
    m_noteController->showQuery(&queryParam);
    
    queryParam.SetQueryType(EPhoneQueryDialog);
    m_noteController->showQuery(&queryParam);
    
    queryParam.SetQueryPrompt(_L("TestQuery"));
    m_noteController->showQuery(&queryParam);
    m_noteController->removeQuery();

    queryParam.SetQueryResourceId(R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY);
    m_noteController->showQuery(&queryParam);
    
    QTest::qWait(5000);
    m_noteController->showQuery(&queryParam);
    
    m_noteController->removeQuery();
    
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowClobalWaitNote()
{
    TPhoneCmdParamQuery queryParam;    
    
    queryParam.SetQueryType(EPhoneGlobalWaitNote);
    m_noteController->showQuery(&queryParam);
    QTest::qWait(5000);
    m_noteController->removeGlobalWaitNote();
    QTest::qWait(5000);
    
    queryParam.SetQueryPrompt(_L("TestQuery"));
    m_noteController->showQuery(&queryParam);
    QTest::qWait(5000);
    m_noteController->removeGlobalWaitNote();
    QTest::qWait(5000);
    
    queryParam.SetQueryPrompt(KNullDesC);
    TBuf<4> buf(_L("test"));
    queryParam.SetDataText(&buf);
    queryParam.SetTimeOut(2000);
    m_noteController->showQuery(&queryParam);
    QTest::qWait(5000);
     
    queryParam.SetCustomCommandForTimeOut(10);
    m_noteController->showQuery(&queryParam);
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowIndicationDialogDefault ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetNotificationDialog( ETrue );
    globalNoteParam.SetTimeout(0);
    m_noteController->showGlobalNote(&globalNoteParam);
    QTest::qWait(2500);
    
    globalNoteParam.SetTimeout(KPhoneNoteNoTimeout);
    m_noteController->showGlobalNote(&globalNoteParam);
    QTest::qWait(2500);
}

void TestPhoneNoteController::testShowIndicationDialogWithResourceId ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetNotificationDialog( ETrue );
    globalNoteParam.SetTextResourceId(R_NOTETEXT_NO_ANSWER);
    globalNoteParam.SetType( EPhoneNotificationDialog );
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(5000);
}

void TestPhoneNoteController::testShowIndicationDialogWithText ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetNotificationDialog( ETrue );
    globalNoteParam.SetType( EPhoneMessageBoxWarning );
    globalNoteParam.SetText(_L("Test indication 1"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
}

void TestPhoneNoteController::testShowIndicationDialogTextAndResourceId ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetNotificationDialog( ETrue );
    globalNoteParam.SetTextResourceId(R_PHONE_TEXT_COLP_CONNECTED);
    globalNoteParam.SetType( EPhoneNotificationDialog );
    globalNoteParam.SetText(_L("Indication number"));
    globalNoteParam.SetTimeout(1000);
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(2000);
}

void TestPhoneNoteController::testShowIndicationDialogTwoTimes ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetNotificationDialog( ETrue );
    //globalNoteParam.SetTextResourceId(R_NOTETEXT_NO_ANSWER);
    globalNoteParam.SetType( EPhoneNotificationDialog );
    globalNoteParam.SetText(_L("Test indication 2"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    globalNoteParam.SetText(_L("Test indication 3"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(5000);

}

void TestPhoneNoteController::testShowIndicationDialogThreeTimes ()
{
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetNotificationDialog( ETrue );
    globalNoteParam.SetType( EPhoneNotificationDialog );
    globalNoteParam.SetText(_L("Test indication 4"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    globalNoteParam.SetText(_L("Test indication 5"));
    
    globalNoteParam.SetTimeout(1000);
    m_noteController->showGlobalNote(&globalNoteParam);
    
    globalNoteParam.SetText(_L("Test indication 6"));
    
    m_noteController->showGlobalNote(&globalNoteParam);
    
    QTest::qWait(20000);
}

PHONE_UITEST_MAIN(TestPhoneNoteController)
#include "unit_tests.moc"
