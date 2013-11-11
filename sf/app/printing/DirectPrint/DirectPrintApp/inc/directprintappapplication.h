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
#ifndef __DIRECTPRINTAPPAPPLICATION_H__
#define __DIRECTPRINTAPPAPPLICATION_H__

// System includes
#include <aknapp.h>
 
// User includes
#include "DirectPrintApp.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidDirectPrintAppApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CDirectPrintAppApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CDirectPrintAppApplication is the application part of the
 * AVKON application framework for the DirectPrintApp example application.
 */
class CDirectPrintAppApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidDirectPrintAppApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CDirectPrintAppDocument document object. The returned
	 * pointer in not owned by the CDirectPrintAppApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __DIRECTPRINTAPPAPPLICATION_H__
// End of File
