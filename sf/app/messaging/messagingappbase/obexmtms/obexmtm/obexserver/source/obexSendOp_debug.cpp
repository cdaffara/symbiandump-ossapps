// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: obexSendOp_debug.cpp $
// $Author: Stevep $
// $Revision: 22 $
// $Date: 25/03/02 10:52 $
// 
//

//class include
#include <obexsendop.h>

#ifdef __OBEX_SEND_OP_FILE_DEBUG_MODE__
#pragma message ("//")
#pragma message ("//                                                                          //")
#pragma message ("//                               FILE DEBUG MODE                            //")
#pragma message ("//                                                                          //")
#pragma message ("//")

//System includes
#include <msvids.h>
#include <msventry.h>	//CMsvServerEntry
#include <btmsgtypeuid.h>	//KUidMsgTypeBt
#include <irmsgtypeuid.h>	//KUidMsgTypeIr
//#include <obexpindlguid.h> //KObexPinNotifierUid

//user includes
// Following are named KErrIrObexClientXXXXX because they are using the irobex.dll (which 
// should really be called obex.dll now that it supports more than IR)
const TInt KErrIrObexClientPutPeerCannotHandleObject = -5504;
const TInt KErrIrObexClientFirstPutFailed = -5510;
const TInt KErrIrObexClientSubsequentPutFailed = -5511;

_LIT(KObexFileDebugIRServiceFolder, "c:\\system\\obexdebug\\IRservice\\output");
_LIT(KObexFileDebugBTServiceFolder, "c:\\system\\obexdebug\\BTservice\\output");
_LIT(KObexFileDebugUSBServiceFolder, "c:\\system\\obexdebug\\USBservice\\output");
_LIT(KOutputFileFormatString, "%S\\%d.obex");


//
//  CObexServerSendOperation....	CMsvOperation derived op that is intended to be owned by a CMsvReporterOperation or similar. 
//							When this completes, the CMsvReporterOperation completes too
//

void CObexServerSendOperation::BuildSpecificConstructL()
/**
Debug second phase constructor. Sets the service path as appropriate to the MTM.
*/
	{
	//Connect to the file store
	User::LeaveIfError(iServiceFs.Connect());

	//select the output directory dependent upon the MTM being used
	if (iMtm == KUidMsgTypeBt)
		{
		iServicePath = KObexFileDebugBTServiceFolder;
		}
	if (iMtm == KUidMsgTypeIrUID)
		{
		iServicePath = KObexFileDebugIRServiceFolder;
		}
	}

void CObexServerSendOperation::BuildSpecificDestructor()
/**
 * Destructor. Cancel()s, deletes owned objects and Close()s the connection to the FileServer.
 */
	{
	iServiceFs.Close();
	}

EXPORT_C const TDesC8& CObexServerSendOperation::ProgressL()
/**
Returns current progress information.

@return A reference to a TPckgC<TObexMtmProgress> package pointer descriptor containing progress information on this send operation.
@leave KErrXXX system wide error codes
*/
	{
	// State operation type (and progress).
	TObexMtmProgress progress;

	if (progress.iSendState != EInitialsed)
		progress.iSendState=iSendState;
	else
		progress.iSendState = EInitialise;

	progress.iTotalEntryCount = iAttachmentEntryCount + iFileNameEntryCount;
	progress.iEntriesDone = progress.iTotalEntryCount - (iNextAttachment+1) - (iFileNameList? iFileNameList->Count() : 0);
	progress.iCurrentEntrySize = 0; 
	progress.iCurrentBytesTrans = 0;
	progress.iError = iStatus.Int();

	// prepate package buffer for return value, we copy it to a data member so progress
	// data isn't destroyed when we go out of scope
	TPckgC<TObexMtmProgress> progressDes(progress); 
    iProgressPckg.Copy(progressDes); //important to use copy() not set(), set() just points to our local descriptor 
	return iProgressPckg;
	}

TInt CObexServerSendOperation::PutObexObjectToServiceFileL()
/**
 * Output the obex object to a file in the service folder
 *
 * @leave KErrXXX system wide error codes
 */
	{
	//count the number of entries already in the folder
	CDir* serviceDirectoryList = 0;
	TFileName servicePath = iServicePath;
	_LIT(KSlash, "\\");
	servicePath.Append(KSlash);

	//Close the file by Reset()ing the obex object--otherwise it can't be copied
	iObexObject->Reset();

	//copy a file to the service folder
	CFileMan* fileMan = CFileMan::NewL(iServiceFs);
	TInt err = fileMan->Copy(iSendFile, iServicePath);

	//count the number of files
	err = iServiceFs.GetDir(servicePath, KEntryAttNormal, ESortByName, serviceDirectoryList);
	if (err != KErrNone)
		{
		ActivateRunLWithError(err);
		}

	//Create a new filename consisting of the service path followed by the new file number .obex
	TFileName newFileName = iServicePath;
	newFileName.Format(KOutputFileFormatString, &iServicePath, serviceDirectoryList->Count());

	//No longer need serviceDirectoryList
	delete serviceDirectoryList;

	//Write the iObexObject member to the file
	//err = iObexObject->WriteToFile(newFileName);


	//Now rename it the file
	//first make the original filename
	TParse parser;
	parser.Set(iSendFile, 0, 0);
	servicePath.Append(parser.NameAndExt());	//servicePath now contains the full path and filename
	//now actually rename it
	err = fileMan->Rename(servicePath, newFileName);

	delete fileMan;
	return err;
	}

