/**
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
* Description:   test module for VideoPlaybackStatusPaneControl
* 
*/

// Version : %version:  4 %


#ifndef __TESTSTATUSPANECONTROL_H__
#define __TESTSTATUSPANECONTROL_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoBasePlaybackView;
class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;
class VideoPlaybackStatusPaneControl;



class TestStatusPaneControl : public QObject
{
    Q_OBJECT

    public:

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
    
        // test functions for the test framework    
        void setup();
         
    private slots:
        void testSetVisible();
        void testSetMenu();
        void testUpdateControlsWithFileDetails();
        void testSetVisibility();
        void testControlListUpdated();
        void testSlot_handleAboutToShow();
        void testSlot_handleAboutToHide();
        void testSlot_openFullScreenView();

    signals:
        void commandSignal();
    
    private:
        VideoBasePlaybackView*             mBaseVideoView;
        VideoPlaybackViewFileDetails*        mFileDetails;
        VideoPlaybackControlsController*     mControlsController;
        VideoPlaybackStatusPaneControl*      mStatusPane;
};


#endif  // __TESTSTATUSPANECONTROL_H__

// End of file
