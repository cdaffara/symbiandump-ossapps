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


#include "cimageprint.h"
#include "cidleguard.h"
#include "cdiscoveryguard.h"
#include "cjobguard.h"
#include "clog.h"

const static TInt KImagePrintVersionMajor = 0;
const static TInt KImagePrintVersionMinor = 0;
const static TInt KImagePrintVersionBuild = 231;

/**********************
Public member functions
**********************/

CImagePrint::CImagePrint ()
	{	
	}

/// Destructor.
CImagePrint::~CImagePrint ()
	{
	LOG("CImagePrint::~CImagePrint BEGIN");
	delete iDiscoveryGuard;				
	delete iIdleGuard;
	delete iJobGuard;
	CCoeEnv::Static()->RemoveForegroundObserver( *this );	
    iClient.Close();
    LOG("CImagePrint::~CImagePrint END");
	}

/**
 * @brief Initialize the server.
 */
void CImagePrint::ConnectL()
	{	
	if( iClient.Handle() == KNullHandle )
		{
		User::LeaveIfError( iClient.ConnectL() );
		iIdleGuard = CIdleGuard::NewL( iClient );
		iDiscoveryGuard  = CDiscoveryGuard::NewL( iClient );
		iJobGuard = CJobGuard::NewL( iClient );
		User::LeaveIfError( iClient.ReserveEngine() );
		iClient.SetForeground( ETrue );	//For aiw cases
		CCoeEnv::Static()->AddForegroundObserverL( *this );
		}
	}

/**
 * @brief Shutdown the server.
 */
void CImagePrint::Close()
	{
	iClient.ReleaseEngine();
	}

/**
 * @brief Returns the current version of the Image Print Server
 * @return The current version of the Image Print Server
 */
TVersion CImagePrint::Version()
	{
	return TVersion(KImagePrintVersionMajor, KImagePrintVersionMinor, KImagePrintVersionBuild);  // Julian Day,  August 18.
	}

/**
 *	@brief Starts the discovery process.
 *
 *  Starts printer discovery on specified printer plug-ins. This is an asynchronous call that will use the 
 *  supplied MPrinterDiscoveryObserver to signal the client when Printers are found and upon error or successful completion.
 *
 *	@param aObserver The discovery observer.
 *	@param aProtocols Used to indicate what protocols to search for. 0 = all, 1 = BPP, 2 = BIP, 4 = OPP_Printer, 8 = DPOF, 16 = PB, 32 = OPP_PC
 *	@return Any standard Symbian error code. 
 *      @sa KImagePrint_PrinterProtocol_BPP @sa KImagePrint_PrinterProtocol_BIP @sa KImagePrint_PrinterProtocol_OPP_Printer 
 *      @sa KImagePrint_PrinterProtocol_DPOF @sa KImagePrint_PrinterProtocol_PictBridge @sa KImagePrint_PrinterProtocol_OPP_PC
 */
TInt CImagePrint::StartDiscoveryL(MPrinterDiscoveryObserver& aObserver, TUint aProtocols)
	{	
	return iDiscoveryGuard->Guard( aObserver, aProtocols );	
	}

/** 
 * Cancels printer discovery on all printer plug-ins. This is an asynchronous call 
 *   that will use the MPrinterDiscoveryObserver previously supplied by 
 *   CImagePrint::StartDiscoveryL to signal the client upon error or successful completion.
 * @brief Cancels the discovery process.
 * @return \c KErrNone if cancel was successfully submitted, otherwise another of the system-wide error codes.
 */
TInt CImagePrint::CancelDiscovery()
	{
	return iClient.CancelDiscovery();
	}

/**
 *	Creates a print job using the default settings and template. All job information beyond the images and Printer are set to the defaults of the printer. 
 *	If the Printer Discovery process is ongoing when \c CreatePrintJobL is received, Printer Discovery is terminated, without sending a notification event to the Client.
 *	@brief Creates a print job in the specified printer.
 *	@param[in] aPrinterID The exposed ID of the printer where the client wishes to create the print job.
 *	@param aImages Array containing the full names of the files to print.
 *	@param aObserver The print event observer.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 *  @note Only one print job can be active at a time.  The Print Job must be created before any 
 *      print job related functions are called.  These functions include changing settings, preview, job submission, canceling, or getting status.
 */
TInt CImagePrint::CreatePrintJobL(TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver)
	{	
	return iJobGuard->Guard(aPrinterID, aImages, aObserver);
	}

/**
 *	@brief Submits a print job already created.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 *  @note The actual job execution is asynchronous so this function returns immediately. If successful, notification of job status and job completion is received by the client through the MPrintEventObserver.
 */
TInt CImagePrint::SubmitPrintJobL()
	{	
	return iClient.SubmitPrintJob();	
	}

/**
 *	@brief Cancels the print job.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 *  @note This is an asynchronous command.  MPrintEventObserver::PrintJobProgressEvent will be called with the status of TPrintJobProgressEventStatus::EDone when the cancellation is completed.
 *  @note The notification can potentially be received by the client before this command returns to the client.
 */
TInt CImagePrint::CancelPrintJob()
	{
	LOG("CImagePrint::CancelPrintJob");
	return iClient.CancelPrintJob();
	}

