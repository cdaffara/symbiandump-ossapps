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
* Description:   ?Description
*
*/

#ifndef __VIDEOSERVICES_H__
#define __VIDEOSERVICES_H__

#include <QStringList>
#include "videoserviceurifetch.h"

//FORWARD CLASS DECLARATION
class VideoServiceUriFetch;

class VideoServices : public QObject
{
    Q_OBJECT

public:

    /**
     * Returns singleton instance for this class.
     *
     * WARNING! Not safe to call this from destructor of another function scope static object!
     *
     * @return The singleton instance.
     */
    static VideoServices *instance();

    /**
     * Decreases the reference count, when count reaches zero cleanup is done.
     *
     */
    void decreaseReferenceCount();

	enum TVideoService
    {
        ENoService,
        EUriFetcher,
        EPlayback
    };

    /**
     * Returns service active status
     *
     * @return bool true if active, false if not active
     *
     */
    VideoServices::TVideoService currentService();
    
    /** 
    * Returns context title.
    */
    QString contextTitle() { return QString(""); }

private:

    /**
     * Constructor
     */
    VideoServices();
 
    /**
     * Destructor.
     */
    virtual ~VideoServices();

    void setCurrentService(VideoServices::TVideoService service);

    Q_DISABLE_COPY(VideoServices)

private:

    /**
     * Singleton instance.
     */
    static VideoServices* mInstance;

    VideoServiceUriFetch* mServiceUriFetch;

    VideoServices::TVideoService mCurrentService;

    friend class VideoServiceUriFetch;

public:

    /**
     * Reference count.
     */
    static int mReferenceCount;

};

#endif //__VIDEOSERVICES_H__
