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
* Description:   TestVideoSettingsAccessPointEntry class definition
* 
*/

#ifndef TESTVIDEOSETTINGSACCESSPOINTENTRY_H
#define TESTVIDEOSETTINGSACCESSPOINTENTRY_H


// INCLUDES
#include <qobject.h>

class CpItemDataHelper;
class VideoSettingsAccessPointEntry;
class VideoSettingsGroup;

class TestVideoSettingsAccessPointEntry : public QObject
	{
    
    Q_OBJECT

signals:
    
    void testSignal();
    
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
     * tests constructor
     */
    void testConstructor();
    
    /**
     * tests destructor
     */
    void testDestructor();

    /**
     * tests setIapId
     */
    void testSetIapId();
    
    /**
     * tests createSettingsView
     */
    void testCreateSettingView();

    /**
     * tests openSelectionDialogSlot
     */
    void testOpenSelectionDialogSlot();

    /**
     * tests accessPointDialogFinished
     */
    void testAccessPointDialogFinished();

private:
    
    CpItemDataHelper* mItemHelper;
    
    VideoSettingsGroup* mGroup;
    
    VideoSettingsAccessPointEntry* mTestObject;
    
    };

#endif  // TESTVIDEOSETTINGSACCESSPOINTENTRY_H

// End of File
