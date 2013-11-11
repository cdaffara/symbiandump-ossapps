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


// RsImageParser.cpp: implementation of the CRsImageParser class.
//
//////////////////////////////////////////////////////////////////////

#include "crsimageparser.h"
#include "clog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRsImageParser* CRsImageParser::NewLC()
	{
	CRsImageParser* self = new (ELeave) CRsImageParser();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CRsImageParser::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}


CRsImageParser::~CRsImageParser()
	{
	iFs.Close();
	}

CRsImageParser::CRsImageParser()							
	{
	}

void CRsImageParser::ValidateL( const TDesC& aFileName, TBool& aValid, TInt& aErr )
	{
	LOG1("CRsImageParser::ValidateL aFileName: %S", &aFileName);				
	aValid = EFalse;
	TInt dataPos( 0 );
	TBuf8<9> imgData;
	TBool bAllDone( EFalse );
	TInt height( 0 );
	TInt width( 0 );
	TInt startOfDataOffset( 0 );
	TInt encoding( EUnknown );
	TInt size( 0 );
	
	RFile file;
	aErr = file.Open( iFs, aFileName, EFileRead | EFileShareReadersOnly );
	LOG1("CRsImageParser::ValidateL file.Open (shareread mode): %d", aErr);
	if ( aErr != KErrNone )
		{
		aErr = file.Open( iFs, aFileName, EFileRead | EFileShareAny );
		LOG1("CRsImageParser::ValidateL file.Open (shareany mode): %d", aErr);
		}
	
	// magic numbers used here are JPEG format related
	else // if ( aErr == KErrNone ) // 1 IF
		{
		CleanupClosePushL( file );
		aErr = file.Size( size );
		if( size && aErr == KErrNone ) // 2 IF
			{
			aErr = ReadData( file, dataPos, imgData );
			if( aErr == KErrNone && imgData.Length() == 9 ) // 3 IF
				{
				// First check for the JPEG header "FF D8"
				if( imgData[0] == 0xff && imgData[1] == 0xd8 ) // 4 IF
					{
					// Start skipping headers until we find the one we're looking for
					dataPos = (imgData[4] << 8) + imgData[5] + 4;

					aErr = ReadData( file, dataPos, imgData );

					while( aErr == KErrNone && imgData.Length() == 9 && imgData[0] == 0xff )
						{
						switch( imgData[1] )
							{
							case 0xda:
								// Start of data
								startOfDataOffset = dataPos + ((imgData[2] << 8) + imgData[3] + 2);
								bAllDone = ETrue;
								break;

							case EDiffArithLossless:
								encoding = imgData[1];
								height = (imgData[5] << 8) + imgData[6];
								width = (imgData[7] << 8) + imgData[8];
								break;
								
							default:
								break;
							} // SWITCH

							if( bAllDone )
								{
								break;
								}
							else
								{
								dataPos += (imgData[2] << 8) + imgData[3] + 2;
								aErr = ReadData( file, dataPos, imgData );
								}
						} // WHILE
					} // 4 IF
				else
					{
					// Not a JPG
					aErr = KErrCorrupt;
					}
				} // 3 IF
			} // 2 IF
			
		CleanupStack::PopAndDestroy( &file ); 
		} // 1 IF

	aValid = Validate( startOfDataOffset, width, height, size, encoding, aErr );
		
	LOG1("CRsImageParser::ValidateL aValid: %d", aValid);
	LOG1("CRsImageParser::ValidateL aErr: %d", aErr);
	LOG("CRsImageParser::ValidateL end");				
	}

TBool CRsImageParser::Validate( TInt aStartOfDataOffset, TInt aWidth, TInt aHeight, TInt aSize, TInt aEncoding, TInt aErr )
	{
	if ( aStartOfDataOffset && aWidth && aHeight && aSize && aEncoding && !aErr )
		{
		return ETrue;
		}
	else 
		{
		return EFalse;
		}
	}

TInt CRsImageParser::ReadData( RFile& aFile, TInt aStartByte, TDes8& aData )
	{
	LOG("CRsImageParser::ReadData begin");
	TInt err = aFile.Read( aStartByte, aData );
	LOG1("CRsImageParser::ReadData return: %d", err);
	return err;	
	}

//  End of File
