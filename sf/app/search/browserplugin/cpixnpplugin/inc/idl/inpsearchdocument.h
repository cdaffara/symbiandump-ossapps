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
#ifndef INPSEARCHDOCUMENT_H_
#define INPSEARCHDOCUMENT_H_

struct NPDocumentFieldObject; 

/**
 * CSearchDocument represents one entity in index database, e.g. it can appear in search result.
 * Each document may have multiple fields, which values may or may not be indexed and stored to database.
 * See CDocumentField for details.
 */
class MNPSearchDocument
	{
public:
	
	/**
	 * FIXME: Add L to mark leaving
	 * 
	 * Find field by name. Returns NULL, if field is not found with given name.
	 * @param aName Name of the field.
	 * @return First field with given name or NULL if no field is found.
	 */
	virtual NPDocumentFieldObject* FieldL(const TDesC& aName) = 0;
	
	/**
	 * Id of this document.
	 * @return Id of this document.
	 */
	virtual const TDesC& Id() const = 0;

	/**
	 * Application class of this document.
	 * @return Id of this document.
	 */
	virtual const TDesC& AppClass() const = 0;

	/**
	 * Excerpt of this document.
	 * @return Id of this document.
	 */
	virtual const TDesC& Excerpt() const = 0;

	/**
	 * Number of fields in this document.
	 * @return Number of fields in this document.
	 */
	virtual TInt FieldCount() const = 0;

	/**
	 * FIXME: Add L to mark leaving
	 * 
	 * Returns a field specified by the index. 
	 * Returns NULL if index is negative or greater than the number of fields currently in document.
	 * @param aIndex Index of a field to be returned.
	 * @return Field pointed by the index, or NULL, if index is out of bounds
	 */
	virtual NPDocumentFieldObject* FieldL( const TInt aIndex ) = 0;
	};

#endif /*INPSEARCHDOCUMENT_H_*/
