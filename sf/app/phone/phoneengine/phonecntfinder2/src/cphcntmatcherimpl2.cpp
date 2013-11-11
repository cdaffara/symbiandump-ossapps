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

#include <QList>
#include <QString>
#include <QChar>
#include <qcontact.h>
#include <qcontactdetailfilter.h>
#include <qcontactphonenumber.h>
#include <qtcontactsglobal.h>
#include <qcontactmanager.h>

#include "cphcntmatcherimpl2.h"
#include "cphcntmatch2.h"
#include "qtphonelog.h"



// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPhCntMatcherImpl2* CPhCntMatcherImpl2::NewL(QContactManager* aContactManager) 
    {
    __ASSERT_ALWAYS(aContactManager!=NULL, User::Leave(KErrArgument));
    CPhCntMatcherImpl2* self = new( ELeave )CPhCntMatcherImpl2(aContactManager);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CPhCntMatcherImpl2::~CPhCntMatcherImpl2() 
    {
    delete iMatch;
    delete iContactManager;
    }

// ---------------------------------------------------------------------------
// From CPhCntMatcher
// Tries to find a contact which has aTelNumber.
// ---------------------------------------------------------------------------
//    
TInt CPhCntMatcherImpl2::MatchNumber( 
    MPhCntMatch*& aMatch,   
    const TDesC& aTelNumber) 
    {
    TInt err = KErrNone;
    QString telnumber((QChar*)aTelNumber.Ptr(),aTelNumber.Length());
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    filter.setValue(telnumber);
    QList<QContactLocalId> contacts;
    contacts = iContactManager->contactIds(filter);
    PHONE_DEBUG2("CPhCntMatcherImpl2::MatchNumber(): matched contacts count is:", contacts.count());
    //only 1 exact match returns a contact to be used 
    if (contacts.isEmpty() || contacts.count() > 1 )
        {
        return KErrNotFound;
        }
    else 
        {
        delete iMatch;
        iMatch = NULL;
        TRAP(err, iMatch = CPhCntMatch2::NewL(iContactManager->contact(contacts.at(0))));
        if (!err)
        {
            aMatch = iMatch;
            PHONE_DEBUG("CPhCntMatcherImpl2::MatchNumber() -- matched contact is ready");
        }
        return err;
        }
    }

// ---------------------------------------------------------------------------
// From CPhCntMatcher
// Tries to find a contact which has aTelNumber.
// ---------------------------------------------------------------------------
//  
TInt CPhCntMatcherImpl2::MatchNumber( 
    MPhCntMatch*& /*aMatch*/,
    const TDesC& /*aTelNumber*/,
    const CPhCntContactId& /*aContactId*/ )
    {
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// From CPhCntMatcher
// Tries to find a contact which has aTelNumber, contactId and fieldId
// ---------------------------------------------------------------------------
// 
TInt CPhCntMatcherImpl2::MatchNumber(
    MPhCntMatch*& /*aMatch*/,
    const TDesC& /*aTelNumber*/,
    const CPhCntContactId& /*aContactId*/,
    const TFieldId /*aFieldId*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From CPhCntMatcher
// Empty implementation. 
// ---------------------------------------------------------------------------
//    
TInt CPhCntMatcherImpl2::MatchVoipNumber(
    MPhCntMatch*& /*aMatch*/,
    const TDesC& /*aMatchString*/,
    TBool /*aAllowUserNameMatch*/,
    TInt /*aCharsForMatching*/ )
    {
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// From CPhCntMatcher
// Empty implementation. 
// ---------------------------------------------------------------------------
//
TInt CPhCntMatcherImpl2::MatchVoipNumber(
    MPhCntMatch*& /*aMatch*/,
    const CPhCntContactId& /*aContactId*/ )
    {
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// From CPhCntMatcher
// Empty implementation. Implemented in CPhCntMatcherVoIPImpl.
// ---------------------------------------------------------------------------
//
TBool CPhCntMatcherImpl2::HasCSNumbers( const CPhCntContactId& /*aContactId*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From CPhCntMatcher
// ---------------------------------------------------------------------------
// 
 TInt CPhCntMatcherImpl2::MatchNumber( 
            MPhCntMatch*& aMatch, 
            const TDesC& aTelNumber,
            TInt aContactId )
    {
    TInt err = KErrNone;
    //now there's no way to check if the contact is valid or not
    QContact contact;
    contact = iContactManager->contact(aContactId);
    //should check if valid or not
    delete iMatch;
    iMatch = NULL;
    TRAP(err, iMatch = CPhCntMatch2::NewL(contact));
    if (!err)
    {
        TRAP(err, iMatch->SetNumberL(aTelNumber));
    }
    if (!err)
    {   
        aMatch = iMatch;
        PHONE_DEBUG("CPhCntMatcherImpl2::MatchNumber(ContactId) -- matched contact is ready");
    }
    return err;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//   
CPhCntMatcherImpl2::CPhCntMatcherImpl2(QContactManager* aContactManager)
:iContactManager(aContactManager)
    {
    iMatch = NULL;
    }


    
// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//    
void CPhCntMatcherImpl2::ConstructL()
    { 
    
    }

