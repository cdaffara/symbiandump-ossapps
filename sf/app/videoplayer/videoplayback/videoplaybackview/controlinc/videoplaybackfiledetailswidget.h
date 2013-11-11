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
* Description:  Implementation of VideoPlaybackFileDetailsWidget
*
*/

// Version : %version: 13 %



#ifndef VIDEOPLAYBACKFILEDETAILSWIDGET_H_
#define VIDEOPLAYBACKFILEDETAILSWIDGET_H_

#include <QObject>

#include <hbwidget.h>

class VideoPlaybackViewFileDetails;
class VideoPlaybackControlsController;
class HbListWidget;

class VideoPlaybackFileDetailsWidget : public HbWidget
{
    Q_OBJECT

    public:
        VideoPlaybackFileDetailsWidget( VideoPlaybackControlsController* controller );
        virtual ~VideoPlaybackFileDetailsWidget();

    public:
        void updateWithFileDetails( VideoPlaybackViewFileDetails* details );

    private:
        void makeTitleItem( VideoPlaybackViewFileDetails* details );
        void makeSizeItem( VideoPlaybackViewFileDetails* details );
        void makeBitRateItem( VideoPlaybackViewFileDetails* details );
        void makeDateTimeItem( VideoPlaybackViewFileDetails* details );
        void addItemToListWidget( QString item, QString text );
        void makeDurationItem( VideoPlaybackViewFileDetails* details );

    private:
        VideoPlaybackControlsController *mController;
        HbListWidget                    *mListWidget;
        bool                             mFileDetailsUpdated;
};

#endif /*VIDEOPLAYBACKFILEDETAILSWIDGET_H_*/

