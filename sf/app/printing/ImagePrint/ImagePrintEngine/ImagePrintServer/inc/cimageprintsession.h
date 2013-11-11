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


#ifndef CIMAGEPRINTSESSION_H
#define CIMAGEPRINTSESSION_H

#include <e32base.h>

#include "tprintcapability.h"
#include "tmessagewrp2.h"

class CImagePrintServer;
class CServerIdleGuard;
class CServerDiscoveryGuard;
class CServerJobGuard;
class CNumOfCopies;


class CImagePrintSession : public CSession2
	{
	public: // from CSession2
		void CreateL();

	private:
		~CImagePrintSession();
		CImagePrintServer& Server();
		void ServiceL( const RMessage2& aMessage );
		void Disconnect(const RMessage2 &aMessage);

	private:
		void ServiceFirstPartL( TInt aIndex );
		void ServiceSecondPartL( TInt aIndex );
		void ServiceThirdPartL ( TInt aIndex );
		void CountConnectionsL( TMessageWrp2& aMessage );
	 	void SetForegroundL( TMessageWrp2& aMessage );
	 	void ReserveEngine( TMessageWrp2& aMessage );
	 	void ReleaseEngine( TMessageWrp2& aMessage );
	 	void SupportedProtocols( TMessageWrp2& aMessage );
	 	void GetNumPrintPages( TMessageWrp2& aMessage );
	 	void GetJobStatus( TMessageWrp2& aMessage );
	 	void GetPrinterStatus( TMessageWrp2& aMessage );
	 	void CancelDiscovery( TMessageWrp2& aMessage );
	 	void SubmitPrintJobL( TMessageWrp2& aMessage );
	 	void CancelPrintJob( TMessageWrp2& aMessage );
	 	void ContinuePrintJobL( TMessageWrp2& aMessage );
	 	void RemoveCachedPrinterL( TMessageWrp2& aMessage );
	 	void GetJobTemplateIconL( TMessageWrp2& aMessage );
	 	void GetNumPreviewPages( TMessageWrp2& aMessage );
	 	void SetJobSettingL( TMessageWrp2& aMessage );
	 	void GetJobSettingL( TMessageWrp2& aMessage );
	 	void GetPrinterCapabilityL( TMessageWrp2& aMessage );	 	
	 	void GetPrinterCapIdL( TMessageWrp2& aMessage );	 	
	 	void GetPrinterCapTypeL( TMessageWrp2& aMessage );
	 	void GetPrinterCapDefL( TMessageWrp2& aMessage );
	 	void GetPrinterCapLowL( TMessageWrp2& aMessage );
	 	void GetPrinterCapHighL( TMessageWrp2& aMessage );
	 	void GetPrinterCapEnumCountL( TMessageWrp2& aMessage );
	 	void GetPrinterCapEnumL( TMessageWrp2& aMessage );	 		 	
	 	void GetPrinterCapabilityIDsCountL( TMessageWrp2& aMessage );
	 	void GetPrinterCapabilityIDL( TMessageWrp2& aMessage );
	 	void RegisterIdleObserverL( TMessageWrp2& aMessage );	 	
	 	void CancelRegisterIdleObserverL( TMessageWrp2& aMessage );
	 	void StartDiscoveryL( TMessageWrp2& aMessage );
	 	void ContinueDiscoveryL( TMessageWrp2& aMessage );
	 	void PrepareJobL( TMessageWrp2& aMessage );	
		void CreateJobL( TMessageWrp2& aMessage );
		void ContinueCreateJobL( TMessageWrp2& aMessage );
		void CancelStartDiscoveryL( TMessageWrp2& aMessage );
		void CancelCreateJobL( TMessageWrp2& aMessage );						
		void IsPictBridgeModeL( TMessageWrp2& aMessage );		
		void SetNumberOfCopiesCountL( TMessageWrp2& aMessage );
		void SetNumberOfCopiesL( TMessageWrp2& aMessage );
		 
	private: // data
	
		RPointerArray<TMessageWrp2> iMsgWrappers;
		RArray<TInt> iCapabilityIDs;
		CServerIdleGuard* iIdleGuard;
		CServerDiscoveryGuard* iDiscoveryGuard;
		CServerJobGuard* iJobGuard;
		TPrintCapability iCapability;
		CNumOfCopies* iNumOfCopies;
		TBool iConnected;
	};


#endif // CIMAGEPRINTSESSION_H

//  End of File
