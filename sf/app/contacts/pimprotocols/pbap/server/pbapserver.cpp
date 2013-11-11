// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include <e32svr.h>

// KMimeVCardExtension is defined in miutmsg.h however including miutmsg.h also includes
// flogger.h and conflicts with our inclusion of commsdebug via btaccesshostlog.h
//#include <miutmsg.h>
_LIT(KMimeVCardExtension, ".vcf");

#include <bt_sock.h>
#include <obex.h>
#include <obexbttransportinfo.h>

#include <cntdb.h>

#include "clientserver.h"
#include "pbapserver.h"
#include "pbapsession.h"
#include "pbapappheader.h"
#include "pbapfoldertree.h"
#include "pbapfoldertelecom.h"
#include "pbapfolderroot.h"
#include "pbapfoldernodepb.h"
#include "pbapfoldernodech.h"
#include "pbapfoldersim.h"
#include "pbapcontactdbviews.h"
#include "pbapauthpasswordgetter.h"
#include "pbapxml.h"
#include "pbapserversecuritypolicy.h"
#include "pbaplogeng.h"

#include "btaccesshostlog.h"


_LIT8(KPBAPLocalWho, "\x79\x61\x35\xf0\xf0\xc5\x11\xd8\x09\x66\x08\x00\x20\x0c\x9a\x66");


const TInt KBufGranularity = 100;
const TInt KContactsDbOpenAttempts = 10;
const TInt KContactsDbOpenDelay = 100000; //0.1 secs

//
// TCleanupItem operations - CPbapServer::ReleaseObex()
//
void PbapServerReleaseObex(TAny* aPtr)
	{
	LOG_STATIC_FUNC
	reinterpret_cast<CPbapServer*>(aPtr)->ReleaseObex();
	}


void CleanupPbapServerReleaseObexPushL(CPbapServer& aPbapServer)
	{		 
	LOG_STATIC_FUNC
	TCleanupItem item(PbapServerReleaseObex, &aPbapServer);
	CleanupStack::PushL(item);
	}	


//
// CPbapServerShutdown
//
/*static*/ CPbapServerShutdown* CPbapServerShutdown::NewL()
	{
	LOG_STATIC_FUNC
	CPbapServerShutdown* self=new(ELeave) CPbapServerShutdown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

inline CPbapServerShutdown::CPbapServerShutdown()
	: CTimer(EPriorityLow)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

inline void CPbapServerShutdown::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	}

inline void CPbapServerShutdown::Start()
	{
	LOG_FUNC
	After(KPbapServerShutdownDelay);
	}

void CPbapServerShutdown::RunL()
	{
	LOG_FUNC
	CActiveScheduler::Stop();
	}


//
// CPbapServer
//
/*static*/ CPbapServer* CPbapServer::NewL()
	{
	LOG_STATIC_FUNC
	CPbapServer* self=new(ELeave) CPbapServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

	
CPbapServer::CPbapServer()
: CPolicyServer(EPriorityStandard, KPbapServerPolicy, ESharableSessions)
	{
	LOG_FUNC
	}


CPbapServer::~CPbapServer()
	{
	LOG_FUNC
	
	// delete the obex server first because its destructor may call
	// functions which this class implements
	ReleaseObex();
	
	if (iAsyncRestartObex)
		{
		iAsyncRestartObex->Cancel();
		delete iAsyncRestartObex;
		}

	delete iPasswordGetter;
	delete iPassword;
	delete iOutboundObject;
	delete iOutboundBuffer;
	delete iVCardExporter;
	delete iShutdown;
	
	// the database must still be open when closing contact views so always 
	// delete the object owning the views before the database
	delete iContactViews;

	delete iFolderTree;
	delete iLogWrapper;
	delete iContacts;
	iFs.Close();
	iBufStreamer.Close();
	iAppHeaderDetails.Close();
	}
	

void CPbapServer::ConstructL()
	{
	LOG_FUNC

	StartL(KPbapServerName);

	// ensure that the server will exit even if the 1st client fails to connect
	iShutdown = CPbapServerShutdown::NewL();
	
	// create buffers ready for first obex get request
	iOutboundBuffer = CBufFlat::NewL(KBufGranularity);
	iOutboundObject = CObexBufObject::NewL(iOutboundBuffer);

	OpenContactsDbL();
	
	// create contact database view manager
	iContactViews = CPbapContactDbViews::NewL(*iContacts);
	
	// if there is a problem with creating a logeng client then we will continue
	// but without any call history folders
	TRAP_IGNORE(CreateLogEngineClientL());
	
	// create vCard exporter
	iVCardExporter = CPbapVCardExporterUtil::NewL(*iContacts, iLogWrapper);

	// create the restart obex callback used when an obex error occurs
 	TCallBack callback(RestartObex, this);
	iAsyncRestartObex = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);

	// start shutdown timer now
	iShutdown->Start();
	}

	
