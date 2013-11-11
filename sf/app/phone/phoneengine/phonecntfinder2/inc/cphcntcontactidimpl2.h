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

#ifndef CPHCNTCONTACTIDIMPL2_H
#define CPHCNTCONTACTIDIMPL2_H

#include <e32base.h>
#include "cphcntcontactid.h"
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

/**
 *  Abstract base class for identifying a single contact
 *  in contact stores.
 *
 *  @lib PhoneCntFinder
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS (CPhCntContactIdImpl2) : public CPhCntContactId
    {
public:

    ~CPhCntContactIdImpl2();
        /**
        * Two-phased constructor.
        * @return New instance
        */
   static CPhCntContactIdImpl2* NewL(const QContact aContact);
   
   //from base class
   
    /**
     * Clones contact id.
     *
     * @since S60 v3.1
     * @return New cloned instance of contact id.
     */
     CPhCntContactId* CloneL() const;
     
     /**
      * Invalidates this contact id. After this call, 
      * IsValid call will return false.
      * @since S60 v3.1
      */
     void Invalidate();
     
     /**
      * Is contact id valid.
      * 
      * @since S60 v3.1
      * @return ETrue if contact id is valid.
      */
     TBool IsValid() const;
     
     /**
      * Gives contact item id. If this id is not valid, then
      * contact item id given is not valid.
      * 
      * @since S60 v3.1
      * @return Contact item id.
      */
     TContactItemId ContactId() const;
     
     /**
      * This function used to pack virtual phonebook contact link.
      * Now it just packs the contact id (with phonebook3)
      * @since S60 v3.1
      * @return Contact Id
      */
      HBufC8* PackLC() const;
    
  
      
private:
    CPhCntContactIdImpl2(const QContact aContact);
private:
    const QContact Contact() const; //contact for the cloning
    
private:
    
    const QContact iContact;
    bool           iValid;
    };

#endif // CPHCNTCONTACTIDIMPL2_H
