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
* Description:  Contains the CDirectPrintRsImageParser class definition.
*
*/


#ifndef CDIRECTPRINTRSDIRECTPARSER_H
#define CDIRECTPRINTRSDIRECTPARSER_H

#include <e32base.h>
#include <f32file.h>

class CDirectPrintRsImageParser : public CBase
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
		static CDirectPrintRsImageParser* NewLC();
		
		/**
		 *	Destructor
		 */			
		~CDirectPrintRsImageParser();
		
		/**
		 *	JPEG image validator
		 *
		 *	@param aFileName image file name
		 *	@param aValid place holder for validity indicator
		 *	@param aErr place holder for error code
		 */			
		void ValidateL( const TDesC& aFileName, TBool& aValid, TInt& aErr );
								
	private:		

		CDirectPrintRsImageParser();
		void ConstructL();	
		TBool Validate( TInt aStartOfDataOffset, TInt aWidth, TInt aHeight, TInt aSize, TInt aEncoding, TInt aErr );
		TInt ReadData( RFile& aFile, TInt aStartByte, TDes8& aData );

	private:
	
		RFs iFs;
	};

#endif // CDIRECTPRINTRSDIRECTPARSER_H

//  End of File
