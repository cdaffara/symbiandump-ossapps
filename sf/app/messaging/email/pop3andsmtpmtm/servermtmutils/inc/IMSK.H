// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the API definition for the class CImTextServerSession.
// This class creates sockets and is responsible for transmitting data between client and the server 
// 
//

/**
 @file
 @publishedAll
 @released
*/
#ifndef __IMSK_H__
#define __IMSK_H__

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <f32file.h>
#include <mentact.h>
#include <miutlog.h>
#include <miut_err.h>
#include <miuthdr.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"				
#endif

#ifdef _DEBUG
#define __IMSK_SIMULATION
#endif

// Enable Scripting in Debug Builds only
#ifdef _DEBUG
#define __IMSK_SCRIPTING
#endif

/** 
@publishedAll
@released
*/
const TInt EActivePriorityHigh = 1;
/** 
@publishedAll
@released
*/
const TInt KCarriageLineFeedLength=2;
/** Max numeric IP address len = "0:0:0:0:0:0:194.217.242.23" = 45 chars 
@publishedAll
@released
*/
const TInt KImskIPAddressLen=45;				// Max numeric IP address len = "255.255.255.255" = 15 chars

/** Maximum buffer size of the received data 
@publishedAll
@released
*/
typedef TBuf8<KImMailMaxBufferSize> TImMailBuffer;

/** received buffer data line type
@publishedAll
@released
*/
enum TImLineType
	{ 
	ECRLFTerminated, 
	EBufferTooSmall, 
	EReceiveBufferFull,
	EReceiveBufferEmpty
	};


class CImLog;
class CImTextServerScript;
class CImIAPPreferences;
class CImConnect;
class CSecureSocket;
class CImSocketIdleTimer;

/**
Creates/opens socket and sends and receives data.

@publishedAll
@released	
*/
class CImTextServerSession : public CMsgActive
	{
public:
	enum TImOperationMode{
		EImServerMode,
		EImClientMode
		};

/**
	//cat				Construction
	@fn				NewL()
	Intended Usage	:	Static factory constructor. Uses two phase construction and 
						leaves nothing on the CleanupStack.
	@since			6.0
	@leave			KErrNoMemory.
	@return			A pointer to the newly created CImTextServerSession object. 
	@pre 			None
	@post			A fully constructed and initialised CImTextServerSession object.
	
 */
	IMPORT_C static CImTextServerSession *NewL();

	/**
	@publishedAll
	@released
	*/
	IMPORT_C static CImTextServerSession* NewL(RSocketServ& aSocketServ, CImConnect& aConnect);

/**
	//cat				Construction
	@fn				NewL(TInt aSendIdleTime, TInt aReceiveIdleTime)
	Intended Usage	:	Static factory constructor. Uses two phase construction and 
						leaves nothing on the CleanupStack.
	@since			7.0s
	@param			aSendIdleTime is the time for which the idle timer runs (for a send)
	@param			aReceiveIdleTime is the time for which the idle timer runs (for a recieve)
	@leave			KErrNoMemory.
	@return			A pointer to the newly created CImTextServerSession object. 
	@pre 				None
	@post				A fully constructed and initialised CImTextServerSession object.
 */
	IMPORT_C static CImTextServerSession* NewL(TInt aSendIdleTime, TInt aReceiveIdleTime);

	/**
	@publishedAll
	@released
	*/
	IMPORT_C static CImTextServerSession* NewL(TInt aSendIdleTime, TInt aReceiveIdleTime, RSocketServ& aSocketServ, CImConnect& aConnect);

/**
	//cat				Destruction
	@fn				~CImTextServerSession()
	Intended Usage	:	Destructor.
	@since			6.0	
 */
	~CImTextServerSession();

/**
	@fn				QueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool aEnableTimeout = ETrue)
	Intended Usage	:	Queue a connect assuming the socket is successfully opened.
	Error Condition	:	KErrNoMemory
	@since			6.0
	@param			aStatus	Asynchronous completion status
	@param			anAddressDesc is the IP address		
	@param			aIAPPreferences is the IAP connection preference to be used		
	@param			aPortNum is the port number eg. 143, 25, 110.
	@param			aEnableTimeout Not used		
	@pre 			None
	@post			connection is ready to send and receive data.
 */
	IMPORT_C void QueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool aEnableTimeout = ETrue);

	IMPORT_C void QueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, const TDesC8& aSSLDomainName);

