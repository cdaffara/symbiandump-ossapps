/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Harvester server commands
*
*/

#ifndef HARVESTERLOG_H
#define HARVESTERLOG_H

#include <e32debug.h>
#include <utf.h>

#ifdef _DEBUG

/**
 * Function for debug string output.
 *
 * @param aLogText C-string to write to debug output.
 * @lib charconv.lib
 */
 /*
inline void HLOG( char* aLogText ) 
    {
    _LIT( KFormat, "%S");
    TPtrC8 desc( (const TUint8*)aLogText );
    HBufC* output = HBufC::NewLC( desc.Length() );  //FIXME: TRAP/ignore this line
    TPtr outputText = output->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8( outputText, desc );
    RDebug::Print( KFormat, &outputText );
    CleanupStack::PopAndDestroy( output );
    }
*/
/**
 * Function for debug string output.
 *
 * @param aLogText String to write to debug output.
 * @deprecated
 */
/*
inline void HLOG( TPtrC16 aLogText ) 
    {
    RDebug::Print( aLogText );
    }
*/
/**
 * Function for debug string output.
 *
 * @param aLogText String to write to debug output.
 * @deprecated
 */
/*
inline void HLOG( TPtrC8 aLogText ) 
    {
    HBufC* output = HBufC::NewLC( aLogText.Length() );
    TPtr outputText = output->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8( outputText, aLogText );
    RDebug::Print( outputText );
    CleanupStack::PopAndDestroy( output );
    }
*/
/**
 * Function for debug output.
 *
 * @param aFormat Format for output string.
 * @param aLog Value to write to debug output. Any type accepted by RDebug is ok.
 * @lib charconv.lib
 */
/*
template<typename T>
inline void HLOG1( char* aFormat, T aLog ) 
    {
    TPtrC8 fmt( (const TUint8*)aFormat );
    HBufC* format = HBufC::NewLC( fmt.Length() );
    TPtr formatPtr = format->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8( formatPtr, fmt );
    RDebug::Print( formatPtr, aLog );
    CleanupStack::PopAndDestroy( format );
    }
*/
/**
 * Function for debug output.
 *
 * @param aFormat Format for output string.
 * @param aLogText String, int, etc to output.
 * @deprecated
 */
/*
template<typename T>    
inline void HLOG1( TPtrC16 aFormat, T aLogText ) 
    {
    RDebug::Print( aFormat, aLogText );
    }
*/

#define WRITELOG(a) RDebug::Print(_L(a))
#define WRITELOG1(a,b) RDebug::Print(_L(a),(b))
#define WRITELOG2(a,b,c) RDebug::Print(_L(a),(b),(c))
#define WRITELOG3(a,b,c,d) RDebug::Print(_L(a),(b),(c),(d))

#define HLOG(a)  RDebug::Print((a))
#define HLOG1(a, b)  RDebug::Print((a), (b))
#define HLOG2(a, b, c)  RDebug::Print((a), (b), (c))
#define HLOG3(a, b, c, d)  RDebug::Print((a), (b), (c), (d))
#define HLOG4(a, b, c, d, e)  RDebug::Print((a), (b), (c), (d), (e))
#define HLOG5(a, b, c, d, e, f)  RDebug::Print((a), (b), (c), (d), (e), (f))

#else 

#define WRITELOG(a)
#define WRITELOG1(a,b)
#define WRITELOG2(a,b,c)
#define WRITELOG3(a,b,c,d)

#define HLOG(a)
#define HLOG1(a, b)
#define HLOG2(a, b, c) 
#define HLOG3(a, b, c, d) 
#define HLOG4(a, b, c, d, e) 
#define HLOG5(a, b, c, d, e, f) 

#endif

#endif  // HARVESTERLOG_H