void CPbapServer::OpenContactsDbL()
	{
	LOG_FUNC
	TInt attempts=0;
	TInt error=KErrNone;
	while (attempts < KContactsDbOpenAttempts)
		{
		TRAP(error, iContacts = CContactDatabase::OpenL());

		if (error==KErrLocked || error==KErrInUse)
			{
			// db maybe locked due to race condition so try again
			++attempts;
			User::After(KContactsDbOpenDelay);
			}
		else
			{
			// success or not a db locked error so exit loop
			break;
			}
		}
	if (error)
		{
		LOG1(_L("Contacts database failed to open, error=%d"), error);
		User::Leave(error);	
		}
	}


void CPbapServer::CreateLogEngineClientL()
	{
	LOG_FUNC
	User::LeaveIfError(iFs.Connect());
	
	// create access to call history information
	iLogWrapper = CPbapLogWrapper::NewL(iFs);
	}


void CPbapServer::BuildVirtualFolderTreeL()
	{
	LOG_FUNC

	// create the root folder
	CVirtualFolders* rootFolder = CVirtualFolders::NewLC();

	rootFolder->PlaceFolderL(CFolderRoot::NewL(*this)); //ownership passed
	rootFolder->AttachSubtree(TelecomSubTreeL()); //ownership passed
	
#ifdef __INCLUDE_SIM1_FOLDERS__
	// create the SIM1 folder below root			
	CVirtualFolders* sim1Folder = CVirtualFolders::NewLC();

	sim1Folder->PlaceFolderL(CFolderSIM1::NewL(*this)); //ownership passed
	sim1Folder->AttachSubtree(SIMTelecomSubTreeL()); //ownership passed
	rootFolder->AttachSubtree(sim1Folder); //ownership passed

	CleanupStack::Pop(sim1Folder);
#endif // __INCLUDE_SIM1_FOLDERS__
		
	CleanupStack::Pop(rootFolder);	
	iFolderTree = rootFolder;
	iCurrentFolder = iFolderTree;
	}


CVirtualFolders* CPbapServer::TelecomSubTreeL()
	{
	LOG_FUNC
	
	// create telecom folder
	CVirtualFolders* telecomFolder = CVirtualFolders::NewLC();
	telecomFolder->PlaceFolderL(CFolderTelecom::NewL(*this)); //ownership passed
	
	// create pb sub folder
	CVirtualFolders* pbFolder = CVirtualFolders::NewLC();

	pbFolder->PlaceFolderL(CFolderNodePb::NewL(*this)); //ownership passed
	telecomFolder->AttachSubtree(pbFolder); //ownership passed

	CleanupStack::Pop(pbFolder);
	
	// make sure that we successfully created a logeng client
	if (iLogWrapper && (iLogWrapper->ClientAvailable()))
		{
		// create ich sub folder
		CVirtualFolders* ichFolder = CVirtualFolders::NewLC();

		ichFolder->PlaceFolderL(CFolderNodeIch::NewL(*this)); //ownership passed
		telecomFolder->AttachSubtree(ichFolder); //ownership passed

		CleanupStack::Pop(ichFolder);

		//create och sub folder
		CVirtualFolders* ochFolder = CVirtualFolders::NewLC();

		ochFolder->PlaceFolderL(CFolderNodeOch::NewL(*this)); //ownership passed
		telecomFolder->AttachSubtree(ochFolder); //ownership passed

		CleanupStack::Pop(ochFolder);

		// create mch sub folder
		CVirtualFolders* mchFolder = CVirtualFolders::NewLC();

		mchFolder->PlaceFolderL(CFolderNodeMch::NewL(*this)); //ownership passed
		telecomFolder->AttachSubtree(mchFolder); //ownership passed

		CleanupStack::Pop(mchFolder);

		// create cch sub folder
		CVirtualFolders* cchFolder = CVirtualFolders::NewLC();

		cchFolder->PlaceFolderL(CFolderNodeCch::NewL(*this)); //ownership passed
		telecomFolder->AttachSubtree(cchFolder); //ownership passed

		CleanupStack::Pop(cchFolder);
		}
	
	CleanupStack::Pop(telecomFolder);	
	return telecomFolder;
	}