/**
	@fn				SSLQueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool aEnableTimeout = ETrue)
	Intended Usage	:	Queue a wrapped SSL connect on an socket assuming the socket is successfully opened.
	Error Condition	:	KErrNoMemory, EImskSocketOpen
	@since			7.0s
	@param			aStatus	Asynchronous completion status
	@param			anAddressDesc is the IP address		
	@param			aIAPPreferences is the IAP connection preference to be used		
	@param			aPortNum is the port number eg. 993, 465, 995.
	@param			aEnableTimeout Not used		
	@pre 			None
	@post			connection is ready to send and receive data.
 */
	IMPORT_C void SSLQueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool aEnableTimeout = ETrue);

	IMPORT_C void SSLQueueConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, const TDesC8& aSSLDomainName);

/**
	@fn				Disconnect()
	Intended Usage	:	To disconnect and close socket connection.
	Error Condition	:	None
	@since			6.0
	@pre 			None
	@post			connection is closed.
  */
	IMPORT_C void Disconnect();
	IMPORT_C void Disconnect(TRequestStatus &aStatus);
	void TimeOut();


	IMPORT_C const TDesC& LocalName();
/**
	This method should be called before Send/SendQueueReceive() 
	TLS command is sent.
	@fn				SetSSLTLSResponseL(const TDesC8& aDesc)
	Intended Usage	:	To use for secure connection.
	Error Condition	:	KErrNoMemory
	@since			6.2
	@param			aDesc is the positive response the MTM expects from the connected
					server for TLS command. e.g.  OK, +OK, 220 etc..		
	@pre 			aDesc cannot be zero length
	@leave			EImskNoTLSResponseString
	@post			iTLSResponse is filled with aDesc contents

  */
	IMPORT_C void SetSSLTLSResponseL(const TDesC8& aDesc);

/**
	@fn				Send(TRequestStatus &aStatus, const TDesC8& aDesc)
	Intended Usage	:	To send data.
	Error Condition	:	None
	@since			6.0
	@param			aStatus	Asynchronous completion status
	@param			aDesc is the aData to be sent
	@post			None

  */

	//sends
	IMPORT_C void Send(TRequestStatus &aStatus, const TDesC8& aDesc);
	IMPORT_C void SendWithTimeout(TRequestStatus& aStatus, TInt aIdleTime, const TDesC8& aDesc); 
	IMPORT_C void SendQueueReceiveWithTimeout(TRequestStatus& aStatus, TInt aIdleTime, const TDesC8& aDesc); 
	IMPORT_C void Send(TRequestStatus &aStatus, TRefByValue<const TDesC8> aFmt,...);
/**
	@fn				SendQueueReceive(TRequestStatus &aStatus, const TDesC8& aDesc)
	Intended Usage	:	To send data and queue a receive.
	Error Condition	:	None
	@since			6.0
	@param			aStatus	Asynchronous completion status	
	@param			aDesc is the aData to be sent
	@post			Sends the data and queues a receive.
	GetCurrentTextLine should be called after this method to read received data
	*/
	IMPORT_C void SendQueueReceive(TRequestStatus &aStatus, const TDesC8& aDesc);
	IMPORT_C void SendQueueReceive(TRequestStatus &aStatus, TRefByValue<const TDesC8> aFmt,...);

    //receives
/**
	Returns first full line of data received from socket to user
	@fn				GetCurrentTextLine(TDes8& aDesc)
	Intended Usage	:	To return received data.
	Error Condition	:	None
	@since			6.0
	@param			aDesc will be filled with received buffer
	@post			returns the received buffer in the form of aDesc
	@return			EReceiveBufferEmpty when received data is empty
					EBufferTooSmall when aDesc length is smaller than received data
					ECRLFTerminated when received data is complete (full line)
					EReceiveBufferFull when received data length is more than aDesc length
	GetCurrentTextLine should be called after Send methods
	*/
	IMPORT_C TImLineType GetCurrentTextLine(TDes8& aDesc);
/**
	User queues a new request from the socket (unless there's a full line of data in buffer
	then signal user and there's no need to make a receive request)
	@fn				QueueReceiveNextTextLine(TRequestStatus &aStatus)
	Intended Usage	:	User queues a new request from the socket.
	Error Condition	:	None
	@since			6.0
	@param			aStatus for asynchronous request
	
	*/
	IMPORT_C void QueueReceiveNextTextLine(TRequestStatus &aStatus);
