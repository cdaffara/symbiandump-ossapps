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
#ifndef __DIRECTPRINTSETTINGLISTOBSERVER_H__
#define __DIRECTPRINTSETTINGLISTOBSERVER_H__

// Class declaration
/**
 *  Setting list observer mix-in class
 *  more_complete_description
 */
class MDirectPrintSettingListObserver
	{
public:
	/**
	 * Handles index.
	 *
	 * @param aIndex The index of selected item.
	 * @param aChanged The flag of change data.
	 */
	virtual void HandleEditItemL(TInt aIndex, TBool aChanged) = 0;
	};

#endif // __DIRECTPRINTSETTINGLISTOBSERVER_H__
