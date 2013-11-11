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
#ifndef __DIRECTPRINTSETTINGLISTBOX_H__
#define __DIRECTPRINTSETTINGLISTBOX_H__

// System includes
#include <aknsettingitemlist.h>

// User includes
#include "directprintsettinglistobserver.h"

// Class declaration
/**
 *  Setting listbox class
 *  more_complete_description
 */
class CDirectPrintSettingListBox : public CAknSettingItemList
	{
public:
	/** Constructors */
	static CDirectPrintSettingListBox* NewL(const TRect& aRect);
	static CDirectPrintSettingListBox* NewLC(const TRect& aRect);
	/** Destructor */
	virtual ~CDirectPrintSettingListBox();

public: // from CAknSettingItemList
//	CAknSettingItem* CreateSettingItemL(TInt identifier);
	/**
	 * Set focus.
	 *
	 * @param aIndex The index of listbox item
	 * @param aCalledFromMenu The flag of called from menu
	 */
	void EditItemL(TInt aIndex, TBool aCalledFromMenu);

public: // New function
	/**
	 * Set list observer.
	 *
	 * @param aObserver Pointer to list observer.
	 */
	void SetSettingListObserver(MDirectPrintSettingListObserver* aObserver);

private:
	CDirectPrintSettingListBox();
	void ConstructL(const TRect& aRect);

private:
	/** Pointer to list observer. */
	MDirectPrintSettingListObserver* iObserver;
	};

#endif // __DIRECTPRINTSETTINGLISTBOX_H__
