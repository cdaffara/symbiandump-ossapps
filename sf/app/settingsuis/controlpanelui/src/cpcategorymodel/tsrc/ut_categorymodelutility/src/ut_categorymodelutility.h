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
*       test the functions in cpcategorysettingformmodel class 
*/

#ifndef UT_CATEGORYMODELUTILITY_H_ 
#define UT_CATEGORYMODELUTILITY_H_

#include <QObject>

class TestCpCategoryModelUtility :public QObject
{
    Q_OBJECT
private slots:
    
    void initTestCase();

    void testSetEntryItemReturnFalse(); // test the setEntryItemContentIfEmpty() function can return false
    void testSetEntryItemContent(); // 
    void testDrives();  // test the drives() function
    void testPluginDirectories();  // test the pluginDirectories() function
    void testConfigFileDirectories(); // test the configFileDirectories() function
    void testBuildConfigPluginItems(); // test the buildConfigPluginItems() function
    void testCreateCpPluginItemData();       
    void cleanupTestCase();    
};
#endif /* UT_CATEGORYMODELUTILITY_H_ */
