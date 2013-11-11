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
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactavatar.h>
#include <qcontactringtone.h>
#include <qcontactphonenumber.h>
#include <qtcontactsglobal.h>
#include "cphcntmatch2.h"
#include "cphcntcontactidimpl2.h"
#include "qtphonelog.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPhCntMatch2::CPhCntMatch2(const QContact aContact):
iContact(aContact), iNumber(NULL), iContactId(NULL), iDTMFArray(NULL)
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPhCntMatch2::~CPhCntMatch2()
    {
    delete iNumber;
    delete iDTMFArray;
    delete iContactId; 
    }
    
// -----------------------------------------------------------------------------
// CPhCntMatch2::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCntMatch2* CPhCntMatch2::NewL(const QContact aContact)
    {
    CPhCntMatch2* self = new( ELeave ) CPhCntMatch2(aContact);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Release
// -----------------------------------------------------------------------------
//
void CPhCntMatch2::Release()
{
    delete iNumber;
    iNumber = NULL;
}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CPhCntContactId* CPhCntMatch2::ContactId() const
{
    return iContactId;
}

// -----------------------------------------------------------------------------
// NumberType
// -----------------------------------------------------------------------------
//
MPhCntMatch::TNumberType CPhCntMatch2::NumberType() const
{
    return MPhCntMatch::ENone;
}

// -----------------------------------------------------------------------------
// Cli
// -----------------------------------------------------------------------------
//

MPhCntMatch::TCliType CPhCntMatch2::Cli( HBufC*& /*aCliText*/ ) const
{
    return ECliNumber;
}

// -----------------------------------------------------------------------------
// FirstName
// -----------------------------------------------------------------------------
//
TPtrC CPhCntMatch2::FirstName() const
{
    return iFirstname.utf16();

}
// -----------------------------------------------------------------------------
// LastName
// -----------------------------------------------------------------------------
//
TPtrC CPhCntMatch2::LastName() const
{
    return iLastname.utf16();
   
}
// -----------------------------------------------------------------------------
// CompanyName
// -----------------------------------------------------------------------------
//
TPtrC CPhCntMatch2::CompanyName() const
{
  return KNullDesC();
}

// -----------------------------------------------------------------------------
// GroupName
// -----------------------------------------------------------------------------
//
TPtrC CPhCntMatch2::GroupName() const
{
    return iGroupname.utf16();  
}

// -----------------------------------------------------------------------------
// Number
// -----------------------------------------------------------------------------
//
TPtrC CPhCntMatch2::Number()
{
    //how many numbers are returned here? should be only one that matched 
    if ( iNumber == NULL ) {      
        TPtrC ptr(FieldValue(QContactPhoneNumber::DefinitionName, 
                   QContactPhoneNumber::FieldNumber).utf16());
        iNumber = ptr.Alloc();
        if (!iNumber) {
            return KNullDesC();
        }
    }  
    return iNumber->Des();
}
// -----------------------------------------------------------------------------
// PersonalRingingTone
// -----------------------------------------------------------------------------
//
TPtrC CPhCntMatch2::PersonalRingingTone() const
{
    return iPersonalRingtone.utf16();
}
// -----------------------------------------------------------------------------
// BelongsToGroups
// -----------------------------------------------------------------------------
//
TBool CPhCntMatch2::BelongsToGroups( 
    const CArrayFix<TContactItemId>& /*aGroupArray*/ ) const
{
    return EFalse;
}
// -----------------------------------------------------------------------------
// TextToSpeechTextL
// -----------------------------------------------------------------------------
//
HBufC* CPhCntMatch2::TextToSpeechTextL() const
{
    return NULL;
}
// -----------------------------------------------------------------------------
// CallText
// -----------------------------------------------------------------------------
//    
TPtrC CPhCntMatch2::CallText() const
{
    return KNullDesC();
}
// -----------------------------------------------------------------------------
// CallImage
// -----------------------------------------------------------------------------
//       
TPtrC CPhCntMatch2::CallImage() const
{
    return iCallImage.utf16();
}
// -----------------------------------------------------------------------------
// HasThumbnailImage
// -----------------------------------------------------------------------------
//       
TBool CPhCntMatch2::HasThumbnailImage() const
{
    return EFalse;
}
// -----------------------------------------------------------------------------
// AllDtmfNumbers
// -----------------------------------------------------------------------------
//
CDesCArray& CPhCntMatch2::AllDtmfNumbers() const
{
    return *iDTMFArray;
}
// -----------------------------------------------------------------------------
// SetNumberL
// -----------------------------------------------------------------------------
//

void CPhCntMatch2::SetNumberL(const TDesC& aNumber)
{
    delete iNumber;
    iNumber = NULL;
    iNumber = aNumber.AllocL();
}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CPhCntMatch2::ConstructL()
    {
    iContactId = CPhCntContactIdImpl2::NewL(iContact);
    iDTMFArray = new CDesCArrayFlat(1);
    
    //Get first and last name
    iFirstname = FieldValue(
             QContactName::DefinitionName,
             QContactName::FieldFirstName); 
    
    
    iLastname = FieldValue(
             QContactName::DefinitionName, 
             QContactName::FieldLastName);
    
    if (iContact.type() == QContactType::TypeGroup) {
        QContactName contactName = iContact.detail(QContactName::DefinitionName);
        iGroupname = contactName.customLabel();     
    }
    
    //Get caller's image
    QList<QContactAvatar> details = iContact.details<QContactAvatar>();
    for (int i=0;i<details.count();++i) {
        if (!details.at(i).imageUrl().isEmpty()) {
            iCallImage = details.at(i).imageUrl().toString();
            break;
        }
    }
    PHONE_DEBUG2("CPhCntMatch2::callImage:: value is :", iCallImage);
    
    
    //Get personal ringtone
    QContactRingtone ringtonedetail = iContact.detail<QContactRingtone>();
    iPersonalRingtone = ringtonedetail.audioRingtoneUrl().toString();
    PHONE_DEBUG2("CPhCntMatch2::personalRingingtone:: value is :", iPersonalRingtone);

    }

// -----------------------------------------------------------------------------
// FieldValue
// -----------------------------------------------------------------------------
//
QString CPhCntMatch2::FieldValue(const QString& name, const QString& key) const
{
    QString returnValue;
    QList<QContactDetail> details = iContact.details(name);
    if (details.count() == 1)
        {
        returnValue = details.first().value(key);        
        }
    PHONE_DEBUG4("CPhCntMatch2::FieldValue:: key is :", key, "..value is :", returnValue);
    return returnValue;    
}


