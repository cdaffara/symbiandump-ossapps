/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CapParser header file
*
*/



#ifndef _CAPPARSER_H_
#define _CAPPARSER_H_

// INCLUDES

#include <e32base.h>
#include <badesca.h>

//
// Class CapParser
//
NONSHARABLE_CLASS( CapParser )
    {

public:
    /**
     * Parses element's id and type (eg. "<Memory>" -> EMemory, EElementBegin)
     * @param aText
     * @param aId
     * @param aType
     * @return none
     */
    static void ParseElement(const TDesC& aText, TInt& aId, TInt& aType);
    
public:
    /**
     * Constructs element with value (eg. "<Free>23456</Free>").
     * @param aText
     * @param aId
     * @param aValue
     * @return none
     */
    static void MakeElementL(TDes& aText, TInt aId, const TDesC& aValue);   
    /**
     * Constructs element without value (eg. "<Memory>" or "</Memory>").
     * @param aText
     * @param aId
     * @param aType
     * @return none
     */
    static void MakeElementL(TDes& aText, TInt aId, TInt aType);
    /**
     * Constructs element with attributes "Version" and "Date"(eg. 
     * "<FW Version="2.2"/>").
     * @param aText
     * @param aId
     * @param aVersion
     * @param aDate
     * @return none
     */
    static void MakeElementL(TDes& aText, TInt aId, const TDesC& aVersion, 
                            const TDesC& aDate);
    /**
     * Constructs element with attributes
     * @param aText
     * @param aId
     * @param aAttributes
     * @return none
     */
    static void MakeElementL(TDes& aText, TInt aId, CDesCArrayFlat* aAttributes);
    
public:
    /**
     * Returns capability element id (eg. EMemory).
     * @param aText
     * @return none
     */
    static TInt ElementId(const TDesC& aText);
    /**
     * Returns capability element tag (eg. "Memory").
     * @param aText
     * @param aId
     * @return none
     */
    static void GetTagL(TDes& aText, TInt aId);
    
private:
    /**
     * Replaces special characters to xml compliant.
     * @param aText
     * @param aValue
     * @return none
     */
    static void ReplaceSpecialCharsL( TDes& aText, const TDesC& aValue );
    };


//
// Class TXmlParser
//
NONSHARABLE_CLASS( TXmlParser )
    {

public:
    enum TElementType
        {
        EElementValue,
        EElementBegin,
        EElementEnd,
        EElementEmpty,
        EElementAtt,
        EElementComment,
        EElementVersion,
        EElementDocType,
        EElementUnknown
        };


public:
    /**
     * Sets a tag
     * @param aText The tag
     * @return none
     */
    void Set(const TDesC& aText);
    /**
     * Returns type
     * @return type
     */
    TInt Type() const;
    /**
     * Returns a pointer to a tag
     * @return The tag pointer
     */
    TPtrC Tag() const;
    /**
     * Gets the tag
     * @param aText The tag buffer
     * @return none
     */
    void GetTag(TDes& aText) const;

private:
    /**
     * Parses value (eg. <Language>en</Language>)
     * @return Operation success
     */
    TBool ElementValue();
    /**
     * Parses beginning tag (eg. <General>)
     * @return Operation success
     */
    TBool ElementBegin();
    /**
     * Parses ending tag (eg. </General>)
     * @return Operation success
     */
    TBool ElementEnd();
    /**
     * Parses empty element (eg. <CaseSenN/>)
     * @return Operation success
     */
    TBool ElementEmpty();
    /**
     * Parses attribute element (eg. <HW Version="1.5"/>)
     * Note: Attribute values cannot contain equals (=) or quotations (")
     * @return Operation success
     */
    TBool ElementAtt();
    /**
     * Identifies comment element ("<!-- OBEX Capability Object -->").
     * @return Operation success
     */
    TBool ElementComment();
    /**
     * Parses unknown elements
     * @return Operation success
     */
    TBool ElementUnknown();
    /**
     * Identifies version element ("<?xml version="1.0" ?>") 
     * @return Operation success
     */
    TBool ElementVersion();
    /**
     * Identifies document type element 
     * ("<!DOCTYPE Capability SYSTEM "obex-capability.dtd">").
     * @return Operation success
     */
    TBool ElementDocType();

private:
    /**
     * Inits the parser
     * @return none
     */
    void Init();
    /**
     * Parses xml-element.  
     * @return none
     */
    void Parse();

public:
    /**
     * Validates the character
     * @param aChar The char to be validated.
     * @return ETrue if the char is valid
     */
    static TBool ValidChar(const TChar aChar);
    /**
     * Validates the tag
     * @param aText The tag to be validated.
     * @return ETrue if the tag is valid
     */
    static TBool ValidTag(const TDesC& aText);
    /**
     * Returns the number of characters (aChar)
     * @param aText The buffer
     * @param aChar The character
     * @return The number of characters
     */
    static TInt Count(const TDesC& aText, const TChar aChar);
    /**
     * Returns the position of the first non space character
     * @param aText The buffer
     * @return The position
     */
    static TInt FirstNonSpace(const TDesC& aText);
    /**
     * Returns the position of the last non space character
     * @param aText The buffer
     * @return The position
     */
    static TInt LastNonSpace(const TDesC& aText);
    /**
     * Function returns a sub-string between aPos1 and aPos2
     * @param aText The buffer
     * @param aText Start position
     * @param aText End position
     * @return The pointer to the substring
     */
    static TPtrC SubStr(const TDesC& aText, TInt aPos1, TInt aPos2);
    /**
     * Function performs string copy with length checking.
     * @param aTarget Target buffer
     * @param aSource Source buffer
     * @return none
     */
    static void StrCopy(TDes& aTarget, const TDesC& aSource);

private:    
    /**
     * Returns a string
     * @return A pointer to a string
     */
    TPtrC Str() const;
    /**
     * Trimmer function
     * @return none
     */
    void TrimOriginal();
    /**
     * Function returns sub string between aPos1 and aPos2
     * @param aPos1 Start position
     * @param aPos2 End posiotion
     * @return none
     */
    TPtrC SubStr(TInt pos1, TInt pos2);

private:
    TPtrC iOriginal;
    TPtrC iText;
    TPtrC iTag;
    TPtrC iValue;
    TInt  iType;

    TInt iSlash;
    TInt iFirst;
    TInt iLast;
    };

#endif 

// End of file


