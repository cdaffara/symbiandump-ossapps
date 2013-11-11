/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONCONMLHANDLERERROR_H__
#define __SCONCONMLHANDLERERROR_H__

// -----------------------------------------------------------------------------
// Parser error base
// -----------------------------------------------------------------------------

#define PERRBASE 0x2000

enum TConMLParserError
	{
	KWBXMLParserErrorOk = 0,
	KWBXMLParserErrorEof = PERRBASE,
	KWBXMLParserErrorEofTooEarly,
	KWBXMLParserErrorInvalidTag,
	KWBXMLParserErrorWrongWBXMLVersion,
	KWBXMLParserErrorWrongPublicId,
	KWBXMLParserErrorinvalidDocument
	};

#define GERRBASE 0x3000

enum TWBXMLGeneratorError
	{
	KWBXMLGeneratorOk = 0,
	KWBXMLGeneratorBufferFull = GERRBASE
	};
	
#endif // __SCONCONMLHANDLERERROR_H__