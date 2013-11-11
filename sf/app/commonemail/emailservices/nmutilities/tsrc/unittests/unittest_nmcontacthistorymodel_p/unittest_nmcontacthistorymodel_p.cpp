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

#include "qtcontacts.h"
#include "nmcontacthistorymodel.h"

#include "nmcontacthistorymodel_p.h"
#include "unittest_nmcontacthistorymodel_p.h"

Q_DEFINE_LATIN1_CONSTANT(QContactEmailAddress::FieldEmailAddress, "EmailAddress");
Q_DEFINE_LATIN1_CONSTANT(QContactName::DefinitionName, "Name");
Q_DEFINE_LATIN1_CONSTANT(QContactEmailAddress::DefinitionName, "EmailAddress");
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldFirstName, "FirstName");
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldLastName, "LastName");

bool constructorCalled;
bool destructorCalled;
bool entryCountCalled;
bool getEntryCalled;
bool nameOrderCalled;

bool setDetailDefinitionNameCalled;
bool setMatchFlagsCalled;
bool setValueCalled;
bool ContactIdsCalled;

bool fillMruWithFalseValues;
bool returnZeroEmails;

bool returnFalseFromGetEntry;

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestNmContactHistoryModelPrivate::initTestCase()
{
    // Put some allied forces operation names in to array for testing.
    mTestNames << "Oatmeal" << "Olive" << "Olympic" << "Open Door" << "Opium" <<
    "Orange" << "Oration" << "Outflank" << "Overlord" << "Overthrow";
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestNmContactHistoryModelPrivate::init()
{
    constructorCalled = false;
    destructorCalled = false;
    entryCountCalled = false;
    getEntryCalled = false;
    nameOrderCalled = false;

    setDetailDefinitionNameCalled = false;
    setMatchFlagsCalled = false;
    setValueCalled = false;
    ContactIdsCalled = false;

    fillMruWithFalseValues = false;
    returnZeroEmails = false;

    returnFalseFromGetEntry = false;
    
    mTestObject = 0;
    mSignalEmitted = false;
    mTestObject = new NmContactHistoryModelPrivate(EmailAddressModel);

    QVERIFY(mTestObject);

    mTestObject->mModelReady = false;
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestNmContactHistoryModelPrivate::cleanup()
{
    if(mTestObject)
    {
        delete mTestObject;
        mTestObject = 0;
    }
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//

void TestNmContactHistoryModelPrivate::cleanupTestCase()
{
    mTestNames.clear();
}

void TestNmContactHistoryModelPrivate::test_queryMruDatabase() // Ready
{
    /* Email MRU Stub will always return following values:
       (if fillMruWithFalseValue == true)

    displayName = ""; [0]
    address = "camel.case@plusplus.com";

    displayName = "Agent Plus"; [1]
    address = "plus@plusplus.com";

    displayName = "Agent Smith"; [2]
    address = "noreply@mib.com"; */

    mTestObject->mMruMatches.clear();
    mTestObject->mMruList.clear();

    QVERIFY(mTestObject->mMruMatches.count() == 0);

    // Test query with empty string
    fillMruWithFalseValues = true;

    bool rValue = mTestObject->queryMruDatabase(QString());

    QVERIFY( entryCountCalled );
    QVERIFY( getEntryCalled );
    QVERIFY( rValue );

    //TODO: Fix the "bug"
    //QVERIFY(mTestObject->mMruMatches.count() == 0);

    // Test query with no display name
    // and using the start of the email as display name
    mTestObject->mMruMatches.clear();
    mTestObject->mMruList.clear();

    rValue = mTestObject->queryMruDatabase("Camel");
    QVERIFY( rValue );

    rValue = mTestObject->mMruMatches.contains("camel.case");
    QVERIFY( rValue );

    QVERIFY(mTestObject->mMruMatches.count() == 1);

    // Test query with match in email

    mTestObject->mMruMatches.clear();
    mTestObject->mMruList.clear();

    rValue = mTestObject->queryMruDatabase("noReply");

    QVERIFY( rValue );
    rValue = mTestObject->mMruMatches.contains("camel.case");
    QVERIFY(mTestObject->mMruMatches.count() == 1);
    
    mTestObject->mType = PhoneNumberModel;
    rValue = mTestObject->queryMruDatabase("");
    
    QVERIFY( rValue == false );
    mTestObject->mType = EmailAddressModel;
}

void TestNmContactHistoryModelPrivate::test_queryContactDatabase() // Ready.
{
    QVERIFY(mTestObject->mModelItemList.count() == 0);
    QVERIFY(mTestObject->mPrivateItemList.count() == 0);

    QList<QContactLocalId> rValues;

    rValues = mTestObject->queryContactDatabase("MATRIX");

    QVERIFY( setDetailDefinitionNameCalled );
    QVERIFY( setMatchFlagsCalled );
    QVERIFY( setValueCalled );
    QVERIFY( ContactIdsCalled );

    QVERIFY( rValues.contains(12) );
    QVERIFY( rValues.contains(24) );
    QVERIFY( rValues.contains(48) );

}

void TestNmContactHistoryModelPrivate::test_populateListWithMruItems() // Ready
{
    QVERIFY(mTestObject->mModelItemList.count() == 0);
    QVERIFY(mTestObject->mPrivateItemList.count() == 0);

    mTestObject->mMruMatches.clear();

    //MRU (key = name, value = address);
    mTestObject->mMruMatches.insert("Test Name","name@test.com");
    mTestObject->mMruMatches.insert("Tset Eman","eman@tset.com");
    mTestObject->mMruMatches.insert("Tset, Tset","test@name.com");

    mTestObject->populateListWithMruItems("Name");

    QVERIFY( mTestObject->mPrivateItemList.count() == 1 );

    QSharedPointer<NmContactHistoryModelItemData> itemData = mTestObject->mPrivateItemList[0];
    QVERIFY( itemData->mItems[0].mMatchingRanges.count() == 2 );
    QVERIFY( itemData->mItems[1].mMatchingRanges.count() == 2 );

    QString dbgString = itemData->mItems[0].mItemText;

    int markup1Start = itemData->mItems[0].mMatchingRanges[0];
    int markup1Stop = itemData->mItems[0].mMatchingRanges[1];

    int markup2Start = itemData->mItems[1].mMatchingRanges[0];
    int markup2Stop = itemData->mItems[1].mMatchingRanges[1];

    QVERIFY( markup1Start == 5 );
    QVERIFY( markup1Stop == 8 );

    QVERIFY( markup2Start == 0 );
    QVERIFY( markup2Stop == 3 );

    // Second Test (Double markup in the name + no email match)
    mTestObject->mPrivateItemList.clear();
    mTestObject->populateListWithMruItems("Tset");

    QVERIFY( mTestObject->mPrivateItemList.count() == 2 );

    itemData = mTestObject->mPrivateItemList[1];
    QVERIFY( itemData->mItems[0].mMatchingRanges.count() == 4 );
    QVERIFY( itemData->mItems[1].mMatchingRanges.count() == 0 );

    dbgString = itemData->mItems[0].mItemText;

    markup1Start = itemData->mItems[0].mMatchingRanges[0];
    markup1Stop = itemData->mItems[0].mMatchingRanges[1];

    markup2Start = itemData->mItems[0].mMatchingRanges[2];
    markup2Stop = itemData->mItems[0].mMatchingRanges[3];

    QVERIFY( markup1Start == 0 );
    QVERIFY( markup1Stop == 3 );

    QVERIFY( markup2Start == 6 );
    QVERIFY( markup2Stop == 9 );

    mTestObject->mMruMatches.clear();
    mTestObject->mPrivateItemList.clear();
    mTestObject->mMruMatches.insert("Name","name@test.com");   
    mTestObject->mMruMatches.insert("Tset Eman","eman@tset.com");
    
    mTestObject->populateListWithMruItems("Name");

    mTestObject->mPrivateItemList.clear();    
    mTestObject->populateListWithMruItems("Name");    
}

void TestNmContactHistoryModelPrivate::test_populateListWithContactItems() // Ready
{
    QVERIFY(mTestObject->mModelItemList.count() == 0);
    QVERIFY(mTestObject->mPrivateItemList.count() == 0);

    QList<QContactLocalId> cnt_ids;
    cnt_ids.append(12);

    // Match in first name.
    mTestObject->populateListWithContactItems(cnt_ids, "First");
    QVERIFY( mTestObject->mPrivateItemList.count() == 1 );
    QSharedPointer<NmContactHistoryModelItemData> itemData = mTestObject->mPrivateItemList[0];
    QVERIFY( itemData->mItems[0].mMatchingRanges.count() == 2 );
    QString dbgString = itemData->mItems[0].mItemText;

    int markup1Start = itemData->mItems[0].mMatchingRanges[0];
    int markup1Stop = itemData->mItems[0].mMatchingRanges[1];

    QVERIFY( markup1Start == 0 );
    QVERIFY( markup1Stop == 4 );

    mTestObject->mPrivateItemList.clear();

    // Match in last name.
    mTestObject->populateListWithContactItems(cnt_ids, "Last");

    QVERIFY( mTestObject->mPrivateItemList.count() == 1 );
    itemData = mTestObject->mPrivateItemList[0];
    QVERIFY( itemData->mItems[0].mMatchingRanges.count() == 2 );
    dbgString = itemData->mItems[0].mItemText;

    markup1Start = itemData->mItems[0].mMatchingRanges[0];
    markup1Stop = itemData->mItems[0].mMatchingRanges[1];

    QVERIFY( markup1Start == 10 );
    QVERIFY( markup1Stop == 13 );

    mTestObject->mPrivateItemList.clear();

    // Match in email.
    mTestObject->populateListWithContactItems(cnt_ids, "Email");
    QVERIFY( mTestObject->mPrivateItemList.count() == 1 );
    itemData = mTestObject->mPrivateItemList[0];
    QVERIFY( itemData->mItems[1].mMatchingRanges.count() == 2 );
    dbgString = itemData->mItems[1].mItemText;

    markup1Start = itemData->mItems[1].mMatchingRanges[0];
    markup1Stop = itemData->mItems[1].mMatchingRanges[1];

    QVERIFY( markup1Start == 0 );
    QVERIFY( markup1Stop == 4 );

    mTestObject->mPrivateItemList.clear();

    // No emails.
    returnZeroEmails = true;
    mTestObject->populateListWithContactItems(cnt_ids, "First");
    QVERIFY( mTestObject->mPrivateItemList.count() == 0 );
}

void TestNmContactHistoryModelPrivate::test_queryDatabases() // Ready
{
    mSignalEmitted = false;
    QObject::connect(mTestObject, SIGNAL(queryCompleted(int)), this, SLOT(test_handleQueryCompleted(int)));

    mTestObject->queryDatabases("Agent");

    QVERIFY( mSignalEmitted );

    QVERIFY( mTestObject->mMruMatches.count() == 1 );
    QVERIFY( mTestObject->mPrivateItemList.count() == 1 );

    QVERIFY( constructorCalled );
    QVERIFY( destructorCalled );
    QVERIFY( entryCountCalled );
    QVERIFY( getEntryCalled );
    QVERIFY( nameOrderCalled );

    QVERIFY( setDetailDefinitionNameCalled );
    QVERIFY( setMatchFlagsCalled );
    QVERIFY( setValueCalled );
    QVERIFY( ContactIdsCalled );
    
    mTestObject->queryDatabases("Agent Orange");    
}

void TestNmContactHistoryModelPrivate::test_refreshDataModel() // Ready
{
    mTestObject->mPrivateItemList.clear();

    QVERIFY(mTestObject->mModelItemList.count() == 0);
    QVERIFY(mTestObject->mPrivateItemList.count() == 0);

    QSharedPointer<NmContactHistoryModelItemData> itemData (new NmContactHistoryModelItemData);

    // For Display name (ex. John Doe)
    NmContactHistoryModelSubItem itemSubItem1;
    // For Email address (ex. john.doe@company.com)
    NmContactHistoryModelSubItem itemSubItem2;

    itemSubItem1.mItemText = "Test Name";
    itemSubItem2.mItemText = "test@name.com";

    itemSubItem1.mMatchingRanges.append(0);
    itemSubItem1.mMatchingRanges.append(itemSubItem1.mItemText.length()-1);

    itemSubItem2.mMatchingRanges.append(0);
    itemSubItem2.mMatchingRanges.append(itemSubItem2.mItemText.length()-1);

    itemData->mContactId = 12;
    itemData->mItems.append(itemSubItem1);
    itemData->mItems.append(itemSubItem2);

    mTestObject->mPrivateItemList.append(itemData);

    mTestObject->refreshDataModel();

    QVERIFY( mTestObject->mModelReady );

    QVERIFY(mTestObject->mPrivateItemList.count() == 1);
    QVERIFY(mTestObject->mModelItemList.count() == 1);

    QSharedPointer<NmContactHistoryModelItemData> testData1
    (mTestObject->mPrivateItemList[0]);

    NmContactHistoryModelItem testData2 = mTestObject->mModelItemList[0];

    QVERIFY(testData1->mContactId == testData2.contactId());
    QVERIFY(testData2.subItemCount() == 2);

    NmContactHistoryModelSubItem testItem1 = testData2.subItemAt(0);
    NmContactHistoryModelSubItem testItem2 = testData2.subItemAt(1);

    QVERIFY( testItem1.mItemText == itemSubItem1.mItemText );
    QVERIFY( testItem2.mItemText == itemSubItem2.mItemText );

    QVERIFY( testItem1.mMatchingRanges.count() == 2 );
    
    // Test NmContactHistoryModelItem untested functions here.
    NmContactHistoryModelItem modelItem;
    NmContactHistoryModelSubItem subItem1;
    NmContactHistoryModelSubItem subItem2;
    
    subItem1.mItemText = "Test Object";
    subItem2.mItemText = "test.object@cplusplus.com";    

    modelItem.appendSubItem(subItem1);
    modelItem.appendSubItem(subItem2);
    
    NmContactHistoryModelSubItem testItem3 = modelItem.subItemAt(0xBEEF);
    QVERIFY( testItem3.mItemText.isEmpty() );
    QVERIFY( testItem3.mMatchingRanges.count() == 0 );

    testItem3 = modelItem.subItemAt(0);
    QVERIFY( testItem3.mItemText == "Test Object" );

    QList<NmContactHistoryModelSubItem> testItems;
    testItems = modelItem.subEntries();
    
    QVERIFY( testItems.count() == 2 );    
    testItems.clear();
    
    NmContactHistoryModelItem modelItem2;    
    testItems = modelItem2.subEntries();
    QVERIFY( testItems.count() == 0 );
}

void TestNmContactHistoryModelPrivate::test_rowCount() // Ready
{
    QVERIFY(mTestObject->mModelItemList.count() == 0);

    NmContactHistoryModelItem item;
    item.setContactId(0);
    mTestObject->mModelItemList.append(item);

    QModelIndex testIndex = QModelIndex();
    int rowCount = mTestObject->modelRowCount(testIndex);
    QVERIFY(rowCount == 1);

    mTestObject->mModelItemList.clear();
    rowCount = mTestObject->modelRowCount(testIndex);
    QVERIFY(mTestObject->mModelItemList.count() == 0);

}

void TestNmContactHistoryModelPrivate::test_privateDataCount() // Ready
{
    mTestObject->mPrivateItemList.clear();
    QVERIFY(mTestObject->mPrivateItemList.count() == 0);
    int rowCount = mTestObject->privateDataCount();


    QList<QContactLocalId> cnt_ids;
    cnt_ids.append(12);

    // Match in first name.
    mTestObject->populateListWithContactItems(cnt_ids, "First");
    QVERIFY( mTestObject->mPrivateItemList.count() == 1 );
    rowCount = mTestObject->privateDataCount();
    QVERIFY(rowCount == 1);  
}


void TestNmContactHistoryModelPrivate::test_dataFunc() // Ready
{
    QModelIndex testIndex = QModelIndex();
      
    mTestObject->mModelReady = false;
    QVariant testVariant = mTestObject->data(testIndex, Qt::DisplayRole);
    QCOMPARE(testVariant, QVariant());

    mTestObject->mModelReady = true;
    testVariant = mTestObject->data(testIndex, Qt::DisplayRole);
    QCOMPARE(testVariant, QVariant());

    for (int i = 0; i < 10; i++)
    {
        NmContactHistoryModelItem item;
        item.setContactId(i);
        mTestObject->mModelItemList.append(item);
    }

    mTestObject->mModelItemList.clear();
}

void TestNmContactHistoryModelPrivate::test_fillMruMatchList() // Ready
{
    mTestObject->mMruList.clear();
    bool success = mTestObject->fillMruMatchList();

    QVERIFY( constructorCalled );
    QVERIFY( destructorCalled );
    QVERIFY( entryCountCalled );
    QVERIFY( getEntryCalled );
    QVERIFY( nameOrderCalled );

    QVERIFY( success );

    QVERIFY(mTestObject->mMruList.count() == 2);

    bool containsValue = mTestObject->mMruList.contains("camel.case@plusplus.com");
    QVERIFY(containsValue);

    containsValue = mTestObject->mMruList.contains("byvalue@plusplus.com");
    QVERIFY( containsValue );
    
    mTestObject->mMruList.clear();
    returnFalseFromGetEntry = true;   
    success = mTestObject->fillMruMatchList();
    
    QVERIFY( success );
    QVERIFY(mTestObject->mMruList.count() == 0); 
}

void TestNmContactHistoryModelPrivate::test_obeyContactOrder() // Ready
{
    QString first = "Test";
    QString last = "Name";

    // LastNameFirstName
    mTestObject->mNameOrder = 0;
    QString resultString = mTestObject->obeyContactOrder(first, last);
    QVERIFY(resultString == "Name Test");

    // LastNameCommaFirstName
    mTestObject->mNameOrder = 1;
    resultString = mTestObject->obeyContactOrder(first, last);
    QVERIFY(resultString == "Name, Test");

    // FirstNameLastName
    mTestObject->mNameOrder = 2;
    resultString = mTestObject->obeyContactOrder(first, last);
    QVERIFY(resultString == "Test Name");

    // Invalid / Default
    mTestObject->mNameOrder = 0xDEAD;
    resultString = mTestObject->obeyContactOrder(first, last);
    QVERIFY(resultString == "Test Name");
}

// Signals

void TestNmContactHistoryModelPrivate::test_queryCompleted() // Ready
{
    mSignalEmitted = false;
    QObject::connect(mTestObject, SIGNAL(queryCompleted(int)), this, SLOT(test_handleQueryCompleted(int)));
    mTestObject->queryDatabases(QString());
    QVERIFY( mSignalEmitted );
}

void TestNmContactHistoryModelPrivate::test_handleQueryCompleted(int err) // Internal
{
   Q_UNUSED(err);
   mSignalEmitted = true;
}

QTEST_MAIN(TestNmContactHistoryModelPrivate)

