// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file contains the definition of the SMILDTD class 
// 
//
/**
 * @file 
 * @publishedPartner
 * @released
 */
#ifndef __SMILDTD_H__
#define __SMILDTD_H__

#include <e32def.h>
#include <gmxmldocument.h>
#include "smildtdenum.h"

/**
MXMLDtd is a mixin class. If User wants  to  derive from MXMLDtd class ,
then  one  should not derive from any other class at the same time. 
i.e A class can not derive from CBase  at all  using MXMLDtd.

Represents the SMIL DTD.
It is used for validation of SMIL documents. 
@publishedPartner
@released
*/
class CSMILDtd: public MXMLDtd
	{
public:	

	/** Allocates and constructs a new SMIL DTD validator.
	
	@leave KErrNoMemory Out of memory	
	@return New SMIL DTD validator */
	IMPORT_C static CSMILDtd* NewL();

	/** Allocates and constructs a new SMIL DTD validator, leaving the object on the 
	cleanup stack.

	@leave KErrNoMemory Out of memory		
	@return New SMIL DTD validator */
	IMPORT_C static CSMILDtd* NewLC();

	/** Destructor. */
  	IMPORT_C virtual ~CSMILDtd();

protected:
	/*
	 * Function to determine whether an Element name is valid in DTD
	 * @return ETrue the if element name valid - else EFalse
	 * @param aElement the element name to be tested
	 * @leave can leave due to OOM
	 */
	virtual TBool IsValidElementL(const TDesC& aElement) const;

	/*
	 * Function to determine whether an attribute name and value is valid in DTD
	 * @return KErrNone if name&value are valid, KXMLBadAttributeName if attrib name invalid
	 * KXMLBadAttributeValue if attrib value invalid
	 * @param aElement the element to which the attributes belong
	 * @param aAttribute the attribute to be tested
	 * @leave aAttributeValue the attributeValue to be tested
	 * @leave can leave due to OOM
	 */
	
	virtual TInt IsValidAttributeForElementL(const TDesC& aElement, const TDesC& aAttribute, const TDesC& aAttributeValue) const;

	/*
	 * Function to determine whether the parent/child relationship is valid in DTD
	 * @return ETrue if parent/child relationship is valid
	 * @param aParentElement the name of the parent element to be tested
	 * @param aChildElements an array of child element name to be tested
	 * @leave can leave due to OOM
	 */
	
	virtual TBool AreValidChildElementsL(const TDesC& aParentElement, const CDesCArray& aChildElements) const;


	/*
	 * Function to determine whether it is valid for a particular element to
	 * have children
	 * @param aElement the name of the element to be tested
	 * @return ETrue if it is valid for element to have children
	 */
	virtual TBool CanElementHaveChildren(const TDesC& aElement) const;


	/*
	 * Constructor
	 */
	CSMILDtd();

private:
	/*
	 * Second stage constructor
	 */
	void ConstructL();
	
	TBool CheckValidChildren(const TSMILDTDChildStateType aStateTrans[],TInt aStateCount, const CDesCArray& aChildElements) const;

	};

#endif



