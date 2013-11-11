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
#ifndef __DIRECTPRINTVIEWBASE_H__
#define __DIRECTPRINTVIEWBASE_H__

// System includes
#include <aknview.h>

// User includes
#include "directprintlistobserver.h"
#include "directprintsettinglistobserver.h"

// Forward declarations
class CDirectPrintModel;
class CDirectPrintUiPluginMgr;
class CDirectPrintEngine;

// Class declaration
/**
 *  View base class
 *  more_complete_description
 */
class CDirectPrintViewBase : public CAknView, public MDirectPrintListObserver, public MDirectPrintSettingListObserver
	{
public:
	/** Destructor */
	~CDirectPrintViewBase();

public:
	/**
	 * From CAknView.
	 * Get the id of view.
	 *
	 * @return The id of view.
	 */
	TUid Id() const;

public:
	/**
	 * Handles index.
	 *
	 * @param aIndex The index of selected item.
	 */
	virtual void HandleListIndexL(TInt aIndex);

public:
	/**
	 * Handles index.
	 *
	 * @param aIndex The index of selected item.
	 * @param aChanged The flag of change data.
	 */
	virtual void HandleEditItemL(TInt aIndex, TBool aChanged);

protected:
	CDirectPrintViewBase();
	void ConstructL();

protected: // from CAknView
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();

protected: // New founctions
	void SetActiveTitleL(TInt aTitleResId);
	CDirectPrintModel& GetModel();
	CDirectPrintUiPluginMgr& GetPluginMgr();
	CDirectPrintEngine& GetEngine();

protected:
	/** View id. */
	TUid iViewId;

private:
	};

#endif // __DIRECTPRINTVIEWBASE_H__
