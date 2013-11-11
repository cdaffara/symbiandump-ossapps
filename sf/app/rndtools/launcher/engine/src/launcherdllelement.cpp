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
* Description:
*
*/

#include "launcherdllelement.h"
#include "launchertraces.h"

// ---------------------------------------------------------------------------

CLauncherDLLElement::~CLauncherDLLElement()
    {
    LOGSTRING("Launcher: CLauncherDLLElement::~CLauncherDLLElement");    
    }

// ---------------------------------------------------------------------------

CLauncherDLLElement::CLauncherDLLElement()
    {    
    }

// ---------------------------------------------------------------------------

CLauncherDLLElement* CLauncherDLLElement::NewL()
    {
    LOGSTRING("Launcher: CLauncherDLLElement::NewL");
    CLauncherDLLElement* self = CLauncherDLLElement::NewLC();    
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------

CLauncherDLLElement* CLauncherDLLElement::NewLC()
    {
    LOGSTRING("Launcher: CLauncherDLLElement::NewLC");
    CLauncherDLLElement* self = new (ELeave) CLauncherDLLElement;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::ConstructL()
    {
    
    }

// ---------------------------------------------------------------------------
   
TBool CLauncherDLLElement::CompareL(const CLauncherDLLElement& aReferenceElement, RArray<TDifference>& aDifferenceList) const
    {
    LOGSTRING2("Launcher: CLauncherDLLElement::CompareL - DLLName: %S", &iName);
    if( iName != aReferenceElement.Name())
        {
        LOGSTRING("Launcher: CLauncherDLLElement::CompareL: ERROR - DLL names don't match.");
        User::Leave(KErrArgument);
        }
    
    TBool diff = EFalse;
            
    if( iUID1 != aReferenceElement.UID1()) 
        {
        diff = ETrue;
        aDifferenceList.AppendL(this->EDifference_UID1);      
        LOGSTRING3("Launcher: CLauncherDLLElement::CompareL - UID1s differs: %x vs. %x", iUID1, aReferenceElement.UID1());  
        }
    if( iUID2!= aReferenceElement.UID2()) 
        {
        diff = ETrue;
        aDifferenceList.AppendL(this->EDifference_UID2);        
        LOGSTRING3("Launcher: CLauncherDLLElement::CompareL - UID2s differs: %x vs. %x", iUID2, aReferenceElement.UID2());  
        }
    if( iUID3 != aReferenceElement.UID3()) 
        {
        diff = ETrue;
        aDifferenceList.AppendL(this->EDifference_UID3);        
        LOGSTRING3("Launcher: CLauncherDLLElement::CompareL - UID3s differs: %x vs. %x", iUID3, aReferenceElement.UID3());  
        }
    if( iSID != aReferenceElement.SID()) 
        {
        diff = ETrue;
        aDifferenceList.AppendL(this->EDifference_SID);        
        LOGSTRING3("Launcher: CLauncherDLLElement::CompareL - SIDs differs: %x vs. %x", iSID, aReferenceElement.SID());  
        }
    if( iCapability != aReferenceElement.Capability()) 
        {
        diff = ETrue;
        aDifferenceList.AppendL(this->EDifference_Capability);        
        LOGSTRING3("Launcher: CLauncherDLLElement::CompareL - Capabilities differs: %x vs. %x", iCapability, aReferenceElement.Capability());  
        }
    
    return diff;
    }

// ---------------------------------------------------------------------------

TFileName CLauncherDLLElement::Name() const
    {
    return iName;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::SetNameL(const TFileName& aName)
    {    
    iName.Copy(aName); 
    }

// ---------------------------------------------------------------------------

TUid CLauncherDLLElement::UID1() const
    {
    return iUID1;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::SetUID1L(TUid aUID1)
    {
    iUID1.iUid = aUID1.iUid;
    }

// ---------------------------------------------------------------------------

TUid CLauncherDLLElement::UID2() const
    {
    return iUID2;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::SetUID2L(TUid aUID2)
    {    
    iUID2 = aUID2;
    }

// ---------------------------------------------------------------------------

TUid CLauncherDLLElement::UID3() const
    {    
    return iUID3;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::SetUID3L(TUid aUID3)
    {    
    iUID3 = aUID3;
    }

// ---------------------------------------------------------------------------

TUid CLauncherDLLElement::SID() const
    {   
    return iSID;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::SetSIDL(TUid aSID)
    {    
    iSID = aSID;
    }

// ---------------------------------------------------------------------------

TUint32 CLauncherDLLElement::Capability() const
    {
    return iCapability;    
    }

// ---------------------------------------------------------------------------

void CLauncherDLLElement::SetCapabilityL(TUint32 aCapability)
    {
    iCapability = aCapability; 
    }
