/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Contains implementations of inline functions
*
*/



// -----------------------------------------------------------------------------
// Returns the iExecuted member of the object
// -----------------------------------------------------------------------------
//
TBool CSmlDmBmCommandElement::Executed()
    {
    return iExecuted;
    }

// -----------------------------------------------------------------------------
// Returns the iStatus member of the object
// -----------------------------------------------------------------------------
//
CSmlDmAdapter::TError CSmlDmBmCommandElement::Status()
    {
    return iStatus;
    }
    
// -----------------------------------------------------------------------------
// Returns the iLeaf member of the object
// -----------------------------------------------------------------------------
//
TBool CSmlDmBmCommandElement::Leaf()
    {
    return iLeaf;
    }
    
// -----------------------------------------------------------------------------
// Returns the iStatusRef member of the object
// -----------------------------------------------------------------------------
//
TInt CSmlDmBmCommandElement::StatusRef()
    {
    return iStatusRef;    
    }
    
// -----------------------------------------------------------------------------
// Returns the iResultRef member of the object
// -----------------------------------------------------------------------------
//
TInt CSmlDmBmCommandElement::ResultRef()
    {
    return iResultRef;
    }
    
// -----------------------------------------------------------------------------
// Returns the iCmdType member of the object
// -----------------------------------------------------------------------------
//
CNSmlDmBmAdapter::TCommandType  CSmlDmBmCommandElement::CmdType()
    {
    return iCmdType;
    }
    
// -----------------------------------------------------------------------------
// Returns the iData member of the object
// -----------------------------------------------------------------------------
//
const HBufC8* CSmlDmBmCommandElement::Data()
    {
    return iData;
    }
    
// -----------------------------------------------------------------------------
// Returns the iLastUriSeg member of the object
// -----------------------------------------------------------------------------
//
const HBufC8* CSmlDmBmCommandElement::LastUriSeg()
    {
    return iLastUriSeg;
    }
    
// -----------------------------------------------------------------------------
// Sets the iExecuted member of the object
// -----------------------------------------------------------------------------
//
void CSmlDmBmCommandElement::SetExecuted( TBool aExecuted )
    {
    iExecuted = aExecuted;
    }

// -----------------------------------------------------------------------------
// Sets the iStatus member of the object
// -----------------------------------------------------------------------------
//
void CSmlDmBmCommandElement::SetStatus( CSmlDmAdapter::TError aStatus )
    {
    iStatus = aStatus;
    }
    
    
// -----------------------------------------------------------------------------
// Sets the iData member of the object. Ownership of aData argument will be 
// transfered to the CSmlDmBmCommandElement
// -----------------------------------------------------------------------------
//
void CSmlDmBmCommandElement::SetDataL( const TDesC8& aData )
    {  
    if ( aData.Length() > iData->Length() )
        {
        iData = iData->ReAllocL( aData.Length() );
        }
    *iData = aData;
    }

//Function definitions for CSmlDmBmSettingsElement
// -----------------------------------------------------------------------------
// Returns reference to iNodeBuf
// -----------------------------------------------------------------------------
RPointerArray<CSmlDmBmCommandElement>& CSmlDmBmSettingsElement::NodeBuf()
    {
    return iNodeBuf;
    }

// -----------------------------------------------------------------------------
// Returns iMappingName
// -----------------------------------------------------------------------------
const HBufC8* CSmlDmBmSettingsElement::MappingName()
    {
    return iMappingName;
    }

// -----------------------------------------------------------------------------
// Returns iExecuted
// -----------------------------------------------------------------------------
TBool CSmlDmBmSettingsElement::Executed()
    {
    return iExecuted;
    }
    
// -----------------------------------------------------------------------------
// Sets the iExecuted member value
// -----------------------------------------------------------------------------
void CSmlDmBmSettingsElement::SetExecuted( TBool aExecuted )
    {
    iExecuted = aExecuted;
    }


// -----------------------------------------------------------------------------
// Returns iLuid
// -----------------------------------------------------------------------------
TBool CSmlDmBmSettingsElement::Luid()
    {
    return iLuid;
    }
    
// -----------------------------------------------------------------------------
// Sets the iExecuted member value
// -----------------------------------------------------------------------------
void CSmlDmBmSettingsElement::SetLuid( TInt aLuid )
    {
    iLuid = aLuid;
    }
