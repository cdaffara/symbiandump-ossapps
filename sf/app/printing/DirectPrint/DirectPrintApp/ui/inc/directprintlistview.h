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
#ifndef __PRINTERLISTVIEW_H__
#define __PRINTERLISTVIEW_H__

// System includes
#include <aknlists.h>
#include <akntoolbarobserver.h>

// User includes
#include "DirectPrintViewBase.h"
#include "DirectPrintListBox.h"

// Class declaration
/**
 *  List view class
 *  more_complete_description
 */
class CDirectPrintListView : public CDirectPrintViewBase, public MAknToolbarObserver
	{
public:
	/** Constructors */
	static CDirectPrintListView* NewL();
	static CDirectPrintListView* NewLC();
	/** Destructor */
	~CDirectPrintListView();

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
	 * From MDirectPrintListObserver.
	 * Handles index.
	 *
	 * @param aIndex The index of list items.
	 */
	void HandleListIndexL(TInt aIndex);

public: // from MAknToolbarObserver
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
	CDirectPrintListView();
	void ConstructL();

	void LoadListDataL();
	void PrepareNewDataL();

private:
	/** Pointer to listbox. */
	CDirectPrintListBox* iListBox;		// Owend Data
	/** Pointer to item array. */
	CDesCArray* iItemArray;
	};

#endif // __PRINTERLISTVIEW_H__