#ifdef __INCLUDE_SIM1_FOLDERS__
CVirtualFolders* CPbapServer::SIMTelecomSubTreeL()
	{
	LOG_FUNC
	
	// create SIM1 telecom folder
	CVirtualFolders* telecomFolder = CVirtualFolders::NewLC();

	telecomFolder->PlaceFolderL(CFolderSIM1Telecom::NewL(*this)); //ownership passed

	// create SIM1 pb sub folder
	CVirtualFolders* pbFolder = CVirtualFolders::NewLC();

	pbFolder->PlaceFolderL(CFolderSIM1NodePb::NewL(*this)); //ownership passed
	telecomFolder->AttachSubtree(pbFolder); //ownership passed

	CleanupStack::Pop(pbFolder);
	
	// create SIM1 ich sub folder
	CVirtualFolders* ichFolder = CVirtualFolders::NewLC();

	ichFolder->PlaceFolderL(CFolderSIM1NodeIch::NewL(*this)); //ownership passed
	telecomFolder->AttachSubtree(ichFolder); //ownership passed

	CleanupStack::Pop(ichFolder);
	
	// create SIM1 och sub folder
	CVirtualFolders* ochFolder = CVirtualFolders::NewLC();

	ochFolder->PlaceFolderL(CFolderSIM1NodeOch::NewL(*this)); //ownership passed
	telecomFolder->AttachSubtree(ochFolder); //ownership passed

	CleanupStack::Pop(ochFolder);
	
	// create SIM1 mch sub folder
	CVirtualFolders* mchFolder = CVirtualFolders::NewLC();

	mchFolder->PlaceFolderL(CFolderSIM1NodeMch::NewL(*this)); //ownership passed
	telecomFolder->AttachSubtree(mchFolder); //ownership passed

	CleanupStack::Pop(mchFolder);
	
	// create SIM1 cch sub folder
	CVirtualFolders* cchFolder = CVirtualFolders::NewLC();

	cchFolder->PlaceFolderL(CFolderSIM1NodeCch::NewL(*this)); //ownership passed
	telecomFolder->AttachSubtree(cchFolder); //ownership passed

	CleanupStack::Pop(cchFolder);
	
	CleanupStack::Pop(telecomFolder);
	return telecomFolder;
	}
#endif // __INCLUDE_SIM1_FOLDERS__

/*virtual*/ CContactDatabase& CPbapServer::ContactDB() const
	{
	LOG_FUNC
	return *iContacts;
	}


/*virtual*/ CPbapContactDbViews& CPbapServer::ContactDbViews()
	{
	LOG_FUNC
	return *iContactViews;
	}

/*virtual*/ CPbapLogWrapper& CPbapServer::LogClient() const
	{
	LOG_FUNC
	return *iLogWrapper;
	}
	
/*virtual*/ MPbapExporter& CPbapServer::Exporter()
	{
	LOG_FUNC
	return *static_cast<MPbapExporter*>(this);
	}

/*virtual*/ CPbapVCardExporterUtil& CPbapServer::ExporterUtil()
	{
	LOG_FUNC
	return *iVCardExporter;
	}

/*virtual*/ MPbapErrorReporter& CPbapServer::ErrorReporter()
	{
	LOG_FUNC
	return *static_cast<MPbapErrorReporter*>(this);
	}


/*virtual*/ void CPbapServer::StartExport()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(!iExportInProgress, Panic(EPbapServerPanicExportInProgress));
	
	// open the obex stream ready for export
	iBufStreamer.Open(*iOutboundBuffer);
	
	// set flag to indicate export started
	iExportInProgress=ETrue;
	}


/*virtual*/ void CPbapServer::ExportListingBeginL()
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{	
		// write the XML DTD header
		PbapDTD::WriteBeginL(iBufStreamer);
		}
	}


/*virtual*/ void CPbapServer::ExportListingEntryL(TInt aHandle, const TDesC& aName)
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{
		// write listing entry to stream
		PbapDTD::WriteListingEntryL(iBufStreamer, aHandle, aName);
		}
	}

	
/*virtual*/ void CPbapServer::ExportListingEndL()
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{
		// write the XML DTD end tag
		PbapDTD::WriteEndL(iBufStreamer);
		}
	}


/*virtual*/ void CPbapServer::ExportCallHistoryL(const CLogEvent& aEvent, TVCardVersion aFormat, TUint64 aFilter)
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{
		// convert log event to a vCard and add to stream
		iVCardExporter->ExportCallHistoryL(aEvent, iBufStreamer, aFormat, aFilter);
		}
	}

		
/*virtual*/ void CPbapServer::ExportContactL(TContactItemId aContactId, TVCardVersion aFormat, TUint64 aFilter)
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{	
		// convert contact to a vCard and add to stream
		iVCardExporter->ExportContactL(aContactId, iBufStreamer, aFormat, aFilter);
		}
	}
	

/*virtual*/ void CPbapServer::ExportEmptyVCardL(TVCardVersion aFormat)
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{	
		// create empty vCard and add to stream
		iVCardExporter->ExportEmptyVCardL(iBufStreamer, aFormat);
		}	
	}


/*virtual*/ void CPbapServer::ExportPhonebookSizeL(TInt aCount)
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{
		// add phonebook size to obex header, total size is 4 bytes
		TInt currentLength = iAppHeaderDetails.Length();
		iAppHeaderDetails.ReAllocL(currentLength+4);
		iAppHeaderDetails.SetMax();
		
		// add 1 byte tag id
		iAppHeaderDetails[currentLength] = CPbapAppHeader::EPhonebookSize;
		
		// add length of arguments, 2 bytes
		iAppHeaderDetails[currentLength+1] = 2;
		
		// add phonebook size, 2 bytes
		BigEndian::Put16(&iAppHeaderDetails[currentLength+2], aCount);
		}
	}


