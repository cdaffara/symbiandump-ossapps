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
* Description: Library to handle resource loading when 
*              CCoeEnv is not available.
*
*/



#include <barsread.h>
#include <bautils.h>
#include <biditext.h>
#include <tulpanics.h>

#include "basepluginresourceloader.h"

// panic category constant
#ifdef _DEBUG
_LIT( KPanicCategory, "CResourceLoader" );
#endif

// CP1252 ellipsis character value.
#define KEllipsis       0x2026  // cp1252=133

// Key characters to format key strings.
const TText KKeyPrefix('%');
const TText KSubStringSeparator(0x0001);    // pipe ('|') character
const TText KDirNotFound( 0x0002 );

const TInt KExtraSpaceForMainStringDirMarker = 1;
const TInt KExtraSpaceForSubStringDirMarkers = 2;
const TText KLRMarker = 0x200E; 
const TText KRLMarker = 0x200F;

// KIntSize and KKeySize define maximum number of characters
// in input TInts and in keystring. KNumOfParams is maximum number of
// parameters in the resource string.
const TInt KIntSize(11); //Max 11 digits.
const TInt KNumOfParams(20); //Max 20 parameters

// Key strings for number and unicode data.
_LIT(KNumKeyBuf, "%N"); //Number data
_LIT(KStringKeyBuf, "%U"); //Unicode data

// KUnknownCount is used as default value for param count in FormatStringL
const TInt KUnknownCount(-99);

EXPORT_C CResourceLoader* CResourceLoader::NewL( const TDesC& aName )
    {
    CResourceLoader* temp = new( ELeave ) CResourceLoader();
    CleanupStack::PushL( temp );
    temp->ConstructL( aName );
    CleanupStack::Pop( temp );
    return temp;
    }

CResourceLoader::CResourceLoader()
    {
    }

void CResourceLoader::ConstructL( const TDesC& aFilename )
    {
    User::LeaveIfError( iFs.Connect() );
    TFileName aFile(aFilename);
    BaflUtils::NearestLanguageFile( iFs, aFile );
    iResFile.OpenL( iFs, aFile );
    iResFile.ConfirmSignatureL();
    }

EXPORT_C CResourceLoader::~CResourceLoader()
    {
    iResFile.Close();
    iFs.Close();
    }

EXPORT_C RFs& CResourceLoader::Fs() 
    {
    return iFs;
    }

EXPORT_C HBufC* CResourceLoader::LoadLC( TInt aResourceId )
    {
    TResourceReader reader;
    HBufC8* readBuffer = CreateResourceReaderLC( reader, aResourceId );
    TPtrC textdata = reader.ReadTPtrC();
    
    HBufC16* textBuffer = HBufC16::NewL( textdata.Length() );
    *textBuffer = textdata;
    CleanupStack::PopAndDestroy( readBuffer );
    CleanupStack::PushL( textBuffer );
    return textBuffer;
    }

EXPORT_C HBufC* CResourceLoader::Load2L( TInt aResourceId )
    {
    HBufC8* readBuffer = iResFile.AllocReadLC( aResourceId );
    const TPtrC16 ptrReadBuffer( ( TText16* ) readBuffer->Ptr(), ( readBuffer->Length()+1 )>>1 );
    HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );
    *textBuffer=ptrReadBuffer;
    CleanupStack::PopAndDestroy( readBuffer );
    return textBuffer;
    }

EXPORT_C HBufC* CResourceLoader::Load2LC( TInt aResourceId )
    {
    HBufC* retbuf = Load2L( aResourceId );
    CleanupStack::PushL( retbuf );
    return retbuf;
    }

