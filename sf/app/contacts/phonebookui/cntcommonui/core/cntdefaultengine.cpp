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

#include <cntabstractviewmanager.h>
#include "cntdefaultengine.h"
#include "cntextensionmanager.h"
#include "cntthumbnailmanager.h"
#include "cntdebug.h"

CntDefaultEngine::CntDefaultEngine( CntAbstractViewManager& aManager ) :
mViewManager( aManager ),
mExtManager( NULL ),
mThumbnail( NULL )
{
}

CntDefaultEngine::~CntDefaultEngine()
{
    qDeleteAll(mBackends);
    mBackends.clear();
    
    delete mExtManager;
    delete mThumbnail;
}

QContactManager& CntDefaultEngine::contactManager( const QString& aType )
{
    CNT_ENTRY
       
    foreach ( QContactManager* mgr, mBackends ) 
    {
        QString uri = mgr->managerUri();
        if ( aType.compare(uri, Qt::CaseInsensitive) == 0 )
        {
            return *mgr;
        }    
    }
    
    QContactManager* manager = QContactManager::fromUri( aType );
       
    if ( manager )
    {
        mBackends.append( manager );
    }
       
    CNT_EXIT
    return *manager;
}

CntAbstractViewManager& CntDefaultEngine::viewManager()
{    
    return mViewManager;
}

CntExtensionManager& CntDefaultEngine::extensionManager()
{
    if ( !mExtManager )
        mExtManager = new CntExtensionManager();
    
    return *mExtManager;
}

CntThumbnailManager& CntDefaultEngine::thumbnailManager()
{
    if ( !mThumbnail )
        mThumbnail = new CntThumbnailManager();
    
    return *mThumbnail;
}

// End of File
