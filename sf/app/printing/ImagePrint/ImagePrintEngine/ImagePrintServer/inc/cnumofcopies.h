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


#ifndef CNUMOFCOPIES_H
#define CNUMOFCOPIES_H

#include <e32base.h>

class CNumOfCopies : public CBase
	{
	public:
	
		/**
		 *	2-phase constructor
		 *
		 *	@return new object
		 */
		static CNumOfCopies* NewL();
		
		/**
		 *	Destructor
		 */
		~CNumOfCopies();	

	private:
		CNumOfCopies();
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


#endif // CNUMOFCOPIES_H

//  End of File
