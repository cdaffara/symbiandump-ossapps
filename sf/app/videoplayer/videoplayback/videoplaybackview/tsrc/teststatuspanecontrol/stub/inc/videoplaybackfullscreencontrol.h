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
* Description:  Implementation of VideoPlaybackFullScreenControl
*
*/

// Version : %version:  3 %



#ifndef VIDEOPLAYBACKFULLSCREENCONTROL_H_
#define VIDEOPLAYBACKFULLSCREENCONTROL_H_

#include <qobject>

#include <videoplaybackcontrol.hrh>
#include <mpxplaybackframeworkdefs.h>

class HbWidget;
class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;

class VideoPlaybackFullScreenControl : public QObject
{
    Q_OBJECT

    public:
        VideoPlaybackFullScreenControl( VideoPlaybackControlsController* controller, 
                                        TVideoPlaybackControls index,
                                        HbWidget* widget, 
                                        TUint controlproperties );

        virtual ~VideoPlaybackFullScreenControl();

    public:

        /**
        * Set visibility of each control
        */
        virtual void setVisibility( TMPXPlaybackState aState );
        
        /**
        * return control index
        */
        TVideoPlaybackControls controlIndex();

        /**
        * set changed state
        */
        void updateState( TMPXPlaybackState state );
        
        /*
         *  Update the controls with the file details
         */
        virtual void updateControlsWithFileDetails( VideoPlaybackViewFileDetails *details );

        virtual void setVisible( bool visible );

        virtual bool isVisible();

        virtual void updateControlProperties( TUint properties );

    protected:
        VideoPlaybackControlsController* mController;
        HbWidget                       * mControl;
        TVideoPlaybackControls           mControlIndex;
        TUint                     mProperties;
        bool                      mVisible;
};

#endif /*VIDEOPLAYBACKFULLSCREENCONTROL_H_*/

