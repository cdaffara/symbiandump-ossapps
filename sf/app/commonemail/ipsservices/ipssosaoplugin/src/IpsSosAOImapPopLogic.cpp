/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Main plugin class
*
*/

#include "ipssosaopluginheaders.h"

const TInt KIpsSosAOImapPopLogicDefGra = 1;
const TInt KMailboxCreatedTimeout = 500000; // 0.5 sec

// ----------------------------------------------------------------------------
// class CIpsSosAOImapPopLogic
// ----------------------------------------------------------------------------
//
CIpsSosAOImapPopLogic::CIpsSosAOImapPopLogic( CMsvSession& aSession ) 
    : iSession( aSession ), iMailboxLogics( KIpsSosAOImapPopLogicDefGra ),
    iNoNWOpsAllowed( EFalse )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOImapPopLogic::~CIpsSosAOImapPopLogic()
    {
    FUNC_LOG;
    iMailboxLogics.ResetAndDestroy();
   // iActiveEvents.ResetAndDestroy();
    delete iEmnResolver;
	//<QMail>

	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOImapPopLogic* CIpsSosAOImapPopLogic::NewL( CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSosAOImapPopLogic* self = new (ELeave) CIpsSosAOImapPopLogic(
            aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::ConstructL()
    {
    FUNC_LOG;
    iEmnResolver = CIpsSosAOEMNResolver::NewL();
	//<QMail>

	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOImapPopLogic::HandleAOServerCommandL( 
    TInt aCommand, TDesC8* aParameters )
    
    {
    
    FUNC_LOG;
    TInt result = KErrNone;

    TInt mailboxId = KErrNotFound;
            
    // Make sure that the parameter length matches Id length
    if ( aParameters && aParameters->Length() == sizeof( TMsvId ) )
        {
        TPckgBuf<TMsvId> paramPack;
        paramPack.Copy( *aParameters );
        
        // Get the mailbox id from the packet
        mailboxId = paramPack();
        }
    
    switch ( aCommand )
        {
        case EAOManagerPluginStart:
            RefreshMailboxListL( );
            if ( !iNoNWOpsAllowed )
                {
                SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStart );
                }
            break;
            
        case EAOManagerPluginStop:
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStop );
            // Return value does not matter, plugin will be stopped anyway.
            break;

        case EAOManagerNWOpsNotAllowed:
            iNoNWOpsAllowed = ETrue;
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStop );
            break;
            
        case EAOManagerNWOpsAllowed:
            iNoNWOpsAllowed = EFalse;
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStart );
            break;
            
        case EAOManagerStartedRoaming:
            SendCommandToMailboxesL( 
                    CIpsSosAOMBoxLogic::ECommandStartedRoaming );
            break;
            
        case EAOManagerStoppedRoaming:
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStoppedRoaming );
            break;
            
        case EAOManagerDiskSpaceAboveCritical:
            if ( !iNoNWOpsAllowed )
                {
                SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStart );
                }
            break;
            
        case EAOManagerDiskSpaceBelowCritical:
            SendCommandToMailboxesL( CIpsSosAOMBoxLogic::ECommandStop );
            break;
        case EAOManagerSuicideQuery:
            // Always tell to server, that we don't 
            // want to make a suicide.
            result = EFalse;
            break;
        // Mailbox handling
        case EAOManagerMailboxAgentSuspend:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandStop );
            break;
        case EAOManagerMailboxAgentResume:
            RefreshMailboxListL( );
            if ( !iNoNWOpsAllowed )
                {
                SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandStart );
                }
            break;
        case EAOManagerMailboxAgentRemove:
            StopAndRemoveMailboxL( mailboxId );
            //cancel all
            break;
        case EAOManagerEMNReceived:
            if ( aParameters )
                {
                HandleEMNMessageL( *aParameters );
                }
            break;
        case EAOManagerMailboxAgentUpdateMailWhileConnected:
            // Not supported atm, was in old plugin Imap-idle
            break;
        case EAOManagerQueryState:
            result = QueryMailboxStatus( *aParameters );
            break;
        case EAOManagerCancelAllAndDisconnect:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandCancelAndDisconnect );
            break;
        case EAOManagerCancelAllAndDoNotDisconnect:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandCancelDoNotDiconnect );
            break;
        case EAOManagerSuspend:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandSuspend );
            break;
        case EAOManagerContinue:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandContinue );
            break;
        case EAOManagerDoNotDisconnect:
            SendCommandToSpecificMailboxL( mailboxId, 
                    CIpsSosAOMBoxLogic::ECommandDoNotDisconnect );
            break;
        // error in CActiveScheduler
        // EAOManagerAOSchdulerError not handled because 
        // there's no badly behaving active objects
        case EAOManagerAOSchdulerError:
        default:    
            result = KErrNotSupported;
            break;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
