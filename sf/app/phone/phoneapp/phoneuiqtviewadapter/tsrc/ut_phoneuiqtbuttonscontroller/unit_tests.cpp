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
* Description:  Unit tests for PhoneUIQtButtonsController.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <hbapplication.h>
#include <QSignalSpy>
#include "phonetestmain.h"
#include "phoneuiqtbuttonscontroller.h"

class TestPhoneUIQtButtonsController : public QObject
{
    Q_OBJECT
public:
    TestPhoneUIQtButtonsController();
    virtual ~TestPhoneUIQtButtonsController();

public slots:
    void initTestCase ();
    void cleanupTestCase ();
    void init ();
    void cleanup (); 
    
private slots:
    void testSetButtonFlags();
    void testUnsetButtonFlags();
    void testGetButtonFlags();

private:
    // Helper functions
    bool checkFlags(bool hold, bool ihf, bool wired, bool btaa);
    void setFlags(bool hold, bool ihf, bool wired, bool btaa);
    
private:
    PhoneUIQtButtonsController *m_buttonsCtrl; // class under test
};

TestPhoneUIQtButtonsController::TestPhoneUIQtButtonsController ()
{
}

TestPhoneUIQtButtonsController::~TestPhoneUIQtButtonsController ()
{
}

void TestPhoneUIQtButtonsController::initTestCase ()
{

}

void TestPhoneUIQtButtonsController::cleanupTestCase ()
{

}

void TestPhoneUIQtButtonsController::init ()
{
    m_buttonsCtrl = new PhoneUIQtButtonsController ();
}

void TestPhoneUIQtButtonsController::cleanup ()
{
    delete m_buttonsCtrl;
}

void TestPhoneUIQtButtonsController::testSetButtonFlags()
{
    // Set one flag
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    QVERIFY(checkFlags(true,false,false,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    QVERIFY(checkFlags(false,true,false,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    QVERIFY(checkFlags(false,false,true,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(false,false,false,true) == true);
    
    // Two flags combinations
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    QVERIFY(checkFlags(true,true,false,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    QVERIFY(checkFlags(true,false,true,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(true,false,false,true) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    QVERIFY(checkFlags(false,true,true,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(false,true,false,true) == true);
    
    // Three flags combinations
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);    
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    QVERIFY(checkFlags(true,true,true,false) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);        
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(true,true,false,true) == true);

    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(true,false,true,true) == true);
    
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(false,true,true,true) == true);
    
    // All flags set
    setFlags(false,false,false,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,true);
    QVERIFY(checkFlags(true,true,true,true) == true);    
}

void TestPhoneUIQtButtonsController::testUnsetButtonFlags()
{
    // Unset one flag
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    QVERIFY(checkFlags(false,true,true,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);
    QVERIFY(checkFlags(true,false,true,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    QVERIFY(checkFlags(true,true,false,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(true,true,true,false) == true);
    
    // Two flags combinations
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);
    QVERIFY(checkFlags(false,false,true,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    QVERIFY(checkFlags(false,true,false,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(false,true,true,false) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    QVERIFY(checkFlags(true,false,false,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(true,false,true,false) == true);
    
    // Three flags combinations
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);    
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    QVERIFY(checkFlags(false,false,false,true) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);        
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(false,false,true,false) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(false,true,false,false) == true);
    
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(true,false,false,false) == true);
    
    // All flags unset
    setFlags(true,true,true,true);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,false);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,false);
    QVERIFY(checkFlags(false,false,false,false) == true);    
}

void TestPhoneUIQtButtonsController::testGetButtonFlags()
{
    setFlags(true,false,false,false);
    QVERIFY((m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold) == true &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Ihf) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Wired) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Btaa) == false) == true);
    
    setFlags(false,true,false,false);
    QVERIFY((m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Ihf) == true &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Wired) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Btaa) == false) == true);
    
    setFlags(false,false,true,false);
    QVERIFY((m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Ihf) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Wired) == true &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Btaa) == false) == true);

    setFlags(false,false,false,true);
    QVERIFY((m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Ihf) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Wired) == false &&
    m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Btaa) == true) == true);
}

bool TestPhoneUIQtButtonsController::checkFlags(bool hold, bool ihf, bool wired, bool btaa)
{
    return (hold == m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Hold) &&
             ihf == m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Ihf) &&
           wired == m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Wired) &&
            btaa == m_buttonsCtrl->getButtonFlags(PhoneUIQtButtonsController::Btaa) );
}

void TestPhoneUIQtButtonsController::setFlags(bool hold, bool ihf, bool wired, bool btaa)
{
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Hold,hold);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Ihf,ihf);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Wired,wired);
    m_buttonsCtrl->setButtonFlags(PhoneUIQtButtonsController::Btaa,btaa);    
}

PHONE_TEST_MAIN(TestPhoneUIQtButtonsController)
#include "unit_tests.moc"
