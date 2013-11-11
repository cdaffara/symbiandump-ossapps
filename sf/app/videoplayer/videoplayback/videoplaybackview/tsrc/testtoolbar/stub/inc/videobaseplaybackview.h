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
* Description:  Video playback view
*
*/

// Version : %version:    3 %



// This file defines the API for .dll

#ifndef __VIDEOBASEPLAYBACKVIEW_H__
#define __VIDEOBASEPLAYBACKVIEW_H__

//  Include Files
#include <QWidget>


//  Constants

//  Forward Declarations

//  Class Definitions
class HbToolBar;
class RWindow;

class VideoBasePlaybackView : public QWidget
{
    Q_OBJECT

    public:
        VideoBasePlaybackView();
        virtual ~VideoBasePlaybackView();

        void hideItems( int item );
        void showItems( int item );
        HbToolBar *toolBar();
        RWindow *getWindow();

    public:
        HbToolBar *mToolBar;
};

#endif  // __VIDEOBASEPLAYBACKVIEW_H__

// EOF
