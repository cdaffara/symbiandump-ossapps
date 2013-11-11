/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


// Protection against nested includes
#ifndef CDIRECTPRINTSESSION_H
#define CDIRECTPRINTSESSION_H

// System includes
#include <e32base.h>

// User includes
#include "directprintcapability.h"
#include "directprintmessage.h"

// Forward declarations
class CDirectPrintServer;
class CDirectPrintServerJobGuard;
class CDirectPrintNumOfCopies;


class CDirectPrintSession : public CSession2
	{
	public: // from CSession2
		void CreateL();

	private:
		~CDirectPrintSession();
		CDirectPrintServer& Server();
		void ServiceL( const RMessage2& aMessage );
		void Disconnect(const RMessage2 &aMessage);

	private:
		void ServiceFirstPartL( TInt aIndex );
		void ServiceSecondPartL( TInt aIndex );
		void ServiceThirdPartL ( TInt aIndex );
		void CountConnectionsL( TDpMessage& aMessage );
		void SetForegroundL( TDpMessage& aMessage );
		void ReserveEngine( TDpMessage& aMessage );
		void ReleaseEngine( TDpMessage& aMessage );
		void SupportedProtocols( TDpMessage& aMessage );
		void GetNumPrintPages( TDpMessage& aMessage );
		void GetJobStatus( TDpMessage& aMessage );
		void GetPrinterStatus( TDpMessage& aMessage );
		void CancelDiscovery( TDpMessage& aMessage );
		void SubmitPrintJobL( TDpMessage& aMessage );
		void CancelPrintJob( TDpMessage& aMessage );
		void ContinuePrintJobL( TDpMessage& aMessage );
		void RemoveCachedPrinterL( TDpMessage& aMessage );
		void GetJobTemplateIconL( TDpMessage& aMessage );
		void GetNumPreviewPages( TDpMessage& aMessage );
		void SetJobSettingL( TDpMessage& aMessage );
		void GetJobSettingL( TDpMessage& aMessage );
		void GetPrinterCapabilityL( TDpMessage& aMessage );
		void GetPrinterCapIdL( TDpMessage& aMessage );
		void GetPrinterCapTypeL( TDpMessage& aMessage );
		void GetPrinterCapDefL( TDpMessage& aMessage );
		void GetPrinterCapLowL( TDpMessage& aMessage );
		void GetPrinterCapHighL( TDpMessage& aMessage );
		void GetPrinterCapEnumCountL( TDpMessage& aMessage );
		void GetPrinterCapEnumL( TDpMessage& aMessage );
		void GetPrinterCapabilityIDsCountL( TDpMessage& aMessage );
		void GetPrinterCapabilityIDL( TDpMessage& aMessage );
		void RegisterIdleObserverL( TDpMessage& aMessage );
		void CancelRegisterIdleObserverL( TDpMessage& aMessage );
		void StartDiscoveryL( TDpMessage& aMessage );
		void ContinueDiscoveryL( TDpMessage& aMessage );
		void PrepareJobL( TDpMessage& aMessage );	
		void CreateJobL( TDpMessage& aMessage );
		void ContinueCreateJobL( TDpMessage& aMessage );
		void CancelStartDiscoveryL( TDpMessage& aMessage );
		void CancelCreateJobL( TDpMessage& aMessage );
		void IsPictBridgeModeL( TDpMessage& aMessage );
		void SetNumberOfCopiesCountL( TDpMessage& aMessage );
		void SetNumberOfCopiesL( TDpMessage& aMessage );
		void GetProtocolNamesCountL( TDpMessage& aMessage );
		void GetProtocolNameL( TDpMessage& aMessage );

	private: // data
	
		RPointerArray<TDpMessage> iMsgWrappers;
		RArray<TInt> iCapabilityIDs;
		CDirectPrintServerJobGuard* iJobGuard;
		TDirectPrintCapability iCapability;
		CDirectPrintNumOfCopies* iNumOfCopies;
		TBool iConnected;
	};


#endif // CDIRECTPRINTSESSION_H

//  End of File
