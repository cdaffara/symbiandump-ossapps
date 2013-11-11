/*
 * ==============================================================================
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
 * Description:
 * 
 * ==============================================================================
 */

#include <e32base.h>
#include <utf.h>
#include <cmpluginpacketdatadef.h>

#include "Utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "UtilsTraces.h"
#endif

// ======== LOCAL CONSTANTS =========

_LIT8( KFalse,               "False" );
_LIT8( KTrue,                "True" );
_LIT8( KWhiteSpace,          " " );

// ======== LOCAL FUNCTIONS =========

// ======== MEMBER FUNCTIONS ========   
//------------------------------------------------------------------------------
// CUtils::CUtils()
//------------------------------------------------------------------------------
CUtils::CUtils() 
    {
    OstTraceFunctionEntry0( CUTILS_CUTILS_ENTRY );
    OstTraceFunctionExit0( CUTILS_CUTILS_EXIT );
    }

//------------------------------------------------------------------------------
// CUtils::~CUtils()
//------------------------------------------------------------------------------
CUtils::~CUtils()
    {
    }

//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetFirstURISeg(const TDesC8& aURI)
//------------------------------------------------------------------------------
TPtrC8 CUtils::GetFirstURISeg( const TDesC8& aURI )
    {
    OstTraceExt1( TRACE_NORMAL, CUTILS_GETFIRSTURISEG_ENTRY, 
                  "ENTRY: CUtils::GetFirstURISeg;aURI=%s", aURI );
    
    TInt i = 0;
    TInt j = 0;
    
    for( i  =0; i <= aURI.Length()-1; i++ )
        {
        if( aURI[i] == '/' )
            {
            if ( i == 0 )
                {
                // Skip first slash
                j = 1;
                continue;
                }
            break;
            }
        }
    OstTraceExt1( TRACE_NORMAL, CUTILS_GETFIRSTURISEG_EXIT, 
                  "EXIT: CUtils::GetFirstURISeg;retval=%s", aURI.Mid( j,i-j ) );
    
    return aURI.Mid( j,i-j );
    }

//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetSecondURISeg(const TDesC8& aURI)
//------------------------------------------------------------------------------
TPtrC8 CUtils::GetSecondURISeg( const TDesC8& aURI )
    {
    OstTraceExt1( TRACE_NORMAL, CUTILS_GETSECONDURISEG_ENTRY, 
                  "ENTRY: CUtils::GetSecondURISeg;aURI=%s", aURI );
    
    TInt j = 0;
    TInt i = 0;
    
    for( i = 0; i <= aURI.Length() - 1; i++ )
        {
        if( aURI[i] == '/' )
            {
            for( j = i + 1; j <= aURI.Length() - 1; j++ )
                {
                if( aURI[j] == '/' ) // Second slash => AP/xxxx
                    {
                    break;
                    }
                }
            break;
            }
        }
    OstTraceExt1( TRACE_NORMAL, CUTILS_GETSECONDTURISEG_EXIT, 
                  "EXIT: CUtils::GetSecondURISeg;retval=%s", aURI.Mid( i+1, j-1-i ) );
    
    return aURI.Mid( i+1, j-1-i );
    }

//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetLastURISeg(const TDesC8& aURI)
// Returns only the last uri segment
//------------------------------------------------------------------------------
TPtrC8 CUtils::GetLastURISeg( const TDesC8& aURI )
    {
    OstTraceExt1( TRACE_NORMAL, CUTILS_GETLASTURISEG_ENTRY, 
                  "ENTRY: CUtils::GetLastURISeg;aURI=%s", aURI );
    
    TInt i;
    
    for( i = aURI.Length() - 1 ; i >= 0; i-- )
        {
        if( aURI[i] == '/' )
            {
            break;
            }
        }
    if( i == 0 )
        {
        OstTraceExt1( TRACE_NORMAL, CUTILS_GETLASTURISEG_EXIT, 
                      "EXIT: CUtils::GetLastURISeg;retval=%s", aURI );
        return aURI;
        }
    else
        {
        OstTraceExt1( TRACE_NORMAL, CUTILS_GETLASTURISEG_EXIT2, 
                      "EXIT: CUtils::GetLastURISeg;retval=%s", aURI.Mid( i+1 ) );
        return aURI.Mid( i+1 );
        }
    }

