/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <avkon.hrh>
#include <aknnotewrappers.h>

#include "cbtprintingdevice.h"
#include "cbtobjectserver.h"
#include "btprotocolconsts.h"
#include "clog.h"
#include "printmessagecodes.h"
#include "tbtmapper.h"
#include "xhtmlfilecomposerconst.h"
#include "rsutils.h"


// ----------------------------------------------------------------------------
// CBtObjectServer::NewL()
// Creates a new instance of the class
// ----------------------------------------------------------------------------

CBtObjectServer* CBtObjectServer::NewL(const TBTDevAddr& aDeviceAddress,
                                               RArray<CImageInfo> &aImgArray,
                                               MBtObjectServerObserver &aObs)
{

    CBtObjectServer* self = CBtObjectServer::NewLC( aDeviceAddress, aImgArray, aObs );
    CleanupStack::Pop( self );
    return self;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::NewLC()
// Creates a new instance of the class
// ----------------------------------------------------------------------------

CBtObjectServer* CBtObjectServer::NewLC(const TBTDevAddr& aDeviceAddress,
                                                RArray<CImageInfo> &aImgArray,
                                                MBtObjectServerObserver &aObs)
{

    CBtObjectServer* self = new ( ELeave ) CBtObjectServer( aDeviceAddress, aObs );
    CleanupStack::PushL( self );
    self->ConstructL(aImgArray);
    return self;

}

// ----------------------------------------------------------------------------
// CBtObjectServer::CBtObjectServer
// The actual constructor of the class
// ----------------------------------------------------------------------------

CBtObjectServer::CBtObjectServer(const TBTDevAddr& aDeviceAddress, MBtObjectServerObserver& aObs )
:  	iAllowedAddress ( aDeviceAddress ),
	iObs ( aObs ),
	iStarted(EFalse),
	iConnected(EFalse),
	iTransportUp(EFalse)
{
    LOG("CBtObjectServer::CBtObjectServer]\t default constructor");

   iImgArray.Reset();
   iImgArray.Close();
}

// ----------------------------------------------------------------------------
// CBtObjectServer::~CBtObjectServer()
// Destructor.
// ----------------------------------------------------------------------------
//
CBtObjectServer::~CBtObjectServer()
{
    LOG("CBtObjectServer::~CBtObjectServer]\t");

	iStarted = EFalse;
	iOperation = EIdle;

    if ( iObexServer && iObexServer->IsStarted() )
    {
        iObexServer->Stop();
    }

	iFs.Close();

    if (iObexServer)
    	delete iObexServer;
    iObexServer = NULL;

	if(iObexBufObject)
    	delete iObexBufObject;
    iObexBufObject = NULL;

	// This must be deleted after the iObexBufObject who uses it.
    if (iObexBody)
	    delete iObexBody;
    iObexBody = NULL;

	if(iAdvertiser)
    	delete iAdvertiser;
    iAdvertiser = NULL;

    iImgArray.Close();
}

// ----------------------------------------------------------------------------
// CBtObjectServer::Cancel()
// Sends current file to the server and then stops the server
// ----------------------------------------------------------------------------
void CBtObjectServer::Cancel()
{
    LOG("[CBtObjectServer::Cancel]\t");

    if (!IsSendingData())
    {
        LOG("[CBtObjectServer::Cancel]\t - not sending data - stopping server now! \n");
        Stop();
    }
    else
    {
    	iOperation = EIdle;
    }
}

// ----------------------------------------------------------------------------
// CBtObjectServer::ConstructL()
// Constucts everything that can leave in the class
// ----------------------------------------------------------------------------
void CBtObjectServer::ConstructL(RArray<CImageInfo> &aImgArray)
{
    LOG("[CBtObjectServer::ConstructL]\t");

    LOG1("[CBtObjectServer::ConstructL]\t aImgArray.Count(): %d", aImgArray.Count());
	for(TInt i = 0; i < aImgArray.Count(); ++i)
	{
		iImgArray.Append(aImgArray[i]);

/* For debug		
#ifdef ENABLE_LOGGING		
		TFileName8 uri;
		aImgArray[i].GetUri(uri);
		TFileName8 log;
		CImageInfo::UriLog(uri, log);
    	LOG1("[CBtObjectServer::ConstructL]\t uri: %d", log);
#endif // ENABLE_LOGGING
*/
	}

    iAdvertiser = CBtDprServiceAdvertiser::NewL();

    // create OBEX object to receive obex transfer. 8 for reserving data in 8 bytes segments
    iObexBody = CBufFlat::NewL( 8 );

    User::LeaveIfError(iFs.Connect());

	InitTransferData();
	iRemoteAddress.Reset();
}

// ----------------------------------------------------------------------------
// CBtObjectServer::Stop()
// Disconnects the server.
// ----------------------------------------------------------------------------
//
void CBtObjectServer::Stop()
{
    LOG("[CBtObjectServer::Stop]\t");
    
    // already stopping
    if(!iStarted)
    	return;
    
	iOperation = ETerminating;

    LOG("[CBtObjectServer::Stop]\t cont...");
    iRemoteAddress.Reset();
    
    //cannot delete here. Stop called from iObexServer Callback function.
    if ( iObexServer && iObexServer->IsStarted())
    {
	    LOG("[CBtObjectServer::Stop]\t stopping iObexServer...");
    	iObexServer->Stop();
	}

    // Trap this: if it fails, we cannot do anything.
    TInt leave; // for remove compiling warnings
    TRAP(leave, iAdvertiser->StopAdvertisingL());
    LOG1("[CBtObjectServer::Stop]\t iAdvertiser->StopAdvertising() leaves with %d", leave);

    LOG1("[CBtObjectServer::Stop]\t iObexServer: %d", iObexServer);

	iOperation = EIdle; 
	iStarted = EFalse;
	iConnected = EFalse;
	iTransportUp = EFalse;

    /* Sending 'stopped' status */
    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerStopped);
}

