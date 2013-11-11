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
* Description: This file implements class CEmailApiUtils.
*
*/

#include <e32std.h>
#include "CFSMailPlugin.h"
#include "emailapidefs.h"
#include "emailclientapiimpldefs.h"
#include "emailapiutils.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CPluginData::CPluginData( TUid aUid ) :
    iData( aUid ),
    iPluginLoadError( KErrNotReady )
    { 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CPluginData::~CPluginData()
    {
    // panic if a user of plugin has not called ReleaseInstance after ClaimInstance
    __ASSERT_ALWAYS( iOwned || !iRefCount, Panic( EMailPanicPluginNotReleased ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CFSMailPlugin* CPluginData::ClaimInstance()
    {
    if ( !iData.iPlugin )
        {
        TRAP( iPluginLoadError, iData.iPlugin = CFSMailPlugin::NewL( iData.iUid ) );
        }
    if ( !iPluginLoadError )
        {
        iRefCount++;
        }
    return iData.iPlugin;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CFSMailPlugin* CPluginData::ClaimInstanceL()
    {
    ClaimInstance();
    User::LeaveIfError( iPluginLoadError );
    return iData.iPlugin;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CPluginData::ReleaseInstance()
    {
    if ( iData.iPlugin )
        {
        iRefCount--;
        if ( !iRefCount )
            {
            delete iData.iPlugin;
            iData.iPlugin = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CPluginData::ReleasePushL()
    {
    TCleanupItem item( &CPluginData::CleanupOperation, this );
    CleanupStack::PushL( item );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CPluginData::CleanupOperation( TAny* aAny )
    {
    CPluginData* pdata = reinterpret_cast<CPluginData*>( aAny );
    pdata->ReleaseInstance();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TUid CPluginData::Uid() const
    {
    return iData.iUid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CPluginData::LoadResult() const
    {
    return iPluginLoadError;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TFSMailMsgId FsMsgId( const CPluginData& aPluginData, const EmailInterface::TBaseId& aId )
    {
    TFSMailMsgId id( aPluginData.Uid(), aId.iId );
    return id;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CleanupReleasePush::PushL( EmailInterface::MEmailInterface& aItem )
    {
    TCleanupItem item( &CleanupReleasePush::Release, &aItem );
    CleanupStack::PushL( item );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CleanupReleasePush::Release( TAny *aPtr )
    {
    reinterpret_cast<EmailInterface::MEmailInterface*>( aPtr )->Release();
    }

// End of file