EXPORT_C HBufC* CResourceLoader::Load2L( TInt aResourceId, const TDesC& aString )
    {
    HBufC* resbuf = Load2LC( aResourceId );
    TPtr retptr(resbuf->Des());
        
    // Get number of sub strings
    TInt count = GetSubStringCount( retptr ); 
    TBool marker(EFalse); 
    
    if (count >= 1)
        {
        HBufC* buffer = ResolveSubStringDirsL( retptr, count, &marker );
        CleanupStack::PushL(buffer);
        
        TBool found(EFalse);
        TBidiText::TDirectionality mainDir = DirectionalityL(*buffer , &found);
        
        //Formating the return string with FormatStringL.
        HBufC* retbuf = FormatStringL(*buffer, KStringKeyBuf, aString, mainDir);
        
        CleanupStack::PopAndDestroy(buffer);
        CleanupStack::PopAndDestroy(resbuf);
                        
        if (marker && retbuf->Length())
            {
            TPtr ptr = retbuf->Des();
            RemoveNoDirMarkers(ptr);
            } 
            
        __ASSERT_DEBUG(retbuf->Length(), 
                    User::Panic(KPanicCategory, EKeyStringNotFound));    
        return retbuf;
        }
     else
        {
        CleanupStack::PopAndDestroy(resbuf);
        HBufC* retbuf = HBufC::NewL(0); // return empty buffer
        __ASSERT_DEBUG(retbuf->Length(), 
                    User::Panic(KPanicCategory, EKeyStringNotFound));
        return retbuf;
        }
    }

EXPORT_C HBufC* CResourceLoader::Load2LC( TInt aResourceId, const TDesC& aString )
    {
    HBufC* retbuf = Load2L( aResourceId, aString );
    CleanupStack::PushL( retbuf );
    return retbuf;
    }

EXPORT_C HBufC* CResourceLoader::Load2L(TInt aResourceId, TInt aInt )
    {
    HBufC* resbuf = Load2LC( aResourceId );
    TPtr retptr = resbuf->Des();
    //
    //Converting input TInt to a descriptor. This way the size
    //of the return string can be controlled.
    //
    TBuf<KIntSize> intbuf;
    intbuf.Num(aInt);

    if (LanguageSpecificNumberConverter::IsConversionNeeded())
        LanguageSpecificNumberConverter::Convert(intbuf);
    //
    // Get number of sub strings
    TInt count = GetSubStringCount(retptr); 
    TBool marker(EFalse); 
    
    if (count >= 1)
        {
        HBufC* buffer = ResolveSubStringDirsL(retptr, count, &marker);
        CleanupStack::PushL(buffer);
        
        TBool found(EFalse);
        TBidiText::TDirectionality mainDir = DirectionalityL(*buffer , &found);
        
        //Adding int    
        HBufC* retbuf = FormatStringL(*buffer, KNumKeyBuf, intbuf, mainDir);
        
        CleanupStack::PopAndDestroy(buffer);
        CleanupStack::PopAndDestroy(resbuf);
                        
        if (marker && retbuf->Length())
            {
            TPtr ptr = retbuf->Des();
            RemoveNoDirMarkers(ptr);
            } 
        __ASSERT_DEBUG(retbuf->Length(), 
                    User::Panic(KPanicCategory, EKeyStringNotFound));
        return retbuf;
        }
     else
        {
        CleanupStack::PopAndDestroy(resbuf);
        HBufC* retbuf = HBufC::NewL(0); // return empty buffer
        __ASSERT_DEBUG(retbuf->Length(), 
                    User::Panic(KPanicCategory, EKeyStringNotFound));
        return retbuf;
        }
    }

EXPORT_C HBufC* CResourceLoader::Load2LC( TInt aResourceId, TInt aInt )
    {
    HBufC* retbuf = Load2L( aResourceId, aInt );
    CleanupStack::PushL( retbuf );
    return retbuf;
    }

EXPORT_C HBufC8* CResourceLoader::CreateResourceReaderLC(TResourceReader& aReader,TInt aResourceId) const
    {
    HBufC8* readBuffer = iResFile.AllocReadLC( aResourceId );
    aReader.SetBuffer( readBuffer );
    return readBuffer;
    }
        