// ----------------------------------------------------------------------------
// CBtObjectServer::StartL()
// Starts the server.
// ----------------------------------------------------------------------------
//
void CBtObjectServer::StartL()
{
    LOG("[CBtObjectServer::StartL]\t");

	TBool btOn = RsUtils::IsBluetoothOn();
	LOG1("[CBtObjectServer::StartL]\t btOn: %d", btOn);
	if( !btOn )
		User::Leave(KErrCancel);

    TRAPD( err, InitialiseServerL() );
    LOG1("CBtObjectServer::StartL]\t InitialiseServerL leaves with %d", err);

    if ( err != KErrNone )
    {
        Stop();
    	User::Leave(err);
    }

    iStarted = ETrue;

    /* Send 'server started' status */
    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerStarted);
}

// ----------------------------------------------------------------------------
// CBtObjectServer::InitialiseServerL()
// Initialises the server.
// ----------------------------------------------------------------------------
//
void CBtObjectServer::InitialiseServerL()
{
    LOG("CBtObjectServer::InitialiseServerL]\t");

    if ( iObexServer )
    {
        ASSERT( IsConnected() ); // server already running
        return;
    }

    LOG("[CBtObjectServer::InitialiseServerL]\t - getting channel");

	// Set the Socket's security with parameters,
	// Authentication, Encryption, Authorisation and Denied
	// Method also return the channel available to listen to.
	TInt channel = SetSecurityWithChannelL( EFalse, EFalse, EFalse, EFalse );

    // start the OBEX server
    TObexBluetoothProtocolInfo obexProtocolInfo;

    LOG1("[CBtObjectServer::InitialiseServerL]\t setting %S ", &KRFCOMMDesC);
    obexProtocolInfo.iTransport.Copy( KRFCOMMDesC() );

    LOG1("[CBtObjectServer::InitialiseServerL]\t setting port %d", channel);
    obexProtocolInfo.iAddr.SetPort( channel );

    LOG("[CBtObjectServer::InitialiseServerL]\t CObexServer::NewL()");
    if(iObexServer)
    {
    	delete iObexServer;
    	iObexServer = NULL;
    }
    iObexServer = CObexServer::NewL( obexProtocolInfo );

    LOG("[CBtObjectServer::InitialiseServerL]\t iObexServer->Start()");
    User::LeaveIfError(iObexServer->Start( this ) );

    TUUID who(KBTSDPDPROService);
	User::LeaveIfError(iObexServer->SetLocalWho(who.LongForm()));

    // advertise this service
    LOG("[CBtObjectServer::InitialiseServerL]\t iAdvertiser->StartAdvertisingL()");
    iAdvertiser->StartAdvertisingL( channel );
    LOG("[CBtObjectServer::InitialiseServerL]\t iAdvertiser->UpdateAvailabilityL()");
    iAdvertiser->UpdateAvailabilityL( ETrue );

}

