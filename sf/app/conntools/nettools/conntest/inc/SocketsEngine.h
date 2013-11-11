/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CSocketsEngine implements an engine for connection 
* component checking: Interface opening/closing, connecting, 
* sending/receiving data, etc.
* CSocketsEngine is an active object
*
*/


#ifndef __SOCKETSENGINE_H__
#define __SOCKETSENGINE_H__

// INCLUDES
#include <in_sock.h>
#include <es_sock.h>
#include <http.h>
#include <commdbconnpref.h>
#include <extendedconnpref.h>
#include "TimeOutNotify.h"
#include "ProgressNotifyHandler.h"
#include "datasendnotifyhandler.h"
#include "conntest.hrh"
#include "alractiveobject.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CSocketsRead;
class CSocketsWrite;
class CTimeOutTimer;
class MUINotify;
class CHttpClient;
class CProgressNotifier;
class CSettingData;
class CCustomPrefsData;
class CSendTimer;

// CLASS DECLARATION

/**
*  CSocketsEngine
*  
*  This class is the main engine part of the sockets application.
*  It establishes a TCP connection using its server name and port 
*  number (performing a DNS lookup operation first, if appropriate).
*  It creates instances of separate active objects to perform reading 
*  from, and writing to, the socket.
*/
class CSocketsEngine : public CActive, 
                       public MTimeOutNotify,
                       public MProgressNotifyHandler, 
                       public MDataSendNotifyHandler,
                       public MMobilityProtocolResp
                       
