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
* Description:   helper class to test VideoThumbnailFetcher
*
*/

#ifndef _VIDEOTHUMBNAILFETCHERTESTER_H_
#define _VIDEOTHUMBNAILFETCHERTESTER_H_

// INCLUDES
#include <QObject>
#include "videothumbnailfetcher.h"

class VideoThumbnailFetcherTester : public VideoThumbnailFetcher
{
    Q_OBJECT

public:

    /**
     * Default constructor
     */
    VideoThumbnailFetcherTester();

    /**
     * Destructor
     */
    virtual ~VideoThumbnailFetcherTester();

    //
    // Test methods for VideoThumbnailFetcher's methods.
    //

    void startThumbnailFetches(bool cancelOngoingFetches);
    
    void startThumbnailCreation();
    
private:

};

#endif // _VIDEOTHUMBNAILFETCHERTESTER_H_

// End of file

