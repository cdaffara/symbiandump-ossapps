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


#ifndef __SCONWBXMLDEFS_H__
#define __SCONWBXMLDEFS_H__

// -----------------------------------------------------------------------------
// Typedefs
// -----------------------------------------------------------------------------
typedef TUint8 TWBXMLTag;

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------
const TWBXMLTag SWITCH_PAGE = 0;
const TWBXMLTag END = 1;
const TWBXMLTag ENTITY = 2;
const TWBXMLTag STR_I = 3;
const TWBXMLTag LITERAL = 4;
const TWBXMLTag OPAQUE = 0xc3; 

const TUint8 KWBXMLHasAttributes = (1<<7);
const TUint8 KWBXMLHasContent = (1<<6);


#endif // __SCONWBXMLDEFS_H__