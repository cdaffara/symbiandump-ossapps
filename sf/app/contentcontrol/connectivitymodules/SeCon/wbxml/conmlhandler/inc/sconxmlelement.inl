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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONXMLELEMENT_INL__
#define __SCONXMLELEMENT_INL__

// ------------------------------------------------------------------------------------------------
inline TXMLElementParams::TXMLElementParams() : iCallbacks(0), iCmdStack(0), iCleanupStack(0)
	{
	}

// ------------------------------------------------------------------------------------------------
inline TAny* TXMLElementParams::Callbacks() const
	{
	return iCallbacks;
	}

// ------------------------------------------------------------------------------------------------
inline CXMLStack<CXMLElement>* TXMLElementParams::CmdStack() const
	{
	return iCmdStack;
	}

// ------------------------------------------------------------------------------------------------
inline CXMLStack<CXMLElement>* TXMLElementParams::CleanupStack() const
	{
	return iCleanupStack;
	}

#endif // __SCONXMLELEMENT_INL__