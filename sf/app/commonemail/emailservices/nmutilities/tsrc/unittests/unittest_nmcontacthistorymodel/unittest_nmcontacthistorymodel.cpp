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

#include "nmcontacthistorymodel.h"
#include "nmcontacthistorymodel_p.h"
#include "unittest_nmcontacthistorymodel.h"

bool constructorCalled;
bool destructorCalled;
bool rowcountCalled;
bool dataCalled;
bool modelcompletedCalled;
bool queryCalled;
bool refreshdatamodelCalled;
bool handlequerycompletedCalled;

/*!
     Constructor
 */
TestNmContactHistoryModel::TestNmContactHistoryModel()
{

}

/*!
     Destructor
 */
TestNmContactHistoryModel::~TestNmContactHistoryModel()
{

}

/*!
     Called before testcase
 */
void TestNmContactHistoryModel::initTestCase()
{
}

/*!
     Called after testcase
 */
void TestNmContactHistoryModel::cleanupTestCase()
{
}

/*!
     Called before every function
 */
void TestNmContactHistoryModel::init()
{
    constructorCalled = false;
    destructorCalled = false;
    rowcountCalled = false;
    dataCalled = false;
    modelcompletedCalled = false;
    queryCalled = false;
    refreshdatamodelCalled = false;
    handlequerycompletedCalled = false;

    mModel= new NmContactHistoryModel(EmailAddressModel);
    mSlotCalled = false;
}

/*!
     Called after everyfunction
 */
void TestNmContactHistoryModel::cleanup()
{
    delete mModel;
    mModel = NULL;
}

void TestNmContactHistoryModel::testConstructor()
{
    QVERIFY(constructorCalled);
    QVERIFY(mModel->d_ptr != NULL);

    NmContactHistoryModelType modelType = mModel->d_ptr->mType;
    QVERIFY(modelType == EmailAddressModel);
}

void TestNmContactHistoryModel::testDestructor()
{
    delete mModel;
    mModel = NULL;
    QVERIFY(destructorCalled);
}

void TestNmContactHistoryModel::testRowCount()
{
    int rowCount = mModel->rowCount(QModelIndex());
    QVERIFY(rowCount == 0);
    QVERIFY(rowcountCalled);
}

void TestNmContactHistoryModel::testData()
{
    mModel->data(QModelIndex(),0);
    QVERIFY(dataCalled);
}

void TestNmContactHistoryModel::testQuery()
{
    mModel->query("TEST");
    QVERIFY(queryCalled);
}

void TestNmContactHistoryModel::test_modelCompleted(int err)
{
    Q_UNUSED(err);
    mSlotCalled = true;
}

void TestNmContactHistoryModel::testHandleQueryCompleted()
{
    mSlotCalled = false;
    QObject::connect(mModel, SIGNAL(modelCompleted(int)), this, SLOT(test_modelCompleted(int)));
    mModel->d_ptr->mPrivateItemList.append("TEST");
    mModel->handleQueryCompleted(0);
    QVERIFY(rowcountCalled);
    QVERIFY(refreshdatamodelCalled);
    QVERIFY(mSlotCalled);
    
    rowcountCalled = false;
    refreshdatamodelCalled = false;
    mSlotCalled = false;
    
    mModel->d_ptr->mPrivateItemList.clear();
    mModel->handleQueryCompleted(0);    

    QVERIFY(rowcountCalled);
    QVERIFY(refreshdatamodelCalled == FALSE);
    QVERIFY(mSlotCalled);
}

QTEST_MAIN( TestNmContactHistoryModel );

