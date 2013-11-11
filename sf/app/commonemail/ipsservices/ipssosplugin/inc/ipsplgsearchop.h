/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class CIpsPlgSearchOp.
*
*/


#ifndef IPSPLGSEARCHOP_H
#define IPSPLGSEARCHOP_H


#include "ipsplgtextsearcher.h"

class CFSMailMessage;
class CImEmailMessage;
class CIpsPlgTextSearcher;

/**
 * Class to handle searching from emails.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
NONSHARABLE_CLASS( CIpsPlgSearchOp ) : 
    public CActive, 
    public MIpsPlgTextSearcherObserver
    {

public:

    /**
     * Symbian 2nd phase construcror
     *
     * @return None
     */
    static CIpsPlgSearchOp* NewL( CIpsPlgSearch& aObserver );

    /**
     * Symbian 2nd phase construcror
     *
     * @return None
     */
    static CIpsPlgSearchOp* NewLC( CIpsPlgSearch& aObserver );

    /**
     * Class destructor
     *
     * @return
     */
    virtual ~CIpsPlgSearchOp();

    /**
     *
     *
     * @return
     */
    void DoCancel();

protected:

    /**
     * Class constructor
     *
     * @param
     * @param
     */
    CIpsPlgSearchOp( CIpsPlgSearch& aObserver );

private:

// Data   

    /**
     * Symbian 2nd phase construct
     *
     */
    void ConstructL();

// From base class CActive

    /**
     *
     *
     * @return
     */
    void RunL();

    /**
     * Handles any leaves that happened during the operation.
     *
     * @return KErrNone, when problems are solved.
     */
    TInt RunError( TInt aError );

// From base class MIpsPlgTextSearcherObserver

	/**
	* Called from the text searcher when all search criteria are met.
	*
	* @param	aSnippet			The snippet
	* @param	aSnippetCharPos		The character position of the keyword match within
	*								 the snippet
	* @param	aStartIncomplete	Indicates that the snippet start is incomplete
	* @param	aEndIncomplete		Indicates that the snippet end is incomplete
	*/
	void HitL( 
	    const TDesC& aSnippet, 
	    TInt aSnippetCharPos, 
	    TBool aStartIncomplete, 
	    TBool aEndIncomplete );

// New functions

    /**
     * Get next available mail or finish the search. Changes state on demand.
     *
     * @return ETrue, when search can be continued
     * @return EFalse, when search is finished.
     */
    TBool NextMailL();

    /**
     * Looks for strings from the body.
     *
     * @param aSearchString Strings to be searched.
     * @return ETrue, when strings are found.
     */
    void SearchBodyL();
    
    /**
     * Search for the string from the email.
     */
    void SearchL();

    /**
     * Filter out javascript and html tags.
     */
    void HandleHtmlL();

    /**
     * Send finish command to observer.
     */
    void FinalizeL();

    /**
     * @param aArray Array to be searched for.
     * @return ETrue if found, EFalse otherwise
     */
    TBool SearchFromArrayL( const CDesCArray& aArray );

    /**
     * @param aString String to be searched for.
     * @return ETrue if found, EFalse otherwise
     */
    TBool SearchFromStringL( const TDesC& aString );
    
    /**
    * Activate and complete
     */
    void ActivateAndComplete( );
    
    /**
    * Search for the string from the email.
    */
    void SearchFromEntireMailL();
    
    /**
    * Search for the string from subject only.
    */
    void SearchFromSubjectL();

// Data

    enum TState
        {
        EStopped = 0,
        EStarted,
        ERunning,
        EFinished,
        EHit,
        ECanceled
        };

    /**
     * Current search state.
     */
    TState iState;
    
    /**
     * Client, which started the operation.
     */
    CIpsPlgSearch& iObserver;
    
    /**
     * Currently accessed message.
     * Owned.
     */
    CImEmailMessage* iMessage;
    
    /**
     * Header of currently accessed message.
     * Owned.
     */
    CImHeader* iHeader;
    
    /**
     * Temporary storage for search strings.
     * Owned.
     */
    
    /**
     * Object to handle the actual searching.
     * Owned.
     */
    CIpsPlgTextSearcher* iSearcher;  
    
//    
    /**
     * Enable changing searching priority on client feedback - must be made in RunL
     */
    TInt iRequiredPriority;
//    
    };

#endif /* IPSPLGSEARCHOP_H */

// End of File
