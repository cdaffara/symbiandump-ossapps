/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Tests for Onlinehandler class.
 *
 */

#include <QtTest/QtTest>
#include "onlinehandler.h"

/**
 * @test Test class for Onlinehandler modules.
 */
class SearchOnlineHandlerTest : public QObject
    {
Q_OBJECT

public:
    SearchOnlineHandlerTest();
    ~SearchOnlineHandlerTest();

private slots:
    void init();
    void initTestCase();
    void cleanupTestCase();
    void testsetDefaultProvider();
    void testinitializePlugin();
    void testactivatePlugin();
    void testactivatePlugin1();
    void testwriteSettings();
    void testloadISPlugin();
    void testunloadPlugin();
    void testunloadPluginandExit();
    void testbackEvent();

    //IsEngine
    /* 
     void testSelectedProvider();
     void testSetSelectedProvider();
     void testProviders();*///covered in above funtions
    void testPerformWebSearch();
    void testPerformInDeviceSearch();

    //IsProviderdbManager & serviceprovider
    /*   
     void testGetProviders();
     void testGetCountryList();
     void testSelectedProvider();
     void testSetSelectedProvider();//covered in above funtions
     */
    };

