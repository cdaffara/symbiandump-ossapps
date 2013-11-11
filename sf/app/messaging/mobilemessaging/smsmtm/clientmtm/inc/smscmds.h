// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// smscmds.h
//
/**
 * @file 
 * @publishedAll
 * @released
 */
#ifndef __SMSCMDS_H__
#define __SMSCMDS_H__

#include <e32def.h>

/**
Numeric value of the first SMS-specific command.

This is only used by TSmsMtmCommand.

@see	TSmsMtmCommand

@publishedAll 
@released
*/
const TInt KMinCommandExpected = 10000;

/**
The SMS-specific commands that can be issued through CSmsClientMtm::InvokeAsyncFunctionL.

SMS messages can be stored in a medium other than the message store itself. For
instance, SMS messages can be stored on the SIM card. This alternative storage
is referred to as the phone store.

Some of the commands defined allow the contents of the phone store to accessed
and manipulated.

@see	CSmsClientMtm::InvokeAsyncFunctionL
@see	CSmsClientMtm::ReadSimParamsL
@see	CSmsClientMtm::WriteSimParamsL
@see	TSmsProgress 

@publishedAll
@released
*/
enum TSmsMtmCommand 
	{
/**
Gets the service centre address (i.e. telephone number) from the GSM handset.

The result may be retrieved from the iServiceCenterAddress member variable 
in the TSmsProgress progress object for this operation. 

The aSelection and aParameter arguments are not used by this command, except 
that aSelection must contain at least one member, preferably the SMS Service 
ID. 

If the telephone handset is unable to supply the service centre address, then 
the progress object for this operation will contain the error. 

NOTE - ESmsMtmCommandReadServiceCenter should only be used if the telephone 
handset or TSY module does not support reading the SMS parameters on the SIM. 
ESmsMtmCommandReadServiceCenter should only be used if the operation 
returned by CSmsClientMtm::ReadSimParamsL completes with KErrNotSupported.

@removed	
This command is not supported from v7.0. The function CSmsClientMtm::ReadSimParamsL
should be used instead to obtain Service Centre number from the phone store.

@see	CSmsClientMtm::ReadSimParamsL
@see	TSmsProgress
*/
	ESmsMtmCommandReadServiceCenter			= KMinCommandExpected,

/** 
Sets a new service centre address (i.e. telephone number) on the GSM handset. 

The new telephone number is passed in aParameter as an 8-bit descriptor containing 
a packaged TSmsServiceCenterAddress. 

The aSelection parameter argument is not used by this command, except that 
aSelection must contain at least one member, preferably the SMS Service ID. 

@removed
This command is not supported from v7.0. The function CSmsClientMtm::WriteSimParamsL
should be used instead to write the Service Centre number to the phone store.

@see	CSmsClientMtm::WriteSimParamsL
@see	TSmsServiceCenterAddress
*/
	ESmsMtmCommandWriteServiceCenter,

/**
Schedules the selected SMS messages to be sent.

The aSelection argument contains the TMsvId of the SMS messages to be sent. An 
empty selection will cause a panic in debug mode. In release mode nothing will 
happen.

The time that each message is scheduled to be sent is set by the date field in
the TMsvEntry for that message. Each message in the selection must have the same
scheduled time as the first message in the selection. A send task is scheduled 
to occur at this time.

In debug mode a panic will occur if all the messages in the selection do not 
have the safe scheduled time. In release mode, the scheduled time sending all 
the messages in the selection will be that of the last message in the selection.

When the scheduled task occurs not only will the selected messages be sent but
also any waiting SMS messages in the Outbox.

If the messages are successfully sent by the scheduled task, then all the sent
messages are moved to the Sent folder.

If any message fails to be sent then the message is marked as failed. Also it
may be re-scheduled to be sent at a later time if that particular error has been
specified as a re-schedulable error. The message remains in its current folder.

The aParameter agrument is not used.

@see	CMsvSession::TransferCommandL

@see	CBaseMtm::InvokeAsyncCommandL

*/
	ESmsMtmCommandScheduleCopy,

/**
Not supported by the SMS MTM.

@publishedAll 
*/
	ESmsMtmCommandScheduleMove,

/**
Removes all messages specified in aSelection from the task scheduler list.

If successful, the messages will have their SendingState() set to KMsvSendStateSuspended 
on completion of this operation. 

The aParameter argument is not used by this function. 

@see	TMsvEntry::SendingState() 
*/
	ESmsMtmCommandDeleteSchedule,

/** 
Checks the current scheduled status of the messages specified by aSelection. 

If successful, the Scheduled() and iDate members of each TMsvEntry identified 
in the selection will be changed to represent that messages's current status. 

The aParameter argument is not used by this function. 

@see	TMsvEntry
*/
	ESmsMtmCommandCheckSchedule,

/**
Starts sending the specified selection of SMS messages.

When a selection of SMS messages are scheduled to be sent using the ESmsMtmCommandScheduleCopy
command this is the actual command that is executed the scheduled time occurs.

The SMS server MTM creates a package contain this command. The package is passed
to the task scheduler who then passes it to the schedule send exe. The exe then
uses the package to ask the SMS server MTM to send the messages. As such this
command should not be used be external clients.

When the scheduled task occurs not only will the selected messages be sent but
also any waiting SMS messages in the Outbox.

If the messages are successfully sent by the scheduled task, then all the sent
messages are moved to the Sent folder.

If any message fails to be sent then the message is marked as failed. Also it
may be re-scheduled to be sent at a later time if that particular error has been
specified as a re-schedulable error. The message remains in its current folder.

@publishedAll 
*/
	ESmsMtmCommandSendScheduledCopy,

/**
Not supported by the SMS MTM.

@publishedAll 
*/
	ESmsMtmCommandSendScheduledMove,

/**
Reads the SMS messages on the phone store and creates a copy of those messages
in an invisible folder under the SMS service in the message store. 

If successful, the iEnumerateFolder member of the operation's progress will 
identify the invisible folder which contains the messages read from the phone 
store.

The aSelection argument must contain at least the SMS servive ID. The aParameter 
argument can optionally be a TPckgC containing the ID of an existing folder to
use for the enumeration. The contents of this folder will be replaced with the
current messages in the phone store.

The operation will fail with KErrArgument if the ID in aParameter is one of the
following - KMsvRootIndexEntryId, KMsvLocalServiceIndexEntryId, KMsvGlobalInBoxIndexEntryId,
KMsvGlobalOutBoxIndexEntryId, KMsvDraftEntryId or KMsvSentEntryId.

This command must be called before using ESmsMtmCommandCopyFromPhoneStore, 
ESmsMtmCommandMoveFromPhoneStore or ESmsMtmCommandDeleteFromPhoneStore. 

Pre v7.0, this was named ESmsMtmCommandEnumerateSim.

@see	TSmsProgress
*/
	ESmsMtmCommandEnumeratePhoneStores,

/** 
Moves the messages identified in aSelection to the folder identified in aParameter 
(e.g. the inbox). 
	
The associated SMS messages are not deleted from the phone store.

The first entry ID in aSelection must be the SMS service ID. All following 
entry IDs in the selection must then represent each message to be transferred. 
aParameter should contain a packaged TMsvId, which identifies the folder to 
which the messages in aSelection will be moved. 

The command ESmsMtmCommandEnumeratePhoneStores must be called before using 
this command. 

Pre v7.0, this was named ESmsMtmCommandCopyFromSim.

@see	TSmsMtmCommand::ESmsMtmCommandEnumeratePhoneStores
*/
	ESmsMtmCommandCopyFromPhoneStore,  

/**
Moves the messages identified in aSelection to the folder identified in aParameter 
(e.g. the inbox), and then deletes the messages from the phone store. 

The first entry ID in aSelection must be the SMS service ID. All following 
entry IDs in the selection must then represent each message to be transferred. 
aParameter should contain a packaged TMsvId, which identifies the folder to 
which the messages in aSelection will be moved. 

The command ESmsMtmCommandEnumeratePhoneStores must be called before using 
this command. 

Pre v7.0, this was named ESmsMtmCommandMoveFromSim.

@see	TSmsMtmCommand::ESmsMtmCommandEnumeratePhoneStores
*/
	ESmsMtmCommandMoveFromPhoneStore,  

/**
Deletes the specified messages from the phone store.

The first entry ID in aSelection must be the SMS service ID. All following 
entry IDs in the selection must then represent each message to be deleted. 
aParameter is not used.

The command ESmsMtmCommandEnumeratePhoneStores must be called before using 
this command. 

Pre v7.0, this was named ESmsMtmCommandDeleteFromSim.

@see	TSmsMtmCommand::ESmsMtmCommandEnumeratePhoneStores
*/
	ESmsMtmCommandDeleteFromPhoneStore,  

/**
Reads the SIM parameters.

This should not be used in the CSmsClientMtm::InvokeAsyncFunctionL function. The
CSmsClientMtm::ReadSimParamsL function should be used to read the SIM parameters.

@see	CSmsClientMtm::ReadSimParamsL

@publishedAll 
*/
	ESmsMtmCommandReadSimParams,

/**
Writes the specified SIM parameters.

This should not be used in the CSmsClientMtm::InvokeAsyncFunctionL function. The
CSmsClientMtm::WriteSimParamsL function should be used to write the SIM parameters.

@see	CSmsClientMtm::WriteSimParamsL

@publishedAll 
*/
	ESmsMtmCommandWriteSimParams,

/**
Copies the SMS messages identified in aSelection to the phone store.

The first entry ID in aSelection must be the SMS service ID. All following 
entry IDs in the selection must then represent each message to be copied.
Single message with multiple recipients is copied onto SIM as multiple messages,
one message for each recipient.Copy/Move from SIM will result in multiple messages,
single message will not be reformed out of the mutiple messages on SIM.


Pre v7.0, this was named ESmsMtmCommandCopyToSim.
*/
	ESmsMtmCommandCopyToPhoneStore,  

/**
Moves the SMS messages identified in aSelection to the phone store.

The first entry ID in aSelection must be the SMS service ID. All following 
entry IDs in the selection must then represent each message to be moved.
SSingle message with multiple recipients is moved onto SIM as multiple messages,
one message for each recipient.Copy/Move from SIM will result in multiple messages,
single message will not be reformed out of the mutiple messages on SIM.


Pre v7.0, this was named ESmsMtmCommandMoveToSim.
*/
	ESmsMtmCommandMoveToPhoneStore
	};

#endif	// __SMSCMDS_H__
