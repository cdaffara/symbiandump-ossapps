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
* Description: 
*     Sortable string pair.
*
*/


#ifndef CWPSTRINGPAIR_H
#define CWPSTRINGPAIR_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Sortable string pair
 */
class CWPStringPair : public CBase
	{
	public:
        /**
        * C++ default constructor.
        */
		CWPStringPair();

        /**
        * Destructor.
        */
		~CWPStringPair();

	public:
		/**
		* Sets the values for string pair.
		* @param aName The name of the string
		* @param aValue The value of the string
		*/
		void SetL( const TDesC& aName, const TDesC& aValue );

	public:
		// Points to iName. Use for constructing TKeyArrayFixPtr.
		TPtrC iNamePtr;

		// Points to iValue. Use for constructing TKeyArrayFixPtr.
		TPtrC iValuePtr;

	private:
		// The name of the pair. Owns.
		HBufC* iName;

		// The value of the pair. Owns.
		HBufC* iValue;
	};

#endif // CWPSTRINGPAIR_H

// End of File
