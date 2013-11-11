/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Encryptable table.
*
*/



#ifndef __ENCRYPTABLE_TABLE_H__
#define __ENCRYPTABLE_TABLE_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <d32dbms.h>

// =======
// CLASSES
// =======

// =============================
// CLASS: MEncryptableTable
// =============================
class MEncryptableTable
	{
	public:

        // ==============
        // PUBLIC METHODS
        // ==============
        	
		virtual TBool EncryptFirstL( TDbBookmark& aNextRow ) = 0;
		virtual TBool EncryptNextL( TDbBookmark& aNextRow ) = 0;
        
        virtual TBool DecryptFirstL( TDbBookmark& aNextRow ) = 0;
        virtual TBool DecryptNextL( TDbBookmark& aNextRow ) = 0;
	
	}; // end class MEncryptableTable
	

#endif  //__ENCRYPTABLE_TABLE_H__
