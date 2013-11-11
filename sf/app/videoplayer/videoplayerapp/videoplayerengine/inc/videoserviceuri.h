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
* Description:  Declaration of VideoServiceUri and VideoServiceUriMms
*
*/

// Version : %version:  1 %

#ifndef __VIDEOSERVICEURI_H__
#define __VIDEOSERVICEURI_H__

#include <xqserviceprovider.h>
#include <QObject>

// FORWARD DECLARATIONS
class VideoServices;
class VideoPlayerEngine;
class QLatin1String;

class VideoServiceUri : public XQServiceProvider
    {
    Q_OBJECT

    public:
        VideoServiceUri( VideoServices *parent, VideoPlayerEngine* engine, QLatin1String service );
        virtual ~VideoServiceUri();

        void setEngine( VideoPlayerEngine* engine );

    public slots:
        bool view( QString uri );

    private:
        VideoPlayerEngine* mEngine;
        VideoServices* mServiceApp;
    };
	
#endif //__VIDEOSERVICEURI_H__
