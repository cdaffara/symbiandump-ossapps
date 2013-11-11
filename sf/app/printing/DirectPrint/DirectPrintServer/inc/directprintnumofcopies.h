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
*
*/


// Protection against nested includes
#ifndef DIRECTPRINTNUMOFCOPIES_H
#define DIRECTPRINTNUMOFCOPIES_H

// System includes
#include <e32base.h>

class CDirectPrintNumOfCopies : public CBase
	{
	public:
	
		/**
		 *	2-phase constructor
		 *
		 *	@return new object
		 */
		static CDirectPrintNumOfCopies* NewL();
		
		/**
		 *	Destructor
		 */
		~CDirectPrintNumOfCopies();	

	private:
		CDirectPrintNumOfCopies();
		void ConstructL();

	public:
	
		/**
		 *	Set the amount of coming array elements
		 *
		 *	@param aCount amount of coming array elements
		 */	 
		void Reset( TInt aCount );
		
		/**
		 *	Append next element to the array
		 *
		 *	@param aNumOfCopies number of copies array element
		 *	@param aFull place holder for value which indicates that
		 *			all elements are added to the array
		 */	
		void AddNumOfCopyL( TInt aNumOfCopies, TBool& aFull );
		
		/**
		 *	Number of copies array getter
		 *
		 *	@return number of copies array
		 */
		const RArray<TInt>& NumsOfCopies() const;					

	private: // data
		TInt iCount;
		RArray<TInt> iNumsOfCopies;
	};


#endif // DIRECTPRINTNUMOFCOPIES_H

//  End of File