// ----------------------------------------------------------------------------
// CBtObjectServer::InitTransferData()
// Initialises the variables needed for progressing and processing the printing
// ----------------------------------------------------------------------------
//
void CBtObjectServer::InitTransferData()
{
    LOG("CBtObjectServer::InitTransferData]\t");

	InitHeaderVariables();
	iProgress = 0;

	// delete this to be ready to send NULL object.
    if(iObexBufObject)
    	delete iObexBufObject;
    iObexBufObject = NULL;

    // no need to delete. Reset is enough.
    iObexBody->Reset();
}

// ----------------------------------------------------------------------------
// CBtObjectServer::InitHeaderVariables()
// Initialises the variables needed for progressing and processing the printing
// ----------------------------------------------------------------------------
//
void CBtObjectServer::InitHeaderVariables()
{
    LOG("CBtObjectServer::InitHeaderVariables]\t");

	iOffset = 0;
	iCount = 0; // KErrNotFound indicates "the rest"
	iSize = KErrNotFound;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::SetSecurityWithChannelL()
// Sets the security on the channel port and returns the available port.
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::SetSecurityWithChannelL( TBool aAuthentication,
                                                  TBool aEncryption,
                                                  TBool aAuthorisation,
												  TBool aDenied )

{

    LOG("CBtObjectServer::SetSecurityWithChannelL]\t");

	// Local variable to channel to listen to.
	TInt channel;

	RSocketServ socketServer;

	// Connect to SocetServer
    LOG("[CBtObjectServer::SetSecurityWithChannelL]\t - connecting to socketServer");
	User::LeaveIfError( socketServer.Connect() );
	CleanupClosePushL( socketServer );

	RSocket socket;

	// Open the Socket connection
    LOG("[CBtObjectServer::SetSecurityWithChannelL]\t - opening a socket");
	User::LeaveIfError( socket.Open( socketServer, KRFCOMMDesC() ) );
	CleanupClosePushL( socket );

	// Retreive to one channel that is available.
    LOG("[CBtObjectServer::SetSecurityWithChannelL]\t - get one channel");
	User::LeaveIfError( socket.GetOpt( KRFCOMMGetAvailableServerChannel,KSolBtRFCOMM, channel ) );

	// Set the Socket's Port.
	TBTSockAddr sockaddr;
	sockaddr.SetPort( channel );

    LOG("[CBtObjectServer::SetSecurityWithChannelL]\t - setting security settings");

	// Set the security according to.
	TBTServiceSecurity serviceSecurity;

	serviceSecurity.SetUid ( KBtProtocolUid ); // UID for _this_ security service; internal inside the phone
	serviceSecurity.SetAuthentication ( aAuthentication );
	serviceSecurity.SetEncryption ( aEncryption );
	serviceSecurity.SetAuthorisation ( aAuthorisation );
	serviceSecurity.SetDenied( aDenied );

	// Attach the security settings.
	sockaddr.SetSecurity(serviceSecurity);

    LOG("[CBtObjectServer::SetSecurityWithChannelL]\t - binding socket to a address");

	// Bind and start listeing the port with security set,
	User::LeaveIfError(socket.Bind(sockaddr));
	User::LeaveIfError(socket.Listen(KSimultainousSocketsOpen ) );

	// now close the socket and the socket server
	CleanupStack::PopAndDestroy(2);  //  socket, socketServer

    LOG("[CBtObjectServer::SetSecurityWithChannelL]\t - returning");

	return channel;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::IsConnected()
// Results true if the server is connected.
// ----------------------------------------------------------------------------
//
TBool CBtObjectServer::IsConnected()
{
    LOG1("CBtObjectServer::IsConnected]\t %d", iConnected);
    return iConnected;
}


// ----------------------------------------------------------------------------
// CBtObjectServer::IsTransporting()
// Results true if the transport connection is up.
// ----------------------------------------------------------------------------
//
TBool CBtObjectServer::IsTransporting()
{
    LOG1("CBtObjectServer::IsTransporting]\t %d", iTransportUp);
    return iTransportUp;
}
// ----------------------------------------------------------------------------
// IsSendingData()
// @return ETrue if the server is sending data to a Bluetooth device
// ----------------------------------------------------------------------------
TBool CBtObjectServer::IsSendingData()
{
    LOG("CBtObjectServer::IsSendingData]\t");

	if(ESending == iOperation)
		return ETrue;
	
	return EFalse;
}

// ----------------------------------------------------------------------------
// IsSendingData()
// @return ETrue if the server is finished and can be deleted
// ----------------------------------------------------------------------------
TBool CBtObjectServer::IsFinished()
{
    LOG("CBtObjectServer::IsFinished]\t");

	if(ESending != iOperation)
		return ETrue;
	
	return EFalse;
}

/*****************************************************************************/
/* Following methods implement the Obex Observer callbacks in this class     */
/*****************************************************************************/

// ----------------------------------------------------------------------------
// CBtObjectServer::ErrorIndication()
// Receive error indication.
// ----------------------------------------------------------------------------
//
void CBtObjectServer::ErrorIndication( TInt aError )
{
    LOG2("CBtObjectServer::ErrorIndication]\t iOperation: %d, aError: %d", iOperation, aError);
    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerError, aError);
}

// ----------------------------------------------------------------------------
// CBtObjectServer::TransportUpIndication()
// Called when the underlying socket transport connection is made from
// a remote client to the server
// ----------------------------------------------------------------------------
//
void CBtObjectServer::TransportUpIndication()
{
    LOG1("CBtObjectServer::TransportUpIndication]\t iOperation: %d", iOperation);

	iTransportUp = ETrue;

    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerTransportUp);
}

