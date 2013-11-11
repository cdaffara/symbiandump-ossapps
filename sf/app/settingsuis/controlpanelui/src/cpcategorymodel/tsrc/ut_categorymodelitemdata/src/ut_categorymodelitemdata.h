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

#ifndef UT_CATEGORYMODELITEMDATA_H_ 
#define UT_CATEGORYMODELITEMDATA_H_

#include <QObject>
class TestCpCategorySettingFormItem :public QObject
{
    Q_OBJECT
private slots:
    
    void initTestCase();
    
    void testFirstConstructor(); // test the constructor
    void testSecondConstructor(); // test the constructor
    void testInit(); // test the initialize function

    void cleanupTestCase();    
};
#endif /* UT_CATEGORYMODELITEMDATA_H_ */
