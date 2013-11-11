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

#ifndef UT_CPCATEGORYSETTINGFORMMODEL_H_
#define UT_CPCATEGORYSETTINGFORMMODEL_H_

#include <QObject>
class TestCpCategorySettingFormModel :public QObject
{
    Q_OBJECT
private slots:
    
    void initTestCase();
    
    void testConstructor(); // test the constructor
    void testInit(); // test the initialize() funtion
        
    void cleanupTestCase();    
};
#endif /* UT_CPCATEGORYSETTINGFORMMODEL_H_ */