void CObexServerSendOperation::RealRunL()
/**
Implementation of the send operation state machine. Progresses as:

  Initialise-->Connect-->ConnectAttemptComplete-->SendObject-->(SendNextObject-->)SendComplete-->Disconnected

  The SendNextObject state is repeated for each attachment in excess of one. 

Also handles UserCancelled and SendError states by CompleteObserver()ing with appropriate error codes.

  Leaves will be passed back to RunL and handled there. 

  @leave Leaves if insufficient memory.
  @leave Leaves if there is a file error when loading an object from a file
*/
	{
	const TInt status=iStatus.Int();

	switch(iSendState)
		{
	case TObexMtmProgress::EInitialise://state 1
		{
		// Timeout for repeatedly trying to connect to remote OBEX device
		iTimeoutTimer->After(iConnectTimeout);  

		iSendState = TObexMtmProgress::EConnect;
		SetActive();
		CompleteSelf(KErrNone);
		break;
		}

	case TObexMtmProgress::EConnect://state 2
		{  
		iSendState=TObexMtmProgress::EConnectAttemptComplete;
		iStatus=KRequestPending;
		SetActive();
		CompleteSelf(KErrNone);
		break;
		}

	case TObexMtmProgress::EConnectAttemptComplete://state 3
		{
		// A connection _attempt_ has completed - see if it was sucessful.
		if (status == KErrNone)
			{
			// Ok - remote Obex device found and connected to
			iTimeoutTimer->Cancel(); //cancel connect timeout
			iSendState=TObexMtmProgress::ESendObject; //Go to send state
			CompleteSelf(KErrNone);
			}
		else if( (status == KErrNotFound) || (status == KErrTimedOut) || (status == KErrIrObexClientNoDevicesFound) )
			{  
			// Connect failed. No remote device was found so we try again.
			// (KErrIrObexClientNoDevicesFound is an OBEX specific error)
			iSendState=TObexMtmProgress::EConnect; //return to connect state above
			CompleteSelf(KErrNone);
			}
		else 
			{
			// KErrDisconnected = Found obex machine but had "Obex" IAS query failure.
			// status>0  means that 'status' obex machines have been found.
			// IAS Query is where the class of transport is not supported.
			iTimeoutTimer->Cancel();
			iSendState=TObexMtmProgress::ESendError;
			CompleteSelf(status);
			}
		SetActive();
		break;
		}

	case TObexMtmProgress::ESendObject://state 4
	case TObexMtmProgress::ESendNextObject:  //state 5: Multiple object support.
		{
		iTimeoutTimer->Cancel(); // Cancel last timeout.

		// For multiObject, check success of previous put. (Single objects won't be affected).
		if(!CheckStatusOfLastObject (status, iSendState))
			{
			//Last message failed - CheckStatusOfLastObject has taken the necessary steps to notify
			break;
			}

		//Get the next object ready to send
		TInt err = PrepareCurrentObjectAndSetStateL();

		SetActive();

		//Check the status of preparation
		if (err == KErrNone)
			{
			//Successful, so try to "send" it
			iTimeoutTimer->After(iPutTimeout); // Start a new one.
			CompleteSelf(PutObexObjectToServiceFileL());
			}
		else
			{
			//Unsuccessful--report the error
			iTimeoutTimer->After(iPutTimeout); // Start a new one.
			CompleteSelf(err);
			}

		break;
		}

	case TObexMtmProgress::ESendComplete://state 6
		{
		iTimeoutTimer->Cancel(); //cancel iPutTimeout

		// Object "send(s)" have completed but latest might have completed with an error
		if(!CheckStatusOfLastObject (status, iSendState))
			{
			//Last message failed - CheckStatusOfLastObject has taken the necessary steps to notify
			break;
			}

		//Now attempt to move the successfully sent message to the sent folder
		MoveToSentAndSetActiveL();
		break;
		}

	case TObexMtmProgress::EMovedToSent://state 7
		{
		//Release the lock on the Sent folder, and mark the message as complete
		CleanupAfterMovedToSent();

		iSendState=TObexMtmProgress::EDisconnected;
		SetActive();
		CompleteSelf(KErrNone);

		// Unfortunately, it is possible for this call to never complete if you send
		// a file that the peer does not understand. This occurs because some peers 
		// (i.e. Palm) don't seem able to handle this scenario correctly and don't 
		// drop the transport when they should. So we restart the 'put' timeout to 
		// catch this error
		iTimeoutTimer->After(iPutTimeout);
		}
		break;

	case TObexMtmProgress::EDisconnected://state 8
		{
		iTimeoutTimer->Cancel();
		if(status != KErrNone)
			{
			// Pass error code onto the owner of this messaging operation
			iSendState=TObexMtmProgress::ESendError;
			SetActive();
			CompleteSelf(status);
			}
		else
			{
			// Completed OK - 
			// Signal MS that the CMsvOperation has ended, and clean up.
			TObexMtmProgress& progress = iProgressPckg();
			progress.iError = KErrNone;
			CompleteObserver();
			}
		break;
		}

	case TObexMtmProgress::EUserCancelled://state 9
		{  
		// Signal that the CMsvOperation has been cancelled by the user.
		TObexMtmProgress& progress = iProgressPckg();
		progress.iError = KErrCancel;
		iTimeoutTimer->Cancel();
		CompleteObserver();
		break;
		}

	case TObexMtmProgress::ESendError://state 10
		{
		// General error handling stage.
		TObexMtmProgress& progress = iProgressPckg();
		progress.iError = status;
		iTimeoutTimer->Cancel();
		CompleteObserver();
		break;
		}

	default:
		Panic(EObexSendOperationUnknownSendState);
		}
	}

#endif	//__OBEX_SEND_OP_FILE_DEBUG_MODE__
