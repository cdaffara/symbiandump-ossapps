/*
 * Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
 *
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * EmbedDev AB - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "connectionengine.h"
#include "settingsengine.h"
#include "podcastmodel.h"

CConnectionEngine* CConnectionEngine::NewL(CPodcastModel& aPodcastModel)
	{
	CConnectionEngine* self = new (ELeave) CConnectionEngine(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CConnectionEngine::~CConnectionEngine()
	{
	delete iMobility;

	Cancel();
	
	iConnection.Close();

	iSocketServer.Close();
	
	iObserverArray.Close();
	}

CConnectionEngine::CConnectionEngine(CPodcastModel& aPodcastModel):
					CActive(CActive::EPriorityStandard),iPodcastModel(aPodcastModel)
	{
	CActiveScheduler::Add(this);	
	}

void CConnectionEngine::ConstructL()
	{
	User::LeaveIfError(iSocketServer.Connect());
	User::LeaveIfError( iConnection.Open( iSocketServer ) );
	}

void CConnectionEngine::RunL()
	{		
	if ( iStatus.Int() == KErrNone )
		{		
		delete iMobility;
		iMobility = NULL;
		iMobility = CActiveCommsMobilityApiExt::NewL( iConnection, *this );
		}
	
	iConnectionState = iStatus.Int() == KErrNone?CConnectionEngine::EConnected:CConnectionEngine::ENotConnected;
	ReportConnectionL( iStatus.Int() );
	}

void CConnectionEngine::DoCancel()
	{
	}

TInt CConnectionEngine::RunError( TInt /*aError*/ )
	{
	return KErrNone;
	}

void CConnectionEngine::PreferredCarrierAvailable( TAccessPointInfo /*aOldAPInfo*/,
		TAccessPointInfo /*aNewAPInfo*/,
		TBool aIsUpgrade,
		TBool aIsSeamless )
	{   
	if ( aIsUpgrade )
		{        
		}
	else
		{       
		}

	if ( aIsSeamless )
		{
		// in S60 3.2, this situation cannot occur.        
		}
	else
		{       
		// Sockets have to be closed at this point.        

		iMobility->MigrateToPreferredCarrier();
		}

	}

void CConnectionEngine::NewCarrierActive( TAccessPointInfo /*aNewAPInfo*/, TBool aIsSeamless )
	{    
	if ( aIsSeamless )
		{
		// in S60 3.2, this situation cannot occur.

		}
	else
		{        
		// Sockets have to be re-opened and check they can connect
		// to their server at this point.        

		iMobility->NewCarrierAccepted();
		}
	}

void CConnectionEngine::Error( TInt /*aError*/ )
	{

	}

TBool CConnectionEngine::ConnectionSettingL()
	{
	TBool selected( EFalse );

	CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
	CleanupStack::PushL( settings );

	TUint listedItems = 
	CMManager::EShowDefaultConnection |
	CMManager::EShowDestinations;

	TBearerFilterArray filter;
	ReportConnectionSelectionStart();
	selected = settings->RunApplicationSettingsL( iUserSelection,
			listedItems,
			filter );

	CleanupStack::PopAndDestroy( settings );
	ReportConnectionSelectionEnd();
	return selected;
	}


