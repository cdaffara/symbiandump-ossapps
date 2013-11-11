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
*        test application for qt cpcategorymodel functions.
*/

#include "ut_categorymodelutility.h"
#include "cptestpluginentryitem.h"
#include <cpsettingformentryitemData.h>
#include "cpcategorysettingformitemdata.h"
#include "cpcategorysettingformmodel.h"
#include "cppluginconfig.h"
#include "cpcategorymodelutility.h"
#include <QtTest/QtTest>
//#include <cpplaceholderitemdata.h>
#include <cppluginconfigreader.h>
#include <cpitemdatahelper.h>
//#include <cpbasepath.h>
//#include <cpevent.h>


//the class for test
#include "cpcategorymodelutility.h"
/*!
      \class TestCpCategoryModelUtility \n
      \brief class name: TestCpCategoryModelUtility \n  
      type of test case: unit test \n
      test cases' number totally: 7 \n
 */

void TestCpCategoryModelUtility::initTestCase()
{
    // initialize public test data here
    
}

/*!
     Test Case Description: \n
     1. Fucntion Name: \n &nbsp;&nbsp;
         static bool setEntryItemContentIfEmpty(CpSettingFormItemData *itemData, 
                                                const QString &displayName,
                                                const QString &description) \n
     2. Case Descrition: \n &nbsp;&nbsp;
         Test the function when setting itemData as 0. \n
     3. Input Parameters:\n &nbsp;&nbsp;
         <1> itemData = 0, QString(), QString() \n              
     4. Expected result:  \n &nbsp;&nbsp;
         <1> bSetResult = false \n         
 */
void TestCpCategoryModelUtility::testSetEntryItemReturnFalse()
{
    bool bSetResult = CpCategoryModelUtility::setEntryItemContentIfEmpty(0, QString(), QString());
    QVERIFY(bSetResult == false);
}

/*!
     Test Case Description: \n
     1. Fucntion Name: \n &nbsp;&nbsp;
         static bool setEntryItemContentIfEmpty(CpSettingFormItemData *itemData, 
                                                const QString &displayName,
                                                const QString &description) \n
     2. Case Descrition: \n &nbsp;&nbsp;
         setEntryItemContentIfEmpty \n
     3. Input Parameters:\n &nbsp;&nbsp;
         new a subclass from CpSettingFormEntryItemData firstly. \n &nbsp;&nbsp;
         <1> itemData = new CpTestPluginEntryItemData(CpItemDataHelper, QString(XX), QString(XX),HbIcon(QString(XX))), \n &nbsp;&nbsp; 
             newText = QString(XX), \n &nbsp;&nbsp;
             newDes = QString(XX),\n &nbsp;&nbsp;
             newIconName = QString(XX).\n &nbsp;&nbsp;
         <2> itemData = new CpTestPluginEntryItemData(CpItemDataHelper, QString(),QString(), HbIcon())\n &nbsp;&nbsp;
             newText = QString(XX),\n &nbsp;&nbsp;
             newDes = QString(XX) \n 
     4. Expected result:  \n &nbsp;&nbsp;
         <1> bSetResult = true,\n &nbsp;&nbsp; 
         <2> bSetResult = true,   \n
 */
void TestCpCategoryModelUtility::testSetEntryItemContent()
{
    CpItemDataHelper *pHelper = new CpItemDataHelper();
    QString textNotEmpty = "TextNotEmpty";
    QString description = "descriptionNotEmpty";
    QString iconName = "iconNameNotEmpty";
    HbIcon icon = HbIcon(iconName);
    CpTestPluginEntryItemData *itemData1 = new CpTestPluginEntryItemData(*pHelper, textNotEmpty, description,icon);
    QString newText = "newSetText";
    QString newDes = "newSetdescription";
    bool bSetResult1 = CpCategoryModelUtility::setEntryItemContentIfEmpty(itemData1, newText, newDes);
    QVERIFY(bSetResult1 == true);
    // verify the text, description and icon name cannot be set when they are not empty.
    QVERIFY( itemData1->text() == textNotEmpty );
    QVERIFY( itemData1->description() == description );
    QVERIFY( itemData1->iconName() == iconName );
    delete itemData1;
    
    CpTestPluginEntryItemData *itemData2 = new CpTestPluginEntryItemData(*pHelper, QString(), QString(),HbIcon());
    bool bSetResult2 = CpCategoryModelUtility::setEntryItemContentIfEmpty(itemData2, newText, newDes);
    QVERIFY(bSetResult2 == true);
    // verify the text, description and icon name are set as default when they are empty.
    QVERIFY(itemData2->text() == newText);    
    QVERIFY( itemData2->description() == newDes );
    QString temp = itemData2->iconName();
    QVERIFY( itemData2->iconName() == DEFAULT_ICON_LISTENTRYITEM );
    delete itemData2;
    delete pHelper;  
}

