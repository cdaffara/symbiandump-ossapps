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

#ifndef CPHCNTCONTACTID_H
#define CPHCNTCONTACTID_H

#include <e32base.h>
#include <cntdef.h>

/**
 *  Abstract base class for identifying a single contact
 *  in contact stores.
 *
 *  @lib PhoneCntFinder
 *  @since S60 v3.1
 */
class CPhCntContactId : public CBase
    {
public:

    /**
     * Clones contact id.
     *
     * @since S60 v3.1
     * @return New cloned instance of contact id.
     */
     virtual CPhCntContactId* CloneL() const  = 0;
     
     /**
      * Invalidates this contact id. After this call, 
      * IsValid call will return false.
      * @since S60 v3.1
      */
     virtual void Invalidate() = 0;
     
     /**
      * Is contact id valid.
      * 
      * @since S60 v3.1
      * @return ETrue if contact id is valid.
      */
     virtual TBool IsValid() const = 0;
     
     /**
      * Gives contact item id. If this id is not valid, then
      * contact item id given is not valid.
      * 
      * @since S60 v3.1
      * @return Contact item id.
      */
     virtual TContactItemId ContactId() const = 0;
     
     /**
      * Packs Virtual phonebook contact link to descriptor.
      * If this link is not valid, then packed data is not valid.
      *
      * @since S60 v3.1
      * @return Packed virtual phonebook contact link.
      */
      virtual HBufC8* PackLC() const = 0;
    
      virtual ~CPhCntContactId() {};
    };

#endif // CPHCNTCONTACTID_H
