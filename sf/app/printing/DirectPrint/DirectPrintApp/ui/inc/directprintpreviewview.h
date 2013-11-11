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
#ifndef __PREVIEWVIEW_H__
#define __PREVIEWVIEW_H__

// System includes
#include <aknview.h>
#include <akntoolbarobserver.h>
#include <prninf.h>
#include <PRNPREV.H>

// User includes
#include "DirectPrintViewBase.h"

// Forward declarations
class CDirectPrintPreviewControl;

// Class declaration
/**
 *  Preview view class
 *  more_complete_description
 */
class CDirectPrintPreviewView : public CDirectPrintViewBase
                              , public MAknToolbarObserver
                              , public MPrintProcessObserver
	{
public:
	/** Constructors */
	static CDirectPrintPreviewView* NewL();
	static CDirectPrintPreviewView* NewLC();
	/** Destructor */
	~CDirectPrintPreviewView();

public:
	/**
	 * From CAknView.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void HandleCommandL(TInt aCommand);
	/**
	 * From CAknView.
	 * Get the id of view.
	 *
	 * @return The id of view.
	 */
	TUid Id() const;

protected: // from CAknView
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();

public:
	/**
	 * From MAknToolbarObserver.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void OfferToolbarEventL(TInt aCommand);

private:  // from MPrintProcessObserver 
	void NotifyPrintStarted(TPrintParameters aPrintParams);
	void NotifyPrintEnded(TInt anErrorCode);
	void NotifyBandPrinted(TInt aPercentageOfPagePrinted, TInt aCurrentPageNum, TInt aCurrentCopyNum);

private:
	CDirectPrintPreviewView();
	void ConstructL();

	void ChangeVisibleState();

private:
	/** Pointer to preview control. */
	CDirectPrintPreviewControl* iPrevCtrl;

	/** Parameter. */
	TPrintParameters iParameters;
	/** Preview format. */
	TPrintPreviewFormat iPrintPreviewFormat;
	/** Number of pages in document. */
	TInt iNumPagesInDoc;
	/** Display flag. */
	TInt iFirstPageToDisplay;
	/** Number of pages. */
	TInt iNumPagesToView;
	/** Number of bands. */
	TInt iNumBands;
	/** Margin state. */
	CPrintPreviewImage::TMarginState iMarginState;
	};

#endif // __PREVIEWVIEW_H__
