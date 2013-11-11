/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*       test application for qt cpcategorymodel functions.
*/

#include "ut_cpcategorysettingformmodel.h"

#include <QtTest/QtTest>
#include <cpitemdatahelper.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <HbView.h>
//the class for test
#include "cpcategorysettingformmodel.h"
/*!
    \class TestCpCategorySettingFormModel 
    \brief describe the test case's goal, like: \n
      class name: cpcategorysettingformmodel \n
      class's description \n
      type of test case: unit test \n
      test cases' number totally: 2\n
 */

void TestCpCategorySettingFormModel::initTestCase()
{
    // initialize public test data here
}

/*!
     Test Case Description:\n 
     1. Fucntion Name:  explicit CpCategorySettingFormModel(const QString &configFile);\n
     2. Case Descrition: test the constructor function. \n
     3. Input Parameters: \n &nbsp;&nbsp;
      (1) configFile = QString("pluginfileforformmodel.cpcfg")\n &nbsp;&nbsp;
      (2) configFile = QString()\n
      (3) conFile = QString("invaildConfigFile")\n
4. Expected result:  \n &nbsp;&nbsp;
    (1) no crash\n &nbsp;&nbsp;
    (2) no crash\n &nbsp;&nbsp;
    (3) no crash \n
 */
void TestCpCategorySettingFormModel::testConstructor()
{
    CpCategorySettingFormModel *testCategoryModel = new CpCategorySettingFormModel(QString("pluginfileforformmodel.cpcfg"));
    QVERIFY(testCategoryModel!=0);
    delete testCategoryModel;
    testCategoryModel = 0;
    testCategoryModel = new CpCategorySettingFormModel(QString());
    QVERIFY(testCategoryModel!=0);
    delete testCategoryModel;
    testCategoryModel = 0;
    testCategoryModel = new CpCategorySettingFormModel(QString("invaildConfigFile"));
    QVERIFY(testCategoryModel!=0);
    delete testCategoryModel;
    testCategoryModel = 0;
}

/*!
     Test Case Description: \n
     1. Fucntion Name:  virtual void initialize(CpItemDataHelper &itemDataHelper)\n
     2. Case Descrition: Verify that it can initialize the new categoryformmodel. \n
     3. Input Parameters: (1) itemDataHelper = CpItemDataHelper\n
     4. Expected result:  (1) no crash\n
 */
void TestCpCategorySettingFormModel::testInit()
{
    CpCategorySettingFormModel *testCategoryModel = new CpCategorySettingFormModel(QString("pluginfileforformmodel.cpcfg"));
    CpItemDataHelper *pHelper = new CpItemDataHelper();
    testCategoryModel->initialize(*pHelper);   
    // set mInitialized = TRUE
    testCategoryModel->initialize(*pHelper);    
    delete testCategoryModel;
    testCategoryModel = 0;
    delete pHelper;
    pHelper = 0;
}

/*!
    Release all the test data.
 */
void TestCpCategorySettingFormModel::cleanupTestCase()
{
    // release all test data
    QCoreApplication::processEvents();
}

QTEST_MAIN(TestCpCategorySettingFormModel)
//QTEST_APPLESS_MAIN(TestCpCategorySettingFormModel)