// ----------------------------------------------------------------------------
// CBtObjectServer::TransportDownIndication()
// Transport connection is dropped.
// ----------------------------------------------------------------------------
//
void CBtObjectServer::TransportDownIndication()
{
    LOG1("[CBtObjectServer::TransportDownIndication]\t iOperation: %d", iOperation);

	iTransportUp = EFalse;
	
	if(ETerminating != iOperation)
		iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerTransportDown);
}

// ----------------------------------------------------------------------------
// CBtObjectServer::ObexConnectIndication()
// Invoked when an OBEX connection is made from a remote client.
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::ObexConnectIndication(
    const TObexConnectInfo& /*aRemoteInfo*/, const TDesC8& /*aInfo*/ )
{
    LOG("CBtObjectServer::ObexConnectIndication]\t");

	TInt message = KErrAccessDenied;

    /* Check the address of the remote device */
    if(AllowConnection())
    {
    	message = KErrNone;
    	iConnected = ETrue;
	    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerConnected);
    }

    LOG1("CBtObjectServer::ObexConnectIndication]\t returns %d", message);
	return message;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::ObexDisconnectIndication()
// OBEX server has been disconnected.
// ----------------------------------------------------------------------------
//
void CBtObjectServer::ObexDisconnectIndication( const TDesC8& /*aInfo*/ )
{
    LOG("CBtObjectServer::ObexDisconnectIndication]\t");

	if(ETerminating != iOperation)
	{
	    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerDisconnected);
	}
   	iConnected = EFalse;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::PutRequestIndication()
