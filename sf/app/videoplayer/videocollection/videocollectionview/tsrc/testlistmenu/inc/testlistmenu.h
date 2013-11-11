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
* Description:   tester for methods in VideoListView
* 
*/

#ifndef __TESTLISTMENU_H__
#define __TESTLISTMENU_H__

// INCLUDES
#include <QtTest/QtTest>

class VideoListMenu;
class VideoCollectionUiLoader;
class VideoListView;
class VideoListWidget;
class VideoProxyModelGeneric;
class VideoListDataModel;

class TestListMenu : public QObject
{
    Q_OBJECT

    // Test functions for the test framework.

    /**
     * Default init
     */
    void init();
    
    int visibleMenuActions();
    
    void setVisibilityForMenuActions(bool visibility);
    
private slots:

    /**
     * Will be called after every test function.
     */
    void cleanup();    

	/**
	 * Tests testInitializeMenu.
	 */
	void testInitializeMenu();

    /**
     * Tests testStartSorting.
     */
    void testStartSorting();
    
    /**
     * Tests deleteItemsSlot
     */
    void testDeleteItemsSlot();

    /**
     * Tests createCollectionSlot
     */
    void testCreateCollectionSlot();

    /**
     * Tests addVideosToCollectionSlot
     */
    void testAddVideosToCollectionSlot();
    
    /**
     * Tests aboutToShowMainMenuSlot
     */
    void testAboutToShowMainMenuSlot();
    
    /**
     * Tests prepareBrowseServiceMenu
     */
    void testPrepareBrowseServiceMenu();
    
    /**
     * Tests objectReadySlot
     */
    void testObjectReadySlot();    

signals:

    void testSignal();
    void testSignal2();
    
    void testObjectReadySignal(QObject*, const QString);
    
private:
    
    VideoListMenu *mTestObject;

    VideoCollectionUiLoader* mUiLoader;
    
    VideoListView *mListView;
    
    VideoListWidget *mListWidget;
    
    VideoProxyModelGeneric *mModel;
    
    VideoListDataModel *mDataModel;
};


#endif  // __TESTLISTMENU_H__

// End of file
    
