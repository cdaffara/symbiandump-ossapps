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

#ifndef CNTTHUMBNAILMANAGER_H_
#define CNTTHUMBNAILMANAGER_H_

#include <QObject>
#include <QMap>
#include <thumbnailmanager_qt.h>
/*!
 * Wrapper class for ThumbnailManager. Reason for this class is to share
 * thumbnail manager instances for performance reasons. 
 * 
 * When a thumbnail is requested, class checks that if there is a manager already
 * for the requested size.  
 */
class CntThumbnailManager : public QObject
{
    Q_OBJECT
    
public:
    CntThumbnailManager();
    ~CntThumbnailManager();
    
    int getThumbnail( ThumbnailManager::ThumbnailSize aSize, const QString& aFile );
    void cancelRequest( int aRequestId );
    
signals:
    void thumbnailReady( QPixmap , void* , int , int );
    
private:
    QMap<int, ThumbnailManager*> mManagers;
};
#endif /* CNTTHUMBNAILMANAGER_H_ */
