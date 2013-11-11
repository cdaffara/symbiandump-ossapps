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
#ifndef _DIRECTPRINTUIPLUGINMGR_H_
#define _DIRECTPRINTUIPLUGINMGR_H_

// System includes
#include <e32base.h>
#include <implementationproxy.h>

// Forward declarations
class CDirectPrintModel;
class CDirectPrintUiPlugin;

// Class declaration
/**
 *  UiPrugin manager class
 *  more_complete_description
 */
class CDirectPrintUiPluginMgr : public CBase
	{
public:
	/** Constructors */
	static CDirectPrintUiPluginMgr* NewL(CDirectPrintModel& aModel);
	static CDirectPrintUiPluginMgr* NewLC(CDirectPrintModel& aModel);
	/** Destructor */
	~CDirectPrintUiPluginMgr();

public:
	/**
	 * Load plugin object.
	 *
	 * @param aUid The Uid of plugin.
	 */
	void LoadPluginL(TUid aUid);
	/**
	 * Load plugin object.
	 *
	 * @param aKey default_data string.
	 */
	void LoadPluginL(const TDesC8& aKey);
	/**
	 * Get current plugin object.
	 *
	 * @return Pointer to plugin object.
	 */
	CDirectPrintUiPlugin* CurrentPluginL();

protected:
	CDirectPrintUiPluginMgr(CDirectPrintModel& aModel);
	void ConstructL();

private:
	/** Reference to model class. */
	CDirectPrintModel& iModel;
	/** Pointer to plugin. */
	CDirectPrintUiPlugin* iPlugin;
	/** Uid of current plugin. */
	TUid iCurrentPluginUid;
	};

#endif // _DIRECTPRINTUIPLUGINMGR_H_