/**
 *	@brief Continues printing and / or submitting a print job. Implementation may vary from protocol to protocol.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::ContinuePrintJobL()
	{
	return iClient.ContinuePrintJob();		
	}

/**
 *	@brief Gets the number of pages to be printed.
 *	@return Number of pages to print. 
 */
TInt CImagePrint::GetNumPrintPages()
	{
	return iClient.GetNumPrintPages();
	}

/**
 *	@brief Gets the status of the printing job created.
 *	@return Printing job status.
 */
TInt CImagePrint::GetJobStatus()
	{
	return iClient.GetJobStatus();
	}

/**
 *	@brief Gets the status of the printer.
 *	@param[in] aPrinterID The exposed ID of the printer the client wants to know about.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::GetPrinterStatus( TInt aPrinterID )
	{
	return iClient.GetPrinterStatus( aPrinterID );
	}

/**
 *	@brief Gets the IDs of the capabilities supported by the printer specified.
 *	@param[in] aPrinterID The exposed ID of the printer the client wants to know about.
 *	@param[out] aCapabilityIDs Array containing the ID's of the capabilities of the specified printer.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::GetPrinterCapabilityIDsL(TInt aPrinterID, RArray<TInt>& aCapabilityIDs)
	{	
	return iClient.GetPrinterCapabilityIDs(aPrinterID, aCapabilityIDs);
	}

/**
 *	@brief Gets the detail of a certain capability of a certain printer.
 *	@param[in] aPrinterID The exposed ID of the printer the client wants to know about.
 *	@param[in] aCapabilityID ID of the capability of interest.
 *	@param[out] aCapability Capability information.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::GetPrinterCapabilityL(TInt aPrinterID, TInt aCapabilityID, TPrintCapability& aCapability)
	{
	return iClient.GetPrinterCapability(aPrinterID, aCapabilityID, aCapability);
	}

/**
 *	@brief Gets a setting configured for the created print job.
 *	@param[in] aCapabilityID ID of the capability of interest.
 *	@param[out] aValue Capability value.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{	
	return iClient.GetJobSetting(aCapabilityID, aValue);
	}

/**
 *	@brief Sets a print job setting.
 *	@param[in] aCapabilityID ID of the capability of interest.
 *	@param[in] aValue New capability value.
 *	@param[out] aAffectedCapability ID of any affected capability.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::SetJobSettingL(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	return iClient.SetJobSetting(aCapabilityID, aValue, aAffectedCapability);
	}

/**
 *	@brief Gets the number of pages in the preview.
 *	@return Number of pages in the preview. 
 */
TInt CImagePrint::GetNumPreviewPages()
	{	
	return iClient.GetNumPreviewPages();	
	}

/**
 *	@brief Gets the icon representing a given layout or template.
 *	@param[in] aTemplateID ID of the template / layout of interest.
 *	@param[out] aFbsBitmapHandle Handle to the appropriate bitmap.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::GetJobTemplateIconL(TInt /*aTemplateID*/, TInt& /*aFbsBitmapHandle*/)
	{
	return KErrNotSupported;	
	}

/**
 *	@brief Gets number of copies for each image.
 *	@param[out] aArray contains number of copies for each image.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::GetNumberOfCopies( RArray<TInt>& /*aArray*/ )
    {
    return KErrNotSupported;
    }

/**
 *	@brief Sets number of copies for each image.
 *	@param[in] aArray contains number of copies for each image.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::SetNumberOfCopies( const RArray<TInt>& aArray )
    {
    return iClient.SetNumberOfCopies( aArray );
    }

/**
 *	@brief Creates a preview image.
 *	@param[in] aPageNumber Page number to create preview image of.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.
 */
TInt CImagePrint::CreatePreviewImage(TInt /*aPageNumber*/)
	{
	return KErrNotSupported;		
	}

/**
 *	@brief Removes a specified cached printer.
 *	@param[in] aPrinterID The exposed ID of the printer the client wants to remove.
 *	@return Any standard Symbian error code or KErrInvalidSequence / KErrInvalidData.k
 */
TInt CImagePrint::RemoveCachedPrinterL(TInt aPrinterID)
	{	
	return iClient.RemoveCachedPrinter( aPrinterID );	
	}

/**
 *	@brief Returns the Protocols supported by the DLL. It can be any of the KImagePrint_PrinterProtocol* constants.
 *	@return An OR'ed value with the supported protocols information.
 *	@sa KImagePrint_PrinterProtocol_BPP @sa KImagePrint_PrinterProtocol_BIP @sa KImagePrint_PrinterProtocol_OPP 
 *	@sa KImagePrint_PrinterProtocol_DPOF @sa KImagePrint_PrinterProtocol_PictBridge
 */
TUint CImagePrint::SupportedProtocols()
	{
	return iClient.SupportedProtocols();
	}

/**
 *  @brief Registers the IdleObserver to send idle event notifications
 *  @param A pointer to an object implementing MIdleObserver interface that will be notified of events
 *  @note In order to stop sending notifications, send NULL as the parameter.
 */
void CImagePrint::RegisterIdleObserver(MIdleObserver *aObserver)
	{
	iIdleGuard->Guard( aObserver );
	}

void CImagePrint::HandleGainingForeground()
	{
	iClient.SetForeground( ETrue );
	}
	
void CImagePrint::HandleLosingForeground()
	{
	iClient.SetForeground( EFalse );
	}

//  End of File
