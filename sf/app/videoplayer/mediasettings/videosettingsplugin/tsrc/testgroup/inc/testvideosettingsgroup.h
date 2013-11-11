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
* Description:   TestVideoSettingsGroup class definition
* 
*/

#ifndef TESTVIDEOSETTINGSGROUP_H
#define TESTVIDEOSETTINGSGROUP_H


// INCLUDES
#include <qobject.h>
#include <qabstractitemmodel.h>

class CpItemDataHelper;
class VideoSettingsGroup;

class TestVideoSettingsGroup : public QObject
	{
    
    Q_OBJECT

signals:
    
    void testTextSignal();
    void testIntSignal(int);
    void testShownSignal(const QModelIndex&);
    
private slots:
    
    /**
     * called at the very beginning of the test
     */
    void initTestCase();
    
    /**
     * called at the very end of the test
     */
    void cleanupTestCase();
    
    /**
     * called at the start of every test.
     */
    void init();
    
    /**
     * called at the end of every test.
     */
    void cleanup();
    
    /**
     * tests mSettingsModel construction.
     */
    void testSettingsConstruction();

    /**
     * tests mAccessPointItem construction.
     */
    void testAccessPointConstruction();
    
    /**
     * tests mLowestUDPPortItem construction.
     */
    void testLowestUdpPortConstruction();
    
    /**
     * tests mHighestUDPPortItem construction.
     */
    void testHighestUdpPortConstruction();
    
    /**
     * tests mUseProxyItem construction.
     */
    void testUseProxyConstruction();
    
    /**
     * tests mProxyServerItem construction.
     */
    void testProxyServerConstruction();
    
    /**
     * tests mProxyPortItem construction.
     */
    void testProxyPortConstruction();
    
    /**
     * tests destructor
     */
    void testDestructor();
    
    /**
     * tests getAccessPointId
     */
    void testGetAccessPointId();
    
    /**
     * tests setAccessPointId
     */
    void testSetAccessPointId();
    
    /**
     * tests lowestUdpPortEditingFinished
     */
    void testLowestUdpPortEditingFinished();
    
    /**
     * tests highestUdpPortEditingFinished
     */
    void testHighestUdpPortEditingFinished();
    
    /**
     * tests useProxyToggled
     */
    void testUseProxyToggled();
    
    /**
     * tests proxyServerEditingFinished
     */
    void testProxyServerEditingFinished();
    
    /**
     * tests proxyPortEditingFinished
     */
    void testProxyPortEditingFinished();
    
    /**
     * tests itemShown
     */
    void testItemShown();
    
private:
    
    CpItemDataHelper* mItemHelper;
    
    VideoSettingsGroup* mTestObject;
    
    };

#endif  // TESTVIDEOSETTINGSGROUP_H

// End of File
