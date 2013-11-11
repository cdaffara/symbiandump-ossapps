// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// cimapcompoundbase.cpp
//

#include "cimapcompoundbase.h"
#include "cimapsession.h"
#include "cimapsettings.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"				
#endif

CImapCompoundBase::CImapCompoundBase( CImapSyncManager& aSyncManager,
					   				  CMsvServerEntry& aServerEntry,
					   				  CImapSettings& aImapSettings) :
	CMsgActive(EPriorityStandard ),
	iSyncManager(aSyncManager),
	iServerEntry(aServerEntry),
	iImapSettings(aImapSettings),
	iStopForMigrate(EFalse),
	iCancelForMigrate(EFalse),
	iProgressState(TImap4GenericProgress::EIdle),
	iSyncProgressState(TImap4SyncProgress::EIdle)
	{

	}

CImapCompoundBase::~CImapCompoundBase( )
	{
	}

#ifdef __IMAP_LOGGING
void CImapCompoundBase::DoComplete(TInt& aErr)
#else
void CImapCompoundBase::DoComplete(TInt& /*aErr*/)
#endif //_IMAP_LOGGING
	{
	__LOG_FORMAT((iSession->LogId(), "CImapCompoundBase::DoComplete(aErr = %d) CurrentStep = %d", aErr, iCurrentStep));
	}

/**
Called when asynchronous service requests complete.
Handles errors returned by aSynchronous services. 

The state machine for derived compound commands is managed by 
the pure-virtual DoRunLoopL() which is called by DoRunL(). 

This form of DoRunL also allows synchronous steps to be performed 
sequentially.
*/
void CImapCompoundBase::DoRunL()
	{
	// Handle any server errors
	if (iStatus.Int()!=KErrNone)
		{
		if (TInt err=ProcessSessionError() != KErrNone)
			{
			Complete(err);
			return;
			}
		}

	// Operation state machine handled by DoRunLoopL()
	while (!IsActive() && !DoRunLoopL())
		{
		// do nothing in the body of this
		}
	}


/**
Base class method for handling errors returned by the session.
Called on completion of an asynchronous request on the IMAP session,
ie at the start of the DoRunL() method. If this method returns
a value other than KErrNone, the compound operation should complete
with the return value as the error code.

This will catch any positive error codes returned by the imap
session These indicate either NO/BAD server responses or a more
serious error has occurred, for example a disconnect, or corrupt
data received from the remote server.

NO/BAD server responses are handled by the derived compound
operation classes (these are non-fatal errors, an attempt is
made to continue the requested operation)

Other errors are more serious - these are passed up to the
Protocol Controller to deal with.

@return KErrNone if the error has been handled,
        otherwise the positive error code as returned by
        the IMAP session.
*/
TInt CImapCompoundBase::ProcessSessionError()
	{
	TInt errCode = iStatus.Int();
	if (errCode==KErrImapNo || errCode==KErrImapBad)
		{
		return ProcessNegativeServerResponse();
		}
	else
		{
		return errCode;
		}
	}

/**
Checks the input selection for entries of specified types.
In this context, "Messages" means complete messages, not RFC888
parts of another message. "parts" means attachment or text bodies.

@param aSelection    - original list of message entry ids
@param aLocalCopy    - filtered copy of message entry ids.
@param aMessages     - accept entries of type KUidMsvMessageEntry
@param aParts        - accept entries of type KUidMsvEmailTextEntry,
											  KUidMsvAttachmentEntry, 
											  KUidMsvMessageEntry
											  KUidMsvEmailHtmlEntry
  	  	                                      KUidMsvEmailExternalBodyEntry
  	  	                                      KUidMsvEmailRtfEntry
@param aFolders		 - accept entries of type KUidMsvFolderEntry
@param aIsInService	 - only accept entries under the IMAP service entry

@return KErrNotSupported if no valid entries,
        KErrNone otherwise
*/
TInt CImapCompoundBase::CheckSelectionL(const CMsvEntrySelection& aSelection,
									    CMsvEntrySelection* aLocalCopy,
									    const TBool aMessages,
									    const TBool aParts,
									    const TBool aFolders,
									    const TBool aIsInService)
	{
	// reset the local copy
	aLocalCopy->Reset();
	
	// get the service ID
	TMsvId serviceId = iImapSettings.ServiceId();

	// Check all entries are messages
	for(TInt a=0;a<aSelection.Count();a++)
		{
		// Does entry exist?
		TBool addIt = EFalse;

		if (iServerEntry.SetEntry(aSelection[a])==KErrNone)
			{
			TUid type = iServerEntry.Entry().iType;
			if ((aMessages && type==KUidMsvMessageEntry) ||
				(aParts && (type==KUidMsvEmailTextEntry || type==KUidMsvAttachmentEntry || type==KUidMsvMessageEntry || type==KUidMsvEmailHtmlEntry || type==KUidMsvEmailExternalBodyEntry || type==KUidMsvEmailRtfEntry)) ||
				(aFolders && type==KUidMsvFolderEntry))
				{
				TBool inEnclosingMessage=EFalse;

				// Do we need to check if it's in the local service or
				// if it is a complete message
				if (aIsInService || (!aParts && type==KUidMsvMessageEntry))
					{
					// Work up the tree until we get to the service or the root
					do
						{
						SetEntryL(iServerEntry.Entry().Parent());
						if (iServerEntry.Entry().iType==KUidMsvMessageEntry)
							{
							inEnclosingMessage=ETrue;
							}
						}
					while(iServerEntry.Entry().iType!=KUidMsvServiceEntry &&
						  iServerEntry.Entry().Id()!=KMsvRootIndexEntryId);		
	
					// Are we at the service that this MTM referrs to?
					// if offline iServiceId==0 so allow all
					if (!aIsInService || serviceId==0 || iServerEntry.Entry().Id()==serviceId)
						{
						// it's OK if it is not a message type (in
						// which case it has already been checked and
						// passed) or it is not within an enclosing message
						if (type!=KUidMsvMessageEntry || !inEnclosingMessage || aParts)
							{
							addIt = ETrue;
							}
						}
					}
				else
					{
					// Add to local copy
					addIt = ETrue;
					}
				}
			}

		if (addIt)
			{
			aLocalCopy->AppendL(aSelection[a]);
			}
#ifdef _DEBUG
		// UI shouldn't really be giving us bogus items so panic
		else
			{
			TImapServerPanic::ImapPanic(TImapServerPanic::EInvalidMsvTypeToCommand); 
			}
#endif
		}

	// Anything to do?
	if (!aLocalCopy->Count())
		{
		// Nothing valid to work with
		return(KErrNotSupported);
		}
	else
		{
		// All OK, the selection isn't empty
		return(KErrNone);
		}
	}

