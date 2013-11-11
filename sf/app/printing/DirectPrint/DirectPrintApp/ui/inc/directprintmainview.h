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
#ifndef __PRINTMAINVIEW_H__
#define __PRINTMAINVIEW_H__

// System includes
#include <aknlists.h>
#include <akntoolbarobserver.h>

// User includes
#include "DirectPrintViewBase.h"
#include "DirectPrintListBox.h"

// Class declaration
/**
 *  Main view class
 *  more_complete_description
 */
class CDirectPrintMainView : public CDirectPrintViewBase, public MAknToolbarObserver
	{
private:
	// Data types
	/** The index of list items */
	enum
		{
		EItemPrinter,
		EItemBearer,
		EItemSetting
		};

public:
	/** Constructors */
	static CDirectPrintMainView* NewL();
	static CDirectPrintMainView* NewLC();
	/** Destructor */
	~CDirectPrintMainView();

public: // from CAknView
	/**
	 * From CAknView.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void HandleCommandL(TInt aCommand);

public:
	/**
	 * From MDirectPrintListObserver.
	 * Handles index.
	 *
	 * @param aIndex The index of list items.
	 */
	void HandleListIndexL(TInt aIndex);

public:
	/**
	 * From MAknToolbarObserver.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void OfferToolbarEventL(TInt aCommand);

protected: // from CAknView
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();

private:
	CDirectPrintMainView();
	void ConstructL();

	void LoadListDataL();
	void MakePropertyStringL(TDes& aProperty);

private:
	/** Pointer to listbox. */
	CDirectPrintListBox* iListBox;		// Owend Data
	/** Pointer to item array. */
	CDesCArray* iItemArray;
	};

#endif // __PRINTMAINVIEW_H__
