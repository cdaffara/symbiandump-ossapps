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
 *   
 */

#ifndef UT_CPKEYSCREENMODEL_H_
#define UT_CPKEYSCREENMODEL_H_

/*!
    \class TestCpKeyScreenModel \n
    \brief
      class name: CpKeyScreenModel \n
      class's description: control key screen settings\n
      type of test case: unit test\n 
      test cases' number totally: 13\n
 */
#include <QObject>
class CpKeyScreenModel;

class TestCpKeyScreen:public QObject
{
    Q_OBJECT
private slots:
    // init function
    void initTestCase();
    
    // CpKeyScreenModel
    void TestConstructorAndDestructor();
    
    void TestKeyguardSupported();
    void TestScreensaverSupported();
    void TestBrightnessSupported();
    void TestRotateSupported();
    
    void TestSetAndGetKeyGuard();
    void TestSetAndGetKeyGuardWithInvalidInput();
    
    void TestSetAndGetRotateWithTrue();
    void TestSetAndGetRotateWithFalse();
    
    void TestSetAndGetBrightness();
    void TestSetAndGetBrightnessWithInvalidInput();
    
    void TestSetAndGetScreenSaverWithTrue();
    void TestSetAndGetScreenSaverWithFalse();
    
    // CpKeyScreenPlugin
    void TestPluginConstructorAndDestructor();
    void TestPluginInterface();
    
    //CpKeyScreenView
    void TestViewConstructorAndDestructor();
    
    void cleanupTestCase();
private:
    CpKeyScreenModel *mModel;
};
#endif /* UT_CPKEYSCREENMODEL_H_ */
