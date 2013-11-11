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
#ifndef __SEARCHRESULTVIEW_H__
#define __SEARCHRESULTVIEW_H__

// System includes
#include <aknlists.h>
#include <akntoolbarobserver.h>
#include <AknProgressDialog.h>	// CAknProgressDialog
#include <EIKPROGI.H>

// User includes
#include "DirectPrintViewBase.h"
#include "DirectPrintListBox.h"

// Class declaration
/**
 *  Search result view class
 *  more_complete_description
 */
class CDirectPrintSearchResultView : public CDirectPrintViewBase
									, public MAknToolbarObserver
									, public MProgressDialogCallback
	{
public:
	/** Constructors */
	static CDirectPrintSearchResultView* NewL();
	static CDirectPrintSearchResultView* NewLC();
	/** Destructor */
	~CDirectPrintSearchResultView();

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

public:
	/**
	 * From MAknToolbarObserver.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void OfferToolbarEventL(TInt aCommand);

public: // from MProgressDialogCallback
    /**
    * DialogDismissedL
    * Called when/if the dialog has been dismissed.
    * @param aButtonId
    */
    void DialogDismissedL( TInt aButtonId );

protected: // from CAknView
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();

private:
	CDirectPrintSearchResultView();
	void ConstructL();

	void StartSearchProgressNoteL();
	void EndProgressNoteL();

	void StartSearchL();

	static TInt HandleTimeout(TAny* aAny);
	void HandleTimeoutL();

private:
	/** Pointer to listbox. */
	CDirectPrintListBox* iListBox;		// Owend Data
	/** Pointer to array. */
	CDesCArray* iItemArray;

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
	/** Pointer to progress message. */
	HBufC* iProgressMessage;
	/** Pointer to periodic timer object. */
	CPeriodic* iPeriodic;

	/** Count of timeout. */
	TInt iTimeoutCount;
	};

#endif // __SEARCHRESULTVIEW_H__
