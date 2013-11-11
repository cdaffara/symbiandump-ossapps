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
#ifndef CNPDOCUMENTFIELD_H
#define CNPDOCUMENTFIELD_H

//  INCLUDES
#include "CNPDocumentFieldInterface.h"

// FORWARD DECLARATION
class CDocumentField; 
class CNPSearchDocument; 

// CLASS DECLARATION

/**
 * CNPDocumentField
 * Implements services described in MCPixNPSearcher. 
 * 
 * Maintains a reference count in the surrounding CNPSearchDocument, because 
 * CNPSearchDocument owns the object referred by iField. If CNPSearchDocument 
 * were to be destroyed, iField reference would become a dangerous reference
 * to invalid memory location. 
 */
class CNPDocumentField : public CNPDocumentFieldInterface
    {
public:
	
	/**
	 * @param aDocument ownership transferred
	 */
    static CNPDocumentField *NewL( CNPSearchDocument& aOwner, const CDocumentField& aField );
    virtual ~CNPDocumentField();
    
private:
	
	CNPDocumentField( CNPSearchDocument& aOwner, const CDocumentField& aField );
    void ConstructL();
    
public: // from MNPDocumentField
	
	virtual const TDesC& Name() const;

	virtual const TDesC& Value() const;
	
private:

	CNPSearchDocument& iOwner;

	// Don't held ownership 
	const CDocumentField& iField;
	
    };

#endif /*CNPDOCUMENTFIELD_H*/
