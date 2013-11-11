// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cdbpreftable.h>

#include <commsdat.h>
#include <commsdattypesv1_1.h>
#include <commdbconnpref.h>
#include <miut_err.h>
#include <es_enum.h>		// For TConnectionInfo
#include <imutcon.h>
#include "IMSK.H"


#ifdef _DEBUG
#define __IMSK_SIMULATION
#include <etel.h>
#endif

#define __IMSK_LOGGING


CImConnect* CImConnect::NewL(const CImIAPPreferences& aIAPPreferences, CImTextServerSession &aTextSession)
	{
	CImConnect* self = new (ELeave) CImConnect(aIAPPreferences, &aTextSession, NULL);
	CActiveScheduler::Add(self);
	return self;
	}

EXPORT_C CImConnect* CImConnect::NewL(const CImIAPPreferences& aIAPPreferences, RSocketServ& aSocketServerHandle)
	{
	CImConnect* self = new (ELeave) CImConnect(aIAPPreferences, NULL, &aSocketServerHandle);
	CActiveScheduler::Add(self);
	return self;
	}

void CImConnect::RunL()
	{
	User::RequestComplete(iReportStatus, iStatus.Int());
	}

void CImConnect::DoCancel()
	{
	iConn.Close();
	User::RequestComplete(iReportStatus, KErrCancel);
	}

EXPORT_C void CImConnect::StartL(TRequestStatus& aStatus)
	{
	// Attempt the first connection using the appropriate IAP.
	iReportStatus = &aStatus;
	if (iTextSession != NULL)
		{
		User::LeaveIfError(iConn.Open(iTextSession->GetSocketServ()));
		}
	else
		{
		User::LeaveIfError(iConn.Open(*iSocketServerHandle));
		}
	ConnectL();
	SetActive();
	aStatus=KRequestPending;
	}

#ifdef __IMSK_SIMULATION
void CImConnect::SetIAPsToFail(TInt aIAPsToFail)
	{
	iIAPsToFail=aIAPsToFail;
	}
#endif

void CImConnect::ConnectL()
	{
	if (iIAPPreferences.SNAPDefined())
		{
		ConnectWithSNAP();
		}
	else
		{
		ConnectWithIAPListL();
		}
	}

void CImConnect::ConnectWithSNAP()
	{
	TConnSnapPref snapPref(iIAPPreferences.SNAPPreference());
	iConn.Start(snapPref, iStatus);
	}

void CImConnect::ConnectWithIAPListL()
	{
	TInt iapCount=iIAPPreferences.NumberOfIAPs();

	if(iapCount==0)
		{
#ifdef __IMSK_LOGGING
		_LIT8(KImConDefault,"CImConnect connecting with defaults");
		if (iTextSession != NULL)
			{
			iTextSession->LogText(KImConDefault);
			}
#endif
		iConn.Start(iStatus);
		}
	else
		{
#ifdef __IMSK_SIMULATION
		if(iapCount<=iIAPsToFail)
			{
#ifdef __IMSK_LOGGING
			_LIT8(KImConSimFail,"CImConnect simulated failure of all IAPs");
			if (iTextSession != NULL)
				{
				iTextSession->LogText(KImConSimFail);
				}
#endif
			TRequestStatus* status = &iStatus;
			// Just pick an etel error and pretend we failed
   			User::RequestComplete(status, KErrEtelModemNotDetected);  
			return;
			}
#endif
		// Set up the appropriate override
#ifdef __IMSK_LOGGING
		_LIT8(KImConOverride,"CImConnect trying overrides");
		if (iTextSession != NULL)
			{
			iTextSession->LogText(KImConOverride);
			}
#endif

		TInt rank=1;
#ifdef __IMSK_SIMULATION
		for(TInt i=iIAPsToFail;i<iapCount;i++)
#else
		for(TInt i=0;i<iapCount;i++)
#endif
			{
			TCommDbConnPref pref;
			pref.SetDirection(ECommDbConnectionDirectionOutgoing);
			
			// legacy code which restricted the type of bearer
			pref.SetBearerSet(KCommDbBearerCSD | KCommDbBearerWcdma | KCommDbBearerLAN |
			                  KCommDbBearerVirtual | KCommDbBearerPAN | KCommDbBearerWLAN);

			TImIAPChoice iapChoice = iIAPPreferences.IAPPreference(i);

			pref.SetIapId(iapChoice.iIAP);
			pref.SetDialogPreference(iapChoice.iDialogPref);

			User::LeaveIfError(iPrefs.SetPreference(rank, pref));

			rank++;
			}

		iPrefs.SetConnectionAttempts(rank-1);
		iConn.Start(iPrefs, iStatus);
		}
	}

CImConnect::~CImConnect()
	{
	Cancel();
	// Fix for DEF40210:
	// Although it is not necessary because the RSocketServ will delete all RConnection handles when it is closed, 
	// we call this for completeness as this class should tidy up all resources it creates
	iConn.Close();
	}