{
public:
    enum TRoamingSetting
        {
        ERoamingOff,
        ERoamingAutomatic,
        ERoamingManual
        };
    
    // Utilized when selecting UI menu items
    enum TRoamingUIState
        {
        EIdle,
        EPendingPreferredCarrier,
        EPendingNewCarrierActive
        };
    
    enum TSocketsEngineState 
    {
        EInterfaceDown,
        EStartingInterface,
        EInterfaceUp,
        EConnecting,
        EConnected,
        EListening,
        ETimedOut, 
        ELookingUp,
        ELookUpFailed,
        EConnectFailed,
        EDisconnecting
    };
    
    enum TSocketsEngineStartType
    {
        EAskIap,
        EExtPrefs,
        EExtPrefsInternet,
        ENoPrefs,
        ENetworkId,
        ESnap,
        ETypeUnknown
    };
    
    
    /**
    * Two-phased constructor.
    * @param aConsole console to use for ui output
    * @return a pointer to the created instance of CSocketsEngine
    */
    static CSocketsEngine* NewL(MUINotify& aConsole);
    
    /**
    * Two-phased constructor.
    * @param aConsole console to use for ui output
    * @return a pointer to the created instance of CSocketsEngine
    */
    static CSocketsEngine* NewLC(MUINotify& aConsole);

    /**
    * Destructor.
    */
    ~CSocketsEngine();

    /**
     * Calls From ConnTestView that are responds to roaming notifications
     */
    void Migrate();
    void Ignore();
    void AcceptCarrier();
    void RejectCarrier();
    
    /**
    * Register to mobility API
    */
	void RegisterToMobilityAPIL();

    /**
    * UnRegister from mobility API
    */
	void UnRegisterFromMobilityAPI();
    
    /**
    * Start interface
    * @param aStartType Define start type (with or without preferences, by nework id)
    * @param aConnect   If True, make connection after startup
    */
    void StartInterfaceL(TSocketsEngineStartType aStartType, TBool aConnect = EFalse);
    
     /**
    * Start interface and possible connection with snap
    * @param aConnect   If True, make connection after startup
    */
    void StartConnWithSnapL(TBool aConnect = EFalse);

    /**
    * Starting method for testing RConnection::Close
    */
    void StartCloseInterfaceL();

    /**
    * Stop interface by calling RConnection::Stop
    */
    void StopInterfaceL();
    
    /**
    * Stop interface by calling RConnection::Close
    */
    void CloseInterface();

    /**
    * Make socket connection
    */
    void ConnectL();

    /**
    * Disconnect socket
    */
    void Disconnect();
    
    /**
    * Opens a Listening socket
    */
    void ListenL();
    
    /**
    * Write data to socket
    * @param aData data to be written
    */
    void WriteL(const TDesC8& aData);

    /**
    * Write data to socket (flood)
    * @param aData data to be written
    * @param aCount The data will be written aCount times
    */
    void WriteFloodL(const TDesC8& aData, TInt aCount);
    
    /**
    * Write HTTP request over socket
    * @param aHasBody defines if request has body
    */
	void SendHttpRequestOverSocketL(TBool aHasBody, TBool aDoPerformance);
    
    /**
    * Initiate read of data from socket
    */
    void Read();
    
    /**
    * Send HTTP request using http framework.
    * @param aHasBody       If true, send body data with POST
    * @param aDoPerformance If true, calculate performance, no outputs
    * @param aIsSecure      If true, make HTTPS request
    */
    void SendHttpFrameworkRequestL(TBool aHasBody, TBool aDoPerformance, TBool aIsSecure);

    /**
    * Print information about active connections and their
    * clients and sockets
    */
    void ConnectionInfoL();
    
    /**
    * Print information about interfaces.
    */
    void InterfaceInfoL();

    /**
    * Get socket engine state
    * @return Socket engine state
    */
    TSocketsEngineState GetSocketEngineState();
    
    /**
     * Gets Roaming state
     * @return TRoamingState
     */
    TRoamingUIState GetSocketEngineRoamingState();
    
    /**
     * Gets MobilityAPIImplementation
     * @return CALRActiveObject
     */
    CALRActiveObject* CSocketsEngine::GetMobilityAPI();
    
    /**
    * Get socket engine connection type
    * @return socket engine connection type
    */
    TSocketsEngineStartType GetSocketEngineConnType();
    
    /**
    * Set socket engine connection type
    */
    void SetSocketEngineConnType(const CSocketsEngine::TSocketsEngineStartType aConnStartType);

    /**
    * Set connection information
    * @param aData
    */
    void SetConnectionData(const CSettingData* aData);
    
    /**
    * Set Custom preferences data
    * @param aData
    */
    void SetCustomPrefsData(const CCustomPrefsData* aData);
    
    /**
    * QoS methods
    */
    void SetWLANQoS(TInt aClass);
    
    void QoS1();
    void QoS2();
    
    /**
    * Start the sending of the data
    * @param aData the data to send
    */
    void SendDataL(const TBuf8<KSendDataSize> aData);

    /**
    * Actual sending udp packets 
    * Must be public because of the timer calling it
    */
    void DoSendDataL();

public: // MDataSendNotifyHandler
    /**
    * The function to be called when data has been sent
    * @param aAmount the number of packets sent
    */
    void NotifySend(TInt aAmount);
    
public: // from MTimeOutNotify
    
    /**
    * The function to be called when a timeout occurs
    */
    void TimerExpired(); 

public: // from MProgressNotifyHandler
    
    /**
    * Handle progress notification
    */
    void ProgressNotifyReceivedL(TInt aStage, TInt aError);
    
    /**
    * Handle progress notification error
    */
    void ProgressNotifyError(TInt aStatus);

public: // from MMobilityProtocolResp
    /**
     * This method is called by middleware to notify that there are no suitable connections available.
     */
    virtual void Error( TInt aError ); 
    
    /**
     * This method is called by the middleware to notify a client about a new preferred connection.
     */
    virtual void PreferredCarrierAvailable(
                TAccessPointInfo aOldAP, 
                TAccessPointInfo aNewAP, 
                TBool aIsUpgrade, 
                TBool aIsSeamless ); 
    /**
     * This method is called by the middleware to notify a client that a preferred connection has been activated.
     */ 
    virtual void NewCarrierActive( TAccessPointInfo aNewAP, TBool aIsSeamless ); 

protected: // from CActive

    /**
    * Called when operation completes
    */
    void RunL();

    /**
    * Cancel any outstanding operation
    */
    void DoCancel();

private: // New methods

    /**
    * Perform the first phase of two phase construction 
    */
    CSocketsEngine(MUINotify& aConsole);
    
    /**
    * Perform the second phase construction of a CSocketsEngine 
    * @param aConsole the console to use for ui output
    */
    void ConstructL();
    
    /**
    * Initiate a connect operation on a socket
    * @param aAddr the ip address to connect to
    */
    void ConnectL(const TInetAddr& aAddr);

    /**
    * Handle a change in this object's status
    * @param aNewStatus new status
    */
    void ChangeStatus(TSocketsEngineState aNewStatus);
    
    /**
    * Display text on the console
    * @param aDes text to display
    */
    void PrintTextToConsole(const TDesC& aDes);

public: // for performance testing  
  	/**
	* Marks starting time into memory
	*
	*/
    
	void UDPFlood(TInt aPackets, TInt aPacketSize, TDesC& aData);
    /**
   	* Marks starting time into memory
    */
	void inline StartTickCount();
	
	/**
	* Calculates time interval using tick count and prints it
	*/
	void StopTickCount(const TDesC& aComponentName);

private:
	// Member variables
	TUint32						iStartTime; // for performance testing only
	TTime                       iStartTTime; // for performance testing only
	TTime						iThroughputStartTime; // for performance testing only
	TInt                        iTroughputDataSize;
    TCommDbConnPref             iPrefs;
    TConnPrefList*              iPrefsList;
    TExtendedConnPref*          iExtPrefs;
    CCustomPrefsData*           iCustomPrefsData;    
    RConnection                 iConnection;
    RConnection*                iCloseConnection;
    CProgressNotifier*          iProgressNotifier;
    CProgressNotifier*          iTempProgressNotifier;
    CHttpClient*                iHttpClient;
    TRoamingUIState             iRoamingState;
    TRoamingSetting             iRoaming;
	TSocketsEngineState         iEngineStatus; // this object's current status
	MUINotify&                  iConsole; // console for displaying text etc
	CSocketsRead*               iSocketsRead; // socket reader active object
	CSocketsWrite*              iSocketsWrite; // socket writer active object
	RSocket                     iSocket; // the actual socket
	RSocket						iListeningSocket;
	RSocketServ                 iSocketServ; // the socket server
	RHostResolver               iResolver; // DNS name resolver
	TNameEntry                  iNameEntry;
	TNameRecord                 iNameRecord;
	CTimeOutTimer*              iTimer; // timer active object
	TInetAddr                   iAddress;
    TInt                        iPort; // port number to connect to
    TBuf<KMaxServerNameLength>  iServerName; // server name to connect to
    TUint                       iProtocol;
    TUint                       iSocketType;
    TBool                       iConnectAfterStartup;
    TBuf<KMaxServerNameLength>  iHttpPage;
	CSettingData*               iSettingData;
    CSendTimer*                 iSendTimer; // timer active object
    HBufC8*                     iData; // The data to be sent
    TInt                        iSendCount;
    TBool                       iUseTTime;
    TBool 						iIsRegisteredToMobAPI; //flag to mark state of registration to Mob API
    CALRActiveObject*           iMobility; //pointer to mob API instance   
    TSocketsEngineStartType     iSockEngineStartType; //type of sock engine connection
    };
        
#endif // __SOCKETSENGINE_H__

