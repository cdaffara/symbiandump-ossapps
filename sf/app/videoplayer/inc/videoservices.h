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

// Version : %version: 7 %

#ifndef __VIDEOSERVICES_H__
#define __VIDEOSERVICES_H__

#include <QStringList>
#include "videoplayerappexport.h"

//FORWARD CLASS DECLARATION
class VideoServiceUriFetch;
class VideoServicePlay;
class VideoServiceView;
class VideoServiceBrowse;
class VideoPlayerEngine;
class VideoServiceUri;

class VIDEOPLAYERAPP_DLL_EXPORT VideoServices : public QObject
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
            EUriView,
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
        void itemSelected(const QString& item);
        void browsingEnded();
    
    signals:
    
        /*
         * Emitted when service user has set the title
         */    
        void titleReady(const QString& title);
    
        /*
         * Emitted to acticate requested plugin
         */
        void activated(int command);
    
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
    
        void setEngine( VideoPlayerEngine* engine );
    
        /**
         * Sets the active service
         *
         * @param service
         *
         */
        void setCurrentService(VideoServices::TVideoService service);
    
        /**
         * Returns the current engine
         *
         * @return engine
         *
         */
        VideoPlayerEngine* engine();
    
        Q_DISABLE_COPY( VideoServices )
    
    private:
    
        /**
         * Reference count.
         */
        int mReferenceCount;
    
        /**
         * Singleton instance.
         */
        static VideoServices* mInstance;
    
        /**
         * VideoServiceUriFetch service instance.
         */
        VideoServiceUriFetch* mServiceUriFetch;
    
        /**
         * Deprecated VideoServiceUriFetch service instance.
         */
        VideoServiceUriFetch* mServiceUriFetchDeprecatedNewService;
        
        /**
         * Deprecated VideoServiceUriFetch service instance.
         */
        VideoServiceUriFetch* mServiceUriFetchDeprecatedOldService;
    
        /**
         * VideoServicePlay service instance.
         */
        VideoServicePlay* mServicePlay;
        
        /**
         * VideoServicePlay service instance.
         */
        VideoServicePlay* mServicePlayDeprecatedNewService;
        
        /**
         * VideoServicePlay service instance.
         */
        VideoServicePlay* mServicePlayDeprecatedOldService;    
        
        /**
         * VideoServiceView service instance.
         */    
        VideoServiceView* mServiceView; 
        
        /**
         * VideoServiceView service instance.
         */    
        VideoServiceView* mServiceViewDeprecatedNewService; 
        
        /**
         * VideoServiceView service instance.
         */    
        VideoServiceView* mServiceViewDeprecatedOldService;     
        
        /**
         * VideoServiceBrowse service instance.
         */
        VideoServiceBrowse *mServiceBrowse;
    
        /**
         * Deprecated VideoServiceBrowse service instance.
         */
        VideoServiceBrowse *mServiceBrowseDeprecatedNewService;
        
        /**
         * Deprecated VideoServiceBrowse service instance.
         */
        VideoServiceBrowse *mServiceBrowseDeprecatedOldService;
                
        /**
         * VideoServiceUri service instance.
         */
        VideoServiceUri *mServiceUriView;        
    
        /**
         * Pointer of VideoPlayerEngine.
         */
        VideoPlayerEngine* mEngine;
    
        /*
         * Current service
         */
        VideoServices::TVideoService mCurrentService;
    
        friend class VideoServiceUriFetch;    
        friend class VideoServicePlay;        
        friend class VideoServiceView;        
        friend class VideoServiceBrowse;        
	    friend class VideoServiceUri;
	    
    public:
        
        /*
         * indicate fetch service and attach operation has been selected
         */
        bool mFetchSelected;
        
};

#endif //__VIDEOSERVICES_H__