void CIpsSosAOImapPopLogic::HandleMsvSessionEventL(
     MMsvSessionObserver::TMsvSessionEvent aEvent, 
     TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/ )
    {
    FUNC_LOG;
    switch( aEvent )
        {
        case MMsvSessionObserver::EMsvEntriesCreated:
            handleEntriesCreatedL(aArg1);
            break;
        case MMsvSessionObserver::EMsvEntriesChanged:
            {
            TMsvId parent = (*(TMsvId*) (aArg2));
            //we check that parent is the root. if not, it cannot be an 
            //event from service, thus can't be from connection change..
            if ( parent == KMsvRootIndexEntryId )
                {
                const CMsvEntrySelection* selection = 
                    static_cast<CMsvEntrySelection*>( aArg1 );
                
                TMsvEntry tEntry;
                TMsvId service;
                if ( selection->Count() )
                    {
                    iSession.GetEntry( selection->At(0), service, tEntry );
                    }
                
                if ( !tEntry.Connected() )
                    {
                    SendCommandToSpecificMailboxL( 
                            tEntry.Id(), 
                            CIpsSosAOMBoxLogic::ECommandClearDoNotDisconnect );
                    }
                }
            }
            break;
        case MMsvSessionObserver::EMsvEntriesDeleted:
            {
            TMsvId parent = (*(TMsvId*) (aArg2));
            //we check that parent is the root. if not, it cannot be an 
            //event indicating deleted mailbox entry
            if ( parent == KMsvRootIndexEntryId )
                {
                RemoveOrphanLogicsL();
                }
            }
            break;
        case MMsvSessionObserver::EMsvEntriesMoved:
        default:
            break;
        };
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::SendCommandToMailboxesL( 
        TInt aCommand )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iMailboxLogics.Count(); i++ )
        {
        iMailboxLogics[i]->SendCommandL( aCommand );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::StopAndRemoveMailboxL( TMsvId aMailboxId )
    {
    FUNC_LOG;
    TInt index = GetMailboxLogicIndex( aMailboxId );
    
    if ( index != KErrNotFound )
        {
        SendCommandToSpecificMailboxL( aMailboxId, 
                CIpsSosAOMBoxLogic::ECommandStop );
        delete iMailboxLogics[index];
        iMailboxLogics.Remove( index );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::SendCommandToSpecificMailboxL( 
        TMsvId aMailboxId, 
        TInt aCommand )
    {
    FUNC_LOG;
    TInt index = GetMailboxLogicIndex( aMailboxId );
    
    if ( index != KErrNotFound )
        {
        iMailboxLogics[index]->SendCommandL( aCommand ); // faulty CS warning
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::HandleEMNMessageL(
    const TDesC8& aParameters )
    {
    FUNC_LOG;
    TEMNElement elements;

    if ( iEmnResolver->ParameterDispatchTEMNElement( 
        aParameters, elements ) == KErrNone )
        {
        
        TInt index = iEmnResolver->FindEMNMailbox( 
                elements.mailbox, iMailboxLogics );
        
        if ( index != KErrNotFound )
            {
            CIpsSosAOMBoxLogic* logic = iMailboxLogics[index];
            
			if ( !logic->FirstEMNReceived() )
                {
                logic->SetFirstEMNReceived();
                }
            if ( !iNoNWOpsAllowed && 
                 !logic->IsMailboxRoamingStoppedL() )
                {
                SendCommandToSpecificMailboxL( 
                    logic->GetMailboxId(), // faulty CS warning
                    CIpsSosAOMBoxLogic::ECommandStartSync );
                }
            else
                {
                logic->SetEmnReceivedFlagL( ETrue );                
                }
            logic = NULL;
            }

        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::RefreshMailboxListL( )
    {
    FUNC_LOG;
    CMsvEntry* cEntry = iSession.GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    
    CMsvEntrySelection* childEntries = cEntry->ChildrenWithMtmL( KSenduiMtmPop3Uid );
    CleanupStack::PushL( childEntries );
    UpdateLogicArrayL( *childEntries );
    CleanupStack::PopAndDestroy( childEntries );
    
    childEntries = cEntry->ChildrenWithMtmL( KSenduiMtmImap4Uid );
    CleanupStack::PushL( childEntries );
    UpdateLogicArrayL( *childEntries );
    CleanupStack::PopAndDestroy( childEntries );
    CleanupStack::PopAndDestroy( cEntry );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::UpdateLogicArrayL( 
        const CMsvEntrySelection& aChilds )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < aChilds.Count(); i++ )
        {
        TBool found = EFalse;
        TMsvId mboxId = aChilds.At(i);
        for ( TInt j = 0; !found && j < iMailboxLogics.Count(); j++  )
            {
            if ( iMailboxLogics[j]->GetMailboxId() == mboxId )
                {
                found = ETrue;
                }
            }
		//<QMail>
        CIpsSosAOSettingsHandler* settings = 
                 CIpsSosAOSettingsHandler::NewL(iSession, mboxId);
        CleanupStack::PushL(settings);
         
        if ( !found  && ( settings->AlwaysOnlineState() 
                != IpsServices::EMailAoOff || 
                    settings->EmailNotificationState() 
                != IpsServices::EMailEmnOff || 
                    !settings->FirstEmnReceived() )
                 )
            {
            CIpsSosAOMBoxLogic* newLogic = CIpsSosAOMBoxLogic::NewL( 
                    iSession, mboxId );
            CleanupStack::PushL( newLogic );
            iMailboxLogics.AppendL( newLogic );
            CleanupStack::Pop( newLogic );
            }
        else if ( found && settings->AlwaysOnlineState() 
                == IpsServices::EMailAoOff &&
                    settings->EmailNotificationState() 
                == IpsServices::EMailEmnOff &&
                    settings->FirstEmnReceived() )
            {
            StopAndRemoveMailboxL( mboxId );
            }
        
        CleanupStack::PopAndDestroy(settings);
		//</QMail>
        }
    
    //finally, check for orphans ( mailbox has been deleted )
    RemoveOrphanLogicsL();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOImapPopLogic::QueryMailboxStatus( TDesC8& aParameter )
    {
    FUNC_LOG;
    TPckgBuf<RMessage2> param;
    param.Copy( aParameter );
    // get ipc message from param
    RMessage2 msg = param();
    TPckgBuf<TMsvId> mboxBuf;
    msg.Read(0, mboxBuf);
    
    TMsvId mbox = mboxBuf();
    
    TInt error = KErrNone;
    
    TInt index = GetMailboxLogicIndex( mbox );


    if ( index != KErrNotFound )
        {
        TInt state = iMailboxLogics[index]->GetCurrentState(); // faulty CS warning

        if ( state == CIpsSosAOMBoxLogic::EStateSyncOngoing
          || state == CIpsSosAOMBoxLogic::EStateFetchOngoing )
            {
            TPckgBuf<TInt> event( EIpsAOPluginStatusSyncStarted );
            msg.Write(1, event );
            }
        else
            {
            TPckgBuf<TInt> event( EIpsAOPluginStatusSyncCompleted );
            msg.Write(1, event );
            }
        }
    else
        {
        error = KErrNotFound;
        }
    
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSosAOImapPopLogic::GetMailboxLogicIndex( TMsvId aMailboxId )
    {
    FUNC_LOG;
    TInt index = KErrNotFound;
    for ( TInt i = 0; 
            index == KErrNotFound && i < iMailboxLogics.Count(); i++ )
        {
        if ( iMailboxLogics[i]->GetMailboxId() == aMailboxId )
            {
            index = i;
            }
        }
    return index;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOImapPopLogic::RemoveOrphanLogicsL()
    {
    CMsvEntry* cEntry = iSession.GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( cEntry );
    
    CMsvEntrySelection* popEntries = cEntry->ChildrenWithMtmL( KSenduiMtmPop3Uid );
    CleanupStack::PushL( popEntries );
    
    CMsvEntrySelection* imapEntries = cEntry->ChildrenWithMtmL( KSenduiMtmImap4Uid );
    CleanupStack::PushL( imapEntries );
        
        
    TInt count = iMailboxLogics.Count();
    
    for(TInt i=count-1; i>-1;i--)
        {
        if( popEntries->Find(iMailboxLogics[i]->GetMailboxId()) == KErrNotFound &&
            imapEntries->Find(iMailboxLogics[i]->GetMailboxId()) == KErrNotFound)
            {
            StopAndRemoveMailboxL( iMailboxLogics[i]->GetMailboxId() );
            }
        }
    
    CleanupStack::PopAndDestroy( 3, cEntry );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapPopLogic::handleEntriesCreatedL(const TAny* aArg1)
    {
    FUNC_LOG;
    TMsvEntry entry;
    getFirstEntryFromSelectionL(static_cast<const CMsvEntrySelection*>(aArg1), entry);

    if( entry.iMtm==KSenduiMtmImap4Uid || entry.iMtm==KSenduiMtmPop3Uid)
        {
        // handling imap4 or pop3 entry
        if(entry.iType.iUid==KUidMsvServiceEntryValue)
            {
            // new mailbox has been created, starting sync for it
            User::After(KMailboxCreatedTimeout); // sleep for 0.5 sec; guarantees that mailbox is fully set up, and message server ready to serve it
            RefreshMailboxListL();
            SendCommandToSpecificMailboxL( entry.Id(), CIpsSosAOMBoxLogic::ECommandStart );
            SendCommandToSpecificMailboxL( entry.Id(), CIpsSosAOMBoxLogic::ECommandStartSync );
            }
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapPopLogic::getFirstEntryFromSelectionL(const CMsvEntrySelection* aSelection, TMsvEntry& aEntry)
    {
    FUNC_LOG;
    if ( aSelection->Count() == 0)
        {
        User::Leave( KErrArgument );
        }

    TMsvId dummy( KMsvNullIndexEntryIdValue );
    User::LeaveIfError( iSession.GetEntry( aSelection->At(0), dummy, aEntry ) );
    }

// End of file