/**
Counts the number of substrings (separated by KSubStringSeparators) in the text. 
Needed for correct memory allocations.
*/
TInt CResourceLoader::GetSubStringCount(const TDesC& aText)
    {
    TInt subCount = 0;
    TInt i = 0;

    while (i < aText.Length())
        {
        TPtrC remainder = aText.Right(aText.Length() - i);
        TInt nextKey = remainder.Locate(KSubStringSeparator);
        i += nextKey;

        // Always increase subcount, as a string without any separators counts as one substring.
        // However, if string length is zero, then there are no substrings.
        subCount++;

        if (nextKey != KErrNotFound && i < aText.Length() - 1)
            {
            i++; // skip separator
            }
        else
            break;
        }

    return subCount;
    }

HBufC* CResourceLoader::ResolveSubStringDirsL(TDes& aText, TInt aCount, TBool* aMarker)
    {   
    // Allocating heap buffer for return string.
    TInt destlength(aText.Length());
    destlength = destlength + (KExtraSpaceForMainStringDirMarker) * aCount;
    HBufC* retbuf = HBufC::NewLC(destlength);
    TPtr retptr(retbuf->Des());
    TInt freeSpace(destlength); 
    TInt i(0);
    TInt j(0);
    TBuf<1> subsMarker;
    subsMarker.Append(KSubStringSeparator);
    TInt count(aCount - 1);

    while (i  < aCount)
        {
        // Resolve sub string
        HBufC* buffer = ResolveSubStringL(aText, aMarker);
        TPtr ptr = buffer->Des();
        CleanupStack::PushL(buffer); 
        
        // Restore sub string separators
        if (freeSpace >= ptr.Length()) 
            {
            retptr.Insert(j, ptr);
            freeSpace -= ptr.Length();
            j += ptr.Length();
            if (freeSpace > KExtraSpaceForMainStringDirMarker && i < count)
                {
                retptr.Append(subsMarker);
                j ++;
                }
            }    
        CleanupStack::PopAndDestroy(buffer);  
        i++;                       
        } 
    
    retbuf = retbuf->ReAllocL(retptr.Length());
    CleanupStack::Pop(); //retbuf  
    return retbuf;
    }

TBidiText::TDirectionality CResourceLoader::DirectionalityL(const TDesC& aText, TBool* aFound)
    {
    // Resolve directionality of sub string
    HBufC* dirbuf = aText.AllocL();
    TPtr dirptr = dirbuf->Des();
    TBidiText::TDirectionality dir = ResolveDirectionality(dirptr, aFound);
    delete dirbuf;
    return dir;
    }
    
/**
Resolves directionality of the given source text.
Place-holder strings are removed so that they don't affect the result.
*/
TBidiText::TDirectionality CResourceLoader::ResolveDirectionality(TDes& aText, TBool* aFound)
    {
    TInt i = 0;

    // Remove place-holders from string so they don't affect directionality
    // length parameters e.g. "[29]" do not contain strongly directional
    // characters so can ignore them.
    while (i < aText.Length())
        {
        TPtrC remainder = aText.Right(aText.Length() - i);
        TInt nextKey = remainder.Locate(KKeyPrefix);
        i += nextKey;

        if (nextKey != KErrNotFound && i < aText.Length() - 1)
            {
            TInt lastCharInKey = i + 1;

            // skip possible key index
            TText t = aText[lastCharInKey];
            if (t >= '0' && t <= '9')
                {
                lastCharInKey++;

                if (lastCharInKey < aText.Length())
                    {
                    t = aText[lastCharInKey];
                    if (t >= '0' && t <= '9')
                        {
                        lastCharInKey++;
                        }
                    }
                }

            // lastCharInKey is now the index of 'N' or 'U' in the key

            if (lastCharInKey < aText.Length())
                {
                TText t = aText[lastCharInKey];
                if (t == 'U' || t == 'N')
                    {
                    // found key, delete it and continue loop to
                    // search for the next key
                    aText.Delete(i, lastCharInKey - i + 1);
                    }
                // This was not a proper key - check the remaining string
                else
                    {
                    i = lastCharInKey + 1;
                    }
                }
            // end of string encountered - stop
            else
                {
                break;
                }
            }
        // no key found - stop
        else
            {
            break;
            }
        }

    return TBidiText::TextDirectionality(aText, aFound);
    }

