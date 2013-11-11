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
* Description:  Test class for hsapplicationlauncher
*
*/

#include <QtTest/QtTest>

/**
* @test Test class for homescreen::hsapplicationlauncher modules.
*/
class t_hsapplicationlauncher : public QObject
{
    Q_OBJECT

private slots:

    /**
     * initialize
     */
    void initTestCase();
    void cleanupTestCase();

    /**
     * hsapplicationlauncher.cpp
     */
    void testActivate();
    void testRunL();
    void testInitProcessMonitorL();
    void testDoCancel();
    void testShutdownApp();
    void testRunError();
    void testListeningLanguageSelection();
    void testHandleNotifyInt();
    void testStartHomeScreen();
    void testProcessEnded();
    void testProcessMonitoringError();
    void testSystemShutdownEvent();
    void testIsLanguageSelectedL();
    
    /**
     * hsprocessmonitor.cpp
     */
    void testProcessMonitorRunL();
    void testProcessMonitorDoCancel();
    void testProcessMonitorRunError();
    
    /**
	 * hsshutdownmonitor.cpp
	 */
	 void testShutDownMonitorRunL();
	 void testShutDownMonitorDoCancel();
	 void testShutDownMonitorRunError();


private:    
};
