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
* Description:  VideoServices STUB class definition
*
*/

// Version : %version:  1 %

#ifndef __VIDEOSERVICES_H__
#define __VIDEOSERVICES_H__

#include <QStringList>

#include "videoserviceurifetch.h"

class VideoServiceUriFetch;

class VideoServices : public QObject
{
    Q_OBJECT

    public:

        enum TVideoService
        {
            ENoService,
            EUriFetcher,
            EPlayback,
            EView
        };
    
        static VideoServices *instance();
        void decreaseReferenceCount();
        QString contextTitle() { return QString(""); }
        VideoServices::TVideoService currentService();
    
    public slots:
    
        void itemSelected( const QString& item );

    private:
        
        VideoServices();
        virtual ~VideoServices();
        void setCurrentService( VideoServices::TVideoService service );

    Q_DISABLE_COPY( VideoServices )

    private:
        
        static VideoServices* mInstance;
        VideoServiceUriFetch* mServiceUriFetch;
        VideoServices::TVideoService mCurrentService;

    private:
        
        friend class VideoServiceUriFetch;

    public:
        
        static int mReferenceCount;
        bool mItemSelected;

};

#endif //__VIDEOSERVICES_H__