/*virtual*/ void CPbapServer::ExportNewMissedCallsL(TInt aCount)
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	if (iExportInProgress)
		{
		// add new missed call count to obex header, total size is 3 bytes
		TInt currentLength = iAppHeaderDetails.Length();
		iAppHeaderDetails.ReAllocL(currentLength+3);
		iAppHeaderDetails.SetMax();
		
		// add 1 byte tag id
		iAppHeaderDetails[currentLength] = CPbapAppHeader::ENewMissedCalls;

		// add length of arguments, 1 byte
		iAppHeaderDetails[currentLength+1] = 1;
		
		// add missed call count, 1 byte
		iAppHeaderDetails[currentLength+2] = aCount;
		}
	}

		
/*virtual*/ void CPbapServer::FinaliseExportL()
	{
	LOG_FUNC
	if(iExportInProgress == EFalse)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNotReadyForExport));
		User::Leave(KErrNotReady);
		}
	// set flag to indicate no more writing to the stream
	if (iExportInProgress)
		{	
		// close the stream
		iBufStreamer.Close();
	
		// add header information
		if (iAppHeaderDetails.Length())
			{
			iOutboundObject->SetAppParamL(iAppHeaderDetails);
			}

		// reset app header
		iAppHeaderDetails.Zero();

		// request active mode to improve sending if object is large enough
		const TObexTransportInfo* transInfo = iObex->TransportInfo();
		if (transInfo && (iOutboundObject->DataBuf()->Size() > transInfo->iTransmitMtu))
			{
			// ignore any error, the Open() may have failed
			iPhysLinkAdaptor.ActivateActiveRequester();
			}
		
		// send over obex
		User::LeaveIfError(iObex->RequestIndicationCallback(iOutboundObject));
		
		iExportInProgress=EFalse;
		}
	}


/*virtual*/ void CPbapServer::CancelExport()
	{
	LOG_FUNC
	// set flag to indicate
	iExportInProgress=EFalse;
	iAppHeaderDetails.Zero();
	
	// cleanup the buffers used for the get requests
	CleanupGetRequest();
	}


	
/*virtual*/ void CPbapServer::SendServiceUnavailableError()
	{
	LOG_FUNC
	ReportObexError(KErrGeneral);
	}

	
/*virtual*/ void CPbapServer::SendPreconditionFailedError()
	{
	LOG_FUNC
	ReportObexError(KErrArgument);		
	}


/*virtual*/ void CPbapServer::SendNotFoundError()
	{
	LOG_FUNC
	ReportObexError(KErrNotFound);		
	}


/**
 create new session
 */		
