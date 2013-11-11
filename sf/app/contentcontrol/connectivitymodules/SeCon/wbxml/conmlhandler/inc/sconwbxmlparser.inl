/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SCONWBXMLPARSER_INL__
#define __SCONWBXMLPARSER_INL__


inline TWBXMLStackItem::TWBXMLStackItem( TUint8 aTag )  : iTag(aTag)
	{
	}

inline TUint8 TWBXMLStackItem::Tag() const
	{
	return iTag;
	}

#endif // __SCONWBXMLPARSER_INL__
