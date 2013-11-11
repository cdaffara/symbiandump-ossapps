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
//


#ifndef __OBEXSENDOP_H__
#define __OBEXSENDOP_H__



//#define __OBEX_SEND_OP_FILE_DEBUG_MODE__

#include <msvapi.h>
#include <badesca.h> // CDesCArray
#include <obex.h>
#include <obexclientmtm.h>	//TObexMtmProgress


class CObexHeaderList;


//
// Panic Code
//

//Obex Send Operation Panics
enum TObexSendOperationPanic
	{
	EObexSendOperationAlreadyActive, //< The current Obex send operation is already active
	EObexSendOperationUnknownJob, //<
	EObexSendOperationUnknownSendState, //< The current value of the Obex send state of the send operation is not found in TObexMtmProgress::TSendState
	EObexSendOperationUnexpectedTimeout //< Panic if send has timeout unexpectedly
	};

GLDEF_C void Panic(TObexSendOperationPanic aPanic);



//
// CObexServerSendOperation 
//
class CObexSendOpTimeout;
class CObexPasswordGetter;

class CObexServerSendOperation : public CActive, public MObexAuthChallengeHandler
/**
class CObexServerSendOperation

Obex Server Send Operation:

Base class for Send operations using Obex protocol.

Implements a state machine with the following states:
Initialise-->Connect-->ConnectAttemptComplete-->SendObject-->(SendNextObject-->)SendComplete-->MovedToSent-->Disconnected

The pure virtual function InitialiseObexClientL() must be overridden in the base class to initialise the
iObexClient member to use the desired Obex transport mechanism (e.g. infrared, Bluetooth).

In order to allow asynchronous transport initialisations (for example BT's SDP Query)
implementations of this function must set iAsyncInit and provide a mechanism to 
complete a request.  See the bluetooth server mtm code for implementation details.
 
@internalTechnology
@released
*/
	{
public:
	
	/**
	 * Destructor. Cancel()s, deletes owned objects and Close()s the connection to the FileServer.
	 */
	
	virtual IMPORT_C ~CObexServerSendOperation();
	
	// Must be implemented in the derived class to initialise the iObexClient member to use the desired Obex transport mechanism
	virtual void InitialiseObexClientL() =0;
	
	/**
	 * This is not required to do anything in the base implementation.
	 */
		
	IMPORT_C virtual void SecondPhaseObexClientInitL();		// may be overridden to do anything required in a second phase.  default implementation is blank.

	/**
	 * Operations to perform before attempting a connection.
	 * As multiple connection attempts can be made, it is necessary for this
	 * routine to ensure it can handle being called multiple times.
	 * May be overridden. Default implementation is blank.
	 */
	IMPORT_C virtual void PreConnectOperations();

	/**
	 * Operations to perform after attempting a connection.
	 * As multiple connection attempts can be made, it is necessary for this
	 * routine to ensure it can handle being called multiple times.
	 * May be overridden. Default implementation is blank.
	 */
	IMPORT_C virtual void PostConnectOperations();

	/**
	 * Operations to perform before attempting to send a set of objects.
	 * May be overridden. Default implementation is blank.
	 */
	IMPORT_C virtual void PreSendOperations();

	/**
	 * Operations to perform after attempting to send a set of objects.
	 * May be overridden. Default implementation is blank.
	 */
	IMPORT_C virtual void PostSendOperations();


protected: // for use by derived classes
	/**
	 * Constructor.
	 *
	 * @param aMsgTypeUid UID of message type
	 * @param aSendObj Reference to the object to send.
	 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
	 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
	 * @param aObserverRequestStatus TRequestStatus of owning active object.
	 */
	
	IMPORT_C CObexServerSendOperation(TUid aMsgTypeUid, CMsvServerEntry& aSendObj, TInt aConnectTimeoutMicroSeconds, TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus);
	
	/**
	 * Second phase constructor. Sets up connection to the FileServer, initialises attachments or filename list then 
	 * starts sending process by initialising.
	 *
	 * @param aConnectPassword Pointer to the password to be used for authentication.
	 * @leave Leaves if insufficient memory.
	 * @leave Leaves if cannot connect to FileServer.
	 */
	
	IMPORT_C void ConstructL(const TDesC* aConnectPassword);

	/**
	 * Cancels the current operation, deletes the client and Cancel()s the timeout timer. Only completes the observer
	 * (by a call to CompleteObserver) if an external entity (i.e. the owner) has called Cancel().
	 * Otherwise the observer is not completed.
	 */
	
	IMPORT_C virtual void DoCancel();

	 /* Constructor, Alternative version
	 *
	 * @param aMsgTypeUid UID of message type
	 * @param aSendObj Reference to the object to send.
	 * @param aConnectTimeoutMicroSeconds Timeout period for Connect operation in microseconds.
	 * @param aPutTimeoutMicroseconds Timeout period for Put operation in microseconds.
	 * @param aObserverRequestStatus TRequestStatus of owning active object.
	 * @param aLastSendAttempt TBool flag to check for the second send attempt and also control header sending.  EFalse sends full headers, ETrue only sends name and size.
	 */
	
	IMPORT_C CObexServerSendOperation(TUid aMsgTypeUid, CMsvServerEntry& aSendObj, TInt aConnectTimeoutMicroSeconds,
	                                  TInt aPutTimeoutMicroSeconds, TRequestStatus& aObserverRequestStatus, 
	                                  TBool aLastSendAttempt);
	
	/**
	* Tells the derived class that the base class is about to complete the observer.
	* This is the first thing called when CompleteObserver is called.
	* Since the behaviour of CompleteObserver is to clean up the message that it was trying to send,
	* this calls gives the derived class an opportunity to either stop this deletion or recover any information
	* synchronously from the message.
	* If the derived class has no need to use this functionality, the default implementation allows deletion.
	* @param aErrorCode The last error code encountered
	* @return TBool True delete the message
	* @return TBool False DO NOT delete the message
	*/
	IMPORT_C virtual TBool CompletingObserver(TInt aErrorCode);

public: // called by CObexSendOpTimeout
	
	/**
	 * Called when the current operation times out. Causes the current operation to be cancelled, then reactivates with
	 * the appropriate error code (KErrIrObexClientNoDevicesFound or KErrIrObexClientPutPeerAborted).
	 */
	
	void TimeOut();

public:
	
	/**
	 * Returns current progress information.
	 *
	 * @return A reference to a TPckgC<TObexMtmProgress> package pointer descriptor containing progress information on this send operation.
	 * @leave KErrXXX system wide error codes
	 */
	IMPORT_C const TDesC8& ProgressL();
	
	TBool iAsyncInit;

private: // From MObexAuthChallengeHandler

	/**
	 * Called by the Obex Client when authentication is requested to pass the password back. If the password is invalid, this
	 * call should succeed but the send operation as a whole will inevitably fail.
	 *
	 * @param aRelm ignored, but could be used to indicate which password to use.
	 * @leave KErrXXX system wide error codes. Shouldn't leave just because the password is invalid.
	 */
	
	IMPORT_C virtual void GetUserPasswordL(const TDesC& aUserID);

private: // From CActive
	
	
	/**
	 * Calls RealRunL(), and traps errors
	 *
	 * @leave Leaves with errors from RealRunL()
	 */
	
	IMPORT_C virtual void RunL();

private:

	/**
	 * Destructor. Cancel()s, deletes owned objects and Close()s the connection to the FileServer.
	 */
	
	void BuildSpecificDestructor();
	
	/**
	 * Normal second phase constructor.
	 */
	
	void BuildSpecificConstructL();
	
	/**
	 * Cancels the current operation, then reactivates with the given error code.
	 *
	 * @param aError Error code to be passed to CompleteSelf.
	 */
	
	void ActivateRunLWithError(TInt aError);
	
	/**
	 * Cancel any pending obex operation without completing the observer. 
	 */

	void ExplicitCancel();  // Must call this instead of just Cancel(), otherwise the owner of this op will be completed.
	
	/**
	 * Complete the observer, reporting any error via the progress. THIS METHOD MUST BE CALLED ONCE ONLY.
	 *
	 */
	
	void CompleteObserverL();
	
	/**
	 * This causes this active object's request to complete which means
	 * RunL() will be called again if we are active (immediately if there 
	 * are no higher priority active objects in the active scheduler).
	 *
	 * @param aError Error to be passed forward to the next step of the state machine
	 */
	
	void CompleteSelf(TInt aError);

	/**
	 * Implementation of the send operation state machine. Progresses as:
	 * Initialise-->Connect-->ConnectAttemptComplete-->SendObject-->(SendNextObject-->)SendComplete-->Disconnected
	 * The SendNextObject state is repeated for each attachment in excess of one. 
	 * Also handles UserCancelled and SendError states by CompleteObserver()ing with appropriate error codes.
	 * Leaves will be passed back to RunL and handled there. 
	 *
	 * @leave KErrXXX system wide error codes
	 */
	
	void RealRunL();
	
	/**
	 * Delete the outbox entry as operation has 'completed'.
	 * Will be invisible&InPreparation anyway (MS delete will delete it the next 
	 * time it starts).
	 */
	
	TInt SynchronousEntryDelete();
	
	/**
	 * Load an attachment into the obex sending buffer, and create a new Obex object of name TMsvEntry::iDetails.
	 *
	 * @param aParent Reference to CMsvServerEntry to be sent.
	 * @param aWhichAttachment Zero-based index of attachment to send.
	 * @leave KErrXXX system wide error codes
	 */
	
	void InitialiseAttachmentL(CMsvServerEntry& aParent, TInt aWhichAttachment);
	
	void LoadFileIntoObjectL(const TDesC& aFileName, const TDesC& aObexName, const TDesC8& aMimeType);


	/**
	 * Checks the last object was sent correctly, and tries to action appropriate error feedback if not. Only to be called
	 * from ESendObject/ESendNextObject or ESendComplete states. 
	 *
	 * @param aStatus Status of last object
	 * @return ETrue if message was OK--EFalse if message failed and this function has taken the necessary action
	 */

	TBool CheckStatusOfLastObject(TInt aStatus, TObexMtmProgress::TSendState aSendState);
	
	/**
	 * Loads the next object to be sent, whether an attachment or a file in the file list.
	 *
	 * @return KErrXXX standard error code
	 * @return KErrNotFound if there were neither attachments nor files in the file list
	 * @leave KErrXXX system wide error codes
	 */
	
	TInt PrepareCurrentObjectAndSetStateL();
	
	/**
	 * Moves the newly sent message to the global sent items folder, and sets active ready for its completion.
	 *
	 * @leave KErrXXX system wide error codes
	 */
	
	void MoveToSentAndSetActiveL();
	
	/**
	 * Restores after the message has been moved to the inbox, and marks the message as visible.
	 */

	void CleanupAfterMovedToSent();
	
	/**
	 * Returns a reference to the file session (RFs) of the message
	 *
	 * @return A reference to the file session of the the message 
	 */

	RFs& FileSession();


#ifdef __OBEX_SEND_OP_FILE_DEBUG_MODE__
	/**
	 * Output the obex object to a file in the service folder
	 *
	 * @leave KErrXXX system wide error codes
	 */
	
	TInt PutObexObjectToServiceFileL();
#endif	//__OBEX_SEND_OP_FILE_DEBUG_MODE__

protected: 
	CObexClient* iObexClient; //<The Obex client memeber
	TBool        iLastSendAttempt;	// check for second send attempt and also to control header sending

private:
	//From member initialisation list
	TRequestStatus&					iObserverRequestStatus;		//<TRequestStatus for notifying observer (eventually  ClientMTM) of completion
	CMsvServerEntry&				iMsvSendParent;				//<The message being sent (i.e. the parent of any attachments being sent)
	const TInt						iConnectTimeout;			//<Connection attempt timeout in microseconds
	const TInt						iPutTimeout;				//<Put attempt timeout in microseconds
	TBool							iCancelWithoutCompleting;	//<Flag to allow cancellation without completing observer
	TPckgBuf<TObexMtmProgress>		iProgressPckg;				//<Progress package buffer
	const TUid						iMtm;						//<UID of this MTM


	TInt							iNextAttachment;			//<Index of next attachment to be sent
	CObexFileObject*				iObexObject;				//<Obex object currently being sent
	TFileName						iSendFile;					//<Filename of the object to be sent--necessary since CObexFileObject does not export its copy
	CObexSendOpTimeout*				iTimeoutTimer;				//<Timeout timer used for various operations
	HBufC*							iConnectPassword;			//<Authentication password 
	TObexMtmProgress::TSendState	iSendState;					//<State machine state
	TInt							iAttachmentEntryCount;		//<Number of attachments to send (==0 if iFileNameEntryCount!=0)


	CMsvOperation*					iMoveOperation;				//<Operation to govern the movement of the sccessfully sent message to the sent folder
	CMsvEntrySelection*				iMoveEntrySelection;		//<Entry selection containing the entry to rename

	TFileName						iServicePath;				//<Path of service folder
	};



//
// CObexSendOpTimeout 
//

class CObexSendOpTimeout : public CTimer
/**
class CObexSendOpTimeout

Obex Send Operation Timeout active object:

Straightforward active object used for timeout operations by CObexServerSendOperation.

@internalComponent
@released
*/
  	{
public:
	
	/**
	 *Canonical NewL function, which also sets the owner operation.
	 *
	 *@param aSendOperation Obex send operation which will be "timed out" when the timer expires
	 */

	static CObexSendOpTimeout* NewL(CObexServerSendOperation* aSendOperation);
private:
	
	/**
	 * Constructor. Calls CTimer's constructor with priority EPriorityStandard
	 */

	CObexSendOpTimeout();
	
	/**
	 * Second phase constructor. Calls CTimer::ConstructL(), and adds itself to the active scheduler
	 *
	 * @leave KErrXXX system wide error codes
	 */
	
	void ConstructL();
	
	/**
	 * Calls the TimeOut method of the associated CObexServerSendOperation when the timer expires
	 *
	 * @leave KErrXXX system wide error codes
	 */
	
	void RunL();
private:
	CObexServerSendOperation* iSendOperation; //<The Obex server send operation
	};


#endif // __OBEXSENDOP_H__
