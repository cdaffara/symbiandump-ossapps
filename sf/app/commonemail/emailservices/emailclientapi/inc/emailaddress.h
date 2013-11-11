/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: mail address implementation definition
*
*/

#ifndef EMAILADDRESS_H
#define EMAILADDRESS_H

#include <memailaddress.h>
#include "emailapiutils.h"

using namespace EmailInterface;

NONSHARABLE_CLASS( CEmailAddress ) : public CBase, public MEmailAddress
    {
public:
    static CEmailAddress* NewL( const TRole aRole, const TDataOwner aOwner );
    static CEmailAddress* NewLC( const TRole aRole, const TDataOwner aOwner );
    
    ~CEmailAddress();

public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();

public: // from MEmailAddress
    void SetAddressL( const TDesC& aAddress );
    TPtrC Address() const;
    
    void SetDisplayNameL( const TDesC& aDisplayName );
    TPtrC DisplayName() const;
    
    TRole Role() const;
    void SetRole( const TRole aRole );
            
private:        
        CEmailAddress( const TRole aRole, const TDataOwner aOwner );
        
private:
        RBuf iAddress;
        RBuf iDisplayName;
        TRole iRole;
        TDataOwner iOwner;
    };

#endif // EMAILADDRESS_H

// End of file