CSession2* CPbapServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
	{
	LOG_FUNC
	TVersion v(KPbapServerMajorVersionNumber,KPbapServerMinorVersionNumber,KPbapServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	// we only support one active session
	if (iActiveSession)
		{
		User::Leave(KErrInUse);
		}

	return new(ELeave) CPbapSession();
	}


/**
 a new session is being created Cancel the shutdown timer if it was running
 */
void CPbapServer::AddSession()
	{
	LOG_FUNC
	iActiveSession = ETrue;
	iShutdown->Cancel();
	}


/**
 a session is being destroyed. Start the shutdown timer if it is the last session.
 */
void CPbapServer::DropSession()
	{
	LOG_FUNC
	iActiveSession = EFalse;	
	iShutdown->Start();
	}
	

void CPbapServer::StartObexL()
	{
	LOG_FUNC

	if (!iObex)
		{
		TBTServiceSecurity serv;
		serv.SetUid(KPbapServerUid);

		serv.SetAuthentication(ETrue); // note: this needs to be reconsidered in light of SSP.
		serv.SetAuthorisation(ETrue);
		serv.SetEncryption(ETrue);

		// now set up Obex...
		TObexBluetoothProtocolInfo info;
		info.iTransport = KObexRfcommProtocol;
		info.iAddr.SetPort(KRfcommPassiveAutoBind);
		info.iAddr.SetSecurity(serv);

		iObex = CObexServer::NewL(info);
		iObex->SetLocalWho(KPBAPLocalWho);
		iObex->SetTargetChecking(CObexServer::EAlways);	

		// push self onto cleanup stack to ReleaseObex if a leave
		CleanupPbapServerReleaseObexPushL(*this);
		
		// start accepting requests to obex server
		TInt error = iObex->Start(this);
		if (error != KErrNone)
			{
			LOG1(_L("Error %d starting Obex server"), error);
			User::Leave(error);
			}

		if (iPassword)
			{
			// a password has been set so enable obex authentication
			TRAP(error, iObex->SetChallengeL(*iPassword));
			if (error != KErrNone)
				{
				LOG1(_L("Error %d setting authentication challenge"), error);
				User::Leave(error);
				}		
			}
	
		// obex started succesfully, retrieve assigned RFCOMM channel
		const TObexBtTransportInfo* transportInfo = static_cast<const TObexBtTransportInfo*>(iObex->TransportInfo());
		TUint rfcommChannel = transportInfo->iAddr.Port();

		// now the RFCOMM channel is known register the SDP record to allow bluetooth connections
		TRAP(error, RegisterWithSdpL(rfcommChannel));
		if (error != KErrNone)
			{
			LOG1(_L("Error %d registering SDP record"), error);
			User::Leave(error);
			}
		
		// enable client authentication challenge handling
		iObex->SetCallBack(*this);		
		CleanupStack::Pop(this);
		}
	}


void CPbapServer::ReleaseObex()
	{
	LOG_FUNC
	
	// stop all connections to the obex server (we need to delete the server rather
	// than just call Stop() otherwise the rfcomm channel will not be assigned
	// a new value when starting it again)
	delete iObex;
	iObex = NULL;
		
	// delete the stored password
	delete iPassword;
	iPassword = NULL;
	
	// release the SDP record
	ReleaseSdpRegistration();
	}
	
/*static*/ TInt CPbapServer::RestartObex(TAny* aAny)
	{
	LOG_STATIC_FUNC
	CPbapServer* self = static_cast<CPbapServer*>(aAny);
	self->DoRestartObex();
	return KErrNone;
	}
	
void CPbapServer::DoRestartObex()
	{
	LOG_FUNC

	// We need to keep the authentication password as this re-start was not initiated
	// by the Client side application.
	HBufC* currentPassword = iPassword;
	iPassword = NULL;
	ReleaseObex();
	
	iPassword = currentPassword;
	TRAPD(err, StartObexL());

	if (err != KErrNone)
		{
		LOG1(_L("Error %d restarting Obex server"), err);
		
		// nothing left to do, panic server to inform client
		Panic(EPbapServerPanicUnrecoverableObexError);
		}
	}

#ifdef _DEBUG
void CPbapServer::ErrorIndication(TInt aError)
#else
void CPbapServer::ErrorIndication(TInt /*aError*/)
#endif
	{
	LOG_FUNC
	LOG1(_L("Obex Error Indication: %d"), aError);

	// this is a fatal OBEX error so reset our state...
	CleanupOnDisconnect();
	
	// ...and re-start the obex server (needs to be done async from this error indication callback)
	iAsyncRestartObex->CallBack();
	}


void CPbapServer::TransportUpIndication()
	{
	LOG_FUNC
	// Availablity is updated here in a deviation from the specification which 
	// defines the Pbap session as existing from the time the OBEX connection is 
	// established. Since OBEX can only maintain one connection over BT in the 
	// current implemenation it is more practical for us to set our availablity now
	// instead of waiting for the OBEX connection as the RFComm channel is already 
	// closed effectively making our service unavailable
	UpdateAvailability(EPbapUnavailable);
	}

	
void CPbapServer::TransportDownIndication()
	{
	LOG_FUNC
	// underlying transport has disconnected
	UpdateAvailability(EPbapAvailable);
	CleanupOnDisconnect();
	}

	
void CPbapServer::ObexConnectIndication(const TObexConnectInfo& aRemoteInfo, const TDesC8& /*aInfo*/)
	{
	LOG_FUNC
	LOG(_L("Connect packet target header:"));
	LOGHEXDESC(aRemoteInfo.iTargetHeader);

	if (aRemoteInfo.iTargetHeader!=KPBAPLocalWho)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicInvalidTargetHeader));
		}

	else
		{
		// build the virtual folder tree at the start of the session. Failure to build
		// the folder tree is handled later with error respones to get/set path requests
		TRAP_IGNORE(BuildVirtualFolderTreeL());
		
		// get remote address
		TSockAddr sockAddr;
		iObex->RemoteAddr(sockAddr);
		TBTDevAddr devAddr = static_cast<TBTSockAddr>(sockAddr).BTAddr();
		
		// create a physical links adaptor
		if (iSocketServ.Connect() == KErrNone)
			{
			// don't store the error from Open, any further sniff/active requests will not
			// succeed but this will not stop any functionality
			if (iPhysLinkAdaptor.Open(iSocketServ, devAddr) == KErrNone)
				{
				// request sniff straight away as we will request active mode when it is
				// required
				iPhysLinkAdaptor.ActivateSniffRequester();
				}
			}
  		}
	}


void CPbapServer::ObexDisconnectIndication(const TDesC8& /*aInfo*/)
	{
	LOG_FUNC
	CleanupOnDisconnect();
	}


void CPbapServer::PutRequestIndication()
	{
	LOG_FUNC
	// Always return a Bad Request error in response to PUT requests
	ReportObexError(KErrCorrupt);
	}


