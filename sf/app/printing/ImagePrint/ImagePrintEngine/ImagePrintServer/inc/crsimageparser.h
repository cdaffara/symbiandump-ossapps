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
* Description:  Contains the CRsImageParser class definition.
*
*/


#ifndef CRSIMAGEPARSER_H
#define CRSIMAGEPARSER_H

#include <e32base.h>
#include <f32file.h>

class CRsImageParser : public CBase
	{
	public:
		enum TImageEncoding
			{
			EUnknown = 0,
			ENonDiffHuffmanBaseline = 0xc0,
			ENonDiffHuffmanExtendedSeq,
			ENonDiffHuffmanProgressive,
			ENonDiffHuffmanLossless,
			EUnused1,
			EDiffHuffmanSequential,
			EDiffHuffmanProgressive,
			EDiffHuffmanLossless,
			EReserved,
			ENonDiffArithExtendedSeq,
			ENonDiffArithProgressive,
			ENonDiffArithLossless,
			EUnused2,
			EDiffArithSequential,
			EDiffArithProgressive,
			EDiffArithLossless
			};

		/**
		 *	2-phase constructor
		 *
		 *	@return new object. The object is left on the cleanup stack
		 */
		static CRsImageParser* NewLC();
		
		/**
		 *	Destructor
		 */			
		~CRsImageParser();
		
		/**
		 *	JPEG image validator
		 *
		 *	@param aFileName image file name
		 *	@param aValid place holder for validity indicator
		 *	@param aErr place holder for error code
		 */			
		void ValidateL( const TDesC& aFileName, TBool& aValid, TInt& aErr );
								
	private:		

		CRsImageParser();
		void ConstructL();	
		TBool Validate( TInt aStartOfDataOffset, TInt aWidth, TInt aHeight, TInt aSize, TInt aEncoding, TInt aErr );
		TInt ReadData( RFile& aFile, TInt aStartByte, TDes8& aData );

	private:
	
		RFs iFs;
	};

#endif // CRSIMAGEPARSER_H

//  End of File
