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
* Description:  Implementation of VideoPlaybackStatusPaneControl
*
*/

// Version : %version: 12 %



#ifndef VIDEOPLAYBACKSTATUSPANECONTROL_H_
#define VIDEOPLAYBACKSTATUSPANECONTROL_H_


#include "videoplaybackfullscreencontrol.h"


class HbLabel;
class HbAction;
class HbGroupBox;
class QGraphicsWidget;
class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

class VideoPlaybackStatusPaneControl : public VideoPlaybackFullScreenControl
{
    Q_OBJECT

    public:
        VideoPlaybackStatusPaneControl( VideoPlaybackControlsController* controller,
                                        TVideoPlaybackControls index,
                                        HbWidget* widget,
                                        TUint controlproperties );

        virtual ~VideoPlaybackStatusPaneControl();

    public:

        /**
        * Set visibility of each control
        */
        void setVisibility( TMPXPlaybackState aState );

        /*
         *  Update the controls with the file details
         */
        void updateControlsWithFileDetails( VideoPlaybackViewFileDetails *details );

        void setVisible( bool visible );

        bool isVisible();

        void setMenu( VideoPlaybackViewFileDetails* details );

        void controlListUpdated( VideoPlaybackViewFileDetails* details );

    private slots:
        void handleAboutToShow();
        void handleAboutToHide();
        void openFullScreenView();

    private:
        HbAction                *mActionBack;
        HbLabel                 *mTitleLabel;
        HbGroupBox              *mTitleGroupBox;
        QGraphicsWidget         *mTitleLayout;
};

#endif /*VIDEOPLAYBACKSTATUSPANECONTROL_H_*/

