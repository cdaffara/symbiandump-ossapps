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
#ifndef __DIRECTPRINTLISTBOX_H__
#define __DIRECTPRINTLISTBOX_H__

// System includes
#include <aknlists.h>

// User includes
#include "DirectPrintListObserver.h"

// Class declaration
/**
 *  Listbox class
 *  more_complete_description
 */
class CDirectPrintListBox : public CCoeControl, public MEikListBoxObserver
	{
public:
	// Data types
	/** The index of controls */
	enum TControls
		{
		EListBox = 0,
		ELastControl
		};

public:
	/** Constructors */
	static CDirectPrintListBox* NewL(const TRect& aRect, CDesCArray* aItemArray);
	static CDirectPrintListBox* NewLC(const TRect& aRect, CDesCArray* aItemArray);
	/** Destructor */
	virtual ~CDirectPrintListBox();

public:
	/**
	 * From CCoeControl.
	 * Get count of component controls.
	 *
	 * @return Count of component controls.
	 */
	TInt CountComponentControls() const;
	/**
	 * From CCoeControl.
	 * Get component controls.
	 *
	 * @param aIndex The index of controls
	 * @return Component control.
	 */
	CCoeControl* ComponentControl( TInt aIndex ) const;

public:
	/**
	 * From MEikListBoxObserver.
	 * Handles listbox events.
	 *
	 * @param aListBox Pointer to listbox control.
	 * @param aEventType Event type.
	 */
	void HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType);

public: // New Function
	/**
	 * Set listbox items.
	 *
	 * @param aItemArray Pointer to item array.
	 */
	void SetItemL(CDesCArray* aItemArray);
	/**
	 * Set list observer.
	 *
	 * @param aObserver Pointer to list observer.
	 */
	void SetListObserver(MDirectPrintListObserver* aObserver);
	/**
	 * Delete current item from listbox.
	 *
	 * @return Result value.
	 */
	TInt DeleteCurrentItemL();
	/**
	 * redraw listbox.
	 */
	void RedrawListL();
	/**
	 * Get the index of current item.
	 *
	 * @return The index of current listbox item.
	 */
	TInt CurrentIndex();
	/**
	 * Set focus.
	 *
	 * @param aIndex The index of listbox item
	 */
	void SetFocusL(TInt aIndex);

private:
	CDirectPrintListBox();
	void ConstructL(const TRect& aRect, CDesCArray* aItemArray);

	void InitializeControlsL();

private:
	/** Listbox control. */
	CAknDoubleStyleListBox* iListBox;		// Owend Data
	/** Pointer to list observer. */
	MDirectPrintListObserver* iListObserver;
	};

#endif // __DIRECTPRINTLISTBOX_H__
