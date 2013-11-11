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

#ifndef PODCAST_CONNECTIONENGINE_H
#define PODCAST_CONNECTIONENGINE_H


// Connect using mobility extension
#include <in_sock.h>
#include <connpref.h>
#include <commdbconnpref.h>
#include <comms-infras/cs_mobility_apiext.h>

// Using connection manager settings UI
#include <cmapplicationsettingsui.h>
class CPodcastModel;

class MConnectionObserver
	{
public:
	virtual void ConnectionSelectionStart() = 0;
	virtual void ConnectionSelectionEnd() = 0;
	virtual void ConnectCompleteL(TInt aErrorCode) = 0;
	virtual void Disconnected() = 0;
	};

/**
 * This class helps to connect using three available methods
 * SNAP, Mobility extension and Connection manager UI
 */
class CConnectionEngine:public CActive, public MMobilityProtocolResp
	{
public:
	enum TConnectionType
		{
		EUserSelectConnection,		
		ESNAPConnection,		
		EMobilityConnection,
		EDefaultConnection,
		EIAPConnection
		};
	
	enum TConnectionState
		{
		ENotConnected,
		EConnecting,
		EConnected
		};

	static CConnectionEngine* NewL(CPodcastModel& aPodcastModel);
	~CConnectionEngine();
	void StartL(TConnectionType aConnectionType);
	RConnection& Connection();
	TConnectionState ConnectionState();
	IMPORT_C void AddObserver(MConnectionObserver* aObserver);
	RSocketServ& SockServ();
	void Stop();
private: // Methods
	CConnectionEngine(CPodcastModel& aPodcastModel);
	void ConstructL();
protected:
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
protected:
	// From MMobilityProtocolResp
	void PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
			TAccessPointInfo aNewAPInfo,
			TBool aIsUpgrade,
			TBool aIsSeamless );
	
	void NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless );
	void Error( TInt aError );
protected:
	// UI Setting
	TBool ConnectionSettingL();
	
	void ReportConnectionL(TInt aError);
	void ReportConnectionSelectionStart();
	void ReportConnectionSelectionEnd();
	
private:
	RConnection                   iConnection;
	RSocketServ                   iSocketServer;
	// Connect using mobility extension
	CActiveCommsMobilityApiExt*   iMobility;

	// Using connection manager settings UI
	TCmSettingSelection   iUserSelection;
	
	// Connect using Commdb
	TCommDbConnPref iCommdbPreference;
	
	// Connect using SNAP setting
	TConnSnapPref iSnapPreference;
	
	TConnectionType iConnectionType;
	TConnectionState iConnectionState;
	RPointerArray<MConnectionObserver> iObserverArray;
	CPodcastModel &iPodcastModel; 	// reference to the model
	};

#endif // PODCAST_CONNECTIONENGINE_H