TInt CPbapServer::PutPacketIndication()
	{
	LOG_FUNC
	// the remote device should have handled the PUT request error and not sent
	// any data. If not then just respond to each packet with another error
	return KErrIrObexRespForbidden;
	}

	
void CPbapServer::PutCompleteIndication()
	{
	LOG_FUNC
	// the remote device should have handled the PUT request error and not sent
	// any data. If not then just respond with an error 
	ReportObexError(KErrAccessDenied);
	}


// report an OBEX error based on a Symbian global error code
void CPbapServer::ReportObexError(TInt aSymbianError)
	{
	TObexResponse obexErr = ERespSuccess;
	
	switch (aSymbianError)
		{
		case KErrAccessDenied: // the request is barred
			obexErr = ERespForbidden;
			break;
			
		case KErrNotFound: // the requested folder doesnot exist
			obexErr = ERespNotFound;
			break;

		case KErrNotSupported: // the folder does not support get requests (e.g SIM repositories not implemented)
			obexErr = ERespNotImplemented;
			break;

		case KErrArgument: // invalid parameter value in header
			obexErr = ERespPreCondFailed;
			break;

		case KErrCorrupt: // badly formatted header
			obexErr = ERespBadRequest;
			break;

		case KErrGeneral:
		default: // another error stopped request completing (e.g out of memory)
			obexErr = ERespServiceUnavailable;
			break;			
		}
	iObex->RequestIndicationCallbackWithError(obexErr);
	
	// get request failed so clean up for the next one
	CleanupGetRequest();
	}
		
void CPbapServer::GetRequestIndication(CObexBaseObject* aRequiredObject)
	{
	LOG_FUNC
	TRAPD(error, DoGetRequestIndicationL(aRequiredObject));
	if (error != KErrNone)
		{
		ReportObexError(error);
		}
	}

		
void CPbapServer::DoGetRequestIndicationL(CObexBaseObject* aRequiredObject)
	{
	LOG_FUNC
	CPbapAppHeader* appHeader = CPbapAppHeader::NewL();
	CleanupStack::PushL(appHeader);
	
	// parse the application parameters
	appHeader->ParseL(aRequiredObject->AppParam());
		
	// determine the operation from the object type and parsed application params	
	TPbapOperation operation = appHeader->DeterminePBAPOperationL(aRequiredObject->Type());
	
	// we only allow one get operation at a time
	if (iGetRequestFolder)
		{
		// get operation in progress
		User::Leave(KErrInUse);
		}
		
	// check name for .vcf extension
	TPtrC name = aRequiredObject->Name();		
	if (name.Right(KMimeVCardExtension().Length())==KMimeVCardExtension())
		{
		if (operation == EPullVCardListing)
			{
			// we found a .vcf extension but shouldn't have
			User::Leave(KErrArgument);
			}
			
		// trim extension
		name.Set(name.Left(name.Length()-KMimeVCardExtension().Length()));
		}
	else if ((operation == EPullPhoneBook) || (operation == EPullVCard))
		{
		// we didn't find a .vcf extension but should have
		User::Leave(KErrArgument);		
		}
	
	// create a copy of the trimmed name so that it can be modified to remove the path
	// information when we set iGetRequestFolder below
	RBuf objectName;
	objectName.CleanupClosePushL();
	objectName.CreateL(name);

	// navigate to folder (the path is always absolute for x-bt/phonebook type requests)
	iGetRequestFolder = ResolvePath(objectName, appHeader->IsAbsolutePathOp());

	// the remaining object name might be a file representation of a phonebook
	// (e.g pb.vcf) if so it should have a matching folder representation. If this
	// is true then route the get request to this folder
	if (iGetRequestFolder && objectName.Length() && operation != EPullVCard)
		{
		LOG(_L("Re-routing Get request from file representation to folder representation"));
		CVirtualFolders* subFolder = iGetRequestFolder->NavigateFolder(objectName);
		if (subFolder)
			{
			iGetRequestFolder = subFolder;
			objectName.Zero(); // no more information so set length of objectName to zero
			}
		}

	// let the folder handle the get request. The request is completed asynchronously
	// so it is the folders responsibilty to notify obex when processing is complete	
	if (iGetRequestFolder)
		{
		// To supress false positive coverity error for alias and double_free from cleanup stack for appHeader.
		// The function Get takes ownership of appHeader by aliasing and does its own destruction of it either immediately on error                    
		// or when finnished with the object. 
		// coverity[double_free]
		TInt error = iGetRequestFolder->Folder().Get(objectName, appHeader);
		if (error != KErrNone)
			{
			LOG1(_L("Error %d from issuing Get to folder object"), error);
			ReportObexError(error);
			}
			
		// ownership of appHeader passed so don't destroy it
		CleanupStack::PopAndDestroy(); //objectName
		CleanupStack::Pop(); //appHeader
		}
	else
		{
		CleanupStack::PopAndDestroy(2); //appHeader, objectName
		ReportObexError(KErrNotFound);
		}
	}


