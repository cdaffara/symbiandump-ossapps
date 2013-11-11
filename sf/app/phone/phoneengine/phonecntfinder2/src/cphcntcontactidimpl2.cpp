/*!
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
* Description:  Wraps the QContact for the legacy handling in the phone engine
*/

#include <e32base.h>
#include <utf.h>
#include <qcontact.h>
#include "cphcntcontactidimpl2.h"
// ---------------------------------------------------------------------------
// c'tor
// ---------------------------------------------------------------------------
//

CPhCntContactIdImpl2::~CPhCntContactIdImpl2() 
    {
    
    }

// ---------------------------------------------------------------------------
// copy c'tor
// ---------------------------------------------------------------------------
//
CPhCntContactIdImpl2::CPhCntContactIdImpl2(const QContact aContact)
:iContact(aContact), iValid(true)
    {
    
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPhCntContactIdImpl2* CPhCntContactIdImpl2::NewL(const QContact aContact)
    {
    CPhCntContactIdImpl2* self = new( ELeave ) CPhCntContactIdImpl2(aContact);
    return self;
    }
 
// ---------------------------------------------------------------------------
// CloneL
// ---------------------------------------------------------------------------
//
CPhCntContactId* CPhCntContactIdImpl2::CloneL() const
    {
    return CPhCntContactIdImpl2::NewL(Contact());
    }
     
// ---------------------------------------------------------------------------
// Invalidate
// ---------------------------------------------------------------------------
//

void CPhCntContactIdImpl2::Invalidate()
    {
    iValid = false;
    }
     
// ---------------------------------------------------------------------------
// IsValid
// ---------------------------------------------------------------------------
//
TBool CPhCntContactIdImpl2::IsValid() const
    {
    return iValid;
    }
     
// ---------------------------------------------------------------------------
// ContactId
// ---------------------------------------------------------------------------
//
TContactItemId CPhCntContactIdImpl2::ContactId() const
    {
    if (IsValid())
        {
        return iContact.localId();
        }
    else
        {
        return KErrNotFound;
        }
    }
     
// ---------------------------------------------------------------------------
// PackLC
// ---------------------------------------------------------------------------
//
HBufC8* CPhCntContactIdImpl2::PackLC() const


    {
    HBufC8* valueToReturn = NULL;
    
    if (IsValid())
        {
        QString str; 
        str.setNum(iContact.localId());
        TPtrC16 value(str.utf16());
        valueToReturn =  CnvUtfConverter::ConvertFromUnicodeToUtf8L(value);
        CleanupStack::PushL(valueToReturn);
        }
        
    return valueToReturn;
    }

// ---------------------------------------------------------------------------
// Contact
// ---------------------------------------------------------------------------
//
const QContact CPhCntContactIdImpl2::Contact() const
    {
    return iContact;
    }


    

