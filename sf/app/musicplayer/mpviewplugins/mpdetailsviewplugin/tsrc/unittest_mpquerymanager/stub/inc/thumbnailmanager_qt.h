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
* Description: ThumbnailManager stub for testing MpSongData
*
*/

#ifndef THUMBNAILMANAGER_QT_H_
#define THUMBNAILMANAGER_QT_H_
#include <QObject>
#include <QPixmap>


/** default priority value */
const int tnmWrapperPriorityIdle = -100;

class ThumbnailManager : public QObject
    {
    Q_OBJECT

public:


    /** Thumbnail size. */
    enum ThumbnailSize
    {
        /**
         * Small thumbnail
         */
        ThumbnailSmall = 0,
        /**
         * Medium thumbnail
         */
        ThumbnailMedium,
        /**
         * Large thumbnail
         */
        ThumbnailLarge
    };

    /** Mode of thumbnail creation. */
    enum ThumbnailMode
        {
        /**
         * Default mode. This means that:
         * - Thumbnail must be as large as requested (unless the actual object is smaller).
         * - Smaller thumbnails may be up scaled to desired resolution.
         * - Aspect ratio is maintained and thumbnails are not cropped. The
         *   resulting thumbnail may smaller in either width or height if
         *   the aspect ratio of the object does not match the aspect ratio
         *   of the requested size.
         */
        Default = 0,

        /**
         * Allow thumbnails which are smaller than requested are. Thumbnail
         * bitmaps are never up scaled if this flag is set.
         */
        AllowAnySize = 1,

        /**
         * New thumbnail images are not created if this flag is set. Only
         * existing thumbnails may be returned. If a requested thumbnail does
         * not exist null pixmap will be returned.
         */
        DoNotCreate = 2,

        /**
         * Thumbnail images are cropped to match requested aspect ratio. If
         * this mode is set, the size of the resulting thumbnail always
         * matches the requested size.
         */
        CropToAspectRatio = 4
    };

    /**  Quality versus speed preference setting */
    enum QualityPreference
    {
        /**
         * Prefer thumbnails in the highest quality possible disregarding
         * any negative impact on performance.
         */
        OptimizeForQuality,

        /**
         * Get thumbnails as fast as possible, even if
         * it means lower quality.
         */
        OptimizeForPerformance
    };

    // Test utility functions
    static int getInitCounter();
    static void resetInitCounter();

    // Stub functions
    ThumbnailManager( QObject* parentPtr = NULL );
    virtual ~ThumbnailManager();
    bool setMode( ThumbnailMode mode );
    bool setQualityPreference( QualityPreference qualityPreference );
    bool setThumbnailSize( ThumbnailSize thumbnailSize );
    int getThumbnail( const QString& fileName, void * clientData = NULL,
            int priority = tnmWrapperPriorityIdle );
    bool cancelRequest( int id );
    void deleteThumbnails( const QString& fileName );

signals:

    void thumbnailReady( QPixmap , void * , int , int );

public:

    bool    mGetThumbFails;
    int     mThumbnailReqCounter;
    int     mCancelCounter;

};

#endif /* THUMBNAILMANAGER_QT_H_ */
