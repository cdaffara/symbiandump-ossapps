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

#ifndef __TESTLISTVIEW_H__
#define __TESTLISTVIEW_H__


// INCLUDES
#include <QtTest/QtTest>
#include <mpxitemid.h>
#include "hbmessagebox.h"
#include "hbeffect.h"

class VideoListView;
class VideoProxyModelGeneric;
class VideoCollectionWrapper;
class VideoCollectionUiLoader;
class VideoListWidget;
class HbToolBarExtension;

class TestListView : public QObject
{
    Q_OBJECT

    /**
     * Will be called before each testfunction is executed.
     */
    void init(bool initTestView = true); 
     
    /**
     * Returns amount of visible menu actions in the video list view.
     */
    int visibleMenuActions();
    
    /**
     * Set row count in model.
     */
    void setRowCount(int count, VideoProxyModelGeneric *model = 0);
    
    /**
     * Check if action is visible.
     */
    bool isActionVisible(const char *name) const;
    
    // Test functions for the test framework.
    
private slots:

    /**
     * Will be called after every testfunction.
     */
    void cleanup();

	/**
	 * Tests creating and deleting the view.
	 */
	void testCreateDelete();
	
	/**
	 * Tests initializeView.
	 */
	void testInitializeView();
	
    /**
     * Tests createAction.
     */
    void testCreateToolbar();
    
    /**
     * Tests activatView.
     */
    void testActivateView();
    
    /**
     * Tests deactivateView.
     */
    void testDeactivateView();

    /**
     * Tests back.
     */
    void testBack();
    
    /**
     * Tests modelReadySlot.
     */
    void testModelReadySlot();
   
    /**
     * Tests albumListReadySlot();
     */
    void testAlbumListReadySlot();

    /**
     * Tests handleStatusSlot.
     */
    void testHandleStatusSlot();

    /**
     * Tests openAllVideosViewSlot.
     */
    void testOpenAllVideosViewSlot();

    /**
     * Tests openCollectionViewSlot.
     */
    void testOpenCollectionViewSlot();
    
    /**
     * Tests view when orientation changes.
     */
    void testOrientationSlots();
    
    /**
     * Tests collectionOpenedSlot.
     */
    void testCollectionOpenedSlot();

    /**
     * Tests updateSubLabel.
     */
    void testUpdateSubLabel();
    
    /**
     * Tests showHint
     */
    void testShowHint();
    
    /**
     * Tests titleReadySlot
     */
    void testTitleReadySlot();

    /**
     * Tests doDelayedsSlot
     */
    void testDoDelayedsSlot();

    /**
     * Tests aboutToChangeOrientationSlot
     */
    void testAboutToChangeOrientationSlot();

    /**
     * Tests orientationChangedSlot
     */
    void testOrientationChangedSlot();
    
    /**
     * Tests toolbarActionsChanged
     */
    void testToolbarActionsChanged();
    
    /**
     * Tests toolbarExtensionChanged
     */
    void testToolbarExtensionChanged();

signals:

    // Signals needed in tests.
    void testSignal();
    
    void testSignal2();

    void testSignal(int);
    
    void testSignal(const QString &);

    void testLayoutChangedSignal();

    void testCollectionOpenedSignal(bool, const QString&, const TMPXItemId&);

    void testStatusSignal(int, QVariant&);
    
    void testObjectReadySignal(QObject*, const QString);
    
    void testSignal(const QModelIndex &parent, int start, int end);
    
    void testSignal(Qt::Orientation);
    
    void testSignal(const HbEffect::EffectStatus &status);
    
    void testSignal(QList<QAction*>);
    
    void testSignal(HbToolBarExtension*);
    
private:
    
    VideoCollectionUiLoader* mUiLoader;
    
    VideoListView* mTestView;
    
    VideoListWidget* mListWidget;
    
    VideoCollectionWrapper* mWrapper;
};


#endif  // __TESTCOLLECTIONVIEW_H__

// End of file
    
