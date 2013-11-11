/**
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
* Description:   helper class to test VideoThumbnailData
*
*/

#ifndef _VIDEOTHUMBNAILDATATESTER_H_
#define _VIDEOTHUMBNAILDATATESTER_H_

// INCLUDES
#include <qobject.h>
#include "videothumbnaildata_p.h"

class VideoProxyModelGeneric;

class VideoThumbnailDataTester : public VideoThumbnailDataPrivate
{
    Q_OBJECT

public:

    /**
     * Default constructor
     */
    VideoThumbnailDataTester();

    /**
     * Destructor
     */
    virtual ~VideoThumbnailDataTester();

    //
    // Test methods for VideoThumbnailData's signals.
    //
    
    /**
     * Emits doBackgroundFetching signal to itself.
     */
    void emitDoBackgroundFetching();
    
    /**
     * Emits reportThumbnailsReadySignal to itself.
     */
    void emitReportThumbnailsReady();    
    
    /**
     * Emits aboutToQuit signal to itself.
     */
    void emitAboutToQuit();    
    
    //
    // Test methods for VideoThumbnailData's methods.
    //

    /**
     * Calls CVideoThumbnailDataPrivate::initialize
     */
    int initialize();

    /**
     * Calls CVideoThumbnailDataPrivate::cleanup
     */
    void cleanup();
    
    /**
     * Calls CVideoThumbnailDataPrivate::connectSignals
     */
    void connectSignals();

    /**
     * Calls CVideoThumbnailDataPrivate::disconnectSignals
     */
    void disconnectSignals();
    
    /**
     * Calls CVideoThumbnailDataPrivate::defaultThumbnail
     */
    const QIcon* defaultThumbnail(TMPXItemId mediaId);
        
    /**
     * Calls CVideoThumbnailDataPrivate::continueBackgroundFetch
     */
    void continueBackgroundFetch();
    
    /**
     * Calls CVideoThumbnailDataPrivate::startFetchingThumbnails
     */
    int startFetchingThumbnails(const QList<QModelIndex> &indexes, int priority);
    
    /**
    * Calls CVideoThumbnailDataPrivate::startFetchingThumbnail
    */
    int startFetchingThumbnail(TMPXItemId mediaId, int priority);
        
signals:

    /**
     * Signal.
     */
    void doBackgroundFetchingSignal();

    /**
     * Signal.
     */
    void modelChangedSignal();

    /**
     * Signal.
     */
    void reportThumbnailsReadySignal();

    /**
     * Signal.
     */
    void aboutToQuitSignal();       

private:

};

#endif // _VIDEOTHUMBNAILDATATESTER_H_

// End of file

