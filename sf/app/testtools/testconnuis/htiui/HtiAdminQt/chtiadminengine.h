/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HtiAdmin main.
*
*/

#ifndef CHTIADMINENGINE_H
#define CHTIADMINENGINE_H

#include <e32std.h>
#include <e32base.h>
#include <badesca.h> 


class HtiEngineWrapper;
class CPeriodic;
class CHtiCfg;


class CHtiAdminEngine : public CBase
	{
	public:
	
	/**
	 * Constructor
	 */
	CHtiAdminEngine();
	
	/**
	 * Destructor
	 */
	~CHtiAdminEngine();
	
	/**
	 * Two-phased constructor.
	 */
	static CHtiAdminEngine* NewL(HtiEngineWrapper *aEngineWrapper);

	/**
	 * Two-phased constructor.
	 */
	static CHtiAdminEngine* NewLC(HtiEngineWrapper *aEngineWrapper);
	
	void StartHtiL();
	
	void StopHtiL();
	
	void EnableBtByAddressL(const TDesC& aAddress);
	
	void EnableBtByNameL(const TDesC& aAddress);
	
	void BtSearchL();
	
	void IPListenL(const TDesC& aPort, const TDesC& aIAP);
	
	void IPConnectL(const TDesC& aHost, const TDesC& aPort, const TDesC& aIAP);
	
	void EnableSerialL(const TDesC& aComPortNumber);
	
	void EnableOtherCommL(const TDesC& aSelectedPluginName);
	
	void SetPriorityBackgroundL();
	
	void SetPriorityForegroundL();
	
	void SetPriorityHighL();
	
	void SetPriorityAbsoluteHighL();
	
	void AutoStartEnableL(TBool aEnableAutoStart);
	
	void ConsoleEnableL(TBool aEnableConsole);
	
	void ErrorDialogsEnableL(TBool aEnableErrorDialogs);
	
	void SetHtiCfgParamL(const TDesC& aParamName, const TDesC& aParamValue);
	void GetHtiCfgParamL(const TDesC& aParamName, TDes& aParamValue);
	
	void SetBtCfgParamL(const TDesC& aParamName, const TDesC& aParamValue);
	void GetBtCfgParamL(const TDesC& aParamName, TDes& aParamValue);
	
	void SetSerialCfgParamL(const TDesC& aParamName, const TDesC& aParamValue);
	void GetSerialCfgParamL(const TDesC& aParamName, TDes& aParamValue);
	
	void SetIPCfgParamL(const TDesC& aParamName, const TDesC& aParamValue);
	void GetIPCfgParamL(const TDesC& aParamName, TDes& aParamValue);
	
	void GetSelectedComm(TDes& aPluginName);
	
	void CheckStatusesL();
	
	CDesCArray* SearchCommPluginsL();	
	
	CDesCArray* SearchIAPsL();
	
	void CHtiAdminEngine::WatchDogEnableL(TBool aEnableWDog);
	
	/**
	* Kills the HtiWatchDog process if it is running.
	*/
	void KillHtiWatchDogL();

	/**
	* Stops the timer issuing the periodical HTI Framework process
	* status checks.
	*/
	void KillTimer();

	/**
	* The method called by the periodic timer.
	* @param aPtr pointer to this CHtiAdminAppUi instance
	* @return zero if the callback function should not be called again,
	* otherwise non-zero. Current implementation returns always ETrue.
	*/
	static TInt TimerCallBackL( TAny* aPtr );
	
	/**
	* Tries to find the HTIFramework.exe process and open a handle to it.
	* @param aPrs on return contains an open handle to the HTIFramework.exe
	* process if process found and opened successfully
	* @return ETrue if process found and opened successfully,
	* otherwise EFalse
	*/
	TBool OpenHtiProcess( RProcess& aPrs );
	
	/**
	* Updates the HTI Framework process status in the display.
	*/
	void UpdateStatusL();
	
	/**
	* Updates the HTI auto start status in the display.
	*/
	void UpdateAutoStartStatus();
	
	
	void UpdateConsoleStatus();
	
	void UpdateWatchDogStatus();
	
	/**
	* Updates the selected communication in the display.
	*/
	void UpdateSelectedComm();

	private:
		void ConstructL(HtiEngineWrapper *aEngineWrapper);
		
		void SetCfgParamL(const TDesC& aParamName, const TDesC& aParamValue, const TDesC& aFileName);
		void GetCfgParamL(const TDesC& aParamName, TDes& aParamValue, const TDesC& aFileName);
		
		void StartTimer();
		
	private:
		// Enginewrapper that is used for communicating between QT and Symbian
		HtiEngineWrapper* iEngineWrapper;             
		
		// Timer triggering periodic checking of HTIFramework process status
        CPeriodic*        iPeriodic;
        
        // For reading and writing HTI configuration files
        CHtiCfg*          iHtiCfg;
        
        CDesCArrayFlat* iPluginNameArray;
        
        CDesC8ArrayFlat* iPluginDataTypeArray;

        CDesCArrayFlat* iIAPsArray;
	
		
	};

#endif //CHTIADMINENGINE_H