/*!
     Test Case Description: \n &nbsp;&nbsp;
     1. Fucntion Name: drives()\n
     2. Case Descrition: \n &nbsp;&nbsp;
         Verify that it can get all physical drives of the devices \n
     3. Input Parameters: none \n
     4. Expected result:  dirs.count() != 0 \n
 */
void TestCpCategoryModelUtility::testDrives()
{
    QStringList dirs = CpCategoryModelUtility::drives(); 
    // verify the drives "C:, D:, Z:" are contained in the string list.
    QVERIFY( dirs.contains( "C:", Qt::CaseSensitive ) );
    QVERIFY( dirs.contains( "D:", Qt::CaseSensitive ) );
    QVERIFY( dirs.contains( "Z:", Qt::CaseSensitive ) );
}

/*!
     Test Case Description:  \n
     1. Fucntion Name: configFileDirectories() \n
     2. Case Descrition:  \n
         Verify that it can get all config directories of the device \n
     3. Input Parameters: none \n
     4. Expected result:  dirs.count() != 0 \n
 */
void TestCpCategoryModelUtility::testPluginDirectories()
{
    QStringList dirs = CpCategoryModelUtility::pluginDirectories();
    qDebug() << dirs;
    // Verify the right plugin directory path is returned.
    QVERIFY( dirs.contains( QString( "C:/resource/qt/plugins/controlpanel" ) + QDir::separator(), Qt::CaseSensitive ) );
}

/*!
     Test Case Description:  \n
     1. Fucntion Name: configFileDirectories() \n
     2. Case Descrition: Verify that it can get all config directories of the device. \n
     3. Input Parameters: none \n
     4. Expected result:  dirs.count() != 0 \n
 */
void TestCpCategoryModelUtility::testConfigFileDirectories()
{
    QStringList dirs = CpCategoryModelUtility::configFileDirectories();
    qDebug() << dirs;
    // Verify the right config file directory path is returned.
    QVERIFY( dirs.contains( QString( "C:/resource/qt/plugins/controlpanel/config" ) + QDir::separator(), Qt::CaseSensitive ) );
}

/*!
     Test Case Description:  \n &nbsp;&nbsp;
     1. Fucntion Name: buildConfigPluginItems()\n &nbsp;&nbsp;
     2. Case Descrition: Verify that it can load the plugins via the plugin config file and create item data of the corresponding plugin succesfully.\n          
     3. Input Parameters:\n &nbsp;&nbsp;
        <1> parent = HbDataFormModelItem(), configFile = QString(XX), pHelper = new CpItemDataHelper(), startPosition = -10\n &nbsp;&nbsp;
        <2> parent = HbDataFormModelItem(), configFile = QString(), pHelper = new CpItemDataHelper(), startPosition = 1\n

     4. Expected result:  \n &nbsp;&nbsp;
        <1> parent->childCount() == 2 \n 
        <2> parent->childCount() == 2 \n 
 */
void TestCpCategoryModelUtility::testBuildConfigPluginItems()
{
//    HbDataFormModelItem *parent = new HbDataFormModelItem();
//    QString configFile = "configfiletestformodelutility.cpcfg";
//    int startPosition = 1;
//    
//    CpItemDataHelper helper;
//    CpCategoryModelUtility::buildConfigPluginItems(parent, configFile, helper, startPosition);
//    int childCount = parent->childCount();
//    // the configfiletestformodelutility.cpcfg contains two control panel plugins.
//    QVERIFY( parent->childCount() == 2 );
//    
//    CpCategoryModelUtility::buildConfigPluginItems(parent, configFile, helper, -10);
//    QVERIFY( parent->childCount() == 2 );
//
//    delete parent;
}

/*!
     Test Case Description: \n
     1. Fucntion Name: CreateCpPluginItemData() \n
     2. Case Descrition: \n
         
     3. Input Parameters: \n
         
     4. Expected result:  \n
         
 */
void TestCpCategoryModelUtility::testCreateCpPluginItemData()
{
    // 
}

/*!
    Descrition of what you will do in this function
 */
void TestCpCategoryModelUtility::cleanupTestCase()
{
    // release all test data
    QCoreApplication::processEvents();
}

QTEST_MAIN(TestCpCategoryModelUtility)