//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetEndOfURIFromSeg(const TDesC8& aURI,const TDesC8 aName)
// Returns the end of aURI starting from aName
//------------------------------------------------------------------------------
TPtrC8 CUtils::GetEndOfURIFromSeg( const TDesC8& aURI, const TDesC8& aName )
    {
    OstTraceExt2( TRACE_NORMAL, CUTILS_GETENDOFURIFROMSEG_ENTRY, 
                  "ENTRY: CUtils::GetEndOfURIFromSeg;aURI=%s;aName=%s", aURI, aName );
    
    TInt begin = aURI.Find( aName );
    
    if ( begin == KErrNotFound )
        {
        OstTrace1( TRACE_NORMAL, CUTILS_GETENDOFURIFROMSEG_EXIT, 
                   "EXIT: CUtils::GetEndOfURIFromSeg;%d", KErrNotFound );
        return aURI;
        }
    OstTraceExt1( TRACE_NORMAL, CUTILS_GETENDOFURIFROMSEG_EXIT2, 
                  "EXIT: CUtils::GetEndOfURIFromSeg;%s", aURI.Mid(begin) );    
    return aURI.Mid(begin);
    }

//------------------------------------------------------------------------------
// CUtils::ConvertTo8LC()
// Converts string value to 8-bit
//------------------------------------------------------------------------------
TDesC8& CUtils::ConvertTo8LC( const TDesC& aSource )
    {
    HBufC8* buf = HBufC8::NewLC( aSource.Length() * 2 );
    TPtr8 bufPtr = buf->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
    }

//------------------------------------------------------------------------------
// CUtils::ConvertTo16LC()
// Converts string value to 16-bit      
//------------------------------------------------------------------------------
TDesC16& CUtils::ConvertTo16LC( const TDesC8& aSource )
    {
    HBufC16* buf16 = HBufC16::NewLC( aSource.Length() );
    TPtr bufPtr16 = buf16->Des();

    CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, aSource );

    return *buf16;
    }

// -------------------------------------------------------------------------------------
// CUtils::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CUtils::FillNodeInfoL( MSmlDmDDFObject& aNode,
                            TSmlDmAccessTypes aAccTypes,
                            MSmlDmDDFObject::TOccurence aOccurrence, 
                            MSmlDmDDFObject::TScope aScope, 
                            MSmlDmDDFObject::TDFFormat aFormat,
                            const TDesC8& aDescription )
                                          
    {
    aNode.SetAccessTypesL( aAccTypes );
    aNode.SetOccurenceL( aOccurrence );
    aNode.SetScopeL( aScope );
    aNode.SetDFFormatL( aFormat );
    if( aFormat != MSmlDmDDFObject::ENode )
        {
        // aNode.AddDFTypeMimeTypeL(KNSmlMimeType);
        }
    if ( aDescription != KNullDesC8 )
        {
        aNode.SetDescriptionL( aDescription );
        }
    }

// -------------------------------------------------------------------------------------
// CUtils::StripWhiteSpaces()
// Removes all white spaces from a string
// -------------------------------------------------------------------------------------
void CUtils::RemoveIllegalCharsFromURI( TDes8& aString )
    {
    TInt delPos;
    TInt delLen = 1;
    
    aString.TrimAll();
    delPos = aString.Find( KWhiteSpace );
    while ( delPos != KErrNotFound )
        {
        aString.Delete( delPos,delLen );
        delPos = aString.Find( KWhiteSpace );
        }
    }

// -------------------------------------------------------------------------------------
// TInt CUtils::DesCToBool
// -------------------------------------------------------------------------------------
TInt32 CUtils::DesToBool( const TDesC8& aSource, TBool& aBool )
    {
    TInt retval( KErrNone );
    
    if ( aSource.Match( KTrue ) != KErrNotFound )
        {
        aBool = ETrue;
        }
    else if ( aSource.Match( KFalse ) != KErrNotFound )
        {
        aBool = EFalse;
        }
    else
        {
        retval = KErrGeneral;
        }
        
    return retval;
    }

// -------------------------------------------------------------------------------------
// CUtils::IntToCharL()
// -------------------------------------------------------------------------------------
TDesC8& CUtils::IntToCharL( const TInt& aObject )
    {
    HBufC8* buf = HBufC8::NewLC( 8 );
    TPtr8 ptrBuf = buf->Des();

    ptrBuf.Num( aObject );

    return *buf;
    }
//------------------------------------------------------------------------------
// TPtrC8 CUtils::AppendFirstURISeg( TDes8& aURI, TDes8& aTarget )
//------------------------------------------------------------------------------
TInt CUtils::AppendFirstURISeg( TDes8& aURI, TDes8& aTarget )
    {
    
    TInt i = aURI.FindC(_L8("/"));
    
    if ( i != KErrNotFound )
        {
        aTarget.Append( aURI.Left( i ) );
        aURI.Copy( aURI.Right( aURI.Length() - i - 1 ) );
        }
    else
        {
        aTarget.Copy( aURI );
        aURI.Copy( aURI.Right( 0 ) );
        }
    
    return i;
    }

//=============================================
//      CUtils::IntLUID()
//      Returns IntValue for aLUID
//=============================================
TInt CUtils::IntLUID(const TDesC8& aLUID)
    {
    TInt ret(0);

    TLex8 lex(aLUID);
    
    lex.Val( ret );
    
    return ret;
    }

