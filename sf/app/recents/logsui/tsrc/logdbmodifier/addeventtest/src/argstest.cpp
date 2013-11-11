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



// INCLUDE FILES
#include "argstest.h"

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES  
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================

// none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  CArgs::CArgs
//  C++ default constructor can NOT contain any code, that
//  might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CArgs::CArgs() 
    {
    }

// Destructor
EXPORT_C CArgs::~CArgs()
    {

    ClearArgs();

    }

// -----------------------------------------------------------------------------
//  CArgs::ArgCount
//  Returns count of parsed arguments
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CArgs::ArgCount() const
    {

    return iArgCount;

    }

// -----------------------------------------------------------------------------
//  CArgs::ArgType
//  Returns type of argument (string/numeric)
// -----------------------------------------------------------------------------
//
EXPORT_C TArgType CArgs::ArgType( const TInt aArgIndex ) const
    {

    if ( aArgIndex < 1 || aArgIndex > iArgCount )
        {
        return EArgNotSpecified;
        }
    else
        {
        return iArgType[aArgIndex - 1];
        }

    }

// -----------------------------------------------------------------------------
//  CArgs::NumArg
//  Returns argument value as TInt
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CArgs::NumArg( const TInt aArgIndex ) const
    {

    if ( aArgIndex < 1 ||
         aArgIndex > iArgCount ||
         iArgType[aArgIndex - 1] != EArgNum )
        {
        return 0;
        }
    else
        {
        return iNumArg[aArgIndex - 1];
        }

    }

// -----------------------------------------------------------------------------
//  CArgs::StrArg
//  Returns argument value as string
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CArgs::StrArg( const TInt aArgIndex ) const
    {

    if ( aArgIndex < 1 || aArgIndex > iArgCount )
        {
        return KNullDesC;
        }
    else
        {
        return *iStrArg[aArgIndex - 1];
        }

    }

