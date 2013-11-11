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
* Description:  Contains the CDirectPrintBody class definition.
*
*/


// Protection against nested includes
#ifndef CDIRECTPRINTBODY_H
#define CDIRECTPRINTBODY_H

// System includes
#include <e32base.h>

// User includes
#include "imageprint.h"
#include "directprintprotocolsloader.h"
#include "directprintscheduler.h"
#include "mprotprintingdevice.h"

// Forward declarations
class TDpMessage;

/**
 *
 *	This class is in charge of discoverying devices across the protocols available, of creating
 *	a print job in the right printer and right protocol, etc. It can be considered a controller. 
 *	Implements the MProtPrintingDevice observers (MProtDiscoveryObserver, MProtPrintEventObserver) so it can act
 *	accordingly when the protocols report or need something.
 */
class CDirectPrintBody : public CBase, public MProtDiscoveryObserver, public MProtPrintEventObserver, public MProtIdleObserver
	{
	public:
	
		/**
         *	2-phase constructor
         *
         *	@return new object      
         */
		static CDirectPrintBody* NewL();
		
		/**
         *	Destructor         
         */
		~CDirectPrintBody();

		/**
         *	Start discovery for one or several protocols
         *
         *	@param aObserver observer for notifications about discovery events
         *	@param aProtocols OR:ed protocol ids
         *	@return error code     
         */
		TInt StartDiscovery(MPrinterDiscoveryObserver& aObserver, TUint aProtocols = 0);
		
		/**
         *	Cancel discovery        
         */
		void CancelDiscovery();
		
		/**
         *	Creates print job
         *
         *	@param aPrinterID printer id
         *	@param aImages array of image file names to be printed
         *	@param aObserver observer for notifications about print job events
         *	@return error code     
         */
		TInt CreatePrintJobL(TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver);
		
		/**
         *	Start actual printing of previously created print job
         */
		void SubmitPrintJobL();
		
		/**
         *	Cancel print job
         *
         *	@return error code      
         */
		TInt CancelPrintJob();
		
		/**
         *	Continue print job
         *
         *	@return error code      
         */
		TInt ContinuePrintJobL();
		
		/**
         *	Print pages amount getter
         *
         *	@return amount of print pages with current settings for current print job    
         */
		TInt GetNumPrintPages();
		
		/**
         *	Job status getter
         *
         *	@return job status code    
         */
		TInt GetJobStatus();
		
		/**
         *	Printer status getter
         *
         *	@param aPrinterID printer id
         *	@return printer status code    
         */
		TInt GetPrinterStatus(TInt aPrinterID);
		
		/**
         *	Printer capability id getter
         *
         *	@param aPrinterID printer id
         *	@param aCapabilityIDs place holder for capability ids supported by printer
         *	@return error code    
         */
		TInt GetPrinterCapabilityIDsL(TInt aPrinterID, RArray<TInt>& aCapabilityIDs);
		
		/**
         *	Printer capability getter
         *
         *	@param aPrinterID printer id
         *	@param aCapabilityID capability id
         *	@param aCapability place holder for capability
         *	@return error code    
         */
		TInt GetPrinterCapabilityL(TInt aPrinterID, TInt aCapabilityID, TDirectPrintCapability& aCapability);
		
		/**
         *	Printer setting getter
         *
         *	@param aCapabilityID capability id
         *	@param aValue place holder for capability value
         *	@return error code    
         */
		TInt GetJobSetting(TInt aCapabilityID, TInt& aValue);
		
		/**
         *	Printer setting setter
         *
         *	@param aCapabilityID capability id
         *	@param aValue capability value
         *	@param aAffectedCapability affected capability id
         *	@return error code    
         */
		TInt SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability);
		
		/**
         *	Preview pages amount getter for current protocol
         *
         *	@return preview pages amount      
         */
		TInt GetNumPreviewPages();
		
		/**
         *	Template icon getter
         *
         *	@param aTemplateID template id
         *	@param aFbsBitmapHandle place holder for template bitmap handle
         *	@return error code    
         */
		TInt GetJobTemplateIconL(TInt aTemplateID, TInt& aFbsBitmapHandle);
		
		/**
         *Creates a preview image
         *
         *	@param aPageNumber Page number to create preview image of
         *	@return error code    
         */
		TInt CreatePreviewImage(TInt aPageNumber);
		
		/**
         *	Remove cached printer
         *
         *	@param aPrinterID printer id
         *	@return error code    
         */
		TInt RemoveCachedPrinterL(TInt aPrinterID);
		
		/**
         *	Supported protocols getter
         *
         *	@return OR:ed protocol ids      
         */
		TUint SupportedProtocols();

		/**
         *	Register observer for idle events receiving
         *
         *	@param aObserver observer      
         */
		void RegisterIdleObserver(MIdleObserver* aObserver);

		/**
         *	Number of copies setter for current print job
         *
         *	@param aNumsOfCopies number of copies array
         *	@param aErr place holder for error code      
         */
		void SetNumsOfCopiesL(const RArray<TInt>& aNumsOfCopies, TInt& aErr);

		/**
         *	Handle session disconnection
         *
         *	@param aIdleObserver idle observer object of that session
         *	@param aDiscoveryObserver discovery observer object of that session     
         *	@param aEventObserver print job observer object of that session
         */
		void HandleSessionDisconnect( const MIdleObserver* aIdleObserver,
									const MPrinterDiscoveryObserver* aDiscoveryObserver,
									const MPrintEventObserver* aEventObserver );

		/**
         *	Number of protocols getter
         *
         *	@return number of protocols
         */
		TInt ProtocolCount();

		/**
         *	Protocol name getter
         *
         *	@param aMessage message completed upon event arrival
         *	@return error code
         */
		TInt GetProtocolNameL(TDpMessage& aMessage);


	private: // from MProtDiscoveryObserver
		void FoundDeviceL(TPrinter& aDeviceInfo);
		void DiscoveryStatusL(TInt aStatus, TInt aErrorCode, TInt aErrorStringCode);
		void RemoveDeviceL(TPrinter& aDeviceInfo);

	private: // from MProtPrintEventObserver
		void PrintJobProgressEvent(TInt aStatus, TInt aPercent, TInt aJobStateCode);
		void PrintJobErrorEvent(TInt aErrorCode, TInt aErrorStringCode);
		void PrinterStatusEvent(TInt aErrorCode, TInt aErrorStringCode);
		void PreviewImageEvent(TInt aFsBitmapHandle);
		void ShowMessageL(TInt aMsgLine1Code, TInt aMsgLine2Code);
		TBool AskYesNoQuestionL(TInt aMsgLine1Code, TInt aMsgLine2Code);
		const TDesC& AskForInputL(TInt aMsgLine1Code, TInt aMsgLine2Code);

	private: // from MProtIdleObserver
		void StatusEvent(const TEvent &aEvent, TInt aError, TInt aMsgCode);
	
	private:
		CDirectPrintBody();
		void ConstructL();
		TInt FindInternalDeviceReference(TInt aExternalDeviceID, TInt& aInternalDeviceID, MProtPrintingDevice*& aProtToUse);
		TInt ValidateImagesL(const RPointerArray<HBufC>& aImageList, TInt &aInvalidCount);
		CDirectPrintScheduler& ActiveScheduler() const;
		void TryNextDiscovery();
		static TInt TryNextDiscoveryL(TAny* aObj);
		void DoTryNextDiscoveryL();

	private:
		/// Class loader and manager.
		CDirectPrintProtocolsLoader* iClassLoader;
		/// Pointer to store the current protocol in use for discovery.
		MProtPrintingDevice* iCurrentProtocol;
		/// Pointer to store the current protocol in use for creating a print job.
		MProtPrintingDevice* iCurrentPrintJobProtocol;
		/// DirectPrint Observer.
		MPrinterDiscoveryObserver* iPrinterDiscoveryObserver;
		/// DirectPrint Observer.
		MPrintEventObserver* iPrinterPrintEventObserver;
		/// DirectPrint Idle Observer.
		MIdleObserver* iPrinterIdleObserver;							
		/// Used to store the requested protocols.
		TUint iProtocolsRequested;
		/// Array of the image filenames to print.
		RPointerArray<HBufC> iImageFileNames;
		/// Enables async discovery
		CIdle* iIdleDiscovery;
		
		TBool iDiscoveryCancelled;
	};

#endif // CDIRECTPRINTBODY_H

//  End of File
