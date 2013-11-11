/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class CIpsPlgSosBasePlugin.
*
*/
#ifndef IPSSTATEEXTENSION_H
#define IPSSTATEEXTENSION_H

#include <cmailboxstateext.h>
#include "ipsplgcommon.h"

class MEmailMailboxState;

class MStateObserverCallback
    {
public:
    virtual void ActiveFolderChanged(
            const TFSMailMsgId& aActiveMailboxId,
            const TFSMailMsgId& aActiveFolderId)=0;
    };

NONSHARABLE_CLASS ( CIpsStateExtension ) :
    public CMailboxStateExtension
    {
public: //from CMailboxStateExtension

    static CIpsStateExtension* NewL(MStateObserverCallback& aCallback);

    /**
     * Sets data provider interface.
     * @param aDataProvider data provider.
     */
    void SetStateDataProvider( MEmailMailboxState* aDataProvider );
    
    /**
    * Notification that folder has changed in email application.
    * @param aActiveMailboxId id of the mailbox container the folder
    * @param aActiveFolderId currently active folder id or null id if
    *        there's currently no active folder (e.g. application is closed)
    */
    void NotifyActiveFolderChanged(
        const TFSMailMsgId& aActiveMailboxId,
        const TFSMailMsgId& aActiveFolderId);
    
    virtual ~CIpsStateExtension();
private:
    CIpsStateExtension(MStateObserverCallback& aCallback);
    void ConstructL();
    
private:
    MEmailMailboxState*         iDataProvider;//not owned    
    MStateObserverCallback&     iCallback;
    };

#endif //IPSSTATEEXTENSION_H
