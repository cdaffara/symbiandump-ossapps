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
* Description:  stub tn manager object
 *
*/

// Version : %version:  1 %


#ifndef THUMBNAILMANAGER_QT_H
#define THUMBNAILMANAGER_QT_H

#include <qobject>
#include <QPixmap.h>
#include <qmap.h>

class QString;
class QSize;

class ThumbnailManager : public QObject
    {
    Q_OBJECT

public:
    /** Thumbnail size. */
    enum ThumbnailSize
    {
        /**
         * Small thumbnail (64x64 ?)
         */
        ThumbnailSmall = 0, 
        /**
         * Medium thumbnail (128x128 ?)
         */
        ThumbnailMedium, 
        /**
         * Large thumbnail (256x256 ?)
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

    /** Priority of the request. */
    enum Priority
	{
        PriorityIdle=-100,
        PriorityLow=-20,
        PriorityStandard=0,
        PriorityUserInput=10,
        PriorityHigh=20,
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

    /** Display modes. */
    enum DisplayMode
    {
        /** No display mode */
        None,
        
        /** Monochrome display mode (1 bpp) */
        Gray2,
        
        /** Four grayscales display mode (2 bpp) */
        Gray4,
        
        /** 16 grayscales display mode (4 bpp) */
        Gray16,
        
        /** 256 grayscales display mode (8 bpp) */
        Gray256,
        
        /** Low colour GA 16 colour display mode (4 bpp) */
        Color16,
        
        /** 256 colour display mode (8 bpp) */
        Color256,
        
        /** 64,000 colour display mode (16 bpp) */
        Color64K,
        
        /** True colour display mode (24 bpp) */
        Color16M,
        
        /** (Not an actual display mode used for moving buffers containing bitmaps) */
        Rgb,
        
        /** 4096 colour display (12 bpp). */
        Color4K,
        
        /** True colour display mode (32 bpp, but top byte is unused and unspecified) */
        Color16MU,
        
        /** Display mode with alpha (24bpp colour plus 8bpp alpha) */
        Color16MA,
        
        /** Pre-multiplied Alpha display mode 
         * (24bpp color multiplied with the alpha channel value, plus 8bpp alpha) */
        Color16MAP    
    };
    
    struct TnRequest {
        QString name;
        void *id;
        int priority;
        bool cancelled;
        
        TnRequest(QString name, void *id, int priority, bool cancelled) {
            this->name = name;
            this->id = id;
            this->priority = priority;
            this->cancelled = cancelled;
        }
        
        TnRequest() {
            name = QString("");
            id = 0;
            priority = PriorityIdle;
            cancelled = false;
        }
    };

    /**
     * Constructor
     * 
     * @param parentPtr parent
     */    
    ThumbnailManager( QObject* parentPtr = NULL );

    /**
     * Destructor
     */
    virtual ~ThumbnailManager();

    /**
     * Get quality versus performance preference.
     *
     * @return quality versus performance preference
     */
    QualityPreference qualityPreference() const;

    /**
     * Set quality versus performance preference.
     *
     * @param qualityPreference New quality versus performance preference
     *                           value.
     * @return true on success
     */
    bool setQualityPreference( QualityPreference qualityPreference );

    /**
     * Get the current display mode for thumbnail bitmaps.
     *
     * @return current display mode for thumbnail bitmaps
     */
    DisplayMode displayMode() const;

    /**
     * Set new display mode for thumbnail bitmaps.
     *
     * @param displayMode new display mode for thumbnail bitmaps.
     * @return true on success
     */
    bool setDisplayMode( DisplayMode displayMode );
    
    /**
     * Get the current desired size for thumbnail bitmaps.
     *
     * @return Current desired size for thumbnail bitmaps (in pixels).
     */
    QSize thumbnailSize() const;

    /**
     * Set desired size for thumbnail bitmaps.
     *
     * @param thumbnailSize New desired thumbnail size.
     * @return true on success
     */
    bool setThumbnailSize( const QSize& thumbnailSize );

    /**
     * Set desired size for thumbnail bitmaps.
     *
     * @param thumbnailSize New desired thumbnail size.
     * @return true on success
     */
    bool setThumbnailSize( ThumbnailSize thumbnailSize );
    
    /**
     * Get current mode for thumbnail generation.
     *
     * @return Current mode.
     */
    ThumbnailMode mode() const;

    /**
     * Set mode for thumbnail generation.
     *
     * @param mode New flags.
     * @return true on success 
     */
    bool setMode( ThumbnailMode mode );

    /**
     * Get a thumbnail for an object file. If a thumbnail already exists, it
     * is loaded and if a thumbnail does not exist, it is created
     * transparently. If thumbnail loadinf fails thumbnailReady signal is emited 
     * with null pixmap and error code.
     *
     * @param fileName      Source object or file
     * @param clientData    Pointer to arbitrary client data.
     *                      This pointer is not used by the API for
     *                      anything other than returning it in the
     *                      ThumbnailReady signal.
     * @param priority      Priority for this operation
     * @return              Thumbnail request ID or -1 if request failed. This can be used to
     *                      cancel the request or change priority.
     *                      The ID is specific to this tnm
     *                      instance and may not be shared with other
     *                      instances.
     */
    int getThumbnail( const QString& fileName, void * clientData = NULL, 
            int priority = PriorityIdle );

    /**
     * Get a persistent thumbnail for an object file. If a thumbnail already
     * exists, it is loaded and if a thumbnail does not exist, it is created
     * transparently. If thumbnail loading fails thumbnailReady signal is emited 
     * with null pixmap and error code.
     *
     * @param thumbnailId   Thumbnail ID
     * @param clientData    Pointer to arbitrary client data.
     *                      This pointer is not used by the API for
     *                      anything other than returning it in the
     *                      ThumbnailReady signal.
     * @param priority      Priority for this operation
     * @return              Thumbnail request ID or -1 if request failed. This can be used to
     *                      cancel the request or change priority.
     *                      The ID is specific to this tnm
     *                      instance and may not be shared with other
     *                      instances.
     */    
    int getThumbnail( unsigned long int thumbnailId, void * clientData = NULL, 
            int priority = PriorityIdle );
    
    /**
     * Set a thumbnail for an object file generated from pixmap delivered.
     * thumbnailReady() signal will be emited when the operation is complete. 
     * 
     * @param source             Pixmap from which the thumbnail will be created
     * @param fileName           file name
     * @param clientData         Pointer to arbitrary client data.
     *                           This pointer is not used by the API for
     *                           anything other than returning it in the
     *                           ThumbnailReady callback.
     * @param priority           Priority for this operation
     * @return                   Thumbnail request ID or -1 if request failed. This can be used to
     *                           cancel the request or change priority. 
     *                           
     */    
    int setThumbnail( const QPixmap& source, const QString& fileName,
            void * clientData = NULL, Priority priority = PriorityIdle );
    
    /**
     * Delete all thumbnails for a given object. This is an asynchronous
     * operation, which always returns immediately.
     *
     * @param fileName      Source file
     */
    void deleteThumbnails( const QString& fileName );

    /**
     * Delete all thumbnails for a given object. This is an asynchronous
     * operation, which always returns immediately.
     *
     * @param thumbnailId      thumbnail id
     */
    void deleteThumbnails( unsigned long int thumbnailId );

    /**
     * Cancel a thumbnail operation.
     *
     * @param id      Request ID for the operation to be cancelled.
     * @return         true if cancelling was successful.
     */
    bool cancelRequest( int id );

    /**
     * Change the priority of a queued thumbnail operation.
     *
     * @param id           Request ID for the request which to assign a new
     *                      priority.
     * @param newPriority  New priority value
     * @return              true if change was successful.
     */
    bool changePriority( int id, Priority newPriority );
    
    
    void emitThumbnailReady(int tnId );
    
signals:  
    /**
     * Final thumbnail bitmap generation or loading is complete.
     *
     * @param pixmap     An object representing the resulting thumbnail.
     * @param clientData Client data
     * @param id         Request ID for the operation
     * @param errorCode  error code
     */
    void thumbnailReady( QPixmap , void * , int , int );    
    
public:
    
    static QMap<int, TnRequest> mRequests;
    
    ThumbnailSize mThumbSize;
    QualityPreference mQuality;
    bool mGetThumbFails;
    int mThumbnailReadyError;
    
};

#endif // QTNMWRAPPER_H