CImConnect::CImConnect(const CImIAPPreferences& aIAPPreferences, CImTextServerSession* aTextSession, RSocketServ* aSocketServerHandle) : 
		CActive(EPriorityStandard), iIAPPreferences(aIAPPreferences), iTextSession(aTextSession), iSocketServerHandle(aSocketServerHandle)
	{
	}

TInt CImConnect::RunError(TInt aError)
	{
	if (iReportStatus)
		User::RequestComplete(iReportStatus, aError);

	return KErrNone;
	}

EXPORT_C TInt CImConnect::GetIAPValue(TUint32& aIap)
	{
	// INC040630 - AV20 Panic while disconnecting a service.
	// Cancel closes the RConnection. Check we still have a valid subsession
	// handle.
	TInt iapValue = KErrBadHandle;
	if (iConn.SubSessionHandle() != KNullHandle)
		{		
		TBuf<KCommsDbSvrMaxColumnNameLength> settingName;
		settingName.Copy(KCDTypeNameIAP);
		settingName.Append('\\');
		settingName.Append(TPtrC(KCDTypeNameRecordTag));
		iapValue = iConn.GetIntSetting(settingName, aIap);
		}
	return iapValue;
	}

EXPORT_C TInt CImConnect::GetIAPBearer(TUint32& aBearer)
	{
	TInt err = KErrBadHandle;
	if (iConn.SubSessionHandle() != KNullHandle)
		{
		TBuf<KCommsDbSvrMaxColumnNameLength> iapservicename;	
		TBuf<KCommsDbSvrMaxColumnNameLength> settingName;
		settingName.Copy(KCDTypeNameIAP);
		settingName.Append('\\');
		settingName.Append(TPtrC(KCDTypeNameServiceType));
		
		// Get the current active IAP's service name
		err = iConn.GetDesSetting(settingName, iapservicename);
		if(err==KErrNone)
			{
			// Set the bearer corresponding to the service name
			if( iapservicename.Compare(TPtrC(KCDTypeNameDialOutISP))==0 ||
				iapservicename.Compare(TPtrC(KCDTypeNameDialInISP))==0)
				aBearer=KCommDbBearerCSD;
			else if( iapservicename.Compare(TPtrC(KCDTypeNameOutgoingWCDMA))==0 ||
					 iapservicename.Compare(TPtrC(KCDTypeNameIncomingWCDMA))==0)
				aBearer=KCommDbBearerWcdma;
			else if(iapservicename.Compare(TPtrC(KCDTypeNameLANService))==0)
				aBearer=KCommDbBearerLAN;	
			else 
				err = KErrNotFound;
			}	
		}
	return err;
	}

EXPORT_C RConnection& CImConnect::GetConnection()
	{
	return iConn;
	}

EXPORT_C TInt CImConnect::Progress(TNifProgress& aProgress)
	{
	if (iConn.SubSessionHandle() != KNullHandle)
		return iConn.Progress(aProgress);
	return KErrBadHandle;
	}

EXPORT_C TInt CImConnect::GetRConnectionName(TName &aName)
	{
	return iConn.Name(aName);
	}

EXPORT_C TInt CImConnect::GetLastSocketActivityTimeout(TUint32& aTimeout)
	{
	TInt err = KErrBadHandle;

	if (iConn.SubSessionHandle() != KNullHandle)
		{
		TBuf<KCommsDbSvrMaxColumnNameLength> settingName;
		TBuf<KCommsDbSvrMaxColumnNameLength> iapBearer;

		settingName.Copy(KCDTypeNameIAP);
		settingName.Append('\\');
		settingName.Append(TPtrC(KCDTypeNameBearerType));

		// Get the current active IAP's bearer table name
		err = iConn.GetDesSetting(settingName, iapBearer);

		if (err == KErrNone)
			{
			// Get the last socket activity timeout from the bearer table
			iapBearer.Append('\\');
			iapBearer.Append(TPtrC(KCDTypeNameLastSocketActivityTimeout));
			err = iConn.GetIntSetting(iapBearer, aTimeout);
			}
		}

	return err;
	}

// Continuing the Existing RConnection	
void CImConnect::SecondaryStartL(CImTextServerSession* aPrimaryTextServerSession)
	{
	// openign the connection from socketserver.
	User::LeaveIfError(iConn.Open(iTextSession->GetSocketServ()));

#ifdef __IMSK_LOGGING
	_LIT8(KImConDefault,"SecondarySession connecting with existing RConnection");
	iTextSession->LogText(KImConDefault);
#endif

	TConnectionInfo connInfo;
	TPckg<TConnectionInfo> connInfoPckg(connInfo);
	TUint count;
	// Enumerates the number of currently active interfaces
	User::LeaveIfError(aPrimaryTextServerSession->GetCImConnect()->GetConnection().EnumerateConnections(count));
	// Get the information about currently active connection	
	User::LeaveIfError(aPrimaryTextServerSession->GetCImConnect()->GetConnection().GetConnectionInfo(count, connInfoPckg) );
	// Attaches the RConnection object to an existing interface
	User::LeaveIfError(iConn.Attach(connInfoPckg, RConnection::EAttachTypeNormal));
	}

