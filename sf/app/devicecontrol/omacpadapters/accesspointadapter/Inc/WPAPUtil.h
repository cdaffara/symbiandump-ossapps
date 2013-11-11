/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Class offers common utility methods.
*
*/





#ifndef WPAPUTIL_H
#define WPAPUTIL_H

// INCLUDES

#include <e32std.h>

// CLASS DECLARATION

/**
 * WPAPUtil offers common helper methods.
 */ 
class WPAPUtil
	{
	public: // New methods

		/**
		* Loads a specified resource string from resource file.
		* @param aResourceId is of a resource string.
		* @return pointer to loaded descriptor.
		*/
		static HBufC* ReadHBufCL( TInt aResourceId );

	};

#endif // WPAPUTIL_H

// End of File