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

#include "ut_categorymodelitemdata.h"
#include "cpcategorysettingformitemdata.h"
#include <QtTest/QtTest>
#include <cpitemdatahelper.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <HbView.h>

/*!
      \class TestCpCategorySettingFormItem \n
      \brief class name: TestCpCategorySettingFormItem \n
      type of test case: unit test  \n
      test cases' number totally: 3 \n
 */

void TestCpCategorySettingFormItem::initTestCase()
{
    // initialize public test data here
}

/*!
     Test Case Description:  \n
     1. Fucntion Name:  \n &nbsp;&nbsp;
     CpCategorySettingFormItemData(HbDataFormModelItem::DataItemType type,
                                                      const QString &label,
                                                      const QString &configFile,
                                                      const HbDataFormModelItem *parent = 0) \n
     2. Case Descrition: Test the first constructor function. \n
     3. Input Parameters:  \n &nbsp;&nbsp;
     (1) type = HbDataFormModelItem::DataItemType,
                              label = QString("XX"), 
                              configFile = QString("XX"), 
                              parent = new HbDataFormModelItem() \n &nbsp;&nbsp;
     (2) type = HbDataFormModelItem::DataItemType,
                              label = QString(), 
                              configFile = QString(), 
                              parent = new HbDataFormModelItem()\n &nbsp;&nbsp;
     4. Expected result:  \n &nbsp;&nbsp;
     (1) no crash\n &nbsp;&nbsp;
     (2) no crash \n
 */
void TestCpCategorySettingFormItem::testFirstConstructor()
{
    HbDataFormModelItem::DataItemType aType = HbDataFormModelItem::CheckBoxItem;
    QString aLabel = "testLabel";
    QString configFile = "testFile";
    HbDataFormModelItem *pParent = new HbDataFormModelItem();
    
    CpCategorySettingFormItemData *pCategoryItemData = new CpCategorySettingFormItemData(aType, aLabel, configFile, pParent);
    QVERIFY(pCategoryItemData != 0 ); 
    QVERIFY(pCategoryItemData->type()== aType);
    delete pParent;
    
    pCategoryItemData = new CpCategorySettingFormItemData(aType, QString(), QString(), 0);
    QVERIFY(pCategoryItemData != 0 ); 
    QVERIFY(pCategoryItemData->type()== aType);
    delete pCategoryItemData;
}

/*!
     Test Case Description: \n 
     1. Fucntion Name: \n &nbsp;&nbsp;
        explicit CpCategorySettingFormItemData(const QString &configFile = QString(), const HbDataFormModelItem *parent = 0);
     2. Case Descrition: \n &nbsp;&nbsp;
        Test the second constructor function. \n 
     3. Input Parameters: \n &nbsp;&nbsp;
     (1) configFile = QString("XX"), *parent = new HbDataFormModelItem()\n &nbsp;&nbsp;
     (2) configFile = QString(""), *parent = new HbDataFormModelItem() \n
     4. Expected result:  \n &nbsp;&nbsp;
         (1) no crash \n &nbsp;&nbsp;
         (2) no crash
 */
void TestCpCategorySettingFormItem::testSecondConstructor()
{
    QString configFile = "testFile";
    HbDataFormModelItem *pParent = new HbDataFormModelItem();
    CpCategorySettingFormItemData *pCategoryItemData = new CpCategorySettingFormItemData(configFile, pParent);
    delete pParent;
    
    pCategoryItemData = new CpCategorySettingFormItemData(QString(), 0);
    QVERIFY(pCategoryItemData != 0 ); 
    delete pCategoryItemData;
    pCategoryItemData = 0;
    
    pCategoryItemData = new CpCategorySettingFormItemData();
    QVERIFY(pCategoryItemData != 0 ); 
    delete pCategoryItemData;
}

/*!
     Test Case Description: \n
     1. Fucntion Name: void initialize(CpItemDataHelper &itemDataHelper); \n
     2. Case Descrition: Test the initialize() function. \n
     3. Input Parameters: (1) itemDataHelper = new itemDataHelper() \n
     4. Expected result:  (1) no crash \n
 */
void TestCpCategorySettingFormItem::testInit()
{
    CpItemDataHelper *itemDataHelper = new CpItemDataHelper();
    QString configFile = "pluginfileformodelitemdata.cpcfg";
    HbDataFormModelItem *pParent = new HbDataFormModelItem();
    CpCategorySettingFormItemData *pCategoryItemData = new CpCategorySettingFormItemData(configFile, pParent);
    QVERIFY(pCategoryItemData != 0 ); 
    // mInitialized = FALSE
    pCategoryItemData->initialize(*itemDataHelper);    
    // call initialize() again in order to test another branch (mInitialized = TRUE)
    pCategoryItemData->initialize(*itemDataHelper);
    delete itemDataHelper;
    delete pParent;
}

void TestCpCategorySettingFormItem::cleanupTestCase()
{
    // release all test data
    QCoreApplication::processEvents();
}

QTEST_MAIN(TestCpCategorySettingFormItem)
//QTEST_APPLESS_MAIN(TestCpCategorySettingFormItem)
