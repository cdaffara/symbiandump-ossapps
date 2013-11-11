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
* Description:   tester for methods in VideoProxyModelGeneric
* 
*/

#ifndef __TESTCOLLECTIONVIEW_H__
#define __TESTCOLLECTIONVIEW_H__


// INCLUDES
#include <QtTest/QtTest>

class HbMainWindow;
class VideoCollectionViewPlugin;

class TestCollectionView : public QObject
{
    Q_OBJECT

public:

    // test functions for the test framework
private slots:

    /**
     * call to init a testcase
     */
    void init(); 
    
    /**
     * call to cleanup a testcase
     */
    void cleanup();
    
    /**
     * called before anything else when test starts
     */
    void initTestCase();
     
    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();

    void testCreateView();
    void testActivateView();
    void testDeactivateView();
    void testDestroyView();
    void testBack();
    void testOrientationChange();
    void testTimerEvent();
    void testGetView();

signals:

    void commandSignal(int);
    
private:
    
    VideoCollectionViewPlugin* mTestView;
    
};


#endif  // __TESTCOLLECTIONVIEW_H__

// End of file
    