/**
	@fn				ReceiveBinaryData(TRequestStatus &aStatus, TDes8& aDes,TInt aLen)
	Intended Usage	:	Users queue a request for received data
	Error Condition	:	None
	@since			6.0
	@param			aStatus	Asynchronous completion status	
	@param			aDes is the aData to be received
	@param			aLen is the length of the data the user wants to read
	@post			aDes will be filled with received data of aLen.
	ReceiveBinaryData should be called after Send.
	*/
	IMPORT_C void ReceiveBinaryData(TRequestStatus &aStatus, TDes8& aDes,TInt aLen);

	IMPORT_C void LogText(const TDesC8& aString);						// write string into log file
	IMPORT_C void LogError(const TDesC8& aString,const TInt aError);		// write string and integer into log file

/**
	@fn				GetIAPValue(TUint32 &aIAP)
	Intended Usage	:	Returns the IAP we are connecting/connected with in aIAP or returns an error code
	@since			6.0
	@param			aIAP will be the current IAP 
	@post			aIAP will be filled with current IAP connected.
	@return			will return genconn errors if any
	*/

	IMPORT_C TInt GetIAPValue(TUint32 &aIAP);
/**
	@fn				GetRConnectionName(TName &aName)
	Intended Usage	:	Returns the name of the RConnection.
	@since			9.1
	@param			aName Buffer in which the name is returned. 
	@post			aName will be filled with current name of the RConnection.
	@return			KErrNone if successful, or another of the system-wide error codes.
	*/

	IMPORT_C TInt GetRConnectionName(TName &aName);

/** Returns the bearer type we are connected to with in aBearer or returns an error code */
	IMPORT_C TInt GetIAPBearer(TUint32 &aBearer);

/**
	@publishedAll

	@fn				GetLastSocketActivityTimeout(TUint32& aTimeout)
	Intended Usage	:	Returns the last socket activity timeout value
	@since			9.1
	@param			aTimeout is a return argument containing the timeout if it was found
	@post				aTimeout will be filled with the timeout value
	@return			Returns KErrNone, KErrNotFound or KErrBadHandle
	*/
	IMPORT_C TInt GetLastSocketActivityTimeout(TUint32& aTimeout);

/**
	@fn				GetSocketServ()
	Intended Usage	:	Returns the Socket Server
	@since			8.0
	@return			Socket Server
	*/
	RSocketServ& GetSocketServ();

/**
	@fn				GetConnectionStage()
	Intended Usage	:	Gets the stage of the connection process obtained from RConnection
	@since			7.0s
	@return			The current connection stage from RConnection or a system-wide error code.

	*/
	IMPORT_C TInt GetConnectionStage();

/**
	@fn				SocketIdle()
	Intended Usage	:	disconnects the socket and notifies the observer when the socket has been idle
						for some time
	@since			7.0s
 */
	void SocketIdle();

/**
	@fn				SetPrimaryTextServerSession(CImTextServerSession& aPrimaryTextServerSession)
	Intended Usage	:	Users set only on the secondary session 
	@since			9.2
	@param			aPrimaryTextServerSession is PrimarySession's TextServerSession
	@pre 			None
	@post			Going to be use on the secondary session
*/
	IMPORT_C void SetPrimaryTextServerSession(CImTextServerSession* aPrimaryTextServerSession);

/**
	@fn				GetCImConnect()
	Intended Usage	:	Returns the CImConnect object
	@since			9.2
	@return			CImConnect 
	@post			Going to be use on the secondary session
    */
	CImConnect* GetCImConnect();
	
// Depreciated functions - do not use.
	IMPORT_C void QueueConnect(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, TBool aEnableTimeout = ETrue);
	IMPORT_C void QueueConnect(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TInt aType, TBool aEnableTimeout = ETrue);
	IMPORT_C void QueueConnect(TRequestStatus& aStatus,const TDesC& anAddressDesc, TInt aPortNum, TCallBack aDisconnectFunction,const TUint32 aIntraddress,TInt aType, TBool aEnableTimeout = ETrue);
	IMPORT_C void QueueConnect(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum,const TUint32 aIAPaddress, TInt aType, TBool aEnableTimeout = ETrue);
	IMPORT_C void QueueConnect(TRequestStatus& aStatus,const TDesC& anAddressDesc, TInt aPortNum, TCallBack aDisconnectFunction, TBool aEnableTimeout = ETrue);
	IMPORT_C void QueueConnect(TRequestStatus& aStatus,const TDesC& anAddressDesc, TInt aPortNum, TCallBack aDisconnectFunction, const CImIAPPreferences& aIAPPreferences,TInt aType, TBool aEnableTimeout = ETrue);
	IMPORT_C TInt Send(const TDesC8& aDesc);
	IMPORT_C TInt Send(TRefByValue<const TDesC8> aFmt,...);
	IMPORT_C TInt SendReceive(const TDesC8& aDesc);
	IMPORT_C TInt Receive(TDes8& rDes);
	IMPORT_C void Receive(TRequestStatus &aStatus, TDes8& rDes);
	IMPORT_C static CImTextServerSession *NewLC (TImOperationMode aMode, RSocketServ &aServerServ);
	IMPORT_C static CImTextServerSession *NewL(RSocketServ &aServerServ);
	//Do not call SetSecurity.  Call SetSSLTLSResponseL
	IMPORT_C TInt SetSecurity(TBool aSecurityOn, TBool aUnattendedMode = FALSE);
	IMPORT_C void PerformLogging(TBool aLogging);

