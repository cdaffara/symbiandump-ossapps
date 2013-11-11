/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Extension to monitor Activer folder change events
*
*/
#include "ipsstateextension.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsStateExtension* CIpsStateExtension::NewL(MStateObserverCallback& aCallback)
    {
    CIpsStateExtension* self = new(ELeave) CIpsStateExtension(aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsStateExtension::SetStateDataProvider( MEmailMailboxState* aDataProvider )
    {
    iDataProvider = aDataProvider;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsStateExtension::NotifyActiveFolderChanged(
        const TFSMailMsgId& aActiveMailboxId,
        const TFSMailMsgId& aActiveFolderId)
    {
    iCallback.ActiveFolderChanged( aActiveMailboxId, aActiveFolderId );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsStateExtension::CIpsStateExtension(MStateObserverCallback& aCallback):
        iCallback(aCallback)
    {
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsStateExtension::~CIpsStateExtension()
    {
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsStateExtension::ConstructL()
    {
    
    }
//EOF
