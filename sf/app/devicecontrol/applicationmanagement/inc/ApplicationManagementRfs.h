/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Implementation of applicationmanagement components
*
*/


#ifndef __AMRFS_H__
#define __AMRFS_H__

#include <e32std.h>

class RAppMgmtRfs : public RSessionBase
	{
	public:
	
	/**
	 * Connect Connects to ApplicationManagmeent server (starts it if it's not 
	 *	running
	 * @return KErrNone or Symbian error code
	 */
	IMPORT_C TInt Connect();
	
	
	/**
	 * Close Closes connection to ApplicationManagement server
	 * @return KErrNone or Symbian error code
	 */
	IMPORT_C void Close();

	// New methods
	
	/**
	 * PerformRfsL Causes ApplicationManagement to prepare for RFS. That means
	 * 	uninstall of serveral applications, etc.
	 * @return KErrNone or Symbian error code
	 */
	IMPORT_C void PerformRfsL() const;
	
	  /**
		* Implementation of Service UID() from base class. Returns AM service UID 
		* to framework
		*	
		* @return uid of AM service UID
		*/	
		TUid ServiceUid() const;  
	};
	
#endif

//  End of File
