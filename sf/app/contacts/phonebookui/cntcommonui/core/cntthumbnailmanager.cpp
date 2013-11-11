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
#include "cntthumbnailmanager.h"

CntThumbnailManager::CntThumbnailManager()
{
    
}

CntThumbnailManager::~CntThumbnailManager()
{
    qDeleteAll( mManagers.values() );
    mManagers.clear();
}

int CntThumbnailManager::getThumbnail( ThumbnailManager::ThumbnailSize aSize, const QString& aFile )
{
    if ( !mManagers.contains( aSize ) )
    {
        ThumbnailManager* thumbnailManager = new ThumbnailManager(this);
        thumbnailManager->setMode(ThumbnailManager::Default);
        thumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
        thumbnailManager->setThumbnailSize( aSize );
        connect( thumbnailManager, SIGNAL(thumbnailReady(QPixmap,void*,int,int)), 
                this, SIGNAL(thumbnailReady(QPixmap,void*,int,int)) );
        
        mManagers.insert( aSize, thumbnailManager );
    }
    
    ThumbnailManager* manager = mManagers.value( aSize );
    return manager->getThumbnail( aFile );
}

void CntThumbnailManager::cancelRequest( int aRequestId )
{
    foreach ( ThumbnailManager* mgr, mManagers.values() )
    {
        if ( mgr->cancelRequest(aRequestId) )
            break;
    } 
}
// End of File
