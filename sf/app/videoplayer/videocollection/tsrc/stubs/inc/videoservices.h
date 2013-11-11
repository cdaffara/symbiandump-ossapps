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
* Description:   VideoServices class definition
*
*/

// Version : %version: 4 %

#ifndef __VIDEOSERVICES_H__
#define __VIDEOSERVICES_H__

/**
 *
 * We need stub for this because it has static data and HW compilation of moc file fails because of that.
 *
 */

#include <QStringList>

//FORWARD CLASS DECLARATION
class VideoServiceUriFetch;
class VideoServicePlay;
class VideoServiceView;
class VideoServiceBrowse;
class VideoPlayerEngine;

class VideoServices : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY( VideoServices )

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
    
        /**
         * Returns the context title set by service requestee
         * @return QString the title
         */
        QString contextTitle() const;
    
        /**
         * Returns sort type defined by the service client.
         * @param None.
         * @return int.
         */
        int sortRole() const;
    
        /*
         * Enum reflecting the services provided 
         */
        enum TVideoService
        {
            ENoService,
            EUriFetcher,
            EPlayback,
            EView,
            EBrowse
        };
    
        /**
         * Returns service active status
         * @return bool true if active, false if not active
         */
        VideoServices::TVideoService currentService();
    
        /**
         * Returns browsing category.
         * @return see vcxmyvideosdef.h for default categories.
         */
        int getBrowseCategory() const;
    
    public slots:
        void itemSelected( const QString& item );
        void browsingEnded();
    
    signals:
        /*
         * Emitted when service user has set the title
         */    
        void titleReady( const QString& title );
    
        /*
         * Emitted to acticate requested plugin
         */
        void activated( int command );
    
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
    
    public:
        void setEngine( VideoPlayerEngine* engine );
    
        /**
         * Sets the active service
         * @param service
         */
        void setCurrentService( VideoServices::TVideoService service );
    
        /**
         * Returns the current engine
         * @return engine
         */
        VideoPlayerEngine* engine();
    
    private:
    
        /**
         * Reference count.
         */
        int mReferenceCount;
    
        /**
         * VideoServiceUriFetch service instance.
         */
        VideoServiceUriFetch* mServiceUriFetch;
    
        /**
         * VideoServicePlay service instance.
         */
        VideoServicePlay* mServicePlay;
        
        /**
         * VideoServiceView service instance.
         */    
        VideoServiceView* mServiceView; 
        
        /**
         * VideoServiceBrowse service instance.
         */
        VideoServiceBrowse *mServiceBrowse;
    
        /**
         * Pointer of VideoPlayerEngine.
         */
        VideoPlayerEngine* mEngine;
    
        /*
         * Current service
         */
        VideoServices::TVideoService mCurrentService;
        
        /**
         * Sort role.
         */
        int mSortRole;
    
        friend class VideoServiceUriFetch;    
        friend class VideoServicePlay;        
        friend class VideoServiceView;        
        friend class VideoServiceBrowse;
        
};

#endif //__VIDEOSERVICES_H__
