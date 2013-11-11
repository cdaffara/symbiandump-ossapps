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
* Description:   Declaration of VideoServices Stub
*
*/

// Version : %version: da1mmcf#5 %

#ifndef __VIDEOSERVICES_H__
#define __VIDEOSERVICES_H__

#include <QStringList>
#include "videoserviceurifetch.h"

//FORWARD CLASS DECLARATION
class VideoServiceUriFetch;
class VideoServicePlay;
class VideoServiceView;
class VideoPlayerEngine;

class VideoServices : public QObject
{
    Q_OBJECT

    public:

        /**
         * Returns singleton instance for this class.
         * WARNING! Not safe to call this from destructor of another function scope static object!
         * @return The singleton instance.
         */
        static VideoServices *instance( VideoPlayerEngine* engine = 0 );

        /**
         * Decreases the reference count, when count reaches zero cleanup is done.
         *
         */
        void decreaseReferenceCount();

        enum TVideoService
        {
            ENoService,
            EUriFetcher,
            EPlayback,
            EView
        };

        /**
         * Returns service active status
         * @return bool true if active, false if not active
         */
        VideoServices::TVideoService currentService();

    private:
 
        /**
         * Constructor
         */
        VideoServices();

        /**
         * Constructor
         */
        VideoServices( VideoPlayerEngine* engine );

        /**
         * Destructor.
         */
        virtual ~VideoServices();

        void setCurrentService( VideoServices::TVideoService service );

        Q_DISABLE_COPY( VideoServices )

    signals:
	void activated( int command );

    private:

        /**
         * Singleton instance.
         */
        static VideoServices*        mInstance;

        VideoServiceUriFetch*        mServiceUriFetch;
        VideoServicePlay*            mServicePlay;    
        VideoServiceView*            mServiceView;
        VideoServices::TVideoService mCurrentService;

        friend class VideoServiceUriFetch;
        friend class VideoServicePlay;    
        friend class VideoServiceView;

    public:

        /**
         * Reference count.
         */
        static int mReferenceCount;

        /*
         * indicate fetch service and attach operation has been selected
         */
        bool mFetchSelected;

};

#endif //__VIDEOSERVICES_H__