/**
Return folder which request will be operating on, and modify passed in path
descriptor to contain only the file name
*/
CVirtualFolders* CPbapServer::ResolvePath(TDes& aPath, TBool aAbsolute) const
	{
	LOG_FUNC
	LOG2(_L("Resolving path %S, [absolute=%d]"), &aPath, aAbsolute);

	const TChar KSlash('/');
	TInt index = 0;
	
	// it is ok to have a NULL iFolderTree as we ignore any problems we had when
	// building it so just return NULL
	if (!iFolderTree)
		{
		return NULL;
		}
	
	// if we have a iFolderTree then we must have a iCurrentFolder
	if(!iCurrentFolder)
		{
		__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNoCurrentFolder));
		return NULL;
		}
		
	// start at root if absolute path or the current folder if relative path
	CVirtualFolders* nextTree = aAbsolute ? iFolderTree : iCurrentFolder;

	if (aPath.Length()>=1) // blank means dont change folder
		{
		if (aPath[index]==KSlash)
			{
			aPath.Delete(index,1);
			nextTree = iFolderTree;
			}

		do
			{
			index = aPath.Locate(KSlash);
			if (index!=KErrNotFound)
				{
				// try to enter subfolder
				TPtrC folder(aPath.Left(index));
				nextTree = nextTree->NavigateFolder(folder);
				if (!nextTree)
					{
					// faulty path spec
					nextTree = NULL;
					break;
					}
				else
					{
					// trim off this bit of path AND the '/'!
					aPath.Delete(0, index+1);
					}
				}
			} while (index!=KErrNotFound);
		}
						
	return nextTree;
	}


TInt CPbapServer::GetPacketIndication()
	{
	LOG_FUNC
	// do nothing (the PSE doesnot support a UI with progress updates)
	return KErrNone;
	}

	
void CPbapServer::GetCompleteIndication()
	{
	LOG_FUNC

	TInt err = iObex->RequestCompleteIndicationCallback(KErrNone);
	__ASSERT_DEBUG(err==KErrNone, Panic(EPbapServerPanicUnexpectedError));

	// prepare for next request
	CleanupGetRequest();
	}

	
void CPbapServer::SetPathIndication(const CObex::TSetPathInfo& aPathInfo, const TDesC8& /*aInfo*/)
	{
	LOG_FUNC
	
	TInt err = KErrNone;

	// it is ok to have a NULL iFolderTree as we ignore any problems we had when
	// building it
	if (!iFolderTree)
		{
		LOG(_L("Folder tree not created"));
		err = KErrNotFound;
		}
	else if (aPathInfo.Parent())
		{
		LOG(_L("Set folder to parent"));

		// if we have a iFolderTree then we must have a iCurrentFolder
		if(!iCurrentFolder)
			{
			__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNoCurrentFolder));
			err = KErrNotFound;
			}
		else
			{
			// move to parent folder if it exists (it will not if already at root)
			CVirtualFolders* folder = iCurrentFolder->ParentFolder();
			if (folder)
				{
				iCurrentFolder = folder;
				}
			else
				{
				err = KErrNotFound;
				}
			}
		}
	else if (aPathInfo.iNamePresent)
		{
		if (aPathInfo.iName!=KNullDesC)
			{
			LOG1(_L("Set folder to child [%S]"), &aPathInfo.iName);

			// if we have a iFolderTree then we must have a iCurrentFolder
			if(!iCurrentFolder)
				{
				__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNoCurrentFolder));
				err = KErrNotFound;
				}
			else
				{
				CVirtualFolders* folder = iCurrentFolder->NavigateFolder(aPathInfo.iName);
				if (folder)
					{
					iCurrentFolder = folder;
					}
				else
					{
					err = KErrNotFound;
					}
				}
			}
		else
			{
			LOG(_L("Set folder to root"));

			// if we have a iFolderTree then we must have a iCurrentFolder
			if(!iCurrentFolder)
				{
				__ASSERT_DEBUG(EFalse, Panic(EPbapServerPanicNoCurrentFolder));
				err = KErrNotFound;
				}

			iCurrentFolder = iFolderTree;
			}
		}
	else
		{
		// this is not a request to change to the parent folder therefore we should have a name
		// header but we don't, return error
		LOG(_L("No Name header present"));
		err = KErrArgument;
		}
	
	// return any obex errors
	if (err == KErrNotFound)
		{
		LOG(_L("Error folder does not exist"));
		iObex->RequestCompleteIndicationCallback(ERespNotFound);	
		}
	else if (err == KErrArgument)
		{
		LOG(_L("Error invalid argument"));
		iObex->RequestCompleteIndicationCallback(ERespPreCondFailed);	
		}
	else
		{		
		iObex->RequestCompleteIndicationCallback(KErrNone);	
		}
	}

	