/**
Finds the keystring from the source string and replaces it with the
replacement string.
*/
HBufC* CResourceLoader::FormatStringL(const TDesC& aSource, const TDesC& aKey, const TDesC& aSubs,TBidiText::TDirectionality aDir)
    {
    TInt paramCount(KUnknownCount); // variable needed as it may be updated
    return FormatStringL(aSource, aKey, aSubs, aDir, paramCount, KUnknownCount);
    }

/**
Finds the keystring from the source string and replaces it with the
replacement string.
*/
HBufC* CResourceLoader::FormatStringL(const TDesC& aSource, const TDesC& aKey, const TDesC& aSubs, TBidiText::TDirectionality aDirectionality,
    TInt& aParamCount, TInt aSubCount)
    {
    if (aParamCount == KUnknownCount)
        aParamCount = GetParamCount(aSource);

    if (aSubCount == KUnknownCount)
        aSubCount = GetSubStringCount(aSource);

    // determine lenght of needed buffer 
    TInt sourcelength(aSource.Length()); 
    TInt keylength(aKey.Length());
    TInt subslength(aSubs.Length());
    TInt destlength = 0;
    if (subslength >= keylength)
        {
        destlength = sourcelength + ((subslength - keylength) * aParamCount);
        }
    else
        {
        destlength = sourcelength;
        }

    destlength += KExtraSpaceForMainStringDirMarker * aSubCount;
    destlength += KExtraSpaceForSubStringDirMarkers * aParamCount;
    //
    // Allocating heap buffer for return string.
    //

    HBufC* retbuf = HBufC::NewL(destlength);
    TPtr retptr(retbuf->Des());

    // Formating the resource string. Don't bother with format, 
    // if parameter count is not above zero
    if (aParamCount > 0)
        {
        aParamCount -= Formater(retptr, aSource, aKey, aSubs, aDirectionality);
        __ASSERT_DEBUG(aParamCount >= 0, User::Invariant());
        }

    //
    // If the key string wasn't found, retbuf is empty.
    //
    return retbuf; 
    }

