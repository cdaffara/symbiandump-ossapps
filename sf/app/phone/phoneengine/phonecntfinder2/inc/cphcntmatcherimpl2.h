/*
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
*/

#ifndef CPHCNTMATCHERIMPL_H
#define CPHCNTMATCHERIMPL_H

//  INCLUDES
#include "cphcntmatcher.h"
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <qmobilityglobal.h>

// FORWARD DECLARATIONS
class CPhCntContactId;
class CPhCntMatch2;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

// CLASS DECLARATION


/**
*  Implementation for making matches.
*
*  @lib PhoneCntFinder
*  @since 1.0
*/
NONSHARABLE_CLASS( CPhCntMatcherImpl2 ) :
    public CPhCntMatcher
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @pre aContactManager != NULL
        * @param aContactManager provides matching services. Ownership is transferred.
        * @return New instance
        */
        static CPhCntMatcherImpl2* NewL(QContactManager* aContactManager);
        
        /**
        * Destructor.
        */
        virtual ~CPhCntMatcherImpl2();


    public: // Functions from base classes

        /**
        * From CPhCntMatcher
        */
        TInt MatchNumber( MPhCntMatch*& aMatch,const TDesC& aTelNumber );

        /**
        * From CPhCntMatcher
        */
        TInt MatchNumber( 
            MPhCntMatch*& aMatch,
            const TDesC& aTelNumber,
            const CPhCntContactId& aContactId );

        /**
        * From CPhCntMatcher
        */
        TInt MatchNumber(
            MPhCntMatch*& aMatch,
            const TDesC& aTelNumber,
            const CPhCntContactId& aContactId,
            const TFieldId aFieldId );

            
        /**
        * Match number to phonebook.
        * @param aMatch Found match, owership tranferred. NULL if not found any.
        * @param aNumber Number to match against.
        * @return Error code: KErrNone - one match found
        *                     KErrNotFound - no matches found
        *                     other - normal Symbian OS error
        */    
        TInt MatchVoipNumber(
            MPhCntMatch*& aMatch,
            const TDesC& aNumber,
            TBool aAllowUserNameMatch,
            TInt aCharsForMatching = 0 );
        
         /**
        * From CPhCntMatcher, gets VoIP call contact info from phonebook
        * by contact id.
        * @since Series60 3.0
        * @param aMatch for found match, owership tranferred. Empty if not found.
        * @param aContactId for current contact.
        * @return Error code: KErrNone - VoIP call contact found
        *                     KErrNotFound - no VoIP call contact found
        *                     other - normal Symbian OS error
        */    
        TInt MatchVoipNumber(
            MPhCntMatch*& aMatch,
            const CPhCntContactId& aContactId );

        /**
          * Determines if contact has other type of numbers
          * than VoIP numbers.
          * 
          * @since S60 v3.2.
          * @param aContactId Id of the contact.
          * @return ETrue - CS numbers found from contact.
          *         EFalse - No CS numbers found from contact.
          */
         virtual TBool HasCSNumbers( 
             const CPhCntContactId& aContactId );
             
        /**
        * @see CPhCntMatcher::MatchNumber
        */
        TInt MatchNumber( 
            MPhCntMatch*& aMatch, 
            const TDesC& aTelNumber,
            TInt aContactId );


    private:

        /**
        * C++ constructor.
        */
        CPhCntMatcherImpl2(QContactManager* aContactManager);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        

    private:
        CPhCntMatch2* iMatch; //only one now, must delete anyway.
        QContactManager* iContactManager;

  
    };

#endif      // CPHCNTMATCHERIMPL_H

// End of File
