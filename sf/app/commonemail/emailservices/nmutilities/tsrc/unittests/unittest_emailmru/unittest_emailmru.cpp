/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  unittests fro emailmru
 *
 */

//#include <XQSettingsManager.h>
//#include <XQCentralRepositoryUtils.h>
//#include <XQCentralRepositorySearchCriteria.h>


//#include "XQSettingsManager.h"
//#include "XQCentralRepositoryUtils.h"
//#include "XQCentralRepositorySearchCriteria.h"

#include "emailmru.h"
#include "unittest_emailmru.h"

/*!
     Constructor
 */
TestEmailMru::TestEmailMru()
{

}

/*!
     Destructor
 */
TestEmailMru::~TestEmailMru()
{

}

/*!
     Called before testcase
 */
void TestEmailMru::initTestCase()
{
}

/*!
     Called after testcase
 */
void TestEmailMru::cleanupTestCase()
{
}

/*!
     Called before every function
 */
void TestEmailMru::init()
{
    iMru = new EmailMRU();
}

/*!
     Called after everyfunction
 */
void TestEmailMru::cleanup()
{
    delete iMru;
    iMru = NULL;
}


/*!
     Test what happens when entry with same address
     but different name is applied to MRU
 */

void TestEmailMru::testUpdateEntry()
{
//    EmailMRU *mru = new EmailMRU();
    iMru->reset();
    bool success;

    QString name = "n1";
    QString addr = "a1";
    iMru->updateMRU(name, addr);

    QString name2 = "n2";
    QString addr2 = "a2";
    iMru->updateMRU(name2, addr2);

    QString name3 = "n3";
    QString addr3 = "a3";
    iMru->updateMRU(name3, addr3);

    /*
    CenRep Should be:

    "n1" : "a1"
    "n2" : "a2"
    "n3" : "a3";

     */

    // Update A2
    QString name4 = "n4";
    QString addr4 = "a2";
    iMru->updateMRU(name4, addr4);

    /*
    CenRep Should be:

    "n1" : "a1"
    "n3" : "a3"
    "n4" : "a2"

    */

    QString e_name;
    QString e_addr;

    success = iMru->getEntry(1, e_name, e_addr);
    QCOMPARE(e_name, name);
    QCOMPARE(e_addr, addr);
    QCOMPARE(success, true);

    success = iMru->getEntry(2, e_name, e_addr);
    QCOMPARE(e_name, name3);
    QCOMPARE(e_addr, addr3);
    QCOMPARE(success, true);

    success = iMru->getEntry(3, e_name, e_addr);
    QCOMPARE(e_name, name4);
    QCOMPARE(e_addr, addr4);
    QCOMPARE(success, true);

    e_name.clear();
    e_addr.clear();
    QString emptyString;
    emptyString.clear();

    success = iMru->getEntry(4, e_name, e_addr);
    QCOMPARE(e_name, emptyString);
    QCOMPARE(e_addr, emptyString);
    QCOMPARE(success, false);
}

/*!
     Test what happens when the MRU is filled and
     yet still more entries are applied
 */

void TestEmailMru::testUpperLimit()
{
//    EmailMRU *mru = new EmailMRU();
    iMru->reset();

    // first fill the MRU
    for (qint32 i=0; i < emailAddressHistorySize; i++) {
        QString name = "name";
        QString addr = "addr";
        QString num = QString::number(i);
        name.append(num);
        addr.append(num);
        iMru->updateMRU(name, addr);
    }

    // next add one more pair, this should overwrite the
    // first entries
    QString e_name;
    QString e_addr;
    QString e_name_value = "expected_name";
    QString e_addr_value = "expected_address";
    iMru->updateMRU(e_name_value, e_addr_value);

    iMru->getEntry(1, e_name, e_addr);

    // finally compare if the first entries has been
    // replaced
    QCOMPARE(e_name, e_name_value);
    QCOMPARE(e_addr, e_addr_value);
}

/*!
     Provides data for testMruStrings
 */
void TestEmailMru::testMruStrings_data()
{
    QTest::addColumn<QString>("display_name");
    QTest::addColumn<QString>("email_address");

    QTest::newRow("1") << "name1" << "name1.name1@gmail.com";
    QTest::newRow("2") << "name2" << "name.name.name.namenamenmanamnamenmanemane2.name2@gmailmailmailamil.com";
    QTest::newRow("3") << "näMeöÅ3" << "name3@gmail.com";
    QTest::newRow("4") << "name4$&#/\\" << "name4.name4@gmail.com";
}

/*!
     Check that name and address pairs are successfully
     saved into mru database
 */
void TestEmailMru::testMruStrings()
{
    QFETCH(QString, display_name);
    QFETCH(QString, email_address);

//    EmailMRU *mru = new EmailMRU();
    iMru->reset();
    iMru->updateMRU(display_name, email_address);

    QString name;
    QString address;
    iMru->getEntry(1, name, address);

    QCOMPARE(name, display_name);
    QCOMPARE(address, email_address);
}

/*!
    Reset mru database and verify that it is indeed empty
 */
void TestEmailMru::testReset()
{
//    EmailMRU *mru = new EmailMRU();
    iMru->reset();
    bool success;

    quint32 mruCount = iMru->entryCount();

    QString e_name;
    QString e_addr;
    QString name = "n1";
    QString addr = "a1";
    iMru->updateMRU(name, addr);

    mruCount = iMru->entryCount();

    success = iMru->getEntry(1, e_name, e_addr);
    QCOMPARE(e_name, name);
    QCOMPARE(e_addr, addr);
    QCOMPARE(success, true);

    e_name.clear();
    e_addr.clear();
    QString emptyString;
    emptyString.clear();

    iMru->reset();

    mruCount = iMru->entryCount();

    success = iMru->getEntry(1, e_name, e_addr);
    QCOMPARE(e_name, emptyString);
    QCOMPARE(e_addr, emptyString);
    QCOMPARE(success, false);
}

void TestEmailMru::testNameOrder()
{
    int rVal = EmailMRU::nameOrder();
    QVERIFY( rVal == 0 );
}

QTEST_MAIN( TestEmailMru );

