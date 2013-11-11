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
#include <QDebug>
#include "xqsettingskey.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQSettingsKey::XQSettingsKey
// -----------------------------------------------------------------------------
//
XQSettingsKey::XQSettingsKey( 
        XQSettingsKey::Target target,
        long int uid,
        unsigned long int key )
    {
    
    }


// -----------------------------------------------------------------------------
// XQSettingsKey::~XQSettingsKey
// -----------------------------------------------------------------------------
//
XQSettingsKey::~XQSettingsKey(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQSettingsKey::target
// -----------------------------------------------------------------------------
//
XQSettingsKey::Target XQSettingsKey::target(  ) const
    {
    
    XQSettingsKey::Target variable;
    return variable;
    }


// -----------------------------------------------------------------------------
// XQSettingsKey::uid
// -----------------------------------------------------------------------------
//
long int XQSettingsKey::uid(  ) const
    {
    
    return 0;
    }


// -----------------------------------------------------------------------------
// XQSettingsKey::key
// -----------------------------------------------------------------------------
//
unsigned long int XQSettingsKey::key(  ) const
    {
    
    return 0;
    }


// -----------------------------------------------------------------------------
// XQPublishAndSubscribeSettingsKey::XQPublishAndSubscribeSettingsKey
// -----------------------------------------------------------------------------
//
XQPublishAndSubscribeSettingsKey::XQPublishAndSubscribeSettingsKey( 
        long int categoryUid,
        unsigned long int key ) : 
        XQSettingsKey( XQSettingsKey::TargetPublishAndSubscribe, categoryUid, key )
    {
    
    }


// -----------------------------------------------------------------------------
// XQPublishAndSubscribeSettingsKey::~XQPublishAndSubscribeSettingsKey
// -----------------------------------------------------------------------------
//
XQPublishAndSubscribeSettingsKey::~XQPublishAndSubscribeSettingsKey(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQCentralRepositorySettingsKey::XQCentralRepositorySettingsKey
// -----------------------------------------------------------------------------
//
XQCentralRepositorySettingsKey::XQCentralRepositorySettingsKey( 
        long int repositoryUid,
        unsigned long int key ) : 
        XQSettingsKey( XQSettingsKey::TargetCentralRepository, repositoryUid, key )
    {
    
    }


// -----------------------------------------------------------------------------
// XQCentralRepositorySettingsKey::~XQCentralRepositorySettingsKey
// -----------------------------------------------------------------------------
//
XQCentralRepositorySettingsKey::~XQCentralRepositorySettingsKey(  )
    {
    
    }


