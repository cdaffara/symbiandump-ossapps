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
* Description:  Argument parser
*
*/



#ifndef CARGS_H
#define CARGS_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KMaxArgs = 100;

// MACROS
// none

// DATA TYPES
enum TParseType
    {
    // class.method (arg1, arg2, ...)
    // functionname (arg1, arg2, ...)
    //
    // classname is always found from StrArg(1) (if only funcname provided, "")
    // method/functonname is always found from StrArg(2)
    //
    EParseFunction,

    // // comment here
    //
    EParseComment
    };

enum TArgType
    {
    EArgNotSpecified,
    EArgNum,
    EArgStr
    };

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CArgs is used to parse parameters from a data file
*
*  @lib commonutils.lib
*  @since 
*/
class CArgs : public CBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C CArgs();

        /**
        * Destructor.
        */
        IMPORT_C ~CArgs();

    public: // New functions

        /**
        * Returns count of parsed arguments.
        * @since
        * @return Count of parsed arguments
        */
        IMPORT_C TInt ArgCount() const;

        /**
        * Returns type of argument (string/numeric).
        * @since
        * @param aArgIndex Index of argument
        * @return Type of argument
        */
        IMPORT_C TArgType ArgType( const TInt aArgIndex ) const;

        /**
        * Returns argument value as TInt
        * @since
        * @param aArgIndex Index of argument
        * @return Integer value of argument
        */
        IMPORT_C TInt NumArg( const TInt aArgIndex ) const;

        /**
        * Returns argument value as String
        * @since
        * @param aArgIndex Index of argument
        * @return Argument as String
        */
        IMPORT_C const TDesC& StrArg( const TInt aArgIndex ) const;

        /**
        * Parses arguments from a string
        * @since
        * @param aType Type of argument
        * @param aLine Line to parse
        * @return Errorcode
        */
        IMPORT_C TInt ParseLineL( const TParseType aType, const TDes& aLine );

    public: // Functions from base classes
    // none

    protected:  // New functions
    // none

    protected:  // Functions from base classes
    // none

    private:

        /**
        * Clears all parsed arguments and frees allocated memory
        * @since
        */
        void ClearArgs();

    public:     // Data
    // none

    protected:  // Data
    // none

    private:    // Data

        // Argument count
        TInt iArgCount;

        // Array for arguments revealing the type of argument
        TArgType iArgType[KMaxArgs];

        // Array for numeric arguments
        TInt iNumArg[KMaxArgs];

        // Array for string arguments
        HBufC* iStrArg[KMaxArgs];

    public:     // Friend classes
    // none

    protected:  // Friend classes
    // none

    private:    // Friend classes
    // none

    };

#endif      // CARGS_H

//  End of File  