private:
/**
	//cat			Construction and Destruction
	@fn				CImTextServerSession()
	Intended Usage	:	Constructor. First phase of two-phase construction method. Does 
						non-allocating construction.
	@since			6.0
	
 */
	CImTextServerSession();

	CImTextServerSession(RSocketServ& aSocketServ, CImConnect& aConnect);
/**
	//cat				Construction
	@fn				CImTextServerSession(TInt aSendIdleTime, TInt aReceiveIdleTime)
	Intended Usage	:	Constructor. First phase of two-phase construction method. Does 
						non-allocating construction.
	@since			7.0s
	@param			aSendIdleTime is the time for which the idle timer runs (for a send)
	@param			aReceiveIdleTime  is the time for which the idle timer runs (for a receive)
 */
	CImTextServerSession(TInt aSendIdleTime, TInt aReceiveIdleTime);

	CImTextServerSession(TInt aSendIdleTime, TInt aReceiveIdleTime, RSocketServ& aSocketServ, CImConnect& aConnect);

/**
	//cat			Construction and Destruction.
	@fn				ConstructL()
	Intended Usage	:	Second phase of two-phase construction method. Does any 
						allocations required to fully construct the object.
	@since			6.0
	@leave			KErrNoMemory.
	@pre 			First phase of construction is complete
	@post			The object is fully constructed and initialised.	
 */
	void ConstructL();

	TInt Open();
	void Close();

	void CreateLogFile(TInt aPortNum);
	void OpenScriptFile(TInt aPortNum);

	void DoRunL();
	void DoComplete(TInt& aStatusValue);						// Cleanup code
	void DoCancel();

	// these called from DoRunL;
	void SocketConnect();
	void DoConnectedToSocketL();
	void DoQueueConnect();

	void RealReceive(TDes8& aDesc);
	void RealSend(const TDesC8& aDesc);
/** 
	@fn				ParseSSLTLSResponseL()
	Intended Usage	:	session will compare the response(for TLS command)
						with the MTMs response 						
	@since			6.2
	@leave			KImskSSLTLSNegotiateFailed if the response doesn't match 
	@post			CSecureSocket object is created
	*/
	void ParseSSLTLSResponseL();

/** 
	@fn				CreateSecureSocketL()
	Intended Usage	:	to use secure socket connection 						
	@since			6.2
	@leave			None 
	@post			CSecureSocket object is created and iSecurityState is set to EImSecurityStateOn;
	*/
	void CreateSecureSocketL();

/**
	@fn				QueueGenericConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, TBool aEnableTimeout = ETrue)
	Intended Usage	:	Queue a standard or wrapped SSL connect on an socket assuming the socket is successfully opened.  Called by SSLQueueConnectL and QueueConnectL
	Error Condition	:	KErrNoMemory, EImskSocketOpen
	@since			7.0s
	@param			aStatus	Asynchronous completion status	
	@param			anAddressDesc is the IP address		
	@param			aIAPPreferences is the IAP connection preference to be used		
	@param			aPortNum is the port number eg. 143, 993
	@param			aSSLDomainName SSL domain name to use for secure sockets
	@pre 			None
	@post			connection is ready to send and receive data.
 */
	void QueueGenericConnectL(TRequestStatus &aStatus,const TDesC& anAddressDesc, TInt aPortNum, const CImIAPPreferences& aIAPPreferences, const TDesC8& aSSLDomainName);

