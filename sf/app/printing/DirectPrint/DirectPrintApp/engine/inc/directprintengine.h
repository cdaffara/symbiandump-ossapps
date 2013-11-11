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
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef __DIRECTPRINTENGINE_H__
#define __DIRECTPRINTENGINE_H__

// INCLUDES
#include <e32base.h>
#include <prninf.h>			// MPrintProcessObserver
#include <prnsetup.h>		// CPrintSetup
#include <fldbltin.h>		// MFieldNumPagesInfo
#include <gdi.h>			// CPrinterModelList
#include <AknProgressDialog.h>	// CAknProgressDialog
#include <EIKPROGI.H>
#include <xmlengdocument.h>				// RXmlEngDocument
#include "DirectPrintEngineObserver.h"
#include "directprintband.h"

// CLASS DECLARATION
class CDirectPrintModel;

class CDirectPrintEngine : public CBase,
						   public MPrintProcessObserver,
						   public MFieldNumPagesInfo,
						   public MProgressDialogCallback
	{
public:
	// Constructors and destructor

	/**
	 * Two-phased constructor.
	 */
	static CDirectPrintEngine* NewL(CDirectPrintModel& aModel);

	/**
	 * Two-phased constructor.
	 */
	static CDirectPrintEngine* NewLC(CDirectPrintModel& aModel);

	/**
	 * Destructor.
	 */
	virtual ~CDirectPrintEngine();

	// from MPrintProcessObserver
	virtual void NotifyPrintStarted(TPrintParameters aPrintParams);
	virtual void NotifyBandPrinted(TInt aPercent, TInt aCurrentPageNum, TInt aCurrentCopyNum);
	virtual void NotifyPrintEnded(TInt aErrNum);

	// from MFieldNumPagesInfo
	virtual TInt UpdateFieldNumPages()const;

    // from MProgressDialogCallback
    /**
    * DialogDismissedL
    * Called when/if the dialog has been dismissed.
    * @param aButtonId
    */
    void DialogDismissedL( TInt aButtonId );

	// New functions
	void SetObserver(MDirectPrintEngineObserver* aObserver);

	TInt ModelCount();
	void ModelName(TInt aIndex, TDes& aDes);
	TUid ModelUid(TInt aIndex);

	void SetUseModel(TInt aIndex);

	void PrintL();

	void StartPrintPreviewL();
	void EndPrintPreview();

	inline CPrintSetup* PrintSetup();
	inline MPageRegionPrinter* PageRegionPrinter();
	inline TInt PageCounter();
	inline TInt MaxPageNum();

	void SetParamL(RXmlEngDocument* aParam);

protected:
	/**
	 * Constructor for performing 1st stage construction
	 */
	CDirectPrintEngine(CDirectPrintModel& aModel);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	// New functions
	void DoPrintL(CPrinterPort* aPort);

	void StartPrintProgressNoteL();
	void EndPrintProgressNoteL();
	void UpdatePrintProgressNoteL(TInt aPage);
	void SearchPluginFromFilenameL(const TDesC& aFileName);
	void LoadPluginFromUidL(const TUid aUid);

	static void CleanupBand( TAny* aData );

private:
	// Observer
	MDirectPrintEngineObserver* iObserver;

	// Model object
	CDirectPrintModel& iModel;

	// File server session
	RFs iFs;
	// Member Variable for CPrintSetup
	CPrintSetup* iPrintSetup;

	// Member Variable for print model name
	CPrinterModelList* iModelList;

	// Member Variable for page counter
	TInt iPageCounter;

	// Member Variable for total page
	TInt iMaxPageNum;

	// Member Variable for total page
	TInt iModelIndex;

	CDirectPrintBand*   iPrintBand;
	//MPageRegionPrinter* iRegionPrinter;

    /**
    * iProgressDialog
    * Owned
    */
	CAknProgressDialog* iProgressDialog;
	/**
	* iProgressInfo
    * Not owned by CAknExNoteContainer object.
	*/
	CEikProgressInfo*   iProgressInfo;

	HBufC* iProgressMessage;

	//TBuf<255> iParam;
	RXmlEngDocument*	iParam;

	// identifer returned by ECom 
	TUid				iDtorIdKey;

	// last error
	TInt				iLastError;
	};

inline CPrintSetup* CDirectPrintEngine::PrintSetup()
	{
	return iPrintSetup;
	}

inline MPageRegionPrinter* CDirectPrintEngine::PageRegionPrinter()
	{
	if (iPrintBand)
		{
		return iPrintBand->RegionPrinter();
		}
	return NULL;
	}

inline TInt CDirectPrintEngine::PageCounter()
	{
	return iPageCounter;
	}

inline TInt CDirectPrintEngine::MaxPageNum()
	{
	return iMaxPageNum;
	}


#endif // __DIRECTPRINTENGINE_H__
// End of file
