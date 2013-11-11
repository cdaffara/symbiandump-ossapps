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
* Description:   tester for methods in VideoListToolbar
* 
*/

#ifndef __TESTLISTTOOLBAR_H__
#define __TESTLISTTOOLBAR_H__


// INCLUDES
#include <QtTest/QtTest>
#include <qlist.h>

class VideoListToolbar;
class VideoListView;
class VideoCollectionUiLoader;
class VideoProxyModelGeneric;
class QAction;
class HbToolBarExtension;

class TestListToolbar : public QObject
{
    Q_OBJECT

private slots:

    /**
     * called automatically before anything else when test starts
     */
    void initTestCase();
    
    /**
     * will be called automatically before each testfunction is executed.     
     */
    void init(); 
    
    /**
     * will be called automatically after every testfunction.
     */
    void cleanup();
    
    /**
     * will be called automatically after testing ends
     */
    void cleanupTestCase();

	/**
	 * Tests initializing normal toolbar
	 */
    void testInitialize();
    
    /**
     * Test initializing service
     */
    void testInitializeService();
    
    /**
     * Test viewStateChanged -slot,
     * No service extension, stand alone
     */
    void testViewStateChanged();
    
    /**
     * Test viewStateChanged -slot,
     * service application
     */
    void testViewStateChangedService();
    
    /**
     * Test viewStateChanged -slot,
     * with service extension, stand alone
     */
    void testViewStateChangedWithServiceExt();     
    
    /**
     * Tests addVideosToCollectionSlot
     */
    void testAddVideosToCollectionSlot();
    
    /**
     * Tests removeVideosFromCollectionSlot
     */
    void testRemoveVideosFromCollectionSlot();

private slots:
    
    /**
     * slot can be conected to VideoListToolbar::actionsChanged signal,
     * will save provided qlist into mLastChangedActions
     */
    void testActionsChangedSlot(QList<QAction*>);
    
    /**
     * slot can be conected to VideoListToolbar::toolbarExtensionChanged signal,
     * will save provided HbToolBarExtension into 
     */
    void testToolbarExtensionChangedSlot(HbToolBarExtension*);
   
private:
    
    /**
     * sets provided row count to provided model
     */
    void setRowCount(int count, VideoProxyModelGeneric *model);
    
    /**
     * object under test
     */
    VideoListToolbar *mTestObject;
    
    /**
     * stub listview
     */
    VideoListView *mStubListView;
    
    /**
     * stub -uiloader
     */
    VideoCollectionUiLoader *mStubUiLoader;
    
    /**
     * generic model
     */
    VideoProxyModelGeneric *mStubModel;
    
    /**
     * actions provided to testActionsChangedSlot will be saved here
     */
    QList<QAction*> mLastChangedActions;
    
    /**
     * extension provided to testToolbarExtensionChangedSlot will
     * be saved here
     */
    HbToolBarExtension* mLastChangedExtension;
    
};


#endif  // __TESTCOLLECTIONVIEW_H__

// End of file
    
