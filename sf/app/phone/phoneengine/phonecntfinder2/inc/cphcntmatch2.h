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

#ifndef CPHCNTMATCH2_H
#define CPHCNTMATCH2_H

//  INCLUDES
#include "mphcntmatch.h"
#include <qmobilityglobal.h>

// FORWARD DECLARATIONS
class CPhCntContactIdImpl2;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

// CLASS DECLARATION

/**
*  Implementation for single match result.
*  Passes all function calls to CPhCntMacthItem object.
*
*  @lib PhoneCntFinder
*  @since 1.0
*/
NONSHARABLE_CLASS( CPhCntMatch2 ) : public CBase, public MPhCntMatch
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aContact associated contact
        * @return New instance
        */
        static CPhCntMatch2* NewL(const QContact aContact);
        
        /**
        * Destructor.
        */
        ~CPhCntMatch2();
        
        /**
        * overrides the number returned from the contact
        */
        void SetNumberL(const TDesC& aNumber);

    protected: // Functions from base classes

        /**
        * From MPhCntMatch
        */ 
        void Release();

        /**
        * From MPhCntMatch
        */ 
        CPhCntContactId* ContactId() const;

        /**
        * From MPhCntMatch
        */ 
        TNumberType NumberType() const;

        /**
        * From MPhCntMatch
        */ 
        TCliType Cli( HBufC*& aCliText ) const;

        /**
        * From MPhCntMatch
        */ 
        TPtrC FirstName() const;

        /**
        * From MPhCntMatch
        */ 
        TPtrC LastName() const;

        /**
        * From MPhCntMatch
        */ 
        TPtrC CompanyName() const;

        /**
        * From MPhCntMatch
        */ 
        TPtrC GroupName() const;
        
        /**
        * From MPhCntMatch
        */ 
        TPtrC Number();

        /**
        * From MPhCntMatch
        */ 
        TPtrC PersonalRingingTone() const;

        /**
        * From MPhCntMatch
        */ 
        TBool BelongsToGroups( 
            const CArrayFix<TContactItemId>& aGroupArray ) const;

        /**
        * @see MPhCntMatch::TextToSpeechTextL.
        * @since Series 60 3.0
        */
        virtual HBufC* TextToSpeechTextL() const;
        
        /**
        * From MPhCntMatch
        * @since Series 60 3.1
        */ 
        TPtrC CallText() const;
        
        /**
        * From MPhCntMatch
        * @since Series 60 3.1
        */ 
        TPtrC CallImage() const;
        
        /**
        * From MPhCntMatch
        * @since Series 60 3.1
        */ 
        TBool HasThumbnailImage() const;
        
        /**
        * From MPhCntMatch
        */
        CDesCArray& AllDtmfNumbers() const;

    private:

        /**
        * C++ constructor.
        */
        CPhCntMatch2(const QContact aContact);
        
        
    private:
        
        void ConstructL();
        /*
         * checks for exactly 1 item in the list
         */
         QString FieldValue(const QString& name, const QString& key) const;

    private:    // Data
        const QContact iContact;
        HBufC* iNumber;
        CPhCntContactIdImpl2* iContactId;
        CDesCArrayFlat* iDTMFArray;
        QString iFirstname;
        QString iLastname;
        QString iGroupname;
        QString iPersonalRingtone;
        QString iCallImage;

    };

#endif      // CPHCNTMATCH2_H   
            
// End of File