#if defined(__IMSK_SIMULATION)
	void ReadNextPeriod();
	TUint32 GetTokenValue(TInt aTokenLen, const TPtrC8& aBuffer);
	TBool IsSuspended();	
	TBool SuspendPeriodSet();
	void ResetSuspendPeriod();
	void SetAfterTimer();
	TInt ReadConfigNum(const TDesC& aName);
#endif

	enum TImSocketState
		{
		EImClosed, 
		EImResolve, 
		EImConnect,   
		EImSendReceive,
		EImDialUsingOverride,
		EImTLSHandShakeStarted,
		EImSendReceiveTimedOut
		};

	enum TImSendReceiveState
		{ 
		EImInactive, 
		EImSending,
		EImReceiving, 
		EImReceivingBinaryData,
		EImSendingQueueReceive,
		EImSuspended,          //used when we are in a simulated GPRS suspend.
		};

	enum TImSecurityState
		{
		EImSecurityStateOff,
		EImSecurityStateOn,
		EImSecurityStateFailed
		};
private:
	/** handle to the socket */
	RSocket							iSocket;
	/** handle to RSocketServ */
	RSocketServ						iServ;
	/** handle to Hostresolver */
	RHostResolver					iHostResolver;
	/** handle to secure socket for SSL/TLS connection */
	CSecureSocket*					iSecureSocket;

	TUint32 iCurrentIAPcache;
	/** Used to find if IAP is cached */
	TBool							iIAPCached;  // true if the above value can be used.
	/** result of the host resolver */
	TNameEntry						iHostent;
	/** port number e.g. 25 for SMTP, 143 for IMAP and 110 for POP */
	TInt							iPortNum;
	/**  server address */
	TPtrC							iAddressDesc;
	/** data returned by the socket */
	TImMailBuffer					iReceive;
	TSockXfrLength					iLen;
	TImSocketState					iState;
	TImSendReceiveState				iSendReceive;

	HBufC8*							iBuffer;
	TImLineType						iCurrentLineType;

	/** to log connection data  */
	CImLog*							iLog;
	/** IP address name */
	TBuf<KImskIPAddressLen>			iLocalName;
	/** secure state i.e to use Secure socket */

	TImSecurityState				iSecurityState;
	const CImIAPPreferences*		iIAPPreferences;

	CImConnect*						iConnect;

	TDes8*							iReceiveData; // buffer we just recieved data into


	HBufC8*							iSentData;
	/** response for different mail protocols like 220,+OK, OK, BAD */
	HBufC8*							iTLSResponse; //response for different mail protocols like 220,+OK, OK, BAD
	/** internal flag to read tls response */
	TBool							iReadTLSResponse;
	TBool							iPerformLogging;
	TBool							iWrappedSocket;

	CImSocketIdleTimer*				iSocketIdleTimer;
	TTimeIntervalMicroSeconds32		iSendIdleTime;
	TTimeIntervalMicroSeconds32		iReceiveIdleTime;

	/** A shorter idle timeout, used when a fast response is expected */
	TTimeIntervalMicroSeconds32		iSendShortIdleTime;
	TTimeIntervalMicroSeconds32		iReceiveShortIdleTime;


#ifdef __IMSK_SCRIPTING
	CImTextServerScript*			iScript;
#endif

#if defined(__IMSK_SIMULATION)

	RFs								iFs;


	TUint32							iScriptedIAP; // value to return for iap when scripting
	TUint32							iBearerIAP; // value to return for bearer when scripting


	TImSendReceiveState             iSuspendedState;
	
	RTimer							iSuspendTimer;

	RFile							iGprsFile;

	TBool							iGprsConfigExists;

	TInt							iCfgFilePos;	

	TTime							iLastSuspend;	// time last suspension took place
													// in universal time

	TTimeIntervalSeconds 			iStart;		// number of seconds since last delay
												// before the following delay occurs.
												// if 0 no suspend is required.
	TTimeIntervalSeconds 			iDuration;	// how long to delay. If 0 no suspend is
												// required
	TBool							iRepeat;	// repeat the above delay for an 
												// infinite period

	HBufC8*							iSendData; // suspended data 
#endif // __IMSK_SIMULATION
	
	CImTextServerSession*			iPrimaryTextServerSession; //Only going to be set on the secondary session
	
	TBool iClientOwnsConnection;

	/** SSL domain name for secure sockets */
	HBufC8* iSSLDomainName;
	TBool							iSocketIdleTimeSet;          // iSocketIdleTimeSet=ETrue, if smtp server did not responds within 10 seconds after sending "."
	};

#endif
