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
#ifndef __DIRECTPRINTPROPERTYVIEW_H__
#define __DIRECTPRINTPROPERTYVIEW_H__

// System includes
#include <aknlists.h>
#include <akntoolbarobserver.h>

// User includes
#include "DirectPrintViewBase.h"
#include "directprintsettinglistbox.h"

// Class declaration
/**
 *  Property view class
 *  more_complete_description
 */
class CDirectPrintPropertyView : public CDirectPrintViewBase, public MAknToolbarObserver
	{
public:
	/** Constructors */
	static CDirectPrintPropertyView* NewL();
	static CDirectPrintPropertyView* NewLC();
	/** Destructor */
	~CDirectPrintPropertyView();

public:
	/**
	 * From CAknView.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void HandleCommandL(TInt aCommand);

public:
	/**
	 * From MAknToolbarObserver.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void OfferToolbarEventL(TInt aCommand);

public: // New function
	/**
	 * Process by change tab.
	 *
	 * @param aTabIndex The index of tab.
	 */
	void ChangeTabL(TInt aTabIndex);

protected: // from CAknView
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();

private:
	CDirectPrintPropertyView();
	void ConstructL();

	void LoadListL();
	void ReadDataL();
	void WriteDataL();
	void NewDataL();

	void LoadPaperTabL();
	void LoadPageTabL();
	void LoadMarginTabL();
	void LoadHeaderTabL();
	void LoadFooterTabL();

private:
	/** Pointer to listbox. */
	CDirectPrintSettingListBox* iListBox;
	/** Index of paper size setting. */
	TInt iPaperSize;
	/** Index of orientation setting. */
	TInt iOrientation;
	/** Paper hight. */
	TInt iHight;
	/** Paper width. */
	TInt iWidth;
	/** Index of media type setting. */
	TInt iMediaType;
	/** Index of print page setting. */
	TInt iPrintPage;
	/** Print range. */
	TBuf<50> iPrintRange;
	/** Number of copies. */
	TInt iCopy;
	/** Left margin. */
	TInt iLeftMargin;
	/** Right margin. */
	TInt iRightMargin;
	/** Top margin. */
	TInt iTopMargin;
	/** Bottom margin. */
	TInt iBottomMargin;
	/** Header text. */
	TBuf<50> iHeaderText;
	/** Distance of header. */
	TInt iHeaderDistance;
	/** First page flag. */
	TInt iHeaderFirstPage;
	/** Footer text. */
	TBuf<50> iFooterText;
	/** Distance of footer. */
	TInt iFooterDistance;
	/** First page flag. */
	TInt iFooterFirstPage;

	/** Boaderless flag. */
	TInt iBoaderless;

	/** Index of tab. */
	TInt iTabIndex;
	};

#endif // __DIRECTPRINTPROPERTYVIEW_H__