// ----------------------------------------------------------------------------
//
CObexBufObject* CBtObjectServer::PutRequestIndication()
{
    LOG("CBtObjectServer::PutRequestIndication]\t");

    /* This is the object where server receives the client request over OBEX */
    return iObexBufObject;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::PutPacketIndication()
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::PutPacketIndication()
{
    LOG("CBtObjectServer::PutPacketIndication]\t");

    return KErrNone;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::PutCompleteIndication()
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::PutCompleteIndication()
{
    LOG("CBtObjectServer::PutCompleteIndication]\t");

    return KErrNone;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::GetRequestIndication()
// Called when a full get request has been received from the client.
// ----------------------------------------------------------------------------
//
CObexBufObject* CBtObjectServer::GetRequestIndication( CObexBaseObject* aRequestedObject )
{
    LOG("[CBtObjectServer::GetRequestIndication]\t");

	CObexBufObject* object = NULL;

    /* Check if connection allowed for the remote device */
    if(AllowConnection())
    {
	    iOperation = ESending;
	    TRAPD(leave, object = HandleGetRequestL(aRequestedObject));
        if(KErrNone != leave)
		    LOG1("[CBtObjectServer::GetRequestIndication]\t HandleGetRequestL leaves with %d", leave );

		// Stop advertising if still advertisig...
        TRAP(leave, iAdvertiser->StopAdvertisingL());
        if(KErrNone != leave)
	    	LOG1("[CBtObjectServer::GetRequestIndication]\t StopAdvertisingL leaves with %d", leave);
    }

    LOG1("[CBtObjectServer::GetRequestIndication]\t return object pointer %d", object );
    return object;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::GetPacketIndication()
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::GetPacketIndication()
{
    LOG("[CBtObjectServer::GetPacketIndication]\t");

    /* Setting that we are now sending data */
    iOperation = ESending;

    return KErrNone;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::GetCompleteIndication()
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::GetCompleteIndication()
{
    LOG("[CBtObjectServer::GetCompleteIndication]\t");

    if(iProgress >= 100)
        // Sending status that the whole file sent to BT-device 
	    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerFileSent, iProgress);
	else
	    // Sending status that one package sent to BT-device 
	    iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerPackageSent, iProgress);

    // We are finished for one package - init
    SetCompleted();

    return KErrNone;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::SetPathIndication()
// ----------------------------------------------------------------------------
//
TInt CBtObjectServer::SetPathIndication( const CObex::TSetPathInfo& /*aPathInfo*/,
                                         const TDesC8& /*aInfo*/ )
{
    LOG("CBtObjectServer::SetPathIndication]\t");
    return KErrNone;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::AbortIndication()
// ----------------------------------------------------------------------------
//
void CBtObjectServer::AbortIndication()
{
    LOG("CBtObjectServer::AbortIndication]\t");

    SetCompleted();

    /* Sending status that one file sent to BT-device */
    iObs.HandleObjectServerEvent(KErrAbort);

}

/*****************************************************************************/
/* Own privates	*/
/*****************************************************************************/

// ----------------------------------------------------------------------------
// CBtObjectServer::AllowConnection()
// Checks if the connection should allow.
// ----------------------------------------------------------------------------
//
TBool CBtObjectServer::AllowConnection()
{
    LOG("CBtObjectServer::AllowConnection]\t");

    // Return immediatedly if addresses macth
    if( iRemoteAddress != 0 && iRemoteAddress == iAllowedAddress)
    	return ETrue;
    
    TBool allow = EFalse;

    if( iRemoteAddress == 0 ) 
    {
	    /* Getting address of the remote device */
	    TBTSockAddr remoteAddress;
	    iObexServer->RemoteAddr(remoteAddress);

	    /* Getting the actual bluetooth address */
	    iRemoteAddress = remoteAddress.BTAddr();

	    /* Comparing the two bluetooth device addresses */
	    allow = (iRemoteAddress == iAllowedAddress);

#ifdef _DEBUG

	    TBuf<100> origName;
	    iAllowedAddress.GetReadable(origName);
	    TBuf<100> remoteName;
	    iRemoteAddress.GetReadable(remoteName);

		LOG1("[CBtObjectServer::AllowConnection]\t btAddressOrig: %S", &origName);
	    LOG1("[CBtObjectServer::AllowConnection]\t btAddressRemote: %S", &remoteName);
#endif
    }
    

    if (!allow)
    {
	    LOG("[CBtObjectServer::AllowConnection]\t BT addresses DON'T match!");
        iObs.HandleObjectServerEvent(MBtObjectServerObserver::KObjectServerBTDeviceMismatch);
    }
	else
	{
	    LOG("[CBtObjectServer::AllowConnection]\t BT addresses match!");
	}

	return allow;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::HandleGetRequestL()
// Called when a full get request has been received from the client.
// ----------------------------------------------------------------------------
//
CObexBufObject* CBtObjectServer::HandleGetRequestL( CObexBaseObject* aRequestedObject )
{
	User::LeaveIfNull(aRequestedObject);
    
    TFileName name(aRequestedObject->Name());

	TFileName log;
	CImageInfo::UriLog(name, log);
    LOG1("[CBtObjectServer::HandleGetRequestL]\t file requested: \"%S\"", &log);

	/* get application parameters header */
    GetAppHeaderL(aRequestedObject);

	//Append obex header start in uri
	_LIT(KObex, "obex:");
	_LIT(KSlash, "/");
	TFileName scheme(KObex());
	// Note here: the correct format would be uuid.LongForm(), 
	// but in currently supported services it is not required.
	scheme.AppendNum(KBTSDPDPROService, EHex);
	scheme.Append(KSlash());
	
	TInt pos = name.Find(scheme);
	if(KErrNotFound == pos)
		name.Insert(0, scheme);

    return CreateObexObjectL(name);
}

// ----------------------------------------------------------------------------
// CBtObjectServer::GetAppHeaderL()
// ----------------------------------------------------------------------------
//
void CBtObjectServer::GetAppHeaderL(CObexBaseObject* aRequestedObject)
{
    LOG("CBtObjectServer::GetAppHeaderL]\t");

	/* get header parameters */
	InitHeaderVariables();

	TInt indx = 0; // grows cumulatively during GetTriplet
	TInt tmp = 0;

	// offset
	TUint tag = 0;
	TInt len = KBtTripletLength;
	HBufC8* params = (aRequestedObject->AppParam()).AllocLC();

	if(len <= params->Des().Length())
	{
		tmp = TBtMapper::TripletValue(params->Des(), indx, tag);
	}
	if(KBtObexTagOffset == tag)
		iOffset = tmp;

    LOG1("CBtObjectServer::GetAppHeaderL]\t offset: %d", iOffset);

	// count
	tag = 0;
	len += KBtTripletLength;
	if(len <= params->Des().Length())
	{
		tmp = TBtMapper::TripletValue(params->Des(), indx, tag);
	}
	if(KBtObexTagCount == tag)
		iCount = tmp;

    LOG1("CBtObjectServer::GetAppHeaderL]\t count: %d", iCount);

	// size
	tag = 0;
	len += KBtTripletLength;
	if(len <= params->Des().Length())
	{
		tmp = TBtMapper::TripletValue(params->Des(), indx, tag);
	}
	if(KBtObexTagSize == tag)
		iSize = tmp;

    LOG1("CBtObjectServer::GetAppHeaderL]\t size: %d", iSize);

	CleanupStack::PopAndDestroy(params);
}

// ----------------------------------------------------------------------------
// CBtObjectServer
//
// ----------------------------------------------------------------------------
CObexBufObject* CBtObjectServer::CreateObexObjectL(const TDesC& aUri)
{
	TFileName log;
	CImageInfo::UriLog(aUri, log);
    LOG1("[CBtObjectServer::CreateObexObjectL]\t file requested: \"%S\"", &log);
    
    if(iObexBufObject)
    	delete iObexBufObject;
    iObexBufObject = NULL;

	TInt size = KErrNotFound;

	for(TInt i = 0; i < iImgArray.Count(); ++i)
	{
		if(iImgArray[i].CompareUri(aUri))
		{
			TFileName file;
			iImgArray[i].GetFilePathL(file);
	    	LOG1("[CBtObjectServer::CreateObexObject]\t file = \"%S\"", &file );

			GetDataSequenceL(file, size);
			if(iObexBody)
                {
                LOG2("[CBtObjectServer::CreateObexObject]\t data len: %d, file size: %d", iObexBody->Size(), size);
    			iObexBufObject = CObexBufObject::NewL(iObexBody);
                }

			break;
		}
	}
    if(!iObexBufObject)
    {
	    LOG1("[CBtObjectServer::CreateObexObject]\t return NULL: %d", iObexBufObject);
    	return iObexBufObject;
    }

    // Fill headers
	// File size (application parameters)

	TBuf8<20> triplet;

	triplet.Zero();
	triplet.Append((TChar)KBtObexTagSize);
	triplet.Append((TChar)KBtDataLength4);
	//append value
	triplet.Append((TChar) ((size & 0xFF000000) >> 24) );
	triplet.Append((TChar) ((size & 0xFF0000) >> 16) );
	triplet.Append((TChar) ((size & 0xFF00) >> 8) );
	triplet.Append((TChar) (size & 0xFF) );
	triplet.ZeroTerminate();

	iObexBufObject->SetAppParamL(triplet);

   	return iObexBufObject;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::GetDataSequenceL()
// ----------------------------------------------------------------------------
//
void CBtObjectServer::GetDataSequenceL(const TFileName& aFileName, TInt& aFileSize)
{
    //LOG1("[CBtObjectServer::GetDataSequenceL]\t for file \"%S\"", aFileName);

	RFile file;
	CleanupClosePushL(file);
	TInt err = file.Open(iFs, aFileName, EFileRead|EFileShareReadersOnly);
	if(KErrNone != err)
		User::LeaveIfError(file.Open(iFs, aFileName, EFileRead|EFileShareAny));

	// Get file size
	aFileSize = KErrNotFound;
	file.Size(aFileSize);
    
	if(!iObexBody)
	{
		iObexBody = static_cast<CBufFlat*>(User::LeaveIfNull(CBufFlat::NewL(8)));
	}
	iObexBody->Reset();

	// if iCount is zero, return empty body...
	if(0 == iCount)
		return;

	// count to send
	TInt len = iCount;

	// ...if iCount is -1, return the rest of the file
	// (http://www.bluetooth.com/NR/rdonlyres/276DE16A-BDB9-48BF-8123-AF01E3730E5F/925/BPP_SPEC_V10.pdf)
	if(KErrNotFound == len)
	{
		len = aFileSize - iOffset;
	}
    //LOG2("[CBtObjectServer::GetDataSequenceL]\t iOffset: %d, len: %d", iOffset, len);

	HBufC8* data = static_cast<HBufC8*>(User::LeaveIfNull(HBufC8::NewLC(len)));
	TPtr8 dataPtr( data->Des() );

	dataPtr.Zero();
	User::LeaveIfError( file.Read(iOffset, dataPtr, len) );
    //LOG1("[CBtObjectServer::GetDataSequenceL]\t read data->Des().Length(): %d", data->Des().Length());

	iObexBody->InsertL(0, data->Des());

	CleanupStack::PopAndDestroy(2); // data, file
	
	//count proggress of this file
	if(!iOffset)
		iProgress = iOffset;
	else
		iProgress = (((iOffset << 8) / aFileSize) * 100) >> 8;
	
    LOG1("[CBtObjectServer::GetDataSequenceL]\t Progress: %d", iProgress);
	return;
}

// ----------------------------------------------------------------------------
// CBtObjectServer::SetCompleted()
// ----------------------------------------------------------------------------
//
void CBtObjectServer::SetCompleted()
{
    // method to indicate a package is sent
    LOG("[CBtObjectServer::SetCompleted]\t");

	// init requested parameters
	InitTransferData();

    /* If the user of the server called Cancel() */
    if(ETerminating == iOperation)
    {
	    LOG("[CBtObjectServer::SetCompleted]\t to stop...");
    	Stop();
    }
    else
    {
	    LOG("[CBtObjectServer::SetCompleted]\t package sent...");
    	iOperation = EIdle;
    }

	return;
}

//  End of File
