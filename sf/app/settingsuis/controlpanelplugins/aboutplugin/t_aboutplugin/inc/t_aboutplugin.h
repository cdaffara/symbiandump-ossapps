/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main test class for Control Panel's About plugin 
*
*/

#ifndef T_CPABOUTPLUGIN_H
#define T_CPABOUTPLUGIN_H

#include <QtTest/QtTest>

class CpAboutPlugin;

/**
* TestAboutPlugin
*/
class TestAboutPlugin : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    
    // CpAboutPlugin
    void testCreateSettingFormItemData();
    
    // CpAboutView
    void testCpAboutView();
    void testCpAboutViewOpenThirdPartyView();
    void testCpAboutViewOpenOpenSourceView();
    
    //CpAboutThirdPartyView
    void testCpAboutThirdPartyView();
    
    //CpAboutOpenSourceView
    void testCpAboutOpenSourceView();
          
private:
    CpAboutPlugin *mPlugin; 
    
};

#endif // T_CPABOUTPLUGIN_H
