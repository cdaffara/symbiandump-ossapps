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
* Description: This file defines classes MIpsPlgTextSearcherObserver, TIpsPlgKeywordSearchStatus, CIpsPlgTextSearcher
*
*/


#ifndef IPSPLGTEXTSEARCHER_H
#define IPSPLGTEXTSEARCHER_H

// INCLUDE FILES
// Removed for 3.2
//#include <IdsCommonDefines.h>
// Removed for 3.2
//#include <KeywordSearchStatus.h>

// FORWARD DECLARATIONS

class MIpsPlgTextSearcherObserver
    {
    public:
        /**
        * Called from the text searcher when all search criteria are met.
        *
        * @param    aSnippet         The snippet
        * @param    aSnippetCharPos  The character position of the keyword 
        *                            match within the snippet
        * @param    aStartIncomplete Indicates that the snippet start is 
        *                            incomplete
        * @param    aEndIncomplete   Indicates that the snippet end is 
        *                            incomplete
        */
        virtual void HitL( 
            const TDesC& aSnippet, 
            TInt aSnippetCharPos, 
            TBool aStartIncomplete,
            TBool aEndIncomplete ) = 0;
    };
/**
* Original class TKeywordSearchStatus
* from  <KeywordSearchStatus.h>. Replaced
* for 3.2 version
* Search status belonging to a single search keyword.
*/
class TIpsPlgKeywordSearchStatus
	{
	public:
		// Boolean indicating whether this keyword has been found
		TBool		iFound;
		
		// Integer indicating in what wordposition this keyword was found
		TInt		iFoundAsNthWord;
	};
	
/**
* Original enum used TIdsCriteriaOperation from <IdsCommonDefines.h>
* Replaced for 3.2 version
*/	
// Binary operators for the search criteria.
enum TIpsPlgCriteriaOperation
    {
    EIpsPlgCriteriaOperationAND,
    EIpsPlgCriteriaOperationOR,
    EIpsPlgCriteriaOperationNOT
    };

/**
* Class handling the search of several keywords in a block of text.
* A hit is informed via the MIpsPlgTextSearcherObserver.
*
* Data/text can be supplied in steps and does not need to be known in advance.
*/
NONSHARABLE_CLASS( CIpsPlgTextSearcher ) : public CBase
    {
    public: // Constructors and destructor
        /** 
        * Symbian OS 2 phased constructor.
        *
        * @param    aObserver    Text searcher's observer
        *
        * @return    A pointer to the created instance of CIpsPlgTextSearcher.
        */
        static CIpsPlgTextSearcher* NewL( 
            MIpsPlgTextSearcherObserver& aObserver );

        /** 
        * Symbian OS 2 phased constructor
        *
        * @param    aObserver    Text searcher's observer
        *
        * @return    A pointer to the created instance of CIpsPlgTextSearcher.
        */
        static CIpsPlgTextSearcher* NewLC( 
            MIpsPlgTextSearcherObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CIpsPlgTextSearcher();

    public:    // New functions
        /**
        * Cleans up internal data.  Must be called before each new search 
        * is started. Does not clean up the parameters set via SetParametersL.
        * 
        * When same search parameters are needed with different search text, 
        * call Cleanup but not SetParametersL.
        */
        void Cleanup();

        /**
        * Sets the search parameters for the search.
        *
        * @param    aKeywords      Keywords for the search.
        * @param    aOperator      Determines the operation for the search 
        *                          (AND, OR, ...)
        * @param    aCaseSensitive Flag indicating case sensitivity
        * @param    aRecommendedSnippetLength Recommended snippet length.
        */
        void SetParametersL( 
            const CDesCArray& aKeywords, 
            TIpsPlgCriteriaOperation aOperator, 
            TBool aCaseSensitive,
            TInt aRecommendedSnippetLength );
        
        /**
        * Starts the text search with the previous supplied parameters set 
        * by SetParametersL. 16 bit version.
        *
        * @param    aToBeSearchedDes    16 bit descriptor to be searched.
        * @return ETrue when a hit was found.  EFalse otherwise.
        */
        TBool SearchL( const TDesC& aToBeSearchedDes );
        
        /**
        * Starts the text search with the previous supplied parameters set by 
        * SetParametersL. 8 bit version.
        *
        * @param    aToBeSearchedDes    8 bit descriptor to be searched.
        * @return ETrue when a hit was found.  EFalse otherwise.
        */
        TBool SearchL( const TDesC8& aToBeSearchedDes );

    private:    // New functions
        /**
        * Compares the to be searched string with the list of keywords.  
        * 16 bit version.
        *
        * @param aToBeSearchedDes          Text to be searched.
        * @param aKeywords                 Array of keywords to be matched.
        * @param aKeywordSearchStatusArray Array storing the search result 
        *                                  for each individual search keyword
        * @param aOperator                 Determines the operation for 
        *                                  the search (AND, OR, ...)
        * @param aCaseSensitive            Flag indicating case sensitivity
        *
        * @return ETrue when a hit was found.  EFalse otherwise.
        */
        TBool DoStringCompareWithKeywordsL( 
            const TDesC& aToBeSearchedDes,
            const CDesCArray& aKeywords, 
            RArray<TIpsPlgKeywordSearchStatus>& aKeywordSearchStatusArray,
            TIpsPlgCriteriaOperation aOperator, 
            TBool aCaseSensitive );

        /**
        * Compares the to be searched string with the list of keywords.  
        * 8 bit version.
        *
        * @param aToBeSearchedDes          Text to be searched.
        * @param aKeywords                 Array of keywords to be matched.
        * @param aKeywordSearchStatusArray Array storing the search result 
        *                                  for each individual search keyword
        * @param aOperator                 Determines the operation for 
        *                                  the search (AND, OR, ...)
        * @param aCaseSensitive            Flag indicating case sensitivity
        *
        * @return ETrue when a hit was found.  EFalse otherwise.
        */
        TBool DoStringCompareWithKeywordsL( 
            const TDesC8& aToBeSearchedDes,
            const CDesC8Array& aKeywords, 
            RArray<TIpsPlgKeywordSearchStatus>& aKeywordSearchStatusArray,
            TIpsPlgCriteriaOperation aOperator, 
            TBool aCaseSensitive );

        /**
        * Creates the snippet from the 1st found keyword.  16 bit version.
        * 
        * @param aToBeSearchedDes Text that was searched.
        * @param aSnippetCharPos  The character position of the keyword match 
        *                         within the snippet
        * @param aStartIncomplete Indicates that the snippet start is 
        *                         incomplete
        * @param aEndIncomplete   Indicates that the snippet end is incomplete
        *
        * @return A pointer to the newly created snippet.
        */
        HBufC* CreateSnippetLC( 
            const TDesC& aToBeSearchedDes, 
            TInt& aSnippetCharPos,
            TBool& aStartIncomplete, 
            TBool& aEndIncomplete ) const;

        /**
        * Creates the snippet from the 1st found keyword.  8 bit version.
        * 
        * @param aToBeSearchedDes Text that was searched.
        * @param ASnippetCharPos  The character position of the keyword match 
        *                         within the snippet
        * @param aStartIncomplete Indicates that the snippet start is 
        *                         incomplete
        * @param aEndIncomplete   Indicates that the snippet end is incomplete
        * @return A pointer to the newly created snippet.
        */
        HBufC* CreateSnippetLC( 
            const TDesC8& aToBeSearchedDes, 
            TInt& aSnippetCharPos,
            TBool& aStartIncomplete, 
            TBool& aEndIncomplete );

        /**
         * Makes a conversion from unicode
         *
         * @param aFs             Reference to a file session 
         * @param aUnicodeSource  Source text to be converted
         * @param aTargetEncoding The UID of the non-Unicode character set from 
         *                        or to which to convert.
         * @return   Converted non-unicode descriptor
         */
        HBufC8* ConvertFromUnicodeL(
            RFs& aFs, 
            const TDesC16& aUnicodeSource,
            TUint aTargetEncoding );

        /**
         * Makes a conversion into unicode
         *
         * @param aFs             Reference to a file session.
         * @param aSource         Source text to be converted.
         * @param aSourceEncoding The UID of the non-Unicode character set 
         *                        from or to which to convert.
         * @return   Converted unicode descriptor
         */
        HBufC16* ConvertToUnicodeL( 
            RFs& aFs, 
            const TDesC8& aSource,
            TUint aSourceEncoding );

    private:    // More constructors
        /** 
        * Performs the first phase of two phase construction.
        *
        * @param    aObserver    Text searcher's observer
        */
        CIpsPlgTextSearcher( MIpsPlgTextSearcherObserver& aObserver );
        
        /**
        * Performs the second phase construction.
        */
        void ConstructL();


    private:
        // A file session for the unicode converter
        RFs iRFs;
    
        // Text searcher's observer
        MIpsPlgTextSearcherObserver& iObserver;

        // Array that contains 16 bit keywords to search. Not owned
        const CDesCArray* iSearchKeywords;        
        
        // Array that contains 8 bit versions from keywords to search
        // These are copied from the 16bit version when search is done in text 
        // that is available in 8 bit format
        CDesC8ArrayFlat* iSearchKeywords8;
        
        // Determines the operation for the search (AND, OR, ...)
        TIpsPlgCriteriaOperation iOperator;
        
        // Flag indicating case sensitivity
        TBool iCaseSensitive;
        
        // Flag indicating whether search parameters where set.
        TBool iHaveParameters;
        
        // Array storing the search result for each individual search keyword
        RArray<TIpsPlgKeywordSearchStatus> iKeywordSearchStatusArray;

        // Character position of the 1st keyword match
        TInt iFirstMatchKeywordCharPos;
        
        // Recommended snippet length.
        TInt iSearchResultRecommendedSnippetLength;
    };

#endif  // IPSPLGTEXTSEARCHER_H
