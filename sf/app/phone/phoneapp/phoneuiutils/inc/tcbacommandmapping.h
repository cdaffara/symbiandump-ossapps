/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class holds mapping between CBA and custom commands.
*
*/


#ifndef T_TCBACOMMANDMAPPING_H
#define T_TCBACOMMANDMAPPING_H


#include <e32base.h>

/**
 *  Class holds mapping between CBA and custom commands.
 *
 *  @lib phoneuiutils.dll
 *  @since S60 v5.1
 */
class TCbaCommandMapping
    {

public:
    /**
     * C++ default constructor. Use only for NULL-object creation.
     */    
    TCbaCommandMapping();
        
    /**
     * C++ overloaded constructor.
     */
    TCbaCommandMapping( TInt aCbaCommandId, TInt aCustomCommandId );
    
    /**
     * Returns CBA command identifier.
     * @return  CBA command identifier.
     */
    TInt CbaCommandId() const;
    
    /**
     * Returns custom command identifier.
     * @return  Custom command identifier.
     */
    TInt CustomCommandId() const;
    
    /**
     * Comparison operator for mappings.
     * @return  ETrue if mapping is same, EFalse otherwise.
     */
    TBool operator==( const TCbaCommandMapping& aMapping ) const;

private: // data

    /**
     * CBA command identifier.
     */
    TInt iCbaCommandId;
    
    /**
     * Custom command identifier.
     */
    TInt iCustomCommandId;

    };


#endif // T_TCBACOMMANDMAPPING_H
