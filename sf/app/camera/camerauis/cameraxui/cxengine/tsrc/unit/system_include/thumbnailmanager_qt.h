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
* Description: copy of normally used "system" file for thumbnailmanager for qt
*
*/

#ifndef THUMBNAILMANAGER_QT_H
#define THUMBNAILMANAGER_QT_H

#include <qobject>
#include <QPixmap.h>
#include <QStringList>
#include <QTimer>


class  ThumbnailManager : public QObject
    {
    Q_OBJECT

public:

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
    int getThumbnail(const QString& fileName, void * clientData = NULL, 
            int priority = 0);

    /**
     * Set a thumbnail for an object file generated from pixmap delivered.
     * thumbnailReady() signal will be emited when the operation is complete.
     *
     * @param source             QImage from which the thumbnail will be created
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
    int setThumbnail( const QImage& source, const QString& fileName,
            void * clientData = NULL, int priority = 0 );

    /**
     * Cancel a thumbnail operation.
     *
     * @param id      Request ID for the operation to be cancelled.
     * @return         true if cancelling was successful.
     */
    bool cancelRequest(int id);

   
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

private slots:
    void emulateThumbnailReady();

private:
    int mCurrentThumbnailId;
    QList<int> mThumbnailManagerIds;
    QTimer mTimer;
};

#endif // THUMBNAILMANAGER_QT