// -----------------------------------------------------------------------------
//  CArgs::ParseLineL
//  Parses arguments from a string
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CArgs::ParseLineL(
    const TParseType aType,
    const TDes& aLine )
    {

    TBuf<1024> line;
    line.Append( aLine );

    ClearArgs();
    
    switch ( aType )
        {
        case EParseComment:

            line.Trim();

            // if line starts with "//"
            //
            if ( line.Length() >= 2 &&
                 line[0] == static_cast< TInt >( TChar( '/' ) ) &&
                 line[1] == static_cast< TInt >( TChar( '/' ) ) )
                {
                return KErrNone;
                }
            break;

        case EParseFunction:

            TInt posDot = line.Locate( '.' );
            posDot = -1; // Messes stuff in case of ip addresses
            TInt posBraceOpen = line.Locate( '(' );

            if ( posBraceOpen == -1 )
                {
                return KErrArgument;
                }

            iArgType[0] = EArgStr;
            iArgType[1] = EArgStr;

            // read classname and methodname: "class.method("
            //
            if ( posDot != -1 && posDot < posBraceOpen )
                {
                iStrArg[0] = HBufC::NewL( posDot );
                iStrArg[0]->Des().Append( line.Ptr(), posDot );
                iStrArg[0]->Des().Trim();
                iStrArg[1] = HBufC::NewL( posBraceOpen - posDot );
                iStrArg[1]->Des().Append( line.Ptr() + posDot + 1,
                                          posBraceOpen - ( posDot + 1 ) );
                iStrArg[1]->Des().Trim();
                }
            else // only methodname: "method("
                {
                iStrArg[0] = HBufC::NewL( 0 );
                iStrArg[1] = HBufC::NewL( posBraceOpen );
                iStrArg[1]->Des().Append( line.Ptr(), posBraceOpen );
                iStrArg[1]->Des().Trim();
                }

            iArgCount = 2;

            // remove "class.method(" or "method(" from line
            //
            line.Delete( 0, posBraceOpen + 1 );
            line.Trim();

            // parse arguments
            //
            while ( line.Length() > 0 )
                {
                line.Trim();
                TInt pos = line.Locate( '"' );

                // string
                //
                if ( pos == 0 )
                    {
                    line.Delete( 0, 1 );
                    pos = line.Locate( '"' );
                    if ( pos == -1 )
                        {
                        return KErrArgument;
                        }

                    iArgType[iArgCount] = EArgStr;
                    iStrArg[iArgCount] = HBufC::NewL( pos );
                    iStrArg[iArgCount]->Des().Append( line.Ptr(), pos );
                    iArgCount++;
                    line.Delete( 0, pos + 1 );
                    line.Trim();

                    // remove possible comma
                    //
                    if ( line.Locate( ',' ) == 0 )
                        {
                        line.Delete( 0, 1 );
                        line.Trim();
                        }
                    }

                else // numeric? or enum value
                    {
                    pos = line.Locate( ',' );
                    if ( pos == -1 )
                        {
                        pos = line.Locate( ')' );
                        if ( pos == -1 )
                            {
                            return KErrArgument;
                            }
                        else if ( pos == 0 )
                            {
                            break;
                            }
                        }

                    iArgType[iArgCount] = EArgStr;
                    iStrArg[iArgCount] = HBufC::NewL( pos );
                    iStrArg[iArgCount]->Des().Append( line.Ptr(), pos );
                    iStrArg[iArgCount]->Des().Trim();
                    iArgCount++;
                    line.Delete( 0, pos + 1 );
                    line.Trim();

                    // len must be > 0
                    //
                    if ( iStrArg[iArgCount-1]->Length() == 0 )
                        {
                        return KErrArgument;
                        }

                    TBool isNum = ETrue;
                    TBool isNegative = EFalse;
                    TInt numVal = 0;

                    // check if argument is numeric -> [-]n(0-9)
                    //
                    for ( pos=0; pos < iStrArg[iArgCount - 1]->Length(); pos++ )
                        {
                        if ( pos == 0 &&
                             iStrArg[iArgCount - 1]->Locate( '-' ) == 0 )
                            {
                            isNegative = ETrue;
                            // if number is negative, len must be >= 2
                            //
                            if ( iStrArg[iArgCount - 1]->Length() < 2 )
                                {
                                isNum = EFalse;
                                break;
                                }
                            }
                        else
                            {
                            TInt number;

                            if      ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '0' ) ) ) number = 0;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '1' ) ) ) number = 1;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '2' ) ) ) number = 2;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '3' ) ) ) number = 3;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '4' ) ) ) number = 4;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '5' ) ) ) number = 5;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '6' ) ) ) number = 6;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '7' ) ) ) number = 7;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '8' ) ) ) number = 8;
                            else if ( (*iStrArg[iArgCount - 1])[pos] ==
                               static_cast< TInt >( TChar( '9' ) ) ) number = 9;
                            else
                                {
                                isNum = EFalse;
                                break;
                                }
                            numVal = (numVal * 10) + number;
                            }
                        }
                
                    // argument is numeric
                    //
                    if ( isNum )
                        {
                        if ( isNegative )
                            {
                            numVal = -numVal;
                            }

                        iNumArg[iArgCount - 1] = numVal;
                        iArgType[iArgCount - 1] = EArgNum;
                        } 

                    } // else

                } // while

            return KErrNone;

        } // switch

    return KErrArgument;

    }

// -----------------------------------------------------------------------------
//  CArgs::ClearArgs
//  Clears all parsed arguments and frees allocated memory
// -----------------------------------------------------------------------------
//
void CArgs::ClearArgs()
    {

    for ( TInt i = 0; i < iArgCount; i++ )
        {
        delete iStrArg[i];
        }

    iArgCount = 0;

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

#if defined(__WINS__) && !defined(EKA2)

// -----------------------------------------------------------------------------
// E32Dll 
// 
// DLL entry point function. In a MARM implementation, the 
// entry point is called when a thread is attached to or detached from the 
// DLL.?description
//
// Returns: TInt:   For DLLs which do not set up thread-local storage, 
//                  the function can ignore the TDllReason type 
//                  parameter passed to it and simply return KErrNone;
// -----------------------------------------------------------------------------
//

GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
    {

    return KErrNone;
    
    }
    
#endif

//  End of File  
