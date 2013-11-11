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
* Description: 
*
*/
#include "NPUtils.h"

#include <utf.h>
#include "CCPixNPPluginEcom.h"

namespace nputils {


// -----------------------------------------------------------------------------

TInt VariantToInt( NPVariant& v )
    {
    if ( NPVARIANT_IS_INT32( v ) ) return NPVARIANT_TO_INT32( v );
    if ( NPVARIANT_IS_DOUBLE( v ) )
        {
        TReal value = NPVARIANT_TO_DOUBLE( v );
        if (value > 0) return value + 0.5; 
        return value - 0.5;
        }
    if ( NPVARIANT_IS_BOOLEAN( v ) ) return NPVARIANT_TO_BOOLEAN( v );
    return 0;
    }

// -----------------------------------------------------------------------------

TUint VariantToUnsigned( NPVariant& v )
    {
    TInt value = 0;
    if ( NPVARIANT_IS_INT32( v ) ) value = NPVARIANT_TO_INT32( v );
    if ( NPVARIANT_IS_DOUBLE( v ) ) value = NPVARIANT_TO_DOUBLE( v ) + 0.5;
    if ( NPVARIANT_IS_BOOLEAN( v ) ) value = NPVARIANT_TO_BOOLEAN( v );
    if (value < 0) return 0;
    return value;
    }

// -----------------------------------------------------------------------------

TReal VariantToDouble( NPVariant& v )
    {
    if ( NPVARIANT_IS_INT32( v ) ) return NPVARIANT_TO_INT32( v );
    if ( NPVARIANT_IS_DOUBLE( v ) ) return NPVARIANT_TO_DOUBLE( v );
    if ( NPVARIANT_IS_BOOLEAN( v ) ) return NPVARIANT_TO_BOOLEAN( v );
    return 0;
    }

// -----------------------------------------------------------------------------

void DescriptorToVariantL(const TDesC16& aString, NPVariant& aVariant)
	{
    CCPixNPPluginEcom* pluginEcom = (CCPixNPPluginEcom*) Dll::Tls();
    NPNetscapeFuncs* netscapeFuncs = pluginEcom->Funcs();
    
    HBufC8* utf8String = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aString);
    int length = utf8String->Length();
        
    char* newString = (char *)netscapeFuncs->memalloc( length );
    if (newString)
    	{
        Mem::Copy(newString, utf8String->Des().Ptr(), length);
        STRINGN_TO_NPVARIANT(newString, length, aVariant);
    	}
    delete utf8String;
	}

// -----------------------------------------------------------------------------

void DescriptorToVariant(const TDesC8& aString, NPVariant& aVariant)
    {
    CCPixNPPluginEcom* pluginEcom = (CCPixNPPluginEcom*) Dll::Tls();
    NPNetscapeFuncs* netscapeFuncs = pluginEcom->Funcs();
    
    int length = aString.Length();
    char* newString = (char *)netscapeFuncs->memalloc( length );
    
    if (newString)
        {
        Mem::Copy(newString, aString.Ptr(), length);
        STRINGN_TO_NPVARIANT(newString, length, aVariant);
        }
    }

// -----------------------------------------------------------------------------

void VariantToDescriptor(NPVariant& aVariant, TDes8& aString)
    {
    if ( !NPVARIANT_IS_STRING(aVariant) ) return;
    NPString string = NPVARIANT_TO_STRING(aVariant);
    aString.Copy((TUint8*)string.UTF8Characters, string.UTF8Length);
    }

// -----------------------------------------------------------------------------

void VariantToDescriptorL(NPVariant& aVariant, TDes16& aString)
    {
    if ( !NPVARIANT_IS_STRING(aVariant) ) return;
    NPString string = NPVARIANT_TO_STRING(aVariant);
    RBuf8 buffer;
    buffer.CreateL(string.UTF8Length);
    buffer.Copy((TUint8*)string.UTF8Characters, string.UTF8Length);
    CnvUtfConverter::ConvertToUnicodeFromUtf8(aString, buffer);
    buffer.Close();
    }

HBufC16* VariantToHBufC16LC(NPVariant& aVariant)
    {
    if ( !NPVARIANT_IS_STRING(aVariant) ) return NULL; 
    NPString string = NPVARIANT_TO_STRING(aVariant);
    RBuf8 buffer;
    buffer.CreateL(string.UTF8Length);
    CleanupClosePushL( buffer ); 
    buffer.Copy((TUint8*)string.UTF8Characters, string.UTF8Length);
    HBufC16* ret = CnvUtfConverter::ConvertToUnicodeFromUtf8L( buffer );
    CleanupStack::PopAndDestroy(); // buffer ;
    CleanupStack::PushL( ret ); 
    return ret; 
    }

HBufC8* VariantToHBufC8LC(NPVariant& aVariant)
    {
    if ( !NPVARIANT_IS_STRING(aVariant) ) return NULL; 
    HBufC8* ret = HBufC8::NewLC( NPVARIANT_TO_STRING( aVariant ).UTF8Length );
    TPtr8 des( ret->Des() );
    VariantToDescriptor( aVariant, des ); 
    return ret; 
    }

}