/**
Finds the keystring from the source string and replaces it with the
replacement string. The formated string is stored in the destination
descriptor.
*/
TInt CResourceLoader::Formater(TDes& aDest, const TDesC& aSource, const TDesC& aKey, const TDesC& aSubs, TBidiText::TDirectionality aDirectionality)    
    {
    // substitute string must not contain KSubStringSeparator, 
    // or results will be unpredictable 
    __ASSERT_DEBUG(aSubs.Locate(KSubStringSeparator) == KErrNotFound, 
        User::Panic(KPanicCategory, EInvalidSubstitute));

    TInt keylength(aKey.Length());

    //aDest must be empty.
    aDest.Zero();

    // offset indicates end of last handled key in source
    TInt offset(0);

    // offset in destination string
    TInt desOffset(0);

    // Substring directionalities are adjusted after all changes are done.
    TBool checkSubstringDirectionalities(EFalse);

    // count is the position in the source from which the substring starts
    TInt count(0);

    // Replaced parameters count
    TInt replaceCount(0);

    while (count != KErrNotFound)
        {
        // desCount is the position of the substring starts in destination.
        TInt desCount(0);

        TPtrC remainder = aSource.Right(aSource.Length() - offset);
        count = remainder.Find(aKey);

        TInt maxSubLength = -1;
        if (count != KErrNotFound)
            {
            replaceCount++;
            desOffset += count;
            offset += count;
            count = offset;
            desCount = desOffset;

            // copy source to destination if first time
            if (aDest.Length() == 0)
                aDest.Append(aSource);

            // delete found key from destination
            aDest.Delete(desCount, keylength);

            offset += keylength; // increase offset by key length

            if (count + keylength < (aSource.Length()-1)) // aKey is not at the end of string
                {
                if (aSource[count+keylength] == '[') // Key includes max datalength
                    {
                    maxSubLength = 10*(aSource[count+keylength+1]-'0') 
                                   + (aSource[count+keylength+2]-'0');
                    aDest.Delete(desCount,4); // Length information stored->delete from descriptor
                    offset += 4; // increase offset by max sub length indicator
                    }
                }
         
            aDest.Insert(desCount, aSubs);
        
            desOffset = desCount + aSubs.Length();

            if (maxSubLength > 0 && aSubs.Length() > maxSubLength)
                {
                aDest.Delete(desCount+maxSubLength-1, aSubs.Length()+1-maxSubLength);     
                TText ellipsis(KEllipsis);
                aDest.Insert(desCount+maxSubLength-1, TPtrC(&ellipsis,1));
                desOffset = desCount + maxSubLength;
                }

            TBidiText::TDirectionality subsDir =
                TBidiText::TextDirectionality(aDest.Mid(desCount, desOffset - desCount));

            // If inserted string has different directionality,
            // insert directionality markers so that bidi algorithm works in a desired way.
            if (aDirectionality != subsDir)
                {
                checkSubstringDirectionalities = ETrue;

                TInt freeSpace = aDest.MaxLength() - aDest.Length();

                // Protect the directionality of the inserted string.
                if (freeSpace >= KExtraSpaceForSubStringDirMarkers)
                    {
                    TBuf<1> subsMarker;
                    subsMarker.Append(subsDir == TBidiText::ELeftToRight ?
                        KLRMarker : KRLMarker);

                    aDest.Insert(desOffset, subsMarker);
                    aDest.Insert(desCount, subsMarker);
                    desOffset += KExtraSpaceForSubStringDirMarkers;
                    }
                }
            }
        }

    // Adjust substring directionality markers if necessary
    // and if there is enough room in destination string
    if (checkSubstringDirectionalities)
        {
        TText mainMarker = (aDirectionality == TBidiText::ELeftToRight ? 
            KLRMarker : KRLMarker);

        TInt freeSpace = aDest.MaxLength() - aDest.Length();

        // If not already done, protect the directionality of the original string
        // and all of the KSubStringSeparator separated substrings.
        if (freeSpace > 0 
            && aDest.Length()
            && aDest[0] != mainMarker 
            && aDest[0] != KSubStringSeparator
            && aDest[0] != KDirNotFound)  
            {
            aDest.Insert(0, TPtrC(&mainMarker, 1));
            freeSpace--;
            }

        // Find and protect KSubStringSeparator separated substrings.
        // Go through string backwards so that any changes will not affect indexes 
        // that are not yet checked.
        TInt j(aDest.Length()-1);
        while (freeSpace > 0 && j >= 0) 
            {
            if (aDest[j] == KSubStringSeparator && j < (aDest.Length() - 1) 
                && aDest[j+1] != mainMarker && aDest[j+1] != KDirNotFound)
                {
                aDest.Insert(j+1, TPtrC(&mainMarker, 1));
                freeSpace--;
                }
            j--;
            }
        }

    return replaceCount;
    }

