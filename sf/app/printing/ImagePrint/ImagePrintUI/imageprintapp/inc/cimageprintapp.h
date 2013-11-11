/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CIMAGEPRINTAPP_H
#define CIMAGEPRINTAPP_H

#include <e32base.h>
#include <aknapp.h>

/**
* UID of the application
*/
const TUid KUidImagePrint = { 0x101FFA84 };

//  CLASS DEFINITION
/**
 * Application class
 */
class CImagePrintApp : public CAknApplication
	{

	private:	// Constructors and destructors

		CApaDocument* CreateDocumentL();

    private:	// Methods derived from CApaApplication

		TUid AppDllUid() const;

    };


// INLINE FUNCTIONS

#endif  // CIMAGEPRINTAPP_H

//  End of File