/**
Sums the total size of messages to be copied or moved

@param aSelection - selection of messages to be processed
@return TInt      - the total size of the messages in the selection
*/
TInt CImapCompoundBase::CalculateDownloadSizeL(const CMsvEntrySelection& aSelection)
	{
	TInt totalSize = 0;

	// Do a quick tally on the size of messages which are to be copied / moved.
	TInt count=aSelection.Count();
	while (count--)
		{
		SetEntryL(aSelection.At(count));
		// Only add the size up if the message is not complete.
		if(!iServerEntry.Entry().Complete())
			{
			totalSize += iServerEntry.Entry().iSize;
			}
		}
	return totalSize;
	}

/**
Find the folder that contains specified message

@param aMessage - selection of messages to be processed
@return TMsvId  - the parent folder of aMessage, or the service ID.
*/
TMsvId CImapCompoundBase::FindFolderL(const TMsvId aMessage)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapCompoundBase::FindFolderL(%d)", aMessage));

	// Find folder that encloses this message (has Mailbox flag set), or service,
	// whichever we find first
	SetEntryL(aMessage);
	TMsvEmailEntry entry;
	do
		{
		// Change context to parent of current entry
		SetEntryL(iServerEntry.Entry().Parent());
		entry=iServerEntry.Entry();

		__LOG_FORMAT((iSession->LogId(), "  At %x, type=%x, mailbox=%d", entry.Id(),entry.iType,entry.Mailbox()));
		
		// A folder & a mailbox, or a service?
		if (entry.iType==KUidMsvFolderEntry && entry.Mailbox())
			{
			// This'll do!
			return(entry.Id());
			}
		}
	while(iServerEntry.Entry().iType!=KUidMsvServiceEntry && entry.Id()!=KMsvRootIndexEntryId);

	__LOG_TEXT(iSession->LogId(), "  FindFolderL() Failed");

	return(NULL);
	}

// Do setentry, leave if there is an error
void CImapCompoundBase::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iServerEntry.SetEntry(aId));
	}

/**
Self completes to allow RunL to be called.
*/	
void CImapCompoundBase::CompleteSelf()
	{
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


void CImapCompoundBase::SetCurrentStep()
	{
	iCurrentStep = iNextStep;
	}

/**
Save error code in a message

@param aMessgeId
@param aError
*/
void CImapCompoundBase::MessageErrorL(const TMsvId aMessageId, const TInt aError)
	{
	// Save error code: if we can't access this entry, then it's probably something to do
	// with the error we're trying to report: ignore it silently
	if (iServerEntry.SetEntry(aMessageId)==KErrNone)
		{
		TMsvEntry entry=iServerEntry.Entry();

		// Save unnecessary writes...
		if (entry.iError!=aError)
			{
			entry.iError=aError;
			User::LeaveIfError(iServerEntry.ChangeEntry(entry));
			}
		}
	}

/**
Cancels outstanding service request, and recovers the compound operation
object to a state in which the operation may be restarted following migration
to a new carrier.
The operation will be restarted by a call to ResumeOperationL().

Derived classes must implement a DoCancel that sets iNextStep to the step
that should be performed following the migration, when ReumeOperationL is called;
*/
void CImapCompoundBase::CancelForMigrate()
	{
	__LOG_TEXT(iSession->LogId(), "CImapCompoundBase::CancelForMigrate()");
	iCancelForMigrate = ETrue;
	Cancel();
	iCancelForMigrate = EFalse;
	// Mark Operation as suspended.
	iCurrentStep = ESuspendedForMigrate;
	}

/**
Indicates that the compound operation should complete as soon as it is possible
for the operation to be re-started without requiring a significant amount of
repeated communication.
The operation will be restarted by a call to ResumeOperationL()
*/
void CImapCompoundBase::StopForMigrate()
	{
	__LOG_TEXT(iSession->LogId(), "CImapCompoundBase::StopForMigrate()");
	iStopForMigrate = ETrue;
	}
	
/**
Returns ETrue if the compound operation has been suspended (stopped/cancelled).
This would occur in the case of a bearer migration.
*/
TBool CImapCompoundBase::Suspended()
	{
	if (iCurrentStep == ESuspendedForMigrate)
		{
		return ETrue;
		}
	return EFalse;
	}
	