/**
Counts the number of parameters in the text. 
Needed for correct memory allocations.
*/
TInt CResourceLoader::GetParamCount(const TDesC& aText, TInt aIndex)
    {
    TInt paramCount(0);
    TInt i(0);
    TBool singleIndex((aIndex < 0) || (aIndex > KNumOfParams) ? EFalse : ETrue);

    while (i < aText.Length())
        {
        TPtrC remainder = aText.Right(aText.Length() - i);
        TInt nextKey = remainder.Locate(KKeyPrefix);
        i += nextKey;

        if (nextKey != KErrNotFound && i < aText.Length() - 1)
            {
            TInt lastCharInKey = i + 1;

            // skip possible key index
            TText t = aText[lastCharInKey];
            TInt foundIndex(-1);
            
            if (t >= '0' && t <= '9')
                {
                lastCharInKey++;
                foundIndex = t - '0';

                if (lastCharInKey < aText.Length())
                    {
                    t = aText[lastCharInKey];
                    if (t >= '0' && t <= '9')
                        {
                        foundIndex *= 10;
                        foundIndex += t - '0';
                        lastCharInKey++;
                        }
                    }
                }

            // lastCharInKey is now the index of 'N' or 'U' in the key

            if (lastCharInKey < aText.Length())
                {
                // Only count parameter, if index matches
                if (!singleIndex || (foundIndex == aIndex))
                    {
                    TText t = aText[lastCharInKey];
                    if (t == 'U' || t == 'N')
                        {
                        // found legit key, count it
                        paramCount++;
                        // continue search after index
                        i = lastCharInKey + 1;
                        }
                    else if (t == '%')
                        i = lastCharInKey;
                    else    // continue search after index
                        i = lastCharInKey + 1;
                    }
                else    // continue search after index
                    i = lastCharInKey + 1;
                }
            else    // end of string encountered - stop
                break;
            }
        else     // no key found - stop
            break;
        }

    return paramCount;
    }

/**
Resolves sub string and directionality of the sub string.
Adds no dir marker if directionality of the string not found.
*/
HBufC* CResourceLoader::ResolveSubStringL(TDes& aText, TBool* aMarker)
    {
    // Allocating heap buffer for return string.
    TInt destlength(aText.Length());
    HBufC* retbuf = HBufC::NewLC(destlength + 1); // no dir marker
    TPtr retptr(retbuf->Des());
    
    TBuf<1> marker;
    marker.Append(KDirNotFound);
        
    TPtrC remainder = aText.Right(aText.Length());
    TInt nextKey = remainder.Locate(KSubStringSeparator);
    
    if (nextKey == 0)
        {
        remainder.Set(remainder.Mid(1));
        nextKey = remainder.Locate(KSubStringSeparator);
        if (nextKey != KErrNotFound)
            {
            retptr.Insert(0, aText.Mid(1, nextKey));           
            // Remove string from aText
            aText.Delete(0, nextKey + 1);
            }
        else
            {
            TInt length = aText.Length();
            retptr.Insert(0, aText.Mid(1, length - 1));
            // Remove string from aText
            aText.Delete(0, length);
            }
        }
    else if (nextKey == KErrNotFound)
        {
        retptr.Insert(0, aText); 
        // Remove string from aText
        aText.Delete(0, aText.Length());
        }
    else
        {
        retptr.Insert(0, aText.Mid(0, nextKey));
        // Remove string from aText
        aText.Delete(0, nextKey);
        }
     
    // Resolve directionality of sub string
    HBufC* dirbuf = retbuf->AllocL();
    TPtr dirptr = dirbuf->Des();
    TBool found(EFalse);
    TBidiText::TDirectionality mainDir = ResolveDirectionality(dirptr, &found);
    delete dirbuf;
    
    if (!found)
        {
        retptr.Insert(0, marker);
        *aMarker = ETrue;
        }
    else
        {
        *aMarker = EFalse;
        }
        
    retbuf = retbuf->ReAllocL(retptr.Length());
    CleanupStack::Pop(); //retbuf
    // If the key string wasn't found, retbuf is empty.
    return retbuf;     
    }
    
/**
Removes no dir markers from source text.
*/
void CResourceLoader::RemoveNoDirMarkers(TDes& aText)
    {
    TInt nextkey(0);
    while (nextkey < aText.Length())
        {
        nextkey = aText.Locate(KDirNotFound);
        if (nextkey != KErrNotFound)
            {
            aText.Delete(nextkey, 1);
            nextkey++;    
            }
        else
            {
            break;
            }     
        }
    }
    
// ============================ MEMBER FUNCTIONS ===============================

