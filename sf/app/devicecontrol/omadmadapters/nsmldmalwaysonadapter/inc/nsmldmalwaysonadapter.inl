/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains implementations of inline functions.
*
*/


// -----------------------------------------------------------------------------
// Returns the iExecuted member of the object
// -----------------------------------------------------------------------------
//
TBool CSmlDmAOCommandElement::Executed()
    {
    return iExecuted;
    }

// -----------------------------------------------------------------------------
// Returns the iStatus member of the object
// -----------------------------------------------------------------------------
//
CSmlDmAdapter::TError CSmlDmAOCommandElement::Status()
    {
    return iStatus;
    }
    
// -----------------------------------------------------------------------------
// Returns the iLeaf member of the object
// -----------------------------------------------------------------------------
//
TBool CSmlDmAOCommandElement::Leaf()
    {
    return iLeaf;
    }
    
// -----------------------------------------------------------------------------
// Returns the iStatusRef member of the object
// -----------------------------------------------------------------------------
//
TInt CSmlDmAOCommandElement::StatusRef()
    {
    return iStatusRef;    
    }
    
// -----------------------------------------------------------------------------
// Returns the iResultRef member of the object
// -----------------------------------------------------------------------------
//
TInt CSmlDmAOCommandElement::ResultRef()
    {
    return iResultRef;
    }
    
// -----------------------------------------------------------------------------
// Returns the iCmdType member of the object
// -----------------------------------------------------------------------------
//
CNSmlDmAOAdapter::TCommandType  CSmlDmAOCommandElement::CmdType()
    {
    return iCmdType;
    }
    
// -----------------------------------------------------------------------------
// Returns the iData member of the object
// -----------------------------------------------------------------------------
//
const HBufC8* CSmlDmAOCommandElement::Data()
    {
    return iData;
    }
    
// -----------------------------------------------------------------------------
// Returns the iLastUriSeg member of the object
// -----------------------------------------------------------------------------
//
const HBufC8* CSmlDmAOCommandElement::LastUriSeg()
    {
    return iLastUriSeg;
    }
    
// -----------------------------------------------------------------------------
// Sets the iExecuted member of the object
// -----------------------------------------------------------------------------
//
void CSmlDmAOCommandElement::SetExecuted( TBool aExecuted )
    {
    iExecuted = aExecuted;
    }

// -----------------------------------------------------------------------------
// Sets the iStatus member of the object
// -----------------------------------------------------------------------------
//
void CSmlDmAOCommandElement::SetStatus( CSmlDmAdapter::TError aStatus )
    {
    iStatus = aStatus;
    }
    
    
// -----------------------------------------------------------------------------
// Sets the iData member of the object. Ownership of aData argument will be 
// transfered to the CSmlDmAOCommandElement
// -----------------------------------------------------------------------------
//
void CSmlDmAOCommandElement::SetData( HBufC8* aData )
    {
    iData = aData;
    }

