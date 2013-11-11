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


#ifndef CIMAGEPRINT_H
#define CIMAGEPRINT_H

#include <e32base.h>
#include <coemain.h>

#include "rimageprintclient.h"
#include "imageprint.h"

class TPrintCapability;
class TPrinter;
class TEvent;
class CIdleGuard;
class CDiscoveryGuard;
class CJobGuard;

/**
 *	@brief Class containing the Image Print Library Public API.
 */
class CImagePrint : public CBase, public MCoeForegroundObserver
	{
		// Functions
		public:
			CImagePrint();
			~CImagePrint();
			void ConnectL();
			void Close();
			TVersion Version();
			TInt StartDiscoveryL(MPrinterDiscoveryObserver& aObserver, TUint aProtocols = 0);
			TInt CancelDiscovery();
			TInt CreatePrintJobL(TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver);
			TInt SubmitPrintJobL();
			TInt CancelPrintJob();
			TInt ContinuePrintJobL();
			TInt GetNumPrintPages();
			TInt GetJobStatus();
			TInt GetPrinterStatus(TInt aPrinterID);
			TInt GetPrinterCapabilityIDsL(TInt aPrinterID, RArray<TInt>& aCapabilityIDs);
			TInt GetPrinterCapabilityL(TInt aPrinterID, TInt aCapabilityID, TPrintCapability& aCapability);
			TInt GetJobSetting(TInt aCapabilityID, TInt& aValue);
			TInt SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability);
			TInt GetNumPreviewPages();
			TInt GetJobTemplateIconL(TInt aTemplateID, TInt& aFbsBitmapHandle);
			TInt GetNumberOfCopies( RArray<TInt>& aArray );
			TInt SetNumberOfCopies( const RArray<TInt>& aArray );
			TInt CreatePreviewImage(TInt aPageNumber);
			TInt RemoveCachedPrinterL(TInt aPrinterID);
			TUint SupportedProtocols();
			void RegisterIdleObserver(MIdleObserver *aObserver);

		public: // From MCoeForegroundObserver
			void HandleGainingForeground();
			void HandleLosingForeground();

		protected:
		private:

		// Data
		public:
		protected:
		private:
		
			RImagePrintClient iClient;
			CIdleGuard* iIdleGuard;
			CDiscoveryGuard* iDiscoveryGuard;
			CJobGuard* iJobGuard;
	};

#endif // CIMAGEPRINT_H

//  End of File
