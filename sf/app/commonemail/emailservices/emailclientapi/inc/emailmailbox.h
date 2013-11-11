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
* Description: This file implements class CEmailMailbox.
*
*/

#ifndef EMAILMAILBOX_H
#define EMAILMAILBOX_H

#include "CFSMailCommon.h"
#include <memailmailbox.h>
#include "MFSMailEventObserver.h"
#include "MFSMailRequestObserver.h"
#include "mmailboxcontentobserver.h"

using namespace EmailInterface;

class CFSMailBox;
class CFSMailAddress;
class CFSMailPlugin;
class CPluginData;
class CEmailAddress;

NONSHARABLE_CLASS( CEmailMailbox ) : 
    public CBase, 
    public MEmailMailbox
{
public:
    
    static CEmailMailbox* NewL( 
        CPluginData& aPluginData,
        const TMailboxId& aMailboxId );
    
    ~CEmailMailbox();           

public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();
    
public:    // from MEmailMailbox
    /**@see MEmailMailbox */
    TMailboxId MailboxId() const;
    
    /**@see MEmailMailbox */
    MEmailAddress* AddressL() const;
    
    /**@see MEmailMailbox */
    TPtrC MailboxName() const;
    
    /**@see MEmailMailbox */
    TInt GetFoldersL( RFolderArray& aFolders ) const;

    /**@see MEmailMailbox */
    MEmailFolder* FolderL( const TFolderId& aFolderId ) const;
    
    /**@see MEmailMailbox */
    MEmailFolder* FolderByTypeL( 
        const TFolderType aFolderType ) const;
        
    /**@see MEmailMailbox */
    MEmailMessage* MessageL( const TMessageId& aMessageId );
    
    /**@see MEmailMailbox */
    MEmailMessage* CreateDraftMessageL() const;
    
    /**@see MEmailMailbox */
    MEmailMessage* CreateReplyMessageL( const TMessageId& aMessageId, const TBool aReplyToAll = ETrue ) const;
    
    /**@see MEmailMailbox */
    MEmailMessage* CreateForwardMessageL( const TMessageId& aMessageId ) const;
    
    /**@see MEmailMailbox */
    void SynchroniseL( MMailboxSyncObserver& aObserver );
        
    /**@see MEmailMailbox */
    void CancelSynchronise();
    
    /**@see MEmailMailbox */
    void RegisterObserverL( MMailboxContentObserver& aObserver );
        
    /**@see MEmailMailbox */
    void UnregisterObserver( MMailboxContentObserver& aObserver );
    
    /**@see MEmailMailbox */
    MEmailMessageSearchAsync* MessageSearchL();

    /**@see MEmailMailbox */
    void ShowInboxL();

    /**@see MEmailMailbox */
    void EditNewMessageL();
    

public: // new methods

private:       
    TFSMailMsgId FsMailboxId() const;
    
private: // constructors
    CEmailMailbox( CPluginData& aPluginData, const TMailboxId& aPlugin );
    
    void ConstructL();

private: // inner class for mailbox event handling
    class TObserverEventMapper : public MFSMailEventObserver
    {
    public:
        TObserverEventMapper( CFSMailPlugin* aPlugin, TUid aPluginUid, const TMailboxId& aMailboxId );
        ~TObserverEventMapper();
        void AddObserverL( MMailboxContentObserver& aObserver );
        void RemoveObserver( MMailboxContentObserver& aObserver );
    public: // from MFSMailEventObserver
        virtual void EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        typedef void ( CEmailMailbox::TObserverEventMapper::*TEventMapFunc)( TMailboxId, TAny*, TAny*, TAny* );
    private: // mappers, so say we all!
        void ConvertParamsL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, REmailMessageIdArray& aMessageIds, TFolderId& aFolderId );
        void IgnoreEventL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void NewMessageL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageChangedL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageDeletedL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageMoved( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageCopiedL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void NewFolderL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void FolderChangeL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void FoldersDeletedL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void FoldersMovedL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void ExceptionL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
    
    private:                                                                                                                         
        static TBool Equals( const MMailboxContentObserver& a1, const MMailboxContentObserver& a2 );
    private:
        CFSMailPlugin*  iPlugin;
        TFSMailMsgId    iFsMailboxId;
        RPointerArray<MMailboxContentObserver> iClientObservers;
    };
    class CEmailRequestObserver: public CBase,
                                 public MFSMailRequestObserver                                 
        {
    public:
        CEmailRequestObserver();
        
        void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
        void SetObserverL( MMailboxSyncObserver* aObserver );
    private:
        MMailboxSyncObserver* iObserver;
        };
        
private:        

    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TMailboxId      iMailboxId;
    
    TObserverEventMapper* iEventMapper;

    mutable CEmailAddress* iAddress;

    CFSMailBox*     iFsMailbox;
    
    CEmailRequestObserver* iSyncObserver;    
    
    TInt iRequestId;
};
    
#endif // EMAILMAILBOX_H

// End of file