void CPbapServer::AbortIndication()
	{
	LOG_FUNC
	
	// the get request was aborted during the obex transfer (i.e. after processing
	// the request had completed). So just clean up the obex buffers
	CleanupGetRequest();
	}


void CPbapServer::CancelIndicationCallback()
	{
	LOG_FUNC
	// the get request was cancelled before sending any data.
	// so cancel any ongoing asynchronous processing and cleanup obex buffers
	if (iGetRequestFolder)
		{
		iGetRequestFolder->Folder().CancelGet();
		}
	
	CleanupGetRequest();
	}

void CPbapServer::GetUserPasswordL(const TDesC& aRealm)
	{
	LOG_FUNC
	LOG1(_L("Authentication challenge [Realm = %S]"), &aRealm);

	delete iPasswordGetter;
	iPasswordGetter = NULL;
	iPasswordGetter = CPbapAuthPasswordGetter::NewL(*this);

	// get the Bluetooth device address of the client making the authentication challenge
	TSockAddr sockAddr;
	iObex->RemoteAddr(sockAddr);
	TBTDevAddr devAddr = static_cast<TBTSockAddr>(sockAddr).BTAddr();

	// start async password request
	iPasswordGetter->GetPassword(aRealm, devAddr);
	}

void CPbapServer::HandlePasswordRetrieved(TInt aError, const TDesC& aPassword)
	{
	LOG_FUNC
	
	if (aError == KErrNone)
		{
		// if we successfully got a password, pass it to obex
		TRAP(aError, iObex->UserPasswordL(aPassword));
		}
		
	if (aError != KErrNone)
		{
		LOG1(_L("Error %d creating response to authentication challenge"), aError);
		// creating the challenge response has failed. Since there is no way to report
		// the error to the obex server the only option is to force a reset of the
		// connection state by restarting the server
		iObex->Stop();
		aError = iObex->Start(this);
		if (aError != KErrNone)
			{
			LOG1(_L("Error %d restarting Obex server"), aError);
			ReleaseObex();
			
			// nothing left to do, panic server to inform client
			Panic(EPbapServerPanicUnrecoverableObexError);
			}
		}
	}

void CPbapServer::SetPasswordL(HBufC* aPassword)
	{
	LOG_FUNC
	if (iObex)
		{
		// the obex server exists so set its challenge now
		iObex->SetChallengeL(*aPassword);
		}
	// store the password (transfers ownership)
	delete iPassword;
	iPassword = aPassword;
	}

void CPbapServer::CleanupGetRequest()
	{
	LOG_FUNC

	// request sniff mode again as get request finished, ignore errors
	iPhysLinkAdaptor.ActivateSniffRequester();

	// clean the exporter
	if (iGetRequestFolder)
		{
		iGetRequestFolder->Folder().GetComplete();
		}
	
	iGetRequestFolder = NULL;
	iOutboundBuffer->Reset();
	iOutboundObject->Reset();
	iBufStreamer.Close();
	}


void CPbapServer::CleanupOnDisconnect()
	{
	LOG_FUNC
	
	CleanupGetRequest();
	
	// close physical links adaptor
	iPhysLinkAdaptor.Close();
	iSocketServ.Close();
	
	// destroy the virtual folder tree when the session ends
	delete iFolderTree;
	iFolderTree = NULL;
	iCurrentFolder = NULL;
		
	// close contacts views to save memory (this will also cancel any current requests to sort and search views)
 	iContactViews->CloseAllViews();

	// make sure the authentication notifier is cancelled by deleting its handler
	delete iPasswordGetter;
	iPasswordGetter = NULL;
	}


void Panic(TPbapServerPanicCode aPanic)
	{
	LOG_STATIC_FUNC
	User::Panic(KPbapServerPanic, aPanic);
	}


/**
 Perform all server initialisation, in particular creation of the
 scheduler and server and then run the scheduler
 */
static void RunServerL()
	{
	LOG_STATIC_FUNC

	// naming the server thread after the server helps to debug panics	
	User::RenameThread(KPbapServerName); 
			
	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// create the server (leave it on the cleanup stack)
	CPbapServer* server = CPbapServer::NewL();
	CleanupStack::PushL(server);

	// initialisation complete, now signal the client thread
	RProcess::Rendezvous(KErrNone);

	// ready to run
	CActiveScheduler::Start();

	// cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, scheduler);
	}


/**
 Server process entry-point.
 @return  KErrNone or a standard Symbian error code.
 */
TInt E32Main()
	{
	__UHEAP_MARK;

#ifdef __FLOG_ACTIVE
	// connect to logger
	(void)CBtLog::Connect();
#endif

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r, RunServerL());
		delete cleanup;
		}

#ifdef __FLOG_ACTIVE
	// close logger connection
	CBtLog::Close();
#endif

	__UHEAP_MARKEND;
	return r;
	}
