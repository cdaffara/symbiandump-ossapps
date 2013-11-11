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
* Description:  The implementation for presentation elements.
*
*/


#ifndef MIRPARSEROBSERVER
#define MIRPARSEROBSERVER

#include <e32def.h>

class MIRParserObserver
	{
public:
    /**
    * Function : ParseError
    * Called on Parser Error
    */
    virtual void ParseError( TInt aErrorCode ) = 0;
    
    /**
    * Function : ParsedStructureL
    * called for parsed result
    */    
    virtual void ParsedStructureL(TInt aChoice) = 0;
   };

#endif //MIRPARSEROBSERVER