static TChar NumberToBase(const TChar& aCharacter )
    {
    const TDigitType d[] = { EDigitTypeWestern, 
                       EDigitTypeArabicIndic, 
                       EDigitTypeEasternArabicIndic, 
                       EDigitTypeDevanagari, 
                       EDigitTypeThai };
    
    const TInt num = sizeof( d ) / sizeof( d[0] );
    
    TInt i = 0;
    while( i < num )
        {
        if (aCharacter > TChar( d[i] ) && aCharacter < TChar( d[i]+10 )) 
            return d[i];
        
        i++;
        }
        
    return aCharacter;
    }

/**
This routine is used to convert between European digits and 
Arabic-Indic, Eastern Arabic-Indic, Devanagari or Thai digits 
based on existing digit type setting. 

@param aDes  Parameter to change
*/
void CResourceLoader::LanguageSpecificNumberConverter::Convert( TDes &aDes )
    {   
    TLocale locale;
    locale.Refresh();
    const TDigitType digitType = locale.DigitType();
    TChar toArea = 0x030;
    switch( digitType )
        {
        case EDigitTypeWestern:
        case EDigitTypeArabicIndic:
        case EDigitTypeEasternArabicIndic:
        case EDigitTypeDevanagari:
        case EDigitTypeThai:
            toArea = digitType;
            break;
        case EDigitTypeUnknown:
        case EDigitTypeAllTypes:
            return;
        }
    
    const TInt length = aDes.Length();
    for( TInt i = 0; i < length; i++ )
        {
        TChar character = aDes[i];
        TChar fromArea = NumberToBase( character );
        TChar::TBdCategory cat = character.GetBdCategory();
        switch( cat )
            {
            case TChar::EArabicNumber:
            case TChar::EEuropeanNumber:
                character += toArea;
                character -= fromArea;
                aDes[i] = TUint16( character );
                break;
            default: 
                break;
            }
        }
    }

/**
This routine is used to convert Arabic-Indic, Eastern Arabic-Indic
or Devanagari digits to European digits. 

@param aDes Parameter to change
*/
void CResourceLoader::LanguageSpecificNumberConverter::ConvertToWesternNumbers( TDes &aDes )
    {   
    const TChar toArea = 0x030;
    const TInt KLastDevanagariDigit = 0x96F;
    const TInt KFirstDevanagariDigit = 0x966;
    const TInt length = aDes.Length();

    for( TInt i=0; i<length; i++ )
        {
        TChar character = aDes[i];
        TChar fromArea = NumberToBase( character );
        TChar::TBdCategory cat = character.GetBdCategory();

        if ( cat == TChar::EArabicNumber || cat == TChar::EEuropeanNumber ||
           ( KFirstDevanagariDigit <= character && character <= KLastDevanagariDigit ) )
            {
            character += toArea;
            character -= fromArea;
            aDes[i] = TUint16( character );    
            }        
        }
    }

/**
This routine is used to check if conversion of digits is needed.
Conversion is needed if user language is   
- Arabic, Urdu or Farsi and if digit type is Arabic-Indic
- Urdu or Farsi and digit type is Eastern Arabic_indic
- Hindi and digit type is Devanagari. 

@return  ETrue if conversion is needed, EFalse if not
*/
TBool CResourceLoader::LanguageSpecificNumberConverter::IsConversionNeeded()
    {
    TLocale locale;
    locale.Refresh();
    const TLanguage language = User::Language();
    const TDigitType digitType = locale.DigitType();
    
    if ( ( ( language == ELangArabic || language == ELangUrdu || language == ELangFarsi ) &&
         digitType == EDigitTypeArabicIndic ) 
         || ( ( language == ELangUrdu || language == ELangFarsi ) &&
         digitType == EDigitTypeEasternArabicIndic )
         || ( language == ELangHindi && digitType == EDigitTypeDevanagari )
        )
        {
        return ETrue;
        }

    return EFalse;
    }

