/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CPBPrintingDevice class definition.
*
*/


#ifndef CPBPRINTINGDEVICE_H
#define CPBPRINTINGDEVICE_H

#include <fbs.h>
#include <e32hashtab.h>

#include "mprotprintingdevice.h"
#include "mpbobserver.h"
#include "printmessagecodes.h"
#include "printcapabilitycodes.h"

class CDpsEngine;
class CPBConnect;
class RWriteStream;

enum TPBProtocolState
	{
	EPBUninitialized = 0,
	EPBDiscovery,
	EPBReady,
	EPBJobReady,
	EPBSubmittingJob,
	EPBPrinting,
	EPBCancelling,
	};
	
enum TPBJobSettingIndex
	{
	EPBSettingPaperSize = 0,
	EPBSettingQuality   = 1,
	EPBSettingLayout    = 2
	};

/**
 *	@brief CPBPrintingDevice implements MProtPrintingDevice. This makes the class the "published" class of the DLL it is in.
 */
class CPBPrintingDevice : public CActive, public MProtPrintingDevice, public MPBObserver
	{
public:
	static MProtPrintingDevice* NewL();
		
	// Default constructor and destructor.
	CPBPrintingDevice();
	~CPBPrintingDevice();

public: // From MProtPrintingDevice
			
	// General.
	void ConstructL(const TDesC& aDLLPath);
	TVersion Version();
	TUint SupportedProtocols();
	// Discovery.
	void StartDiscoveryL( MProtDiscoveryObserver& aObserver, TUint aProtocols = 0 );
	TInt RemoveCachedDeviceL( TInt aDeviceID );
	void CancelDiscovery( TBool aDoCallBack = ETrue );
	// Print.
	TInt CreatePrintJobL( TInt aDeviceID, RPointerArray<TDesC>& aImages, MProtPrintEventObserver& aObserver );
	void SubmitPrintJobL();
	TInt CancelPrintJob();
	TInt ContinuePrintJobL();
	TInt GetNumPrintPages();
	TInt GetPrintJobStatus();
	TInt GetPrinterStatus( TInt aDeviceID );
	// Capabilities.
	TInt GetDeviceCapabilityIDsL( TInt aDeviceID, RArray<TInt>& aCapabilityIDs );
	TInt GetDeviceCapabilityL( TInt aDeviceID, TInt aCapabilityID, TPrintCapability& aCapability );
	TInt GetJobSetting( TInt aCapabilityID, TInt& aValue );
	TInt SetJobSettingL( TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability );
	// Preview.
	TInt GetNumPreviewPages();
	TInt GetJobTemplateIconL( TInt aTemplateID, TInt& aFsBitmapHandle );
	TInt CreatePreviewImage( TInt aPageNumber );
	
	void RegisterIdleObserver( MProtIdleObserver *aObserver );
			
	void SetNumsOfCopiesL( const RArray<TInt>& aNumsOfCopies, TInt& aErr );	

public: // from MPBObserver
	void ConnectionNotifyL( TInt aStatus );
	void JobNotify( const TDpsGetJobStatus& aStatus );
	void PrinterNotify( const TDpsGetPrinterStatus& aStatus );
		
protected: // from CActive			
	void DoCancel();	
	void RunL();
	TInt RunError( TInt aError );

private:
	void HandlePapersizesResponseL( TPrintCapability& aCapability );
	void HandleQualitiesResponseL( TPrintCapability& aCapability );
	void HandleLayoutsResponseL( TPrintCapability& aCapability );
	TInt GetPapersizeJobSetting( TInt& aValue );
	TInt GetQualityJobSetting( TInt& aValue );
	TInt GetLayoutJobSetting( TInt& aValue );
	TPrintMessageCodes DpsJobError( const TDpsJobEndReason& aReason );
	void HandlePaperError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr );
	void HandleInkError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr );
	void HandleHwError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr );
	void HandleFileError ( const TDpsJobEndReason& aReason, TPrintMessageCodes& aErr );
	TBool HasCapability( TPrintCapability& aCapability, TPrintCapabilityOptions aOption );
    void SaveConfigInfoL();
    void ReadConfigInfoL();
    void ExternalizeL( RWriteStream& aStream ) const;

private: // data
	CDpsEngine* iDpsEngine;
	CPBConnect* iConnect;
	CActiveSchedulerWait iWait;
	TBool iPrinterConnected;
	TPBProtocolState iState;
	MProtIdleObserver* iIdleObserver;
	RHashMap<TInt, TInt> iCapToDps;
    TDpsGetCapability iCap;
    TDpsStartJob iStart;
    TDpsAbortJob iAbort;
    TDpsContinueJob iContinue;
    TDpsGetPrinterStatus iPrinterS;
    TDpsGetJobStatus iJobS;
    TDpsConfigPrintService iConfigS;
   	HBufC* iCongFileName;
  	TPrinter iPrinter;
	TUint iCancelRetry;
	TInt  iPrintingStateComplete;
	TUint iUserSelectedPaperSize;
	TBool iDisconnectPrinter;
	TBool iNewJobOK;
	};

#endif // CPBPRINTINGDEVICE_H

//  End of File
