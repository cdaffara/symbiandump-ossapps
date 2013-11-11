/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mailbox state extension for notifying email protocol plugins
*               about application state changes.
*
*/

#ifndef CMAILBOXSTATEEXT_H
#define CMAILBOXSTATEEXT_H

#include "cemailextensionbase.h"
#include "CFSMailCommon.h"

// interface UID value passed to CFSMailClient::ExtensionL
const TUid KEmailMailboxStateExtensionUid        = { 0x2001E28B };

/**
* Data providider interface.
*/
class MEmailMailboxState
{
public:
// <qmail>
    /**
    * Returns currently active folder and related mailbox that
    * is currently being used.
    * @param aActiveMailboxId id of currently active mailbox
    * 
    * @param aActiveFolderId id of currently active mail folder
    *        If aActiveFolderId.IsNullId() returns true there is no active
    *        folder. This happens if e.g. email applications is closed. 
    * @return Symbian OS error code
    */
    virtual TInt GetActiveFolderId( 
        TFSMailMsgId& aActiveMailboxId,
        TFSMailMsgId& aActiveFolderId ) const = 0;
// </qmail>    
};

/**
 * Extension interface for passing UI context data to protocol plugins. 
 */
class CMailboxStateExtension : public CEmailExtension
{
public:
    
    /**
     * Sets data provider interface.
     * @param aDataProvider data provider.
     */
    virtual void SetStateDataProvider( MEmailMailboxState* aDataProvider ) = 0;
// <qmail>    
    /**
    * Notification that folder has changed in email application.
    * @param aActiveMailboxId id of the mailbox container the folder
    * @param aActiveFolderId currently active folder id or null id if
    *        there's currently no active folder (e.g. application is closed)
    */
    virtual void NotifyActiveFolderChanged(
        const TFSMailMsgId& aActiveMailboxId,
        const TFSMailMsgId& aActiveFolderId) = 0;
// </qmail>

protected:    
    inline CMailboxStateExtension();

protected:
    MEmailMailboxState* iDataProvider;
};

inline CMailboxStateExtension::CMailboxStateExtension() :
         CEmailExtension( KEmailMailboxStateExtensionUid )
    {
    }

#endif // CMAILBOXSTATEEXT_H
