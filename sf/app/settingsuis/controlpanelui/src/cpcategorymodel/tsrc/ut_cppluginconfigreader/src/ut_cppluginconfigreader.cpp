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
*       test the functions in cppluginconfigreader class 
*/

#include "ut_cppluginconfigreader.h"
#include <QtTest/QtTest>
#include <cppluginconfigreader.h>
#include <cpbasepath.h>
#include <qdir.h>

/*!
      class name: cppluginconfigreader
      class's description: 
      type of test case: unit test 
      test cases' number totally: \n
*/

/*!
     Description of test data \n
 */
void TestCppluginConfigReader::initTestCase()
{
    // initialize public test data here
}

void TestCppluginConfigReader::testConstructor()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "mainview.cpcfg");
    QVERIFY(pReader!=0);
    delete pReader;    
}

/*!
     Test Case Description: \n
     1. Function Name: \n
            QList<CpPluginConfig> readCpPluginConfigs();\n
     2. Case Descrition: \n
         Verify that the valid config file can be read correctly. \n
     3. Input Parameters:\n
         <1> valid configfiles: pluginfortest.cpcfg \n           
     4. Expected result:   \n
         <1> return the correct plugin number \n        
 */
void TestCppluginConfigReader::testReadValidConfigFile()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "pluginfortest.cpcfg");    
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY(mCorrectConfig.count() == 4 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description:  \n
     1. Function Name:  \n
            QList<CpPluginConfig> readCpPluginConfigs(); \n
     2. Case Descrition:  \n
         Verify the empty config file path can be dealed without any crash. \n
     3. Input Parameters: \n
         <1> empty config plugin path \n   
         <2> config plugin path = null   \n     
     4. Expected result:   \n
         <1> no crash   \n
         <2> no crash \n
 */
void TestCppluginConfigReader::testReadEmptyConfigFilePath()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader("");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY(mCorrectConfig.count() == 0 );
    delete pReader;
    pReader = 0;
    pReader = new CpPluginConfigReader(0);
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY(mCorrectConfig.count() == 0 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description: \n 
     1. Function Name:  \n
            QList<CpPluginConfig> readCpPluginConfigs(); \n
     2. Case Descrition:  \n
         Verify that the empty config file could be read. \n
     3. Input Parameters: \n
         <1> empty config file "emptypluginfortest.cpcfg" \n                
     4. Expected result:   \n
         <1> no crash   \n
 */
void TestCppluginConfigReader::testReadEmptyConfigFile()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "emptypluginfortest.cpcfg");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY(mCorrectConfig.count() == 0 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description: \n 
     1. Function Name:  \n
            void readCpPluginConfigs(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList) \n  
     2. Case Descrition:  \n
         Verify that the function can work without crash when reading the plugin files with wrong tag. \n
     3. Input Parameters: \n
         <1> config file with wrong child plugin tag: "pluginfilewithwrongchildtag.cpcfg" \n                
     4. Expected result:   \n
         <1> no crash   \n
 */
void TestCppluginConfigReader::testReaderWithWrongChildPluginTag()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "pluginfilewithwrongchildtag.cpcfg");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY( mCorrectConfig.count() == 0 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description: \n 
     1. Function Name:  \n
            void readChildPluginsElement(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList) \n
     2. Case Descrition:  \n
         Verify that the function can work without crash when reading the plugin files with empty child plugin element. \n
     3. Input Parameters: \n
         <1> config file with empty child plugin element: "pluginfilewithemptychildelement.cpcfg" \n                
     4. Expected result:   \n
         <1> no crash   \n
 */
void TestCppluginConfigReader::testReaderWithEmptyChildElement()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "pluginfilewithemptychildelement.cpcfg");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY( mCorrectConfig.count() == 0 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description: \n 
     1. Function Name:  \n
            void readChildPluginsElement(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList) \n
     2. Case Descrition:  \n
         Verify that the function can work without crash when reading plugin files with wrong tags. \n
     3. Input Parameters: \n
         <1> config file with wrong plugin tag: "pluginfilewithwrongplugintag.cpcfg" \n                
     4. Expected result:   \n
         <1> no crash   \n
 */
void TestCppluginConfigReader::testReaderWithWrongPluginTag()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "pluginfilewithwrongplugintag.cpcfg");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    QVERIFY( mCorrectConfig.count() == 0 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description: \n 
     1. Function Name:  \n
          void readPluginElement(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList); \n
     2. Case Descrition:  \n
         Verify that the function can work without crash when reading the plugin files having no attrs. \n
     3. Input Parameters: \n
         <1> plugin element has no plugin attrs: "pluginfilewithoutattrs.cpcfg" \n  
         <2> plugin element has wrong plugin attrs. \n              
     4. Expected result:   \n
         <1> no crash   \n
         <2> no crash \n
 */
void TestCppluginConfigReader::testReaderWithoutAttrs()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "pluginfilewithoutattrs.cpcfg");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();    
    QVERIFY( mCorrectConfig.count() == 1 );
    delete pReader;
    pReader = 0;
}
/*!
     Test Case Description: \n 
     1. Function Name:  \n
          void readDescElement(QXmlStreamReader &xmlReader,CpPluginConfig &cpPluginConfig) \n
     2. Case Descrition:  \n
         Verify that the function can work without crash when reading the plugin files with wrong tag. \n
     3. Input Parameters: \n
         <1> plugin element has no desc tag: "pluginfilewithwrongdesctag.cpcfg" \n  
         <2> plugin element has wrong desc tag \n              
     4. Expected result:   \n
         <1> no crash   \n
         <2> no crash \n
 */
void TestCppluginConfigReader::testReaderWithWrongDescTag()
{
    CpPluginConfigReader *  pReader = new CpPluginConfigReader(CP_PLUGIN_CONFIG_PATH + QDir::separator() + "pluginfilewithwrongdesctag.cpcfg");
    QVERIFY(pReader!=0);
    mCorrectConfig = pReader->readCpPluginConfigs();
    delete pReader;
    pReader = 0;
}
/*!
    Descrition of what you will do in this function
 */
void TestCppluginConfigReader::cleanupTestCase()
{
    // release all test data
    QCoreApplication::processEvents();
}

QTEST_APPLESS_MAIN(TestCppluginConfigReader)
