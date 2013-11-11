/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email address object
*
*/

#include "emailtrace.h"

//<cmail>
#include <nmcommonheaders.h>
#include "CFSMailAddress.h"
//</cmail>

//<qmail>
#include "nmaddress.h"
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailAddress::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailAddress::NewLC()
    {
    NM_FUNCTION;
    
    CFSMailAddress* adr = new (ELeave) CFSMailAddress();
    CleanupStack::PushL(adr);
    adr->ConstructL();
    return adr;
    } 

// -----------------------------------------------------------------------------
// CFSMailAddress::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailAddress::NewL()
    {
    NM_FUNCTION;
    
    CFSMailAddress* adr =  CFSMailAddress::NewLC();
    CleanupStack::Pop(adr);
    return adr;
    }

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailAddress::NewL( const NmAddress& aNmAddress )
	{
    NM_FUNCTION;
    
    CFSMailAddress* adr = new (ELeave) CFSMailAddress();
    CleanupStack::PushL(adr);
    adr->ConstructL(aNmAddress);
    CleanupStack::Pop(adr);
    return adr;
	}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailAddress::ConstructL
// -----------------------------------------------------------------------------
void CFSMailAddress::ConstructL()
    {
    NM_FUNCTION;
    
// <qmail>
    // Construction of shared data object
    iNmPrivateAddress = new NmAddressPrivate();
// </qmail>
    }
	
// <qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::ConstructL
// --------------------------------_--------------------------------------------
void CFSMailAddress::ConstructL( const NmAddress& aNmAddress )
	{
    NM_FUNCTION;
    
    // shared data object
    iNmPrivateAddress = aNmAddress.d;
	}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailAddress::CFSMailAddress
// -----------------------------------------------------------------------------
CFSMailAddress::CFSMailAddress()
    {
    NM_FUNCTION;
    
// <qmail>
// Unnecessary members initialization removed: iEmailAddress, iDisplayName
// </qmail>
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::~CFSMailAddress
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress::~CFSMailAddress()
    {
    NM_FUNCTION;
    
// <qmail>
// Unnecessary members destruction removed: iEmailAddress, iDisplayName
// </qmail>
        }

// -----------------------------------------------------------------------------
// CFSMailAddress::GetEmailAddress
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailAddress::GetEmailAddress() const
    {
    NM_FUNCTION;
    
// <qmail>
    iEmailAddressPtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateAddress->mAddress.utf16()),
        iNmPrivateAddress->mAddress.length());
    return iEmailAddressPtr;
// </qmail>
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::SetEmailAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailAddress::SetEmailAddress(const TDesC& aAddress)
    {
    NM_FUNCTION;
    
// <qmail>
    QString qtEmailAddress = QString::fromUtf16(aAddress.Ptr(), aAddress.Length());
    iNmPrivateAddress->mAddress = qtEmailAddress;
// </qmail>
	}

// -----------------------------------------------------------------------------
// CFSMailAddress::GetDisplayName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailAddress::GetDisplayName() const
    {
    NM_FUNCTION;
    
// <qmail>
    iDisplayNamePtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateAddress->mDisplayName.utf16()),
        iNmPrivateAddress->mDisplayName.length());
    return iDisplayNamePtr;
// </qmail>
    }

// -----------------------------------------------------------------------------
// CFSMailAddress::SetDisplayName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailAddress::SetDisplayName(const TDesC& aDisplayName)
    {
    NM_FUNCTION;
    
// <qmail>
    QString qtDisplayName = QString::fromUtf16(aDisplayName.Ptr(), aDisplayName.Length());
    iNmPrivateAddress->mDisplayName = qtDisplayName;
// </qmail>
	}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailAddress::GetNmAddress
// -----------------------------------------------------------------------------
EXPORT_C NmAddress CFSMailAddress::GetNmAddress()
	{
    NM_FUNCTION;
    
    NmAddress nmAddress(iNmPrivateAddress);
    return nmAddress;
	}
// </qmail>


