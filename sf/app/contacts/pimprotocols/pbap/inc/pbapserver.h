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

#ifndef PBAPSERVER_H
#define PBAPSERVER_H

#include <e32std.h>
#include <obex.h>
#include <btsdp.h>
#include <bt_sock.h>

#include "pbapfolderclient.h"
#include "pbapexporter.h"
#include "pbaperrorreporter.h"
#include "pbapvcardexporterutil.h"
#include "pbappanic.h"

// forward declarations
class CObexServer;
class CContactDatabase;
class CVirtualFolders;
class CPbapVCardExporter;
class CPbapContactDbViews;
class RFs;
class CPbapLogWrapper;
class CPbapAuthPasswordGetter;
class CPbapVCardExporterUtil;


const TUint KPbapServerShutdownDelay = 5000000;

enum TPbapAvailability 
	{
	EPbapUnavailable	= 0,
	EPbapAvailable		= 0xff
	};

/**
 server shutdown timer
 */
class CPbapServerShutdown : public CTimer
	{
public:
	static CPbapServerShutdown* NewL();
	inline void Start();

private:
	inline CPbapServerShutdown();
	inline void ConstructL();
	void RunL();
	};


/**
 PBAP server
 */
class CPbapServer : public CPolicyServer,
					public MObexServerNotifyAsync,
					public MObexAuthChallengeHandler,
					public MVirtualFolderClient,
					public MPbapExporter,
					public MPbapErrorReporter
	{
public:
	static CPbapServer* NewL();
	~CPbapServer();

	void AddSession();
	void DropSession();

	void StartObexL();
	void ReleaseObex();

	void SetPasswordL(HBufC* aPassword);

	void HandlePasswordRetrieved(TInt aError, const TDesC& aPassword);

private:
	CPbapServer();
	
	void ConstructL();
	void OpenContactsDbL();
	void CreateLogEngineClientL();
	
	void RegisterWithSdpL(TUint aRfcommChannel);
	void ReleaseSdpRegistration();
	void UpdateAvailability(TPbapAvailability aAvailablility);
	void DoUpdateAvailabilityL(TPbapAvailability aAvailablility);

	void BuildVirtualFolderTreeL();
	CVirtualFolders* TelecomSubTreeL();

#ifdef __INCLUDE_SIM1_FOLDERS__
	CVirtualFolders* SIMTelecomSubTreeL();
#endif // __INCLUDE_SIM1_FOLDERS__

	CVirtualFolders* ResolvePath(TDes& aPathInObjectOut, TBool aAbsolute) const;

	void DoGetRequestIndicationL(CObexBaseObject* aRequiredObject);
	
	void CleanupGetRequest();
	void CleanupOnDisconnect();
	
	void ReportObexError(TInt aSymbianError);
	
	static TInt RestartObex(TAny* aAny);
	void DoRestartObex();

private:
	// from CServer2
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

private:
	// from MObexServerNotifyAsync
	virtual void ErrorIndication(TInt aError);
	virtual void TransportUpIndication();
	virtual void TransportDownIndication();
	virtual void ObexConnectIndication(const TObexConnectInfo& aRemoteInfo, const TDesC8& aInfo);
	virtual void ObexDisconnectIndication(const TDesC8& aInfo);
	virtual void PutRequestIndication();
	virtual TInt PutPacketIndication();
	virtual void PutCompleteIndication();
	virtual void GetRequestIndication(CObexBaseObject* aRequiredObject);
	virtual TInt GetPacketIndication();
	virtual void GetCompleteIndication();
	virtual void SetPathIndication(const CObex::TSetPathInfo& aPathInfo, const TDesC8& aInfo);
	virtual void AbortIndication();
	virtual void CancelIndicationCallback();

private:
	// from MObexAuthChallengeHandler
	virtual void GetUserPasswordL(const TDesC &aRealm);
	
private:
	// from MVirtualFolderClient
	virtual CContactDatabase& ContactDB() const;
	virtual MPbapExporter& Exporter();
	virtual MPbapErrorReporter& ErrorReporter();
	virtual CPbapContactDbViews& ContactDbViews();
	virtual CPbapLogWrapper& LogClient() const;
	virtual CPbapVCardExporterUtil& ExporterUtil();
	
private:
	// from MPbapExporter
	virtual void StartExport();
	virtual void ExportListingBeginL();
	virtual void ExportListingEntryL(TInt aHandle, const TDesC& aName);
	virtual void ExportListingEndL();
	virtual void ExportCallHistoryL(const CLogEvent& aEvent, TVCardVersion aFormat, TUint64 aFilter);
	virtual void ExportContactL(TContactItemId aContactId, TVCardVersion aFormat, TUint64 aFilter);
	virtual void ExportEmptyVCardL(TVCardVersion aFormat);
	virtual void ExportPhonebookSizeL(TInt aCount);
	virtual void ExportNewMissedCallsL(TInt aCount);
	virtual void FinaliseExportL();
	virtual void CancelExport();

private:
	// from MPbapErrorReporter
	virtual void SendServiceUnavailableError();
	virtual void SendPreconditionFailedError();
	virtual void SendNotFoundError();

private:
	TBool						iActiveSession;
	CPbapServerShutdown*		iShutdown;

	CObexServer*				iObex;
	CPbapAuthPasswordGetter*	iPasswordGetter;
	HBufC*						iPassword;
	CObexBufObject*				iOutboundObject;
	CBufFlat*					iOutboundBuffer;
	RBufWriteStream				iBufStreamer;
	RBuf8						iAppHeaderDetails;
	TBool						iExportInProgress;

	CContactDatabase*			iContacts;
	CPbapVCardExporterUtil*		iVCardExporter;
	CPbapContactDbViews* 		iContactViews;

	RFs							iFs;
	CPbapLogWrapper*			iLogWrapper;
	
	CVirtualFolders*			iFolderTree;

	RSdp						iSdpServer;
	RSdpDatabase				iSdpDatabase;
	TSdpServRecordHandle		iSdpRecordHandle;

	RSocketServ					iSocketServ;
	RBTPhysicalLinkAdapter		iPhysLinkAdaptor;
	
	CAsyncCallBack*				iAsyncRestartObex;
	
private:
	CVirtualFolders*			iCurrentFolder; //not owned
	CVirtualFolders*			iGetRequestFolder; //not owned
	};

#endif //PBAPSERVER_H
