/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgTextSearcher.
*
*/


// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

const TUint KKeywordsArrayGranularity = 8;
const TInt KUnicodeConversionBufferSize = 100;

// ================= MEMBER FUNCTIONS ====================

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::NewL()
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CIpsPlgTextSearcher* CIpsPlgTextSearcher::NewL( 
    MIpsPlgTextSearcherObserver& aObserver )
    {
    FUNC_LOG;
    CIpsPlgTextSearcher* self = CIpsPlgTextSearcher::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::NewLC()
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CIpsPlgTextSearcher* CIpsPlgTextSearcher::NewLC( 
    MIpsPlgTextSearcherObserver& aObserver )
    {
    FUNC_LOG;
    CIpsPlgTextSearcher* self = 
        new ( ELeave ) CIpsPlgTextSearcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::CIpsPlgTextSearcher()
// Performs the first phase of two phase construction.
// ----------------------------------------------------------------------------
//
CIpsPlgTextSearcher::CIpsPlgTextSearcher( 
    MIpsPlgTextSearcherObserver& aObserver )
    : iObserver( aObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::ConstructL()
// Performs the second phase construction.
// ----------------------------------------------------------------------------
//
void CIpsPlgTextSearcher::ConstructL()
    {
    FUNC_LOG;
    iSearchKeywords8 = 
        new ( ELeave ) CDesC8ArrayFlat( KKeywordsArrayGranularity );
    User::LeaveIfError ( iRFs.Connect() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::~CIpsPlgTextSearcher()
// Destructor
// ----------------------------------------------------------------------------
//
CIpsPlgTextSearcher::~CIpsPlgTextSearcher()
    {
    FUNC_LOG;
    iKeywordSearchStatusArray.Close();
    
    if( iSearchKeywords8 )
        {
        iSearchKeywords8->Reset();
        delete iSearchKeywords8;
        }
    
    iRFs.Close();
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::SetParametersL()
// Sets the search parameters for the search.
// ----------------------------------------------------------------------------
//
void CIpsPlgTextSearcher::SetParametersL(  
    const CDesCArray& aKeywords, 
    TIpsPlgCriteriaOperation aOperator, 
    TBool aCaseSensitive, 
    TInt aSearchResultRecommendedSnippetLength )
    {
    FUNC_LOG;
    iHaveParameters = ETrue;
    
    iSearchKeywords = &aKeywords;
    
    iOperator = aOperator;
    iCaseSensitive = aCaseSensitive;
    iSearchResultRecommendedSnippetLength = 
        aSearchResultRecommendedSnippetLength;

    iKeywordSearchStatusArray.Reset();
    TInt count( aKeywords.Count() );
    for ( TInt i(0); i < count; i++ )
        {
        TIpsPlgKeywordSearchStatus status;
        status.iFound = EFalse;
        status.iFoundAsNthWord = KErrNotFound;
        
        iKeywordSearchStatusArray.AppendL( status );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::Cleanup()
// Cleans up internal data.  Must be called before each new search is started.
//  Does not clean up the parameters set via SetParametersL.
// ----------------------------------------------------------------------------
//
void CIpsPlgTextSearcher::Cleanup()
    {
    FUNC_LOG;
    TInt count( iKeywordSearchStatusArray.Count() );
    for ( TInt i(0); i < count; i++ )
        {    
        iKeywordSearchStatusArray[i].iFound = EFalse;
        iKeywordSearchStatusArray[i].iFoundAsNthWord = KErrNotFound;
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::SearchL()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgTextSearcher::SearchL( const TDesC& aToBeSearchedDes )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iHaveParameters, 
        User::Panic( KIpsPlgPanicCategory, EIpsPlgNoParameters ) );
    
    // when aToBeSearchedDes has no data, it can match *
    if( !aToBeSearchedDes.Length() )
        {
        return EFalse;
        }

    iFirstMatchKeywordCharPos = KErrNotFound;
    
    TBool found( DoStringCompareWithKeywordsL( aToBeSearchedDes, *iSearchKeywords,
        iKeywordSearchStatusArray, iOperator, iCaseSensitive ) );
        
    if ( found )
        {
        // Parameters are not used in function
        iObserver.HitL( KNullDesC, KErrNone, EFalse, EFalse );
        }
    return found;
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::SearchL()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgTextSearcher::SearchL( const TDesC8& aToBeSearchedDes )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iHaveParameters, 
        User::Panic( KIpsPlgPanicCategory, EIpsPlgNoParameters ) );
    
    // when aToBeSearchedDes has no data, it can match *
    if( !aToBeSearchedDes.Length() )
        {
        return EFalse;
        }
        
    // Create 8 bit versions from the keywords for better efficiency
    iSearchKeywords8->Reset();    
    TInt count( iSearchKeywords->Count() );
    for ( TInt i(0); i < count; i++ )
        {
        HBufC8* keyword8Bit = ConvertFromUnicodeL( iRFs,
                     iSearchKeywords->MdcaPoint( i ), 
                     KCharacterSetIdentifierUtf8 );
        CleanupStack::PushL( keyword8Bit );
        iSearchKeywords8->AppendL( *keyword8Bit );
        CleanupStack::PopAndDestroy( keyword8Bit );
        }

    iFirstMatchKeywordCharPos = KErrNotFound;
    
    TBool found( DoStringCompareWithKeywordsL( aToBeSearchedDes, *iSearchKeywords8,
        iKeywordSearchStatusArray, iOperator, iCaseSensitive ) );

    if ( found ) 
        {
        // Parameters are not used in function
        iObserver.HitL( KNullDesC, KErrNone, EFalse, EFalse );
        }
    return found;
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::DoStringCompareWithKeywords()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgTextSearcher::DoStringCompareWithKeywordsL( 
    const TDesC& aToBeSearchedDes,
    const CDesCArray& aKeywords, 
    RArray<TIpsPlgKeywordSearchStatus>& aKeywordSearchStatusArray,
    TIpsPlgCriteriaOperation aOperator, 
    TBool aCaseSensitive )
    {
    FUNC_LOG;
    // Compare the string will all the keywords
    TInt count( aKeywords.Count() );
    if ( count != aKeywordSearchStatusArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    for ( TInt i(0); i < count; i++ )
        {
        if ( !aKeywordSearchStatusArray[i].iFound )    // keyword not yet found
            {
            TInt match;
            if ( aCaseSensitive )
                {                
                match = aToBeSearchedDes.Match( aKeywords[i] );
                }
            else
                {
                match = aToBeSearchedDes.MatchC( aKeywords[i] );
                }

            // KErrNotFound indicates that no match was found
            if ( match != KErrNotFound ) 
                {
                aKeywordSearchStatusArray[i].iFound = ETrue;
                // The 1st found keyword will be also the snippet
                if ( iFirstMatchKeywordCharPos == KErrNotFound  )
                    {
  
                    if ( match == aToBeSearchedDes.Length() ) 
                        {
                        iFirstMatchKeywordCharPos = 0;
                        }
                    else
                        {                        
                        iFirstMatchKeywordCharPos = match;
                        }
                     
                    }

                if ( aOperator == EIpsPlgCriteriaOperationOR )
                    {
                    // As soon as one keyword is found, the OR search can stop
                    return ETrue; 
                    }
                }
            }
        }

    // Check if there is still need to continue searching.
    TBool keywordsFound;
    switch ( aOperator )
        {
        case EIpsPlgCriteriaOperationAND:
            keywordsFound = ETrue;
            break;

        case EIpsPlgCriteriaOperationOR:
            keywordsFound = EFalse;
            break;

        default:
            keywordsFound = ETrue;    // to avoid warning
            break;
        }

    // For AND search, all keywords must be found, thus if 1 is not found -> 
    // No hit
    if( aOperator == EIpsPlgCriteriaOperationAND )
        {
        for ( TInt j(0); j < count; j++ )
            {
            if ( !aKeywordSearchStatusArray[j].iFound )
                {
                return EFalse;
                }
            }
        }
    return keywordsFound;
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::DoStringCompareWithKeywordsL()
// ----------------------------------------------------------------------------
//
TBool CIpsPlgTextSearcher::DoStringCompareWithKeywordsL( 
    const TDesC8& aToBeSearchedDes,
    const CDesC8Array& aKeywords, 
    RArray<TIpsPlgKeywordSearchStatus>& aKeywordSearchStatusArray,
    TIpsPlgCriteriaOperation aOperator, 
    TBool aCaseSensitive )
    {
    FUNC_LOG;
    TInt count( aKeywords.Count() );
    if ( count != aKeywordSearchStatusArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    // Compare the string will all the keywords
    for ( TInt i=0; i<aKeywords.Count(); i++ )
        {
        if ( !aKeywordSearchStatusArray[i].iFound ) // keyword not yet found
            {
            TInt match;
            if ( aCaseSensitive )
                {                
                match = aToBeSearchedDes.Match( aKeywords[i] );
                }
            else
                {
                match = aToBeSearchedDes.MatchC( aKeywords[i] );
                }

            // KErrNotFound indicates that no match was found
            if ( match != KErrNotFound )    
                {
                aKeywordSearchStatusArray[i].iFound = ETrue;
                // The 1st found keyword will be also the snippet
                if ( iFirstMatchKeywordCharPos == KErrNotFound  )
                    {
                    iFirstMatchKeywordCharPos = match;
                    }

                if ( aOperator == EIpsPlgCriteriaOperationOR )
                    {
                    // As soon as one keyword is found, the OR search can stop
                    return ETrue;  
                    }
                }
            }
        }

    // Check if there is still need to continue searching.
    TBool keywordsFound;
    switch ( aOperator )
        {
        case EIpsPlgCriteriaOperationAND:
            keywordsFound = ETrue;
            break;

        case EIpsPlgCriteriaOperationOR:
            keywordsFound = EFalse;
            break;

        default:
            keywordsFound = ETrue;    // to avoid warning
            break;
        }

    // For AND search, all keywords must be found, thus if 1 is not found -> 
    // No hit
    if( aOperator == EIpsPlgCriteriaOperationAND )
        {
        for ( TInt j(0); j < count; j++ )
            {
            if ( !aKeywordSearchStatusArray[j].iFound )
                {
                return EFalse;
                }
            }
        }
    return keywordsFound;
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::CreateSnippetLC()
// Creates the snippet from the 1st found keyword hit.  16 bit version.
// ----------------------------------------------------------------------------
//
HBufC* CIpsPlgTextSearcher::CreateSnippetLC( 
    const TDesC& aToBeSearchedDes, 
    TInt& aSnippetCharPos,
    TBool& aStartIncomplete, 
    TBool& aEndIncomplete ) const
    {
    FUNC_LOG;
    TInt halfSnippetLen ( iSearchResultRecommendedSnippetLength / 2 );
    
    TInt firstChar ( iFirstMatchKeywordCharPos - halfSnippetLen );
    // if firstChar is negative, then we can give more space to lastChar
    TInt lastChar ( 
        iFirstMatchKeywordCharPos + halfSnippetLen - Min( firstChar, 0) );
    
    TInt searchDesLastChar ( aToBeSearchedDes.Length() );
    
    // if lastChar is go out of the total length, put the firstChar head toward
    firstChar = 
        Max( 0, firstChar - Max( 0, ( lastChar - searchDesLastChar ) ) ) ;    
    
    aSnippetCharPos = halfSnippetLen + ( lastChar - searchDesLastChar );
    
    aStartIncomplete = ETrue;
    if ( firstChar <= 0 )
        {
        firstChar = 0;
        aStartIncomplete = EFalse;
        aSnippetCharPos = iFirstMatchKeywordCharPos;
        }

    aEndIncomplete = ETrue;
    if ( lastChar >= searchDesLastChar )
        {        
        lastChar = searchDesLastChar;                    
        aEndIncomplete = EFalse;
        }            
    
    return aToBeSearchedDes.Mid( 
        firstChar, 
        Min( lastChar - firstChar, searchDesLastChar - firstChar ) ).AllocLC();
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::CreateSnippetLC()
// Creates the snippet from the 1st found keyword hit.  8 bit version.
// ----------------------------------------------------------------------------
//
HBufC* CIpsPlgTextSearcher::CreateSnippetLC( 
    const TDesC8& aToBeSearchedDes, 
    TInt& aSnippetCharPos,
    TBool& aStartIncomplete, 
    TBool& aEndIncomplete )
    {
    FUNC_LOG;
    TInt halfSnippetLen ( iSearchResultRecommendedSnippetLength / 2 );
    
    TInt firstChar ( iFirstMatchKeywordCharPos - halfSnippetLen );
    TInt lastChar ( 
        iFirstMatchKeywordCharPos + halfSnippetLen - Min( firstChar, 0) );
    
    TInt searchDesLastChar ( aToBeSearchedDes.Length() );

    firstChar = 
        Max( 0, firstChar - Max( 0, ( lastChar - searchDesLastChar ) ) ) ;    
    
    aSnippetCharPos = halfSnippetLen + ( lastChar - searchDesLastChar );

    aStartIncomplete = ETrue;
    if ( firstChar <= 0 )
        {
        firstChar = 0;
        aStartIncomplete = EFalse;
        aSnippetCharPos = iFirstMatchKeywordCharPos;
        }

    aEndIncomplete = ETrue;
    if ( lastChar >= searchDesLastChar )
        {        
        lastChar = searchDesLastChar;                    
        aEndIncomplete = EFalse;
        }    
        
    HBufC8* snippet8 = aToBeSearchedDes.Mid( firstChar, 
        Min( lastChar - firstChar, searchDesLastChar - firstChar ) ).AllocLC();
    HBufC* snippet= ConvertToUnicodeL( iRFs, *snippet8, 
        KCharacterSetIdentifierUtf8 );
    CleanupStack::PopAndDestroy( snippet8 );
    CleanupStack::PushL( snippet );
    return snippet;
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::ConvertFromUnicodeL()
// Makes a conversion from unicode
// ----------------------------------------------------------------------------
HBufC8* CIpsPlgTextSearcher::ConvertFromUnicodeL(
    RFs& aFs, 
    const TDesC16& aUnicodeSource,
    TUint aTargetEncoding )
    {
    FUNC_LOG;
    CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();
    if ( converter->PrepareToConvertToOrFromL(aTargetEncoding, aFs) 
         != CCnvCharacterSetConverter::EAvailable )
        {
        User::Leave( KErrNotSupported );
        }

    TBuf8<KUnicodeConversionBufferSize> temp8Buffer;
    HBufC8*     target = NULL;
    TPtrC       source16Ptr( aUnicodeSource );

    for( ;; ) // conversion loop
        {
        TInt state = CCnvCharacterSetConverter::KStateDefault;
        TInt returnValue = 
            converter->ConvertFromUnicode( temp8Buffer, source16Ptr, state );
        if ( returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput )
            {
            User::Leave( KErrCorrupt );
            }
        else
            {
            if ( returnValue < 0 ) // future-proof against "TError" expanding
                {
                User::Leave( KErrGeneral );
                }
            }

        if ( !target )
            {
            target = temp8Buffer.AllocLC();
            }
        else
            {
            HBufC8* tmp = 
                target->ReAllocL( target->Length() + temp8Buffer.Length() );
            CleanupStack::Pop( target );
            target = tmp;
            CleanupStack::PushL( target );
            target->Des().Append( temp8Buffer );
            }

        if ( returnValue == 0 ) // All is converted without Errors
            {
            break;
            }

        // There is "returnValue" bytes not converted yet
        source16Ptr.Set( source16Ptr.Right( returnValue ) );
        }
    CleanupStack::Pop( target );  // Ownership is transferred
    CleanupStack::PopAndDestroy( converter );
    return target;
    }

// ----------------------------------------------------------------------------
// CIpsPlgTextSearcher::ConvertToUnicodeL()
// Makes a conversion into unicode
// ----------------------------------------------------------------------------
HBufC16* CIpsPlgTextSearcher::ConvertToUnicodeL( 
    RFs& aFs, 
    const TDesC8& aSource,
    TUint aSourceEncoding )
    {
    FUNC_LOG;
    CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();
    if ( converter->PrepareToConvertToOrFromL( aSourceEncoding, aFs ) 
         != CCnvCharacterSetConverter::EAvailable )
        {
        User::Leave( KErrNotSupported );
        }

    TBuf<KUnicodeConversionBufferSize> temp16Buffer;
    HBufC*  unicode = NULL;
    TPtrC8  source8Ptr( aSource );

    for( ;; ) // conversion loop
        {
        TInt state = CCnvCharacterSetConverter::KStateDefault;

        TInt returnValue = 
            converter->ConvertToUnicode( temp16Buffer, source8Ptr, state );
        if ( returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput )
            {
            User::Leave( KErrCorrupt );
            }
        else
            {
            if ( returnValue < 0 ) // future-proof against "TError" expanding
                {
                User::Leave( KErrGeneral );
                }
            }

        if ( !unicode )
            {
            unicode = temp16Buffer.AllocLC();
            }
        else
            {
            HBufC* tmp = 
                unicode->ReAllocL( unicode->Length() + temp16Buffer.Length() );
            CleanupStack::Pop( unicode );
            unicode = tmp;
            CleanupStack::PushL( unicode );
            unicode->Des().Append( temp16Buffer );
            }

        if ( returnValue == 0 ) // All is converted without Errors
            {
            break;
            }

        // There is "returnValue" bytes not converted yet
        source8Ptr.Set( source8Ptr.Right( returnValue ) );
        }

    CleanupStack::Pop( unicode ); // Ownership is transferred
    CleanupStack::PopAndDestroy( converter );
    return unicode;
    }
    





