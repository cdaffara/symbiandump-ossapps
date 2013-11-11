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

#ifndef CPHCNTPBKCONTACTID_H
#define CPHCNTPBKCONTACTID_H

#include "cphcntcontactid.h"

/**
 *  Contact id used with contact engine.
 *
 *  @lib PhoneCntFinder
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CPhCntPbkContactId ): public CPhCntContactId
    {
public:

    static CPhCntPbkContactId* NewL( TInt aContactId );

    static CPhCntPbkContactId* NewLC( TInt aContactId );

    virtual ~CPhCntPbkContactId();

    /**
     * Gives the contact id used with contact engine.
     *
     * @since S60 v3.1
     * @return Contact id.
     */
    TContactItemId ContactId() const;

// from base class CPhCntContactId

    /**
     * From CPhCntContactId
     * Clones contact id.
     *
     * @since S60 v3.1
     * @return New cloned instance of this contact id.
     */
    CPhCntContactId* CloneL() const;
    
    /**
     * From CPhCntContactId
     * Invalidates this contact id.
     *
     * @since S60 v3.1
     */
    void Invalidate();
    
    /**
     * From CPhCntContactId
     * Determines if this contact id is valid contact id.
     *
     * @since S60 v3.1
     * @return ETrue if contact id is valid.
     */
    TBool IsValid() const;
    
    /**
     * From CPhCntContactId
     * Returns null.
     *
     * @since S60 v3.1
     * @return null.
     */
    HBufC8* PackLC() const;
    
private:

    CPhCntPbkContactId( TInt aContactId );

private: // data

    /**
     * Contact identifier.
     */
    TInt iContactId;

    };

#endif // CPHCNTPBKCONTACTID_H
