/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONXMLELEMENT_H__
#define __SCONXMLELEMENT_H__

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <e32base.h>
#include "sconxmlstack.h"

// -----------------------------------------------------------------------------
// Class forwards
// -----------------------------------------------------------------------------
class MWBXMLConMLCallback;
struct TXMLElementParams;

// -----------------------------------------------------------------------------
// CXMLElement - Base class for XML -elements
// -----------------------------------------------------------------------------
NONSHARABLE_CLASS( CXMLElement ) : public CBase
    {
public:
    enum TAction 
        {
        ENone,
        EPop,
        EPopAndDestroy // Pop from stack, callback, delete
        };

    /**
    * Sets the data of an element
    * @param aData - Data of the element
    * @return none
    */
    virtual void SetDataL( const TDesC8& aData );
    
    /**
    * Returns data of an element
    * @param none
    * @return Data of the element as TPtrC8
    */
    virtual TPtrC8 Data() const;
        
    /**
    * Validates an element's instance 
    * @param none
    * @return ETrue if element current instance is valid, 
    * EFalse if not 
    */
    virtual TBool Validate();
    
    /**
    * Checks if instance needs cleanup
    * @param none
    * @return ETrue if instance pushed into 
    * parser's cleanupstack , EFalse if not
    */
    virtual TBool NeedsCleanup() const;
    
    /**
    * Offers a new element to this element
    * @param aTag - tag of the element to be started 
    * @param aParams - Element parameters
    * @return new Element instance as CXMLElement
    */
    virtual CXMLElement* BeginElementL( 
        TUint8 aTag, const TXMLElementParams& aParams );

    /**
    * Offers a new element to this element
    * @param aTag - tag of the element to be started 
    * @param aParams - Element parameters
    * @return new Element instance as CXMLElement
    */
    virtual TAction EndElementL( TAny* aCallbacks, TUint8 aTag );
    };

// -----------------------------------------------------------------------------
// TElementParams
// -----------------------------------------------------------------------------
NONSHARABLE_STRUCT( TXMLElementParams )
    {
public:
    inline TXMLElementParams();
    TXMLElementParams( 
        TAny* aCallbacks, 
        CXMLStack<CXMLElement>* aCmdStack = 0, 
        CXMLStack<CXMLElement>* aCleanupStack = 0 );
        
    /**
    * Returns the call back function of parameters
    * @param nonoe
    * @return call back function as TAny
    */
    inline TAny* Callbacks() const;
    
    /**
    * Returns the command stack of the parameters
    * @param none
    * @return command stack as CXMLStack
    */
    inline CXMLStack<CXMLElement>* CmdStack() const;

    /**
    * Returns the clean up stack of the parameters
    * @param none
    * @return clean up stack as CXMLStack
    */
    inline CXMLStack<CXMLElement>* CleanupStack() const;

private:
    TAny* iCallbacks;
    CXMLStack<CXMLElement>* iCmdStack;
    CXMLStack<CXMLElement>* iCleanupStack;
    };

#include "sconxmlelement.inl"

#endif // __SCONXMLELEMENT_H__