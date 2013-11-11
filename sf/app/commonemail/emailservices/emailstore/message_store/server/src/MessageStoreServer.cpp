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
* Description:  Message store server implementation.
*
*/



// ========
// INCLUDES
// ========

#include <e32svr.h>
#include <bautils.h>
#include <driveinfo.h>
#include <s32file.h>

#include "MsgStoreTypes.h"
#include "MsgStorePropertyKeys.h"
#include "emailstoreuids.hrh"
#include "MessageStoreServer.h"
#include "MessageStoreSession.h"
#include "ContainerStore.h"
#include "PropertiesSerializer.h"
#include "ShutdownServer.h"
//<cmail>
#include "ImsPointsecMonitor.h"
#include "ImsPointsecObserver.h"
#include "emailstorepskeys.h" // Support for on-the-fly upgrade
//<qmail> removing #include "emailshutdownconst.h"
//</cmail>

// =========
// CONSTANTS
// =========

_LIT8( KNullDescriptor8, "" );


_LIT( KDbFilename, "message_store.db" );

const TMsgStoreCounts KInitialCounts = {0, 0};

// This should be more than sufficient.
const TUint KQuickPropertiesMaxLength = 1024;

const TUint KCommandLineSize = 30;

_LIT( KUninstallParameter, "IMS_UNINSTALL" );
_LIT( KImsUninstaller, "IMS Uninstaller" );
_LIT( KUpgradeDetectionFile, "c:\\System\\EsIms\\canary.txt" );




// SID list
// This is the list of secure Ids that are allowed to use the message store.  This list must be terminated
// with zero.

/*
const TUint32 KAllowedSecureIds[] =
        {
        0x200029E4,  // bridge process
        0x10274F51,  // IMS process
        0x1000EAEA,  // test automation executable
        0x20009C56,  // another automation test executable
        0x200029E6,  // message store exerciser
        0x20003C0A,
        0x20003C19,
        0x101FD64C,  // Idle server (for active idle plugin)
        0xA0002879,  // Native Composer Screen
        0x101F9A02,	 // DMHostServer1
		0x101F9A03,	 // DMHostServer2
		0x101F9A04,	 // DMHostServer3
		0x101F9A05,	 // DMHostServer4

        // UNIT TESTERS
#ifdef _DEBUG
        0x043ECF25,  // message store unit tester
        0x200029EC,
        0x200029EE,
        0x200029F0,
        0x200029F1,
        0x200029F2,
        0x200029F3,
        0x20004205,
#endif

        0
        }; // end KAllowedSecureIds
*/

// -----------------------------------------------------------------------------
// DoUninstallL
// Remove the files in private directory.
// -----------------------------------------------------------------------------
static void DoUninstallL()
    {
	RFs fs;
    if( fs.Connect() == KErrNone )
        {
        if( ! BaflUtils::FileExists( fs, KUpgradeDetectionFile ) )
            {
        	// This is a full uninstallation because the upgrade detection file
        	// does not exist !!
            TFileName privatePath;
            fs.PrivatePath( privatePath );
            
            CFileMan* fm = CFileMan::NewL( fs );
            CleanupStack::PushL( fm );
            TInt err = fm->RmDir( privatePath );
            CleanupStack::PopAndDestroy( fm );
            } // end if
            
        fs.Close();
        } // end if
}

// ==========================================================================
// Server startup code
// ==========================================================================

// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
static void RunServerL( TBool aThreadRendezvous )
  {
  // naming the server thread after the server helps to debug panics
  User::LeaveIfError( User::RenameThread( KMsgStoreServerName ) );

  // create and install the active scheduler we need
  CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
  CleanupStack::PushL( scheduler );
  CActiveScheduler::Install( scheduler );
  
  // Examine the command line parameters, to determine whether to
  // run the server as normal, or to perform the uninstall operation
  // and exit.
  
  TBuf<KCommandLineSize> commandLine;
  if( User::CommandLineLength() > 0 && User::CommandLineLength() < KCommandLineSize )
      {        
      User::CommandLine( commandLine );
      }
  
  // If the executable was launched with the "IMS_UNINSTALL" parameter then do not
  // launch the server, but just performs necessary uninstallation tasks.
  if( commandLine.Length() > 0 && commandLine.Compare( KUninstallParameter ) == 0 )
      {
	  // naming the server thread after the server helps to debug panics
      User::RenameThread( KImsUninstaller );

      // This is an uninstall.  Do not run the server.
      DoUninstallL();
      }
  else
  {
	  // Create the upgrade detection file, if necessary.
      RFs fs;
      if( (fs.Connect() == KErrNone) && !BaflUtils::FileExists( fs, KUpgradeDetectionFile ) )
      {            
    	  TRAP_IGNORE( /*result,*/ BaflUtils::EnsurePathExistsL( fs, KUpgradeDetectionFile ) );
	        
    	  RFile file;
    	  /*result =*/ file.Create( fs, KUpgradeDetectionFile, EFileWrite );
    	  file.Close();

    	  fs.Close();
      } // end if

	  // create the server (leave it on the cleanup stack)
	  CMessageStoreServer* server = CMessageStoreServer::NewLC();
	
	  // Initialisation complete, now signal the client
	  if( aThreadRendezvous )
	    {    
	    RThread::Rendezvous( KErrNone );
	    }
	  else
	    {
	    RProcess::Rendezvous( KErrNone );    
	    }
	  
	  // Ready to run
	  // Start wait loop, will return when server is shutdown
	  CActiveScheduler::Start();
	  
	  // Cleanup the server
	  CleanupStack::PopAndDestroy( server );
  }

  // Cleanup scheduler after shutdown
  CleanupStack::PopAndDestroy( scheduler );

  __LOG_LEAKED_OBJECTS
  
  } // end RunServerL

// ==========================================================================
// Server thread main function
// ==========================================================================
EXPORT_C TInt MessageStoreServerThreadFunction( TAny* aParams )
  {
  __UHEAP_MARK;
  CTrapCleanup* cleanup = CTrapCleanup::New();

  TInt ret = KErrNoMemory;
  if( cleanup )
    {
	// At one time, Message Store runs as a dll with Bridge process, but it is
	// decided that MsgStore running as a seperate process in case Bridge process
	// crashes makes more sense
	// (aParams == NULL) is basically to differentiate dll or exe
    TRAP( ret, RunServerL( (!aParams) ) );
    delete cleanup;
    }
  __UHEAP_MARKEND;

  return ret;
  } // end MessageStoreServerThreadFunction

// ==========================================================================
// RMessagePtr2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// ==========================================================================
void PanicClient(const RMessagePtr2& aMessage, TMessageStorePanic aPanic)
  {
  _LIT( KPanic, "MessageStoreServer" );
  aMessage.Panic( KPanic, aPanic);
  } // end PanicClient

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewLC
// ==========================================================================
CMessageStoreServer* CMessageStoreServer::NewLC()
  {
  CMessageStoreServer* self = new (ELeave) CMessageStoreServer();
  CleanupStack::PushL( self );
  self->ConstructL();
  return self;
  } // end NewLC

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMessageStoreServer::CMessageStoreServer()
  :CPolicyServer( 0, KMessageStorePolicy, ESharableSessions )
  {
  __LOG_CONSTRUCT( "msg", "CMessageStoreServer" )
  
  iLockedByBackupRestore = EFalse;
  iLockedByPointSec = EFalse;
  
  } // end constructor

// ==========================================================================
// FUNCTION: ConstructL
// 2nd phase construction - ensure the timer and server objects are running
// ==========================================================================
void CMessageStoreServer::ConstructL()
    {
    __LOG_ENTER( "ConstructL" )

	// Log filename for this process.
	RProcess thisProcess;
    __LOG_BLOCK( TFileName processName = thisProcess.FileName(); )
	__LOG_WRITE_FORMAT1_INFO( "Filename: %S", &processName );
	thisProcess.Close();
	
    StartL( KMsgStoreServerName );

    iShutdown = CShutdownServer::NewL();
    //starts the PointsecMonitor
    iPointsecMonitor = CImsPointsecMonitor::NewL( *this );
    //must check if PointSec has locked the system or not,
    //only create the container store if PointSec is NOT locked.
    
    
    if ( iPointsecMonitor->IsServiceAllowed() )
        {
        //PointSec does NOT have the device locked, so we can start initialization
        // Construct an instance of the container store to be shared across all message server sessions.
        CreateContainerStoreL();
    
        // Create the inbox/outbox/draft/sent folders, if needed.
        CreatePredefinedFoldersIfNeededL();
        }
    else
        {
        __LOG_WRITE_INFO("PointSec has system locked! Database NOT initialized!")
        //PointSec has the device locked, DO NOT initialize the database
        iLockedByPointSec = ETrue;
        }
    // Register to receive backup/restore notifications.
    iBackupRestoreNotifier = CBackupRestoreNotifier::NewL( *this );
    
    // Ensure that the server still exits even if the 1st client fails to connect.
    // This time isn't started until after the code above, so that the shutdown timer isn't
    // running while the database is possibly being created or wiped.
//    iShutdown->Start();

    // Support for on-the-fly upgrade
    // Watch for KProperty_EmailStore_Upgrade property. When set to our UID3/SECUREID,
    // then, this server should stop.

    // <qmail> removed code to observe shutdown commands

    __LOG_EXIT
    } // end ConstructL

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMessageStoreServer::~CMessageStoreServer()
  {
  delete iUpgradePropertyWatcher;
  delete iShutdown;
  delete iMessageStore;
  iMessageStore = NULL;
  delete iBackupRestoreNotifier;
  delete iPointsecMonitor;
  iSessions.ResetAndDestroy();
  __LOG_DESTRUCT
  } // end destructor

// ==========================================================================
// FUNCTION: NewSessionL
// Create a new client session.
// ==========================================================================
CSession2* CMessageStoreServer::NewSessionL(const TVersion& aVersion, const RMessage2&) const
    {
    __LOG_ENTER( "NewSessionL" )

    if( aVersion.iMajor != KMsgStoreMajorVersion ||
        aVersion.iMinor != KMsgStoreMinorVersion ||
        aVersion.iBuild != KMsgStoreBuild )
        {
        __LOG_WRITE_ERROR( "Session version mismatch" )
        User::Leave( KErrNotSupported );
        } // end if

    // Construct a new session, passing it a pointer to the server object.  This function
    // is const, so the const-ness must be cast away from the this pointer.
    CSession2* returnValue = new (ELeave) CMessageStoreSession( *const_cast<CMessageStoreServer*>(this) );

    __LOG_EXIT
    return returnValue;

    } // end NewSessionL

// ==========================================================================
// FUNCTION: AddSession
// A new session is being created
// Cancel the shutdown timer if it was running
// ==========================================================================
void CMessageStoreServer::AddSessionL( CMessageStoreSession* aSession )
    {
    __LOG_ENTER( "AddSession" )
    iSessions.AppendL( aSession );

    // notify new session of current state
    TMsgStoreEvent event;
    event.iType        = EMsgStoreAvailable;
    event.iId          = KMsgStoreInvalidId;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;

    if ( iLockedByBackupRestore || iLockedByPointSec ) 
        {
        event.iType = EMsgStoreUnavailable;
        }

    aSession->SendEventToObserver( event );

    // A sesssion was added, so the shutdown timer can be stopped.
    //  iShutdown->Stop();

    __LOG_EXIT
    } // end AddSession

// ==========================================================================
// FUNCTION: DropSession
// A session is being destroyed
// Start the shutdown timer if it is the last session.
// ==========================================================================
void CMessageStoreServer::DropSession( CMessageStoreSession* aSession )
    {
    __LOG_ENTER( "DropSession" )

    // Find the current session in the list of sessions.
    TInt index = iSessions.Find( aSession );

    if( index == KErrNotFound )
        {
        // This should never happen.
        __LOG_WRITE_ERROR( "ERROR! SESSION NOT FOUND!" );
        }
    else
        {
        // Remove the session from the list of sessions.
        iSessions.Remove( index );
        } // end if

    // If this was the last session to this server then start the shutdown timer with no delay.  This
    // will immediately shut down the server, unless the container store is currently performing
    // background operations.
//    if( iSessions.Count() == 0 )
//        {
//        iShutdown->Start( 0 );
//        } // end if

    __LOG_EXIT
    } // end DropSession



// ==========================================================================
// FUNCTION: CreateContainerStoreL
// ==========================================================================
void CMessageStoreServer::CreateContainerStoreL()
    {
    __LOG_ENTER( "CreateContainerStoreL" )

    TDriveNumber drive( EDriveC );
    iMessageStore = CContainerStore::NewL( KDbFilename,
                                           drive,
                                           *this,
                                           *iShutdown,
                                           Priority() - 1);  // lower than server


    __LOG_EXIT
    } // end CreateContainerStoreL


// ==========================================================================
// FUNCTION: MessageStoreL
// ==========================================================================
CContainerStore& CMessageStoreServer::MessageStoreL()
  {
  if( !iMessageStore )
    {
    // A backup or restore is in progress.
    // Or PointSec has locked the device.
    User::Leave( KErrInUse );
    } // end if

  return *iMessageStore;
  } // end MessageStoreL

// ==========================================================================
// FUNCTION: CreatePredefinedFoldersIfNeededL
// ==========================================================================
void CMessageStoreServer::CreatePredefinedFoldersIfNeededL()
  {
  __LOG_ENTER( "CreatePredefinedFoldersIfNeededL" )

  // Create predefined folders.
  if ( iMessageStore )
      {
      iMessageStore->CreatePredefinedContainerIfNeededL(
              KMsgStoreRootMailBoxId, EMsgStoreMailBoxBits, KMsgStoreInvalidId,
              KNullDescriptor8 );
      }

  __LOG_EXIT
  } // end CreatePredefinedFoldersIfNeededL

// ==========================================================================
// FUNCTION: QuickPropertiesAndCountsL
// This function traverses the properties, looking for the "quick" properties.
// This is not part of the container store in order to keep the container
// store more generic, without any message-specific logic.
// ==========================================================================
void CMessageStoreServer::QuickPropertiesAndCountsL( TContainerId  aType,
													 const TDesC8& aProperties,
													 RBuf8&        aQuickProperties,
													 TDes8&        aCounts ) const
  {
  __LOG_ENTER( "QuickPropertiesAndCountsL" )

  TMsgStoreCounts counts;

  counts.iMessagesCount = 0;
  counts.iUnreadCount   = 0;
  
    // Quick properties and message counts are for message containers only.
  if( (aType & EMsgStoreContainerMask) == EMsgStoreMessageBits )
    {
    // This container should increment the iMessageCount field of the parent container's child counts.
    counts.iMessagesCount = 1;

    // Iterator through the properties, looking for the quick properties.

    TPropertiesDeserializer propertiesDeserializer( aProperties );
    TPropertiesSerializer   quickPropertiesSerializer( aQuickProperties );

    TBool moreProperties = propertiesDeserializer.First();

    TBool flagsFound       = EFalse;
    TBool completedAtFound = EFalse;
    TBool receivedAtFound  = EFalse;
    TBool replyByFound     = EFalse;
    TBool sizeFound        = EFalse;

    while( moreProperties && !(flagsFound && completedAtFound && receivedAtFound && replyByFound && sizeFound) )
        {
        TBool addProperty = EFalse;

        if( !flagsFound && (propertiesDeserializer.Name().Compare( KMsgStorePropertyFlags ) == 0) )
            {
            addProperty = ETrue;
            flagsFound  = ETrue;

            TPckgBuf<TUint32> flagsPckg;
            flagsPckg.Copy( propertiesDeserializer.Value() );
            
            // The message is considered read if either Read flag is set.
            if ((flagsPckg() & (EMsgStoreFlag_Read_Locally | EMsgStoreFlag_Read) ) == 0)
                {
                // This container should increment the iUnreadCount field of the parent
                // container's child counts.
                __LOG_WRITE_INFO( "is unread" );
                counts.iUnreadCount = 1;
                } // end if
            }
        else if( !completedAtFound && (propertiesDeserializer.Name().Compare( KMsgStorePropertyCompletedAt ) == 0) )
            {
            addProperty      = ETrue;
            completedAtFound = ETrue;
            }
        else if( !receivedAtFound && (propertiesDeserializer.Name().Compare( KMsgStorePropertyReceivedAt ) == 0) )
            {
            addProperty     = ETrue;
            receivedAtFound = ETrue;
            }
        else if( !replyByFound && (propertiesDeserializer.Name().Compare( KMsgStorePropertyReplyBy ) == 0) )
            {
            addProperty  = ETrue;
            replyByFound = ETrue;
            } // end if
        else if( !sizeFound && (propertiesDeserializer.Name().Compare( KMsgStorePropertyMessageSizeOnServer ) == 0) )
            {
            addProperty  = ETrue;
            sizeFound = ETrue;
            } // end if

        if( addProperty )
            {
            // This is a quick property!

            __LOG_WRITE8_FORMAT1_INFO( "found %S", &propertiesDeserializer.Name() );

            quickPropertiesSerializer.AddPropertyL( propertiesDeserializer.Name(),
                                                    propertiesDeserializer.Type(),
                                                    propertiesDeserializer.Value() );

            } // end if

        moreProperties = propertiesDeserializer.Next();

        } // end while

    } // end if

    __LOG_WRITE8_FORMAT2_INFO( "counts total=%i unread=%i", counts.iMessagesCount, counts.iUnreadCount )
    TPckg<TMsgStoreCounts> countsPckg(counts);
    aCounts.Copy( countsPckg );

    __LOG_EXIT

  } // end QuickPropertiesAndCountsL

// ==========================================================================
// FUNCTION: SortableFieldsL
// This function retrieves some sortable fields from a serialized quick properties.
// ==========================================================================
void CMessageStoreServer::SortableFieldsL( const TDesC8&             aQuickProperties, 
                                           const TDesC8&             aProperties,
                                           RMsgStoreSortableFields&  aSortableFields ) const
    {
    __LOG_ENTER( "SortableFieldsL" )
    
    aSortableFields.iFlags = 0;
    aSortableFields.iReceivedDate = 0;
    aSortableFields.iSizeOnServer = 0;
    
    TPropertiesDeserializer qpDeserializer( aQuickProperties );
    
    TBool moreProperties = qpDeserializer.First();

    TBool flagsFound       = EFalse;
    TBool receivedAtFound  = EFalse;
    TBool sizeFound        = EFalse;

    while( moreProperties && !(flagsFound && receivedAtFound && sizeFound) )
        {
        if( !flagsFound && (qpDeserializer.Name().Compare( KMsgStorePropertyFlags ) == 0) )
            {
            flagsFound  = ETrue;
            
            TPckgBuf<TUint32> flagsPckg;
            flagsPckg.Copy( qpDeserializer.Value() );
            
            aSortableFields.iFlags = flagsPckg();
            }
        else if( !receivedAtFound && (qpDeserializer.Name().Compare( KMsgStorePropertyReceivedAt ) == 0) )
            {
            receivedAtFound = ETrue;
            
            TPckgBuf<TInt64> receivedDatePckg;
            receivedDatePckg.Copy( qpDeserializer.Value() );
            
            aSortableFields.iReceivedDate = receivedDatePckg();
            }
        else if( !sizeFound && (qpDeserializer.Name().Compare( KMsgStorePropertyMessageSizeOnServer ) == 0) )
            {
            sizeFound = ETrue;
            
            TPckgBuf<TUint32> sizePckg;
            sizePckg.Copy( qpDeserializer.Value() );
            
            aSortableFields.iSizeOnServer = sizePckg();
            } // end if

        moreProperties = qpDeserializer.Next();
        } // end while
    
    //get the "subject" and "from" the from fields
    TPropertiesDeserializer propDeserializer( aProperties );
    
    moreProperties = propDeserializer.First();

    TBool subjectFound     = EFalse;
    TBool fromFound        = EFalse;
    TBool toFound          = EFalse;

    while( moreProperties && !(subjectFound && fromFound && toFound) )
        {
        if( !subjectFound && (propDeserializer.Name().Compare( KMsgStorePropertySubject ) == 0) )
            {
            RBuf subject;
            CleanupClosePushL( subject );
            
            TPtrC8ToRBuf16L( propDeserializer.Value(), subject );
            
            //get rid of the "Alpha:" "AlphaAlpha:" "AlphaAlphaAlpha:"
            TPtr16 ptr = subject.MidTPtr(0);
            TBool prefixFound = ETrue;
            TChar KColon(':');
            const TUint KPos = 3;
            while ( prefixFound )
                {
                ptr.TrimLeft();
                TInt pos = ptr.Locate( KColon );
       
                if ( pos > 0 && pos <= KPos )
                    {
                    for ( TInt i = 0; i < pos; i++ )
                        {
                        TChar ch = ptr[i];
                        if ( !ch.IsAlpha() )
                            {
                            prefixFound = EFalse;
                            break;
                            }                    
                        }
                    if ( prefixFound )
                        {
                        ptr = ptr.Mid( pos + 1 );
                        __LOG_WRITE_INFO("Prefix ':' found.")
                        }
                    }
                else
                    {
                    prefixFound = EFalse;
                    }
                }
            
            aSortableFields.iSubject.Create( ptr );
            
            __LOG_WRITE_FORMAT1_INFO( "subject=%S", &aSortableFields.iSubject )
            
            CleanupStack::PopAndDestroy( &subject );
            subjectFound = ETrue;
            }
    
    //get the from
        else if( !fromFound && (propDeserializer.Name().Compare( KMsgStorePropertyFrom ) == 0) )
            {
            TPropertiesDeserializer addrDeserializer( propDeserializer.Value() );
            RBuf displayName;
            CleanupClosePushL( displayName );
            if ( addrDeserializer.Find( KMsgStorePropertyDisplayName ) )
                {
                TPtrC8ToRBuf16L( addrDeserializer.Value(), displayName );
                }
            RBuf emailAddr;
            CleanupClosePushL( emailAddr );
            if ( addrDeserializer.Find( KMsgStorePropertyEmailAddress ) )
                {
                TPtrC8ToRBuf16L( addrDeserializer.Value(), emailAddr );
                }
            
            aSortableFields.iFrom.Create( displayName.Length() + emailAddr.Length() );
            aSortableFields.iFrom.Copy( displayName );
            aSortableFields.iFrom.Append( emailAddr );
            
            CleanupStack::PopAndDestroy( &emailAddr );
            CleanupStack::PopAndDestroy( &displayName );
            fromFound = ETrue;
            }
        //get the to
        else if( !toFound && (propDeserializer.Name().Compare( KMsgStorePropertyTo ) == 0) )
            {
            TPropertiesDeserializer addrDeserializer( propDeserializer.Value() );
            RBuf displayName;
            CleanupClosePushL( displayName );
            if ( addrDeserializer.Find( KMsgStorePropertyDisplayName ) )
                {
                TPtrC8ToRBuf16L( addrDeserializer.Value(), displayName );
                }
            RBuf emailAddr;
            CleanupClosePushL( emailAddr );
            if ( addrDeserializer.Find( KMsgStorePropertyEmailAddress ) )
                {
                TPtrC8ToRBuf16L( addrDeserializer.Value(), emailAddr );
                }
            
            aSortableFields.iTo.Create( displayName.Length() + emailAddr.Length() );
            aSortableFields.iTo.Copy( displayName );
            aSortableFields.iTo.Append( emailAddr );
            
            CleanupStack::PopAndDestroy( &emailAddr );
            CleanupStack::PopAndDestroy( &displayName );
            toFound = ETrue;
            }
        moreProperties = propDeserializer.Next();
        }

    __LOG_EXIT
    
    }

// ==========================================================================
// FUNCTION: LengthsL
// ==========================================================================
void CMessageStoreServer::LengthsL( TUint& aCountsLength,
                                    TUint& aQuickPropertiesMaxLength ) const
    {
    aCountsLength = KMsgStoreCountsLength;

    aQuickPropertiesMaxLength = KQuickPropertiesMaxLength;

    } // end LengthsL

// ==========================================================================
// FUNCTION: IncrementParentCounts
// ==========================================================================
void CMessageStoreServer::IncrementParentCounts( TDes8& aParentCounts, const TDesC8& aChildCounts ) const
  {
  __LOG_ENTER_SUPPRESS( "IncrementParentCounts" )
  TMsgStoreCounts*       parentCounts = const_cast<TMsgStoreCounts*>( reinterpret_cast<const TMsgStoreCounts*>( aParentCounts.Ptr() ) );
  const TMsgStoreCounts* childCounts  = reinterpret_cast<const TMsgStoreCounts*>( aChildCounts.Ptr() );

  __LOG_WRITE_FORMAT4_INFO("p.total=%d, p.unread=%d, c.total=%d, c.unread=%d", parentCounts->iMessagesCount, parentCounts->iUnreadCount, childCounts->iMessagesCount, childCounts->iUnreadCount )
  
  parentCounts->iMessagesCount += childCounts->iMessagesCount;
  parentCounts->iUnreadCount   += childCounts->iUnreadCount;
  } // end IncrementParentCounts

// ==========================================================================
// FUNCTION: DecrementParentCounts
// ==========================================================================
void CMessageStoreServer::DecrementParentCounts( TDes8& aParentCounts, const TDesC8& aChildCounts ) const
  {
  TMsgStoreCounts*       parentCounts = const_cast<TMsgStoreCounts*>( reinterpret_cast<const TMsgStoreCounts*>( aParentCounts.Ptr() ) );
  const TMsgStoreCounts* childCounts  = reinterpret_cast<const TMsgStoreCounts*>( aChildCounts.Ptr() );

  parentCounts->iMessagesCount -= childCounts->iMessagesCount;
  parentCounts->iUnreadCount   -= childCounts->iUnreadCount;
  } // end DecrementParentCounts

// ==========================================================================
// FUNCTION: InitializeCounts
// ==========================================================================
void CMessageStoreServer::InitializeCounts( TDes8& aCounts ) const
  {
  TPckg<TMsgStoreCounts> countsPckg( KInitialCounts );
  aCounts.Copy( countsPckg );
  } // end InitializeCounts

  
void CMessageStoreServer::LogCounts( const TDesC& aDescription, const TDesC8& __LOG_BLOCK(aCounts) ) const
    {
    __LOG_ENTER_SUPPRESS( "LogCounts" )
    
    TInt length( aDescription.Length() );                   // noop usage to suppress compilation warning, "INFO" log level not enabled.
    
    __LOG_BLOCK( const TMsgStoreCounts* counts  = reinterpret_cast<const TMsgStoreCounts*>( aCounts.Ptr() ); )
    __LOG_WRITE_FORMAT3_INFO( "%S total=%d unread=%d", &aDescription, counts->iMessagesCount, counts->iUnreadCount )
    }


// ==========================================================================
// FUNCTION: WipeEverythingL
// ==========================================================================
void CMessageStoreServer::WipeEverythingL()
  {
  __LOG_ENTER( "WipeEverythingL" )

  for( TInt i = 0; i < iSessions.Count(); i++ )
    {
    iSessions[i]->ContainerStoreUnavailable();
    } // end if

    if( !iMessageStore )
        {
        // A backup/restore is in progress.  Wipe after it completes.
        iWipeAfterBackupRestore = ETrue;
        }
    else
        {
        delete iMessageStore;
        iMessageStore = NULL;

        // Wipe the message store files in the private area.  (Do not wipe the
        // entire private area since other threads may be storing files in
        // the area.)

        // Note: when the message store server was changed from a process to a
        //       thread, it would have been nice to move all of the message store
        //       files to a specific subdirectory in the private area, but that
        //       change would have been backwards incompatible.
        
        TDriveNumber drive( EDriveC );
		
        TInt result = CContainerStore::WipeEverything( KDbFilename,
                                                       drive );

        TRAP( result, CreateContainerStoreL();
                      CreatePredefinedFoldersIfNeededL() );

        if( result != 0 )
            {
            __LOG_WRITE_ERROR( "failed to recreate message store after wipe" )

            // The server is in a very bad state.  Shut down the server immediately.
            iShutdown->ShutDownNow();
            } // end if

        // Notify observers of wipe event.
        TMsgStoreEvent event;
        event.iType        = EMsgStoreDatabaseWiped;
        event.iId          = KMsgStoreInvalidId;
        event.iParentId    = KMsgStoreInvalidId;
        event.iOtherId     = KMsgStoreInvalidId;
        event.iFlags       = KMsgStoreFlagsNotFound;

        for( TInt i = 0; i < iSessions.Count(); i++ )
            {
            iSessions[i]->ContainerStoreAvailable();
            iSessions[i]->SendEventToObserver( event );
            } // end if

        } // end if

  __LOG_EXIT
  } // end WipeEverythingL

// ==========================================================================
// FUNCTION: BackupOrRestoreInProgress
// ==========================================================================
void CMessageStoreServer::BackupOrRestoreInProgress( TBool /*aIsARestore*/ )
{
    __LOG_ENTER( "BackupOrRestoreInProgress" );

    if ( !iLockedByBackupRestore )
        {
        iLockedByBackupRestore = ETrue;
        SendSystemLockMessage( EMsgStoreBackupOrRestoreInProgress );
        LockSystem();
        }

    __LOG_EXIT
    } // end BackupOrRestoreInProgress

// ==========================================================================
// FUNCTION: BackupOrRestoreCompleted
// ==========================================================================
void CMessageStoreServer::BackupOrRestoreCompleted()
    {
    __LOG_ENTER( "BackupOrRestoreCompleted" );
  
    if ( iLockedByBackupRestore )
        {
        iLockedByBackupRestore = EFalse;
        SendSystemLockMessage( EMsgStoreBackupOrRestoreCompleted );
        TRAP_IGNORE( UnlockSystemL() );
        }
    
    __LOG_EXIT
} // end BackupOrRestoreCompleted

// ==========================================================================
// FUNCTION: PointSecLockStarted
// ==========================================================================
void CMessageStoreServer::PointSecLockStarted()
    {
    __LOG_ENTER( "PointSecLockStarted" );
    if ( !iLockedByPointSec )
        {
        iLockedByPointSec = ETrue;
        SendSystemLockMessage( EMsgStorePointSecLockStarted );
        LockSystem();
        }
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: PointSecLockEnded
// ==========================================================================
void CMessageStoreServer::PointSecLockEnded()
    {
    __LOG_ENTER( "PointSecLockEnded" );
    if ( iLockedByPointSec )
        {
        iLockedByPointSec = EFalse;
        SendSystemLockMessage( EMsgStorePointSecLockEnded );
        TRAP_IGNORE( UnlockSystemL() );
        }
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: LockSystem
// ==========================================================================
void CMessageStoreServer::LockSystem()
    {
    __LOG_ENTER( "LockSystem" );
    
    if ( iMessageStore )
        {
        SendSystemLockMessage( EMsgStoreUnavailable );
        
        for( TInt i = 0; i < iSessions.Count(); i++ )
            {
            iSessions[i]->ContainerStoreUnavailable();
            } // end if
    
        delete iMessageStore;
        iMessageStore = NULL;
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: UnlockSystemL
// ==========================================================================
void CMessageStoreServer::UnlockSystemL()
    {
    __LOG_ENTER( "UnlockSystemL" );

    if  ( ( !iLockedByBackupRestore ) 
       && ( !iLockedByPointSec ) )
        {
        TRAPD( result,
                CreateContainerStoreL(); CreatePredefinedFoldersIfNeededL(); );

        if( result != KErrNone )
            {
            __LOG_WRITE_ERROR( "failed to recreate message store after system lock" )

            // The server is in a very bad state.  Shut down the server immediately.
            iShutdown->ShutDownNow();
            }
       else if  ( ( !iLockedByBackupRestore ) 
               && ( !iLockedByPointSec ) )
            {
            SendSystemLockMessage( EMsgStoreAvailable );

            for( TInt i = 0; i < iSessions.Count(); i++ )
                {
                iSessions[i]->ContainerStoreAvailable();
                } // end if

            if( iWipeAfterBackupRestore )
                {
                iWipeAfterBackupRestore = EFalse;
                WipeEverythingL();
                } // end if

            } // end if
        } // end if
    
    __LOG_EXIT
    }
    


// ==========================================================================
// FUNCTION: SendSystemLockMessage
// ==========================================================================
void CMessageStoreServer::SendSystemLockMessage( TInt aEvent )
    {
    TMsgStoreEvent event;
    event.iType        = aEvent;
    event.iId          = KMsgStoreInvalidId;
    event.iParentId    = KMsgStoreInvalidId;
    event.iOtherId     = KMsgStoreInvalidId;
    event.iFlags       = KMsgStoreFlagsNotFound;

    for( TInt i = 0; i < iSessions.Count(); i++ )
        {
        iSessions[i]->SendEventToObserver( event );
        } // end if
    }

// ==========================================================================
// FUNCTION: TPtrC8ToRBuf16L
// ==========================================================================
void CMessageStoreServer::TPtrC8ToRBuf16L( const TPtrC8& aPtr8, RBuf& aBuf ) const
    {
    RBuf8 val8;
    CleanupClosePushL( val8 );
    
    val8.CreateL( aPtr8 );
    
    const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( val8.Ptr() );
    TPtrC16 val16( valuePtr16, val8.Length() / 2 );
    
    aBuf.CreateL( val16 );
    
    CleanupStack::PopAndDestroy( &val8 );
    }

// FROM: MPSPropertyWatcherObserver
// ==========================================================================
// FUNCTION: OnPSValueChangedToRequired
// ==========================================================================
void CMessageStoreServer::OnPSValueChangedToRequired()
    {
    iShutdown->Start( 0 );
    }

// FUNCTIONS TO SUPPORT AUTOMATED UNIT TESTING

#ifdef _DEBUG
// ==========================================================================
// FUNCTION: InjectBackupRestoreEventL
// ==========================================================================
void CMessageStoreServer::InjectBackupRestoreEventL( TUint aEvent )
    {
    iBackupRestoreNotifier->ProcessEvent( aEvent );
    }

// ==========================================================================
// FUNCTION: InjectPointSecEvern
// ==========================================================================
void CMessageStoreServer::InjectPointSecEvent( TBool aLock )
    {
    if ( aLock )
        {
        PointSecLockStarted();
        }
    else
        {
        PointSecLockEnded();
        }
    }

void CMessageStoreServer::Shutdown()
    {
    iShutdown->Start( 0 );
    }

#endif



