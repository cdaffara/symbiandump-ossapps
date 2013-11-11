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
* Description:  This file implements class CIPSAccountInfo.
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

const TInt KEventGranularity = 2;
//<cmail>
const TInt KMaxDriveUnitValue = 25;
//</cmail>

#ifdef _DEBUG
_LIT( KIpsPlgEventHandlerPanic, "ArrError");
#endif

// ----------------------------------------------------------------------------
// class CIPSAccountInfo
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIPSAccountInfo* CIPSAccountInfo::NewL(
    const TDesC& aName, const TFSMailMsgId& aMBoxId )
    {
    FUNC_LOG;
    CIPSAccountInfo* self = new(ELeave)CIPSAccountInfo( aMBoxId );
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIPSAccountInfo::ConstructL( const TDesC& aName )
    {
    FUNC_LOG;
    iName = aName.AllocL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIPSAccountInfo::CIPSAccountInfo( const TFSMailMsgId& aMBoxId ) :
    iMBoxId( aMBoxId )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIPSAccountInfo::~CIPSAccountInfo()
    {
    FUNC_LOG;
    delete iName;
    }

// ----------------------------------------------------------------------------
// class CIpsPlgEventHandler
// ----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgEventHandler* CIpsPlgEventHandler::NewL(
    CIpsPlgSosBasePlugin& aBasePlugin )
    {
    FUNC_LOG;
    CIpsPlgEventHandler* self = new( ELeave ) CIpsPlgEventHandler(
        aBasePlugin );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgEventHandler::~CIpsPlgEventHandler()
    {
    FUNC_LOG;
    UnRegisterPropertyObserver( this );
    iMBoxObservers.ResetAndDestroy();
    iMBoxObservers.Close();
    iIPSAccounts.ResetAndDestroy();
    iIPSAccounts.Close();
    if ( iCenRep )
        {
        iCenRep->NotifyCancelAll();
        }
    delete iCenRep;
    iIPSSettingsObservers.ResetAndDestroy();
    iIPSSettingsObservers.Close();
    iPropertyObservers.Close();
// <qmail> iConnOpCallbacks, iSettingsApi members removed
    iImapFolderIds.Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgEventHandler::CIpsPlgEventHandler(
    CIpsPlgSosBasePlugin& aBasePlugin )
    :iBasePlugin( aBasePlugin ),
    iMBoxObservers( KEventGranularity ),
    iIPSAccounts( KEventGranularity ),
    iIPSSettingsObservers( KEventGranularity ),
    iIsConnected( EFalse ),
    iPropertyObservers( KEventGranularity )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgEventHandler::ConstructL( )
    {
    FUNC_LOG;
// <qmail>
    // commented out from qmail when extented settings 
    // cen rep not exist in environment
    //iCenRep = CRepository::NewL( KCRUidExtendedSettingsUid );
// </qmail>
    iPluginId = iBasePlugin.PluginId();
    RegisterPropertyObserverL( this );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::CompleteConstructL( CMsvSession* aSession )
    {
    FUNC_LOG;
    iSession = aSession;
// <qmail> iSettingsApi member removed

    // this collects all folderids to array from mailboxes that
    // have registered observers, this is need to do here because
    // observers might be registered before calling complete construct
    RArray<TMsvId> mboxes( KEventGranularity );
    for ( TInt i = 0; i < iMBoxObservers.Count(); i++ )
        {
        TMsvId mboxId = iMBoxObservers[i]->iMBoxId.Id();
        if ( mboxes.Find(mboxId) == KErrNotFound )
            {
            mboxes.AppendL( mboxId );
            }
        }

    for ( TInt i = 0; i < mboxes.Count(); i++ )
        {
        CollectSubscribedFoldersL( mboxes[i] );
        }
    mboxes.Close();

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgEventHandler::SetNewPropertyEvent(
        TInt aMailbox, TInt aEvent, TInt aError )
    {
    FUNC_LOG;
    TIpsPlgPropertyEvent pEvent(  aEvent, aMailbox ,
            iBasePlugin.PluginId() ,aError );
    TInt error = SetProperty( pEvent );
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsPlgEventHandler::SetProperty( TIpsPlgPropertyEvent aEvent )
    {
    FUNC_LOG;
    TInt error = RProperty::Define(
            KIpsPlgPropertyCatUid,
        KIPSSosEventPropertyKey, RProperty::EByteArray,
        KAllowAllPolicy,
        KAllowWriteDeviceDataPolicy );

    iPropertyBuf = aEvent;

    // if definition was success
    if ( error == KErrNone ||
         error == KErrAlreadyExists ||
         error == KErrPermissionDenied )
        {
        error = RProperty::Set(
                KIpsPlgPropertyCatUid,
                KIPSSosEventPropertyKey, iPropertyBuf );

        }
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::RegisterPropertyObserverL(
        MIpsPlgPropertyObserver* aObserver )
    {
    FUNC_LOG;
    if ( !aObserver )
        {
        User::Leave( KErrArgument );
        }

    if ( !iPropertyWatcher )
        {
        iPropertyWatcher = CIpsPlgPropertyWatcher::NewL(
                CActive::EPriorityIdle, *this );
        }

    iPropertyObservers.AppendL( aObserver );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::UnRegisterPropertyObserver(
        MIpsPlgPropertyObserver* aObserver )
    {
    FUNC_LOG;
    TInt index = iPropertyObservers.Find( aObserver );

    if ( index != KErrNotFound )
        {
        iPropertyObservers.Remove( index );
        }

    if ( iPropertyObservers.Count() == 0 )
        {
        delete iPropertyWatcher;
        iPropertyWatcher = NULL;
        }

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::NotifyPropertyEventL(
        TIpsPlgPropertyEvent aEvent )
    {
    FUNC_LOG;
    INFO_1("pluginid == 0x%x", iPluginId);
   //<qmail> not used: TBool doNotify = ETrue;
    switch ( aEvent.iEvent )
        {
        case KIpsSosEmailSyncStarted:
            SignalStartSyncL( TFSMailMsgId( aEvent.iPluginId,
                    aEvent.iMailbox ) );
            break;
        case KIpsSosEmailSyncCompleted:
            {
            SignalSyncCompletedL( TFSMailMsgId( aEvent.iPluginId,
                    aEvent.iMailbox ), aEvent.iError );
                }
            break;
        default:
            break;
        }
    // <qmail> removed doNotify bool below
    for ( TInt i = 0; i < iPropertyObservers.Count(); i++ )
        {
        TRAP_IGNORE( iPropertyObservers[i]->HandlePropertyEventL(
                aEvent.iEvent,  aEvent.iMailbox,
                aEvent.iPluginId, aEvent.iError ) );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandleSessionEventL(
    MMsvSessionObserver::TMsvSessionEvent aEvent,
    TAny* aArg1,
    TAny* aArg2,
    TAny* aArg3)
    {
    FUNC_LOG;
    if ( aEvent == MMsvSessionObserver::EMsvServerReady )
        {
        iBasePlugin.CompleteConstructL();
        IPSAccountsL();
        return;
        }
    if ( iMBoxObservers.Count() == 0 )
        {
        if ( !iPluginObserver )
            {
            //if no observer we can't relay these events.
            return;
            }
        if ( !IsMailboxOrMediaEvent(aEvent, aArg2) )
            {
            // event is related to messages / folders, can return
            // because no mailbox observers found
            return;
            }
        }
    
    switch( aEvent )
        {
        case EMsvEntriesCreated:
            HandleEntriesCreatedL( aArg1, aArg2, aArg3 );
            break;
        case EMsvEntriesDeleted:
            HandleEntriesDeletedL( aArg1, aArg2, aArg3 );
            break;
        case EMsvEntriesChanged:
            HandleEntriesChangedL( aArg1, aArg2, aArg3 );
            break;
        case EMsvEntriesMoved:
            HandleEntriesMovedL( aArg1, aArg2, aArg3 );
            break;
        case EMsvServerTerminated:
            iBasePlugin.SessionTerminated();
            iSession = NULL;
            break;
        case EMsvCloseSession:
            break;
        case EMsvGeneralError:
            break;
        //<cmail>    
        case EMsvMediaChanged:
            HandleMediaChangedL( aArg1, aArg2, aArg3 );
            break;
        //</cmail>
        default:
            //we don't care about unhandled events
            break;
        };
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::AddPluginObserverL(
    MFSMailEventObserver* aObserver )
    {
    FUNC_LOG;
    // there can be several plugin observers??
    iPluginObserver = aObserver;
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::RemovePluginObserver(
    MFSMailEventObserver* /*aObserver*/ )
    {
    FUNC_LOG;
    iPluginObserver = NULL;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::SubscribeMailboxEventsL(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver,
    TUint32 aKey )
    {
    FUNC_LOG;
    CMBoxEventObserver* observer = new(ELeave) CMBoxEventObserver( aObserver, aMailboxId );
    CleanupStack::PushL( observer );
    iMBoxObservers.AppendL( observer );
    CleanupStack::Pop( observer );
    AppendSettingsObserverL( aMailboxId, aKey );
    //at the moment we won't support syncstate events for IMAP alwaysonline.
    //because of imap idle. we can't extract usefull syncstate changes when it is in use.
   /* if ( iBasePlugin.MtmId().iUid != KSenduiMtmImap4UidValue )
        {
        AppendSyncStateObserverL( aMailboxId );
        }*/
    if( aMailboxId.PluginId() == KIpsPlgImap4PluginUid )
        {
        CollectSubscribedFoldersL( aMailboxId.Id() );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::UnsubscribeMailboxEvents(
    const TFSMailMsgId& aMailboxId,
    MFSMailEventObserver& aObserver )
    {
    FUNC_LOG;
    TInt count = iMBoxObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( &iMBoxObservers[i]->iObserver == &aObserver )
            {
            RemoveSettingsObserver( aMailboxId );
            delete iMBoxObservers[i];
            iMBoxObservers.Remove(i);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgEventHandler::IsEventFromIpsSourceL( TAny* aArg1,
                                                  TUid& aSourceType,
                                                  TMsvEntry& aEntry )
    {
    FUNC_LOG;
    const CMsvEntrySelection* selection =
        static_cast<CMsvEntrySelection*>( aArg1 );

    TMsvId dummy( KMsvNullIndexEntryIdValue );
    TMsvId entryId( KMsvNullIndexEntryIdValue );
    if ( selection->Count() == 0)
        {
        User::Leave( KErrArgument );
        }

    if( !iSession )
        {
        User::Leave( KErrNotReady );
        }

    entryId = selection->At(0);
    User::LeaveIfError( iSession->GetEntry( entryId, dummy, aEntry ) );

    //we set the source type regardless is it IPS ( imap/pop )
    aSourceType = aEntry.iType;


    TBool ret = ETrue;
    //we're interested only in events from own mtm type
    //or ny messages in smtp folder
    if ( aEntry.iMtm.iUid != iBasePlugin.MtmId().iUid )
        {
        //when creating a mailbox, we come here only once,
        //so this is reliable way to see mailbox creation.
        ret = EFalse;
        }
    else if ( aEntry.iType.iUid < KUidMsvRootEntryValue ||
             aEntry.iType.iUid > KUidMsvAttachmentEntryValue )
         {
         ret = EFalse;
         }

    if ( !ret && aEntry.iMtm.iUid == KSenduiMtmSmtpUidValue )
        {
        ret = ETrue;
        }


    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
inline TFSMailMsgId CIpsPlgEventHandler::SymId2FsId(
    TMsvId aId, TUint aMtmUid )
    {
    FUNC_LOG;
    TFSMailMsgId id;
    id.SetId( aId );
    if ( aMtmUid == KSenduiMtmImap4UidValue )
        {
        id.SetPluginId( KIpsPlgImap4PluginUid );
        }
    else if ( aMtmUid == KSenduiMtmPop3UidValue )
        {
        id.SetPluginId( KIpsPlgPop3PluginUid );
        }
    else if ( aMtmUid == KSenduiMtmSmtpUidValue )
        {
        // set plugin id to this plugin
        id.SetPluginId( TUid::Uid(iPluginId) );
        }
    return id;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
inline TFSMailMsgId CIpsPlgEventHandler::SymId2FsId(
    const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    return SymId2FsId( aEntry.Id(), aEntry.iMtm.iUid );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
inline void CIpsPlgEventHandler::FillFSMessageArrayL(
    RArray<TFSMailMsgId>& aFSArray,
    const CMsvEntrySelection* aSelection,
    TUint aMtmUid )
    {
    FUNC_LOG;
    // function should used only with empty (new) arrays
    __ASSERT_DEBUG( ( aFSArray.Count() == 0 ),
        User::Panic( KIpsPlgEventHandlerPanic, KErrGeneral ) );

    for ( TInt i = 0; i < aSelection->Count(); i++ )
        {
        aFSArray.AppendL( SymId2FsId(aSelection->At(i), aMtmUid) );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::SendDelayedEventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aArg1,
    TAny* aArg2,
    TAny* aArg3 )
    {
    FUNC_LOG;
    TIpsPlgEvent event;
    event.iAccountId = aMailbox;
    event.iEvent = aEvent;
    event.iArg1 = aArg1;
    event.iArg2 = aArg2;
    event.iArg3 = aArg3;

    RPointerArray<MFSMailEventObserver> observers;
    CleanupClosePushL( observers );

    if( aEvent == TFSEventNewMailbox ||
        aEvent == TFSEventMailboxRenamed ||
        aEvent == TFSEventMailboxDeleted )
        {
        //<cmail>
        if( iPluginObserver )
            {
            // Could been deleted within observer itself
            observers.AppendL( iPluginObserver );
            }
        //</cmail>
        }
    else
        {
        //other events go to mailbox specific observers
        MailboxObserversL( aMailbox, observers );
        }
    for( TInt i = 0; i < observers.Count(); ++i )
        {
        // ignore leave so that other observers get event even if
        // one leaves
        TRAP_IGNORE(observers[i]->EventL(
            event.iEvent,
            event.iAccountId,
            event.iArg1,
            event.iArg2,
            event.iArg3 ));
        }
    CleanupStack::PopAndDestroy( &observers );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::IPSAccountsL()
    {
    FUNC_LOG;
    if( !iSession )
        {
        User::Leave( KErrNotReady );
        }
    CMsvEntry* root = iSession->GetEntryL( KMsvRootIndexEntryIdValue );
    CleanupStack::PushL( root );

    CMsvEntrySelection* pop = root->ChildrenWithMtmL( KSenduiMtmPop3Uid );
    CleanupStack::PushL( pop );

    CMsvEntrySelection* imap = root->ChildrenWithMtmL( KSenduiMtmImap4Uid );
    CleanupStack::PushL( imap );

    TInt count = pop->Count();
    TMsvEntry tEntry;
    TMsvId service;
    for ( TInt i=0; i<count; i++ )
        {
        iSession->GetEntry( pop->At(i), service, tEntry );
        //<cmail>
        if( !AccountExistsL( tEntry ) )
            {
            iIPSAccounts.AppendL( CIPSAccountInfo::NewL( 
                tEntry.iDetails, 
                SymId2FsId( tEntry ) ) );    
            }
        //</cmail>
        }
        
    count = imap->Count();
    for ( TInt i=0; i<count; i++ )
        {
        iSession->GetEntry( imap->At(i), service, tEntry );
        //<cmail>
        if( !AccountExistsL( tEntry ) )
            {  
            iIPSAccounts.AppendL( CIPSAccountInfo::NewL( 
                tEntry.iDetails, 
                SymId2FsId( tEntry ) ) );
            }
        //</cmail>
        }        
    
    CleanupStack::PopAndDestroy( 3, root );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandleEntriesCreatedL(
    TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/ )
    {
    FUNC_LOG;
    TUid type;
    TMsvEntry tNew;

    if( IsEventFromIpsSourceL( aArg1, type, tNew ) )
        {
        TAny* arg1=NULL;
        TAny* arg2=NULL;
        TAny* arg3=NULL;

        TFSMailEvent event;
        TFSMailMsgId mbox;

        if ( type == KUidMsvServiceEntry )
            {
            event = TFSEventNewMailbox;

            mbox = SymId2FsId( tNew );

            if( AccountExistsL( tNew ) )
                {
                //this is a duplicate event. Ignoring...
                return;
                }

            iIPSAccounts.AppendL( CIPSAccountInfo::NewL(
                tNew.iDetails,
                mbox ) );
            SendDelayedEventL( event, mbox,
                arg1, arg2 , arg3 );
            }
        
        // discard any new mail events if message is marked as deleted in imap
        const TMsvEmailEntry& emlEntry(tNew);
        TBool isMarkedAsDeleted = EFalse;
        if ( mbox.PluginId() == KIpsPlgImap4PluginUid
                && (EDisconnectedDeleteOperation == emlEntry.DisconnectedOperation()
                || emlEntry.DeletedIMAP4Flag()) )
            {
            isMarkedAsDeleted = ETrue;
            }
        
        
        if ( type == KUidMsvMessageEntry && !isMarkedAsDeleted )
            {
            TMsvId* parent = static_cast<TMsvId*>(aArg2);
            // NOTE: assumed that event contains only one new message
            event = TFSEventNewMail;
            TFSMailMsgId mbox;

            if ( tNew.iServiceId == KMsvLocalServiceIndexEntryIdValue
                || tNew.Parent() == KMsvDraftEntryIdValue )
                {
                TMsvEntry srvEntry;
                TMsvId dummy;
                if( !iSession )
                    {
                    User::Leave( KErrNotReady );
                    }
                iSession->GetEntry( tNew.iServiceId, dummy, srvEntry );
                iSession->GetEntry( srvEntry.iRelatedId, dummy, srvEntry );
                mbox = SymId2FsId( srvEntry );
                }
            else
                {
                mbox = SymId2FsId( tNew.iServiceId , tNew.iMtm.iUid );
                // start automatic sync
                if ( iBasePlugin.IsUnderUiProcess() &&
                        iBasePlugin.PluginId() == KIpsPlgImap4PluginUidValue )
                    {
                    iBasePlugin.PopulateNewMailL(
                            mbox, tNew.Id(), tNew.Parent() );
                    }
                }

            // set entries array pointer
            RArray<TFSMailMsgId> array(KEventGranularity);
            CleanupClosePushL( array );

            FillFSMessageArrayL(
                array,
                static_cast<const CMsvEntrySelection*>(aArg1),
                tNew.iMtm.iUid );
            arg1 = static_cast<TAny*>(&array);

            // set parent pointer
            parent = static_cast<TMsvId*>(aArg2);
            TFSMailMsgId tfsid( iPluginId, *parent);
            arg2 = static_cast<TAny*>(&tfsid);

            SendDelayedEventL( event, mbox,
                arg1, arg2 , arg3 );
            CleanupStack::PopAndDestroy( &array );
            }
        else if ( type == KUidMsvFolderEntry )
            {
            event = TFSEventNewFolder;
            mbox = SymId2FsId( tNew.iServiceId, tNew.iMtm.iUid );

            // set entries array pointer
            RArray<TFSMailMsgId> array(1);
            CleanupClosePushL( array );
            array.AppendL( SymId2FsId( tNew ) );
            arg1 = &array;

            // set parent pointer
            TMsvId* parent = static_cast<TMsvId*>(aArg2);
            TFSMailMsgId tfsid( iPluginId, *parent);
            arg2 = static_cast<TAny*>(&tfsid);

            SendDelayedEventL( event, mbox,
                arg1, arg2 , arg3 );

            CleanupStack::PopAndDestroy( &array );

            if( tNew.iMtm.iUid == KSenduiMtmImap4UidValue )
                {
                SetFolderIdToArrayL( tNew.Id() );
                }
            }
        else//attachment
            {
            return;
            //do we need to handle this? maybe....
            }

        // set null to prevent later usage in framework side
        // causes kern exec 3 panic...
        arg1 = arg2 = arg3 = NULL;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandleEntriesMovedL(
    TAny* aArg1, TAny* aArg2, TAny* aArg3 )
    {
    FUNC_LOG;
    TUid type;
    TMsvEntry tMoved;
    if( IsEventFromIpsSourceL( aArg1, type, tMoved ) )
        {
        //sent to draft can cause problems
        TAny* arg1=NULL;
        TAny* arg2=NULL;
        TAny* arg3=NULL;

        TFSMailEvent event( TFSMailboxUnavailable );

        RArray<TFSMailMsgId> array(KEventGranularity);
        CleanupClosePushL( array );
        FillFSMessageArrayL(
            array,
            static_cast<const CMsvEntrySelection*>(aArg1),
            tMoved.iMtm.iUid );

        arg1 = &array;
        TFSMailMsgId tempId1 = TFSMailMsgId( iPluginId, *(static_cast<TMsvId*>(aArg2) ) ); //target
        arg2 = &tempId1;
        TFSMailMsgId tempId2 = TFSMailMsgId( iPluginId, *(static_cast<TMsvId*>(aArg3)) ); //source
        arg3 = &tempId2;

        if( tMoved.iType == KUidMsvMessageEntry )
            {
            event = TFSEventMailMoved;
            }
        else if( tMoved.iType == KUidMsvFolderEntry )
            {
            event = TFSEventFoldersMoved;
            }

        TFSMailMsgId mbox;
        // solve mailbox.
        if ( tMoved.iMtm.iUid == KSenduiMtmSmtpUidValue )
            {
            TMsvEntry serv;
            TMsvId service;
            iSession->GetEntry( tMoved.iServiceId, service, serv );
            //get correct imap/pop service to tEntry
            iSession->GetEntry( serv.iRelatedId, service, serv );
            mbox = SymId2FsId( serv.iServiceId, serv.iMtm.iUid );
            }
        else
            {
            mbox = SymId2FsId( tMoved.iServiceId, tMoved.iMtm.iUid );
            }

        SendDelayedEventL(
            event,
            mbox,
            arg1,
            arg2,
            arg3 );

        CleanupStack::PopAndDestroy( &array );
        // set null to prevent later usage in framework side
        // causes kern exec 3 panic...
        arg1 = arg2 = arg3 = NULL;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandleEntriesDeletedL(
    TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/ )
    {
    FUNC_LOG;

    if ( !aArg1 || !aArg2 )
        {
        // sometimes we get null pointers from symbian api
        return;
        }

    TAny* arg1=NULL;
    TAny* arg2=NULL;
    TAny* arg3=NULL;

    TFSMailEvent event( TFSMailboxUnavailable );

    //parent must be root for it to be mailbox
    if( (*(TMsvId*) (aArg2)) == KMsvRootIndexEntryIdValue )
        {
        CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>(aArg1);
        TFSMailMsgId id;
        TBool found = EFalse;
        TInt count = iIPSAccounts.Count();
        TInt selCount = sel->Count();

        for(TInt i=count-1; i>=0; i--)
            {
            TMsvId mboxId = iIPSAccounts[i]->iMBoxId.Id();
            for (TInt j = 0; j < selCount; j++ )
                {
                TMsvId deletedId = sel->At(j);
                if ( deletedId == mboxId)
                    {
                    iIPSAccounts[i];
                    id.SetId( deletedId );
                    id.SetPluginId( iIPSAccounts[i]->iMBoxId.PluginId() );
                    delete iIPSAccounts[i];
                    iIPSAccounts.Remove(i);
                    found = ETrue;
                    break;
                    }
                }
            }

        if(!found)
            {
            //was some other service, ignore.
            return;
            }

        //check is the id of the deleted entry in our account list.

        event = TFSEventMailboxDeleted;
        SendDelayedEventL(
            event,
            id,
            arg1,
            arg2,
            arg3 );
        }
    else
        {
        //aArg2 is a service OR a folder
        TMsvEntry tEntry;
        TMsvId service;
        TBool isFolderId( EFalse );
        if( !iSession )
            {
            User::Leave( KErrNotReady );
            }
        iSession->GetEntry( (*(TMsvId*) (aArg2)), service, tEntry );

        if( tEntry.iMtm.iUid == iBasePlugin.MtmId().iUid )
            {
            CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>(aArg1);
	        TMsvId deletedId( KMsvNullIndexEntryIdValue );
	        TInt count = sel->Count();

    		if( count )
	            {
	            // In case we get several id's, ignore others than
	            // last one which is the actual message/root folder id.
	            deletedId = sel->At(count-1);
	            }

            // message in remote folder (inbox)
            TFSMailMsgId mbox(iPluginId, tEntry.iServiceId );
            RArray<TFSMailMsgId> array( KEventGranularity );
            CleanupClosePushL( array );
            TFSMailMsgId parentId( iPluginId, tEntry.Id() );

            if ( tEntry.iMtm.iUid == KSenduiMtmImap4UidValue )
                {
                isFolderId = MatchFolderIdFound( deletedId );
                }
            if( !isFolderId )
                {
                FindCorrectDeletedEntryAndParentL(
                    tEntry, *sel, parentId, array );

                if ( array.Count() > 0 )
                    {
                    arg1 = &array;
                    arg2 = &parentId;

                    event = TFSEventMailDeleted;
                    SendDelayedEventL(
                        event,
                        mbox,
                        arg1,
                        arg2,
                        arg3 );
                    }
                }
            else
                {
                array.AppendL( SymId2FsId(deletedId, tEntry.iMtm.iUid) );
                arg1 = &array;
                arg2 = &parentId;

                event = TFSEventFoldersDeleted;
                SendDelayedEventL(
                    event,
                    mbox,
                    arg1,
                    arg2,
                    arg3 );
                }

            CleanupStack::PopAndDestroy( &array );
            }
        else if ( tEntry.iMtm.iUid == KSenduiMtmSmtpUidValue )
            {
            // seems that case when deleding from draft, symbian
            // sends event that contains some child part's id of
            // correct deleted mail. So append parents id to array
            TFSMailMsgId msg;
            msg.SetId( tEntry.Id() );
            TFSMailMsgId parent;
            parent.SetId( tEntry.Parent() );
            if( !iSession )
                {
                User::Leave( KErrNotReady );
                }
            //re-load smtp service to tEntry
            iSession->GetEntry( tEntry.iServiceId, service, tEntry );
            //get correct imap/pop service to tEntry
            iSession->GetEntry( tEntry.iRelatedId, service, tEntry );

            TFSMailMsgId mbox;
            mbox.SetId( tEntry.Id() );

            // set plugin id to msg, parent and mbox
            if ( tEntry.iMtm.iUid == KSenduiMtmImap4UidValue )
        		{
        		parent.SetPluginId( KIpsPlgImap4PluginUid );
        		msg.SetPluginId( KIpsPlgImap4PluginUid );
        		mbox.SetPluginId( KIpsPlgImap4PluginUid );
        		}
    		else if ( tEntry.iMtm.iUid == KSenduiMtmPop3UidValue )
        		{
        		parent.SetPluginId( KIpsPlgPop3PluginUid );
        		msg.SetPluginId( KIpsPlgPop3PluginUid );
        		mbox.SetPluginId( KIpsPlgPop3PluginUid );
        		}
            else
                {
                // Should be panic at this point?
                return;
                }

            RArray<TFSMailMsgId> array(1);
            CleanupClosePushL( array );
            array.AppendL( msg );
            arg1 = &array;
            arg2 = &parent;
            event = TFSEventMailDeleted;
            SendDelayedEventL(
                event,
                mbox,
                arg1,
                arg2,
                arg3 );
            CleanupStack::PopAndDestroy( &array );
            }
        else
            {
            //deletion happened under some other service that IPS.
            return;
            }
        }
    // set null to prevent later usage in framework side
    // causes kern exec 3 panic...
    arg1 = arg2 = arg3 = NULL;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandleEntriesChangedL(
    TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/ )
    {
    FUNC_LOG;
    TAny* arg1=NULL;
    TAny* arg2=NULL;
    TAny* arg3=NULL;

    TInt errorCode( KErrNone ); // <qmail>

    TUid uId;
    TMsvEntry tChanged;
    if ( !IsEventFromIpsSourceL( aArg1, uId, tChanged ) )
        {
        return;
        }
//<Qmail>
    
//</Qmail>
    TFSMailEvent event = static_cast<TFSMailEvent>( KErrNotFound );

    RArray<TFSMailMsgId> array(1);
    CleanupClosePushL( array );

    if ( tChanged.iType == KUidMsvMessageEntry )
        {
        TMsvEmailEntry emailEntry(tChanged);
        if( emailEntry.DeletedIMAP4Flag() /*|| tChanged.Deleted()*/ )
            {
            event = TFSEventMailDeleted;
            }
        else
            {
            event = TFSEventMailChanged;
            }

        // message entry
        array.AppendL( SymId2FsId( tChanged )  );
        arg1 = &array;

        // parent entry
        TFSMailMsgId id = SymId2FsId( *(static_cast<TMsvId*>(aArg2)), tChanged.iMtm.iUid );
        arg2 = &id;

        }
    else if ( tChanged.iType == KUidMsvFolderEntry )
        {
        event = TFSEventFolderChanged;

        array.AppendL( SymId2FsId( tChanged ) );
        arg1 = &array;

        TFSMailMsgId id = SymId2FsId( *(static_cast<TMsvId*>(aArg2)), tChanged.iMtm.iUid );
        arg2 = &id;

        if ( tChanged.iMtm.iUid == KSenduiMtmImap4UidValue )
            {
            TMsvEmailEntry eml( tChanged );
            TInt index = iImapFolderIds.Find(tChanged.Id());
            if ( eml.LocalSubscription() && index == KErrNotFound )
                {
                iImapFolderIds.AppendL( tChanged.Id() );
                }
            else if ( !eml.LocalSubscription() && index != KErrNotFound )
                {
                iImapFolderIds.Remove( index );
                }
            }

        }
    else if( IsConnectionChangeL( aArg1, aArg2 ) )
        {
        if ( iIsConnected )
            {
            event = TFSEventMailboxOnline;
            }
        else
            {
            // <qmail> add error code
            event = TFSEventMailboxOffline;
            const CMsvEntrySelection* selection = static_cast<CMsvEntrySelection*>( aArg1 );
            TFSMailMsgId mailboxId( iPluginId, selection->At(0) );

            if ( !(iBasePlugin.HasOperations( mailboxId )) && iBasePlugin.ActivityTimerL( mailboxId ).IsActive() )
                {
                // considering this disconnection event as unexpected
                errorCode = KErrDisconnected;
                arg1 = &errorCode;
                }
            // </qmail>
            }
        }
    else if( IsAccountNameChangeL( aArg1, aArg2 ) )
        {
        event = TFSEventMailboxRenamed;
        }

    // send event only if we found correct one
    if ( event != KErrNotFound )
        {
        TFSMailMsgId mbox;
        // solve mailbox.
        if ( tChanged.iMtm.iUid == KSenduiMtmSmtpUidValue )
            {
            TMsvEntry serv;
            TMsvId service;
            if( !iSession )
                {
                User::Leave( KErrNotReady );
                }
            iSession->GetEntry( tChanged.iServiceId, service, serv );
            //get correct imap/pop service to tEntry
            iSession->GetEntry( serv.iRelatedId, service, serv );
            mbox = SymId2FsId( serv.iServiceId, serv.iMtm.iUid );
            }
        else
            {
            mbox = SymId2FsId( tChanged.iServiceId, tChanged.iMtm.iUid );
            }

        SendDelayedEventL( event,
            mbox, arg1, arg2, arg3 );
        }
    CleanupStack::PopAndDestroy( &array );
    // set null to prevent later usage in framework side
    // causes kern exec 3 panic...
    arg1 = arg2 = arg3 = NULL;
    }
//<cmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandleMediaChangedL( 
    TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/ )
    {
    TAny* arg1(NULL);
    TAny* arg2(NULL);
    TAny* arg3(NULL);    
    TMsvId service;
    TMsvEntry tChanged;
    TFSMailMsgId mbox;
    TFSMailEvent event( TFSEventMailboxDeleted );
    TInt startPoint(iIPSAccounts.Count()-1);

    if ( !aArg1 || !aArg2 )
        {
        return; 
        }
    TInt from(*(TInt*) (aArg1));
    TInt to(*(TInt*) (aArg2));
    if ( from > KMaxDriveUnitValue || to > KMaxDriveUnitValue )
        {
        // Possibly duplicate event 
        // or some other disinformation
        return; 
        }
    if( !iSession )
        {
        User::Leave( KErrNotReady );
        }

    CMsvEntry* cEntry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    
    CMsvEntrySelection* children = cEntry->ChildrenWithMtmL( MtmId() );
    CleanupStack::PushL( children );
        
    for( TInt accIndex(startPoint); accIndex>=0; accIndex-- )
        {
        // Remove all mailboxes, also the ones possibly added to 
        // array from current store to avoid duplicate boxes
        mbox.SetId( iIPSAccounts[accIndex]->iMBoxId.Id() );
        mbox.SetPluginId( iIPSAccounts[accIndex]->iMBoxId.PluginId() );
        iIPSAccounts.Remove(accIndex);
        SendDelayedEventL( event, mbox, arg1, arg2, arg3 );
        }
    for( TInt chdIndex(0); chdIndex<children->Count(); chdIndex++ )
        {
        // With clear table send mailboxes in current store
        // to observers to complete full update
        iSession->GetEntry( children->At(chdIndex), service, tChanged );
        mbox = SymId2FsId( tChanged );
        event = TFSEventNewMailbox;
        SendDelayedEventL( event, mbox, arg1, arg2, arg3 );
        }

    CleanupStack::PopAndDestroy( 2 );
    // set null to prevent later usage in framework side
    // causes kern exec 3 panic...
    arg1 = arg2 = arg3 = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TUid CIpsPlgEventHandler::MtmId() const
    {
    TUid ret = KSenduiMtmImap4Uid;
    if ( iBasePlugin.MtmId().iUid == KIpsPlgPop3PluginUidValue )
        {
        ret = KSenduiMtmPop3Uid;
        }
    return ret;
    }
//</cmail>
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TBool CIpsPlgEventHandler::IsConnectionChangeL( TAny* aArg1, TAny* aArg2 )
    {
    FUNC_LOG;
    TMsvId parent = (*(TMsvId*) (aArg2));

    //we check that parent is the root. if not, it cannot be a
    //event from service, thus can't be from connection change..
    if ( parent == KMsvRootIndexEntryId )
        {
        const CMsvEntrySelection* selection =
            static_cast<CMsvEntrySelection*>( aArg1 );

        TMsvEntry tEntry;
        TMsvId service;
        if( !iSession )
            {
            User::Leave( KErrNotReady );
            }
        if ( selection->Count() )
            {
            iSession->GetEntry( selection->At(0), service, tEntry );
            }

        if ( tEntry.Connected() == iIsConnected )
            {
            return EFalse;
            }
        else
            {
            //state has changed, update iIsConnected accordingly.
            iIsConnected = tEntry.Connected();
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::MailboxObserversL(
    TFSMailMsgId aMailbox,
    RPointerArray<MFSMailEventObserver>& aObservers )
    {
    FUNC_LOG;
    TInt count = iMBoxObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
          CMBoxEventObserver* observer = iMBoxObservers[i];
          if ( observer && observer->iMBoxId == aMailbox )
            {
            aObservers.AppendL( &iMBoxObservers[i]->iObserver );
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsPlgEventHandler::IsAccountNameChangeL( TAny* aArg1, TAny* aArg2 )
    {
    FUNC_LOG;
    TMsvId parent = (*(TMsvId*) (aArg2));

    TBool ret = EFalse;

    //we check that parent is the root. if not, it cannot be a
    //event from service, thus can't be from account name change
    if ( parent == KMsvRootIndexEntryId )
        {
        const CMsvEntrySelection* selection =
            static_cast<CMsvEntrySelection*>( aArg1 );

        TMsvEntry tEntry;
        TMsvId service;
        if( !iSession )
            {
            User::Leave( KErrNotReady );
            }
        if ( selection->Count() )
            {
            iSession->GetEntry( selection->At(0), service, tEntry );
            }

        TInt count = iIPSAccounts.Count();
        for( TInt i=0; i<count; i++ )
            {
            if( iIPSAccounts[i]->iMBoxId.Id() == tEntry.Id() )
                {
                if( iIPSAccounts[i]->iName->CompareC( tEntry.iDetails ) != 0 )
                    {
                    //account has been renamed
                    delete iIPSAccounts[i]->iName;
                    iIPSAccounts[i]->iName=NULL;
                    iIPSAccounts[i]->iName = tEntry.iDetails.AllocL();
                    ret = ETrue;
                    break;
                    }
                }
            }
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::AppendSettingsObserverL(
    TFSMailMsgId aAccount,
    TUint32 aSettingKey )
    {
    FUNC_LOG;
    // recycle setting observers
    TInt find = KErrNotFound;
    for ( TInt i = 0; i < iIPSSettingsObservers.Count(); i++ )
        {
        if ( iIPSSettingsObservers[i]->MailboxId() == aAccount )
            {
            find = i;
            break;
            }
        }

    RPointerArray<MFSMailEventObserver> observers;
    MailboxObserversL( aAccount, observers );

    if ( find != KErrNotFound )
        {
        iIPSSettingsObservers[find]->SetKeyAndActivateL( aSettingKey, observers );
        }
    else
        {
// <qmail>
        // commented out from qmail when extented settings 
        // cen rep not exist in environment
        /*CIpsPlgSettingsObserver* obs =
            CIpsPlgSettingsObserver::NewL( aAccount, *iCenRep, *this );
        obs->SetKeyAndActivateL( aSettingKey, observers );
        iIPSSettingsObservers.AppendL( obs );*/
// </qmail>
        }
    observers.Close();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::RemoveSettingsObserver( TFSMailMsgId aMailboxId )
    {
    FUNC_LOG;
    TInt count = iIPSSettingsObservers.Count();
    for( TInt i=0; i < count; i++ )
        {
        if( iIPSSettingsObservers[i]->MailboxId() == aMailboxId )
            {
            iIPSSettingsObservers[i]->UnActivate();
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
MFSMailEventObserver* CIpsPlgEventHandler::MailboxObserverL(
    TFSMailMsgId aMailbox )
    {
    FUNC_LOG;
    TInt count = iMBoxObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        if ( iMBoxObservers[i]->iMBoxId == aMailbox )
            {
            return &iMBoxObservers[i]->iObserver;
            }
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsPlgEventHandler::AccountExistsL( const TMsvEntry& aEntry )
    {
    FUNC_LOG;
    if ( !(aEntry.iMtm.iUid == KSenduiMtmImap4UidValue
        || aEntry.iMtm.iUid == KSenduiMtmPop3UidValue) )
        {
        // check only imap and pop
        return ETrue;
        }

    TInt count = iIPSAccounts.Count();

    for( TInt i=0; i<count; i++ )
        {
        TInt a = aEntry.Id();
        TInt b = iIPSAccounts[i]->iMBoxId.Id();

        if( a == b )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::FindCorrectDeletedEntryAndParentL(
    const TMsvEmailEntry& aParent,
    const CMsvEntrySelection& aDeletedIds,
    TFSMailMsgId& aFSParent,
    RArray<TFSMailMsgId>& aFSDeletedArray )
    {
    FUNC_LOG;
    if ( aParent.iType.iUid == KUidMsvRootEntryValue )
        {
        // stop recursion
        return;
        }

    // only pop or imap types are alloved beyond this point
    __ASSERT_DEBUG( ( aParent.iMtm.iUid == KSenduiMtmImap4UidValue ||
            aParent.iMtm.iUid == KSenduiMtmPop3UidValue ),
        User::Panic( KIpsPlgEventHandlerPanic, KErrGeneral ) );

    TBool doRecursion = EFalse;

    if ( aParent.iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        if ( aParent.iType.iUid == KUidMsvServiceEntryValue )
            {
            // return in case of service / root entries to avoid
            // forever recursion
            return;
            }
        else if ( aParent.iType.iUid != KUidMsvFolderEntryValue
            || !aParent.LocalSubscription() )
            {
            // some message part is deleted and main message still appears
            // on msg store, this case happens when messages are deleted
            // in offline state
            doRecursion = ETrue;
            }
        }
    else
        {
        // POP
        if ( aParent.iType.iUid != KUidMsvServiceEntryValue )
            {
            if ( iBasePlugin.GetSyncStateHandler().GetMailboxIpsState(
                    aParent.iServiceId ) != KIpsSosEmailSyncStarted )
                {
                // message entries parent must be service in case of POP
                doRecursion = ETrue;
                }
            }
        }


    if ( !doRecursion )
        {
        aFSParent.SetPluginId( TUid::Uid( iPluginId ) );
        aFSParent.SetId( aParent.Id() );
        for ( TInt i = 0; i < aDeletedIds.Count(); i++ )
            {
            aFSDeletedArray.AppendL(
                    TFSMailMsgId( iPluginId, aDeletedIds.At(i) ) );
            }
        }
    else
        {
        TMsvEmailEntry eml;
        TMsvId dummy;
        if ( iSession && iSession->GetEntry( aParent.Parent(), dummy, eml )
                == KErrNone )
            {
            CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection();
            CleanupStack::PushL( sel );
            sel->AppendL( aParent.Id() );
            // recursive call
            FindCorrectDeletedEntryAndParentL(
                eml, *sel, aFSParent, aFSDeletedArray );
            CleanupStack::PopAndDestroy( sel );
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::SignalStartSyncL( const TFSMailMsgId& aAccount )
    {
    FUNC_LOG;
    // <qmail> no need to create event variable here
    //    TFSMailEvent event = TFSEventMailboxSyncStateChanged;
    // <qmail> renamed variable
    TSSMailSyncState syncState( StartingSync );

    // <qmail> remove SaveSyncStatusL as it does nothing
    // SaveSyncStatusL( aAccount.Id(), TInt( syncState ) );

    SendDelayedEventL(
        TFSEventMailboxSyncStateChanged,
        aAccount,
        &syncState,
        NULL,
        NULL );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::SignalSyncCompletedL( const TFSMailMsgId& aAccount, TInt aError )
    {
    FUNC_LOG;
    // <qmail> no need to create event variable here
    //    TFSMailEvent event = TFSEventMailboxSyncStateChanged;
    // <qmail> renamed variable, and setting value in new func
    TSSMailSyncState syncState = ConvertCompletionCode( aError );

    // <qmail> remove SaveSyncStatusL as it does nothing
    //    SaveSyncStatusL( aAccount.Id(), TInt( syncState ) );

    SendDelayedEventL(
        TFSEventMailboxSyncStateChanged,
        aAccount,
        &syncState,
        NULL,
        NULL );
    }

// <qmail> new function
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TSSMailSyncState CIpsPlgEventHandler::ConvertCompletionCode( TInt aCompletionCode )
    {
    FUNC_LOG;
    INFO_1("completioncode == %d", aCompletionCode);
    switch ( aCompletionCode )
        {
        case KErrNone:
            return FinishedSuccessfully;
        case KErrCancel:
            return SyncCancelled;
        case KErrImapBadLogon:
        case KPop3InvalidUser:
        case KPop3InvalidLogin:
        case KPop3InvalidApopLogin:
            return PasswordNotVerified;
        case -5120: // (DndTimedOut) wrong server name in settings
        case KErrHostUnreach: // wrong port number
            return ServerConnectionError;
        case KErrTimedOut:
        default:
            return SyncError;
        }
    }
// </qmail>

//<qmail> not used at all
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//void CIpsPlgEventHandler::SignalMailboxOfflineStateL(
//    const TFSMailMsgId& aAccount )
//    {
//    FUNC_LOG;
//    SendDelayedEventL(
//        TFSEventMailboxOffline,
//        aAccount,
//        NULL,
//        NULL,
//        NULL );
//    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::SettingsChangedL( TFSMailMsgId /*aAccount*/ )
    {
    FUNC_LOG;
    }

//<qmail> not used at all
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//void CIpsPlgEventHandler::SaveSyncStatusL( TMsvId aMailboxId, TInt /*aState*/ )
//    {
//    FUNC_LOG;
//    TMsvEntry tEntry;
//    TMsvId service;
//    if( !iSession )
//        {
//        User::Leave( KErrNotReady );
//        }
//    TInt err = iSession->GetEntry( aMailboxId, service, tEntry );
//
//    if( err == KErrNone )
//        {
//        }
//    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::HandlePropertyEventL(
    TInt aEvent,
    TInt aMailbox,
    TInt /*aPluginId*/,
    TInt /*aError*/ )
    {
    FUNC_LOG;
//  <qmail> remove these events 
//    RProcess process;
//    if ( aEvent == EIPSSosPswErr && process.SecureId() == FREESTYLE_FSSERVER_SID )
//        {
//        TFSMailMsgId mbox = SymId2FsId( aMailbox,
//                        iBasePlugin.MtmId().iUid );
//        TFSMailEvent event = TFSEventException;
//        TFsEmailNotifierSystemMessageType msg = EFsEmailNotifErrLoginUnsuccesfull;
//        SendDelayedEventL( event, mbox, &msg, NULL , (MFSMailExceptionEventCallback*)this );
//        }
//    else if ( aEvent == EIPSSosCredientialsSet || aEvent == EIPSSosCredientialsCancelled )
//        {
//        if ( iConnOpCallback )
//            {
//            iConnOpCallback->CredientialsSetL( aEvent );
//
//            //Set to null after we have used this.
//            //don't delete, we don't own this.
//            iConnOpCallback=NULL;
//            }
//        //if password was changed, we need to send settings changed event also.
//        if( aEvent == EIPSSosCredientialsSet )
//            {
//            TFSMailMsgId mbox = SymId2FsId( aMailbox,
//                    iBasePlugin.MtmId().iUid );
//            TFSMailEvent event = TFSEventMailboxSettingsChanged;
//            SendDelayedEventL( event, mbox, NULL, NULL , NULL );
//            }
//        }
//    else
// </qmail>
    if ( aEvent == EIPSSosSettingsChanged )
        {
        TFSMailMsgId mbox = SymId2FsId( aMailbox,
                iBasePlugin.MtmId().iUid );
        TFSMailEvent event = TFSEventMailboxSettingsChanged;
        SendDelayedEventL( event, mbox, NULL, NULL , NULL );
        }

// <qmail> no need for KIpsSosEmailSyncCompleted event handling here,
// because it has just been sent in NotifyPropertyEventL()
    }

// <qmail> removing unused functions
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//void CIpsPlgEventHandler::QueryUsrPassL( TMsvId aMbox, MIpsPlgConnectOpCallback* aCallback )
//    {
//    FUNC_LOG;
//    iConnOpCallback = aCallback;//can be null, doesn't matter.
//    SetNewPropertyEvent( aMbox, EIPSSosPswErr, 0 );
//    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//void CIpsPlgEventHandler::SignalCredientialsSetL( TInt aMailboxId, TBool aCancelled )
//    {
//    FUNC_LOG;
//    TInt event = EIPSSosCredientialsSet;
//    if ( aCancelled )
//        {
//        event = EIPSSosCredientialsCancelled;
//        }
//
//    SetNewPropertyEvent( aMailboxId, event, 0 );
//    }
// </qmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::SetFolderIdToArrayL( TMsvId aNewId )
    {
    FUNC_LOG;
    TBool idInArray( EFalse );

    for( TInt i=0; i<iImapFolderIds.Count(); i++ )
        {
        if( iImapFolderIds[i] == aNewId )
            {
            idInArray = ETrue;
            }
        }
    if( !idInArray )
        {
        iImapFolderIds.AppendL( aNewId );
        }
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsPlgEventHandler::MatchFolderIdFound( TMsvId aDeletedId )
    {
    FUNC_LOG;
    TBool match( EFalse );

    for ( TInt i(iImapFolderIds.Count()-1); i >= 0; i-- )
        {
        if( aDeletedId == iImapFolderIds[i] )
            {
            iImapFolderIds.Remove( i );
            match = ETrue;
            }
        }
    return match;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgEventHandler::CollectSubscribedFoldersL( TMsvId /*aMailboxId*/ )
    {
    FUNC_LOG;
// <qmail> code removed as it does nothing; keeping the func as it probably will be needed
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsPlgEventHandler::IsMailboxOrMediaEvent( MMsvSessionObserver::TMsvSessionEvent aEvent, 
                                           TAny* aArg )
{
   TBool ret(EFalse); 
   if ( aArg ) 
       {
       if ( (*(TMsvId*)(aArg)) == KMsvRootIndexEntryIdValue 
             || aEvent == EMsvMediaChanged ) 
           {
           ret = ETrue;
           }
   }
   return ret;
}

// <qmail> not needed
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//void CIpsPlgEventHandler::ExceptionEventCallbackL(
//        TFSMailMsgId aMailboxId,
//        TInt /*aEventType*/,
//        TBool /*aResponse*/ )
//    {
//    FUNC_LOG;
//    TFSMailEvent event = TFSEventMailboxSyncStateChanged;
//    TSSMailSyncState state = PasswordNotVerified;
//
//    SendDelayedEventL( event, aMailboxId, &state, NULL , NULL );
//    }
// </qmail>

// End of File