void CConnectionEngine::StartL(TConnectionType aConnectionType)
	{
	DP1("CConnectionEngine::StartL, aConnectionType=%d", aConnectionType);
	
	iConnection.Close();
	User::LeaveIfError( iConnection.Open( iSocketServer ) );
	// Connect using UI Setting
	if(aConnectionType == EDefaultConnection)
		{
		iConnection.Start( iStatus );
		SetActive();
		}
	else if(aConnectionType == EUserSelectConnection)
		{				
		TBool selected = ConnectionSettingL();

		if ( selected )
			{
			switch ( iUserSelection.iResult )
				{
				case CMManager::EDestination:
					{					
					iSnapPreference.SetSnap( iUserSelection.iId );
					iConnection.Start( iSnapPreference, iStatus );
					aConnectionType = ESNAPConnection;
					break;
					}				
				default: // CMManager::EAlwaysAsk
				case CMManager::EDefaultConnection:
					{					
					iConnection.Start( iStatus );
					break;
					}									
				}						
			}
		else
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrCancel);
			}
		
			SetActive();
		}	
	else if (aConnectionType == EIAPConnection)
		{
		iCommdbPreference.SetIapId((iPodcastModel.SettingsEngine().SpecificIAP()& KUseIAPMask));
		iCommdbPreference.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
		iCommdbPreference.SetDirection(ECommDbConnectionDirectionOutgoing);
		iConnection.Start( iCommdbPreference, iStatus );
		SetActive();
		}
	// Connect using SNAP 
	else
		{
		iSnapPreference.SetSnap(iPodcastModel.SettingsEngine().SpecificIAP());
		iConnection.Start( iSnapPreference, iStatus );
		SetActive();	
		}		
	
	iConnectionType = aConnectionType;
	iConnectionState = CConnectionEngine::EConnecting;
	}

void CConnectionEngine::Stop()
	{
	DP("CConnectionEngine::Stop");
	iConnection.Stop();
	iConnectionState = CConnectionEngine::ENotConnected;
	}


RConnection& CConnectionEngine::Connection()
	{
	return iConnection;	
	}

CConnectionEngine::TConnectionState CConnectionEngine::ConnectionState()
	{
	TInt selectedConn = (TInt) iSnapPreference.Snap();
	TInt specIAPSNAP = iPodcastModel.SettingsEngine().SpecificIAP();
	// If we have IAP preference then get that from our current connection and mask out the selected iap
	if((specIAPSNAP&KUseIAPFlag))
		{
		selectedConn = iCommdbPreference.IapId();
		specIAPSNAP = specIAPSNAP&KUseIAPMask;
		}
	
	// IAPSNAP must be > 0 and then  if the IAP/Sel conn IAP differs
	if(specIAPSNAP >0 && specIAPSNAP !=  selectedConn )
		{
		if(iConnection.SubSessionHandle() != 0)
			{
			iConnection.Stop();
			}
		
		iConnectionState = CConnectionEngine::ENotConnected;
		}
	else
		{
		// We have a user selection or default selction, check our current connection state.
		TNifProgress progress;
		if(iConnection.Progress(progress) == KErrNone)
			{
			if(progress.iError == KErrNone && progress.iStage != 0)
				{
				if(progress.iStage == KLinkLayerOpen)
					{
					iConnectionState = CConnectionEngine::EConnected;
					}				
				}
			else if(iConnectionState != CConnectionEngine::EConnecting)
				{
				iConnectionState = CConnectionEngine::ENotConnected;
				}
			}
		else
			{
			iConnectionState = CConnectionEngine::ENotConnected;
			}
		}
	
	return iConnectionState;
	}

EXPORT_C void CConnectionEngine::AddObserver(MConnectionObserver* aObserver)
	{
	iObserverArray.Append(aObserver);
	}

RSocketServ& CConnectionEngine::SockServ()
	{
	return iSocketServer;
	}


void CConnectionEngine::ReportConnectionL(TInt aError)
	{
	TInt noObservers = iObserverArray.Count();
	while(noObservers)
		{
		noObservers--;
		iObserverArray[noObservers]->ConnectCompleteL(aError);
		}
	}


void CConnectionEngine::ReportConnectionSelectionStart()
	{
	TInt noObservers = iObserverArray.Count();
	while(noObservers)
		{
		noObservers--;
		iObserverArray[noObservers]->ConnectionSelectionStart();
		}
	}


void CConnectionEngine::ReportConnectionSelectionEnd()
	{
	TInt noObservers = iObserverArray.Count();
	while(noObservers)
		{
		noObservers--;
		iObserverArray[noObservers]->ConnectionSelectionEnd();
		}
	}
