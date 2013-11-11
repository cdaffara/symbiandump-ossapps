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
#ifndef CNPSEARCHDOCUMENT_H
#define CNPSEARCHDOCUMENT_H

//  INCLUDES
#include "CNPSearchDocumentInterface.h"
#include "CNPDocumentFieldInterface.h"

// FORWARD DECLARATION
class CSearchDocument; 

// CLASS DECLARATION

/**
 * CCPixNPSearchDocument
 * Implements services described in MCPixNPSearchDocument. Provides an access
 * to the associated entity in the index database, e.g. information describing
 * SMS messages, documents etc.  
 * 
 * NOTE: Because the CNPDocumentField:s hold dependency to data owned by this 
 * class, this class MUST NOT be destroyed before all dependent CNPDocumentFields
 * are released. For this reason this class introduces a simple reference counting
 * for CNPDocumentField:s
 */
class CNPSearchDocument : public CNPSearchDocumentInterface
    {
    friend class CNPDocumentField; 
    
public:
	/**
	 * @param aDocument ownership transferred
	 */
    static CNPSearchDocument * NewL( CSearchDocument* aDocument );
    virtual ~CNPSearchDocument();
    
    void Deallocate(); // From CObjectInterface
    
private:
	CNPSearchDocument( CSearchDocument* aDocument );
    void ConstructL();
    
protected: // for CNPDocumentField
	
	void Retain(); // increases reference count by one
	void Release(); // decreases reference count by one
    
public: // from MNPSearchDocument
	
	virtual NPDocumentFieldObject* FieldL(const TDesC& aName);
	virtual const TDesC& Id() const;
	virtual const TDesC& AppClass() const;
	virtual const TDesC& Excerpt() const;
	virtual TInt FieldCount() const;
	virtual NPDocumentFieldObject* FieldL( const TInt aIndex );

private:
	
	CSearchDocument* iDocument;
	
	TInt iReferences; 
	
    };

#endif /*CNPSEARCHDOCUMENT_H*/
