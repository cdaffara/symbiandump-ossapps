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
* Description:
*
*/
#ifndef CXEIMAGEDATAQUEUE_H
#define CXEIMAGEDATAQUEUE_H

#include <QObject> 

class CxeImageDataItem;

class QImage;

/**
 * CxeImageDataQueue is responsible managing a queue of objects that are saved to
 * the file system in the background
 */
class CxeImageDataQueue : public QObject
{
    Q_OBJECT
public:
    /**
     * Get the number of objects in the queue.
     *
     * @return The number of items in the queue
     */
    virtual int size() const = 0;

    /**
     * Clear the queue, cancel the current save operation (if any) and destroy
     * all unsaved objects
     */
    virtual void clear() = 0;

    //virtual void deleteImage( int index ) = 0;
    //virtual void renameImage( int index, const QString& path ) = 0;

    /**
     * Access individual items in the queue.
     *
     * @param index Item index. Must be 0 <= index < size()
     * @return Reference to the item
     */
    virtual CxeImageDataItem &operator[]( int index ) = 0;

signals:

    void fileSaved( int index, int error ); // index, error

protected:
    CxeImageDataQueue() {}

private:
    Q_DISABLE_COPY( CxeImageDataQueue )
};

#endif // CXEIMAGEDATAQUEUE_H
