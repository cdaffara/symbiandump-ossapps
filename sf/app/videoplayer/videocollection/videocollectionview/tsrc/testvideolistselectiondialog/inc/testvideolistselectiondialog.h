/**
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
* Description:   tester for methods in VideoListSelectionDialog
* 
*/

#ifndef __TESTVIDEOLISTSELECTIONDIALOG_H__
#define __TESTVIDEOLISTSELECTIONDIALOG_H__

// INCLUDES
#include <QtTest/QtTest>

#include <videocollectioncommon.h>

class VideoListSelectionDialog;
class VideoCollectionUiLoader;
class VideoListDataModel;
class VideoProxyModelGeneric;
class VideoListWidget;
class VideoListSelectionDialogTesterHelper;

class TestVideoListSelectionDialog : public QObject
{
    Q_OBJECT

private:
    
    void setRowCount(int count, VideoProxyModelGeneric *proxyModel = 0, VideoListDataModel *model = 0);
    
    // test functions for the test framework

private slots:

    /**
     * called before anything else when test starts
     */
    void initTestCase();

    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();
    
    /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
   
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();
    
    /**
     * Tests constructor and destructor.
     */
    void testConstructDestruct();
    
    /**
     * tests VideoListSelectionDialog::setupContent
     */
    void testSetupContent();
    
    /**
     * verifies failure cases in initDialog
     */
    void testSetupInitFailures();
    
    /**
     * tests VideoListSelectionDialog::exec
     */
    void testExec();

    /**
     * tests finishedSlot
     */
    void testFinishedSlot();
    
    /**
     * tests VideoListSelectionDialog::markAllStateChangedSlot
     */
    void testMarkAllStateChangedSlot();

    /**
     * tests VideoListSelectionDialog::selectionChangedSlot
     */
    void testSelectionChangedSlot();

    /**
     * tests VideoListSelectionDialog::singleItemSelectedSlot
     */
    void testSingleItemSelectedSlot();

    /**
     * tests VideoListSelectionDialog::initDialog
     */
    void testModelReadySlot();

    /**
     * tests VideoListSelectionDialog::updateCounterSlot
     */
    void testUpdateCounterSlot();

    /**
     * tests VideoListSelectionDialog::primaryActionTriggeredSlot
     */
    void testPrimaryActionTriggeredSlot();
    
    /**
     * tests VideoListSelectionDialog::getSelectedName
     */
    void testGetSelectedName();
    
private:

    /**
     * test helper
     */
    VideoListSelectionDialogTesterHelper *mTestHelper;  
    
    /**
     * object under test
     */
    VideoListSelectionDialog *mTestObject;
    
    /**
     * dummy UI loader object
     */
    VideoCollectionUiLoader *mTestUiLoader;
    
    /**
     * dummy source model
     */
    VideoListDataModel *mSourceModel;
    
    /**
     * dummy model
     */
    VideoProxyModelGeneric *mModel;
    
    /**
     * dummy widget
     */
    VideoListWidget *mTestWidget;
    
	/**
 	 * flag indicating if last call to init succeeded.
 	 */
    bool mInitOk;

};


#endif  // __TESTVIDEOCOLLECTIONVIEWUTILS_H__

// End of file
    


