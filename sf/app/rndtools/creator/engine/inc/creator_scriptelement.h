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




#ifndef CREATORSCRIPTELEMENT_H_
#define CREATORSCRIPTELEMENT_H_

#include "creator_randomdatafield.h"
#include "creator_modulebase.h"
#include "engine.h"
#include <e32base.h>

class CCreatorScriptElement;
class CCreatorEngine;

_LIT(KEmpty, "");
_LIT(KContextSep, "::");

// Common attributes:
_LIT(KAmount, "amount");
_LIT(KRandomLength, "randomlength");
_LIT(KId, "id");
_LIT(KMaxAmount, "maxamount");

// Common element names:
_LIT(KScript, "creatorscript");
_LIT(KFields, "fields");
_LIT(KContactSetRef, "contact-set-reference");
_LIT(KExistingContacts, "numberofexistingcontacts");

// Common attribute values:
_LIT(KMax, "max");
_LIT(KDefault, "default");
_LIT(KIncrease, "incvalueforeachcopy");

class CCreatorScriptElementCache : public CBase
{
public:
    static CCreatorScriptElementCache* CCreatorScriptElementCache::NewL();
    virtual ~CCreatorScriptElementCache();    
        
    void RemoveElements();    
    void AddElementL(CCreatorScriptElement* aElement);
private:
    CCreatorScriptElementCache();
    void ConstructL();
    RPointerArray<CCreatorScriptElement> iElementCache;

};


class CCreatorScriptAttribute : public CBase
{
public:
    static CCreatorScriptAttribute* NewL(const TDesC& aName, const TDesC& aValue);
    static CCreatorScriptAttribute* NewLC(const TDesC& aName, const TDesC& aValue);
    virtual ~CCreatorScriptAttribute();    
    
    TPtrC Name() const;
    void SetNameL(const TDesC& aName);      
    TPtrC Value() const;
    void SetValueL(const TDesC& aValue);
    
protected:

    CCreatorScriptAttribute();
    virtual void ConstructL(const TDesC& aName, const TDesC& aValue);
    
private:
    HBufC* iName;
    HBufC* iValue;    
};

/**
 * Base class for all elements
 */
class CCreatorScriptElement : public CBase
{
public:

    static CCreatorScriptElement* CCreatorScriptElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);

    /**
     * Destructor
     */
    virtual ~CCreatorScriptElement();
    
    /**
     * Returns the array of sub-elements.
     * @return Reference to the sub-element array.
     */
    virtual RPointerArray<CCreatorScriptElement> const& SubElements() const;
    virtual RPointerArray<CCreatorScriptElement>& SubElements();
    
    /**
     * Returns a sub-element.
     * @param aIndex Sub-element array index.
     * @return Pointer to a sub-element
     */ 
    virtual CCreatorScriptElement* SubElement(TInt aIndex);
    
    /**
     * Finds a sub-element by a name.
     * @param aName Name of the sub-element
     * @return Pointer to the first matching sub-element. NULL if not found.
     */
    virtual CCreatorScriptElement* FindSubElement(const TDesC& aName);
    
    /**
     * Removes all sub-elements.
     */
    virtual void RemoveSubElements();
    
    /**
     * Removes a sub-element
     * @param aIndex Index of the element to be removed
     */
    virtual void RemoveSubElementL(TInt aIndex);
    
    /**
     * Adds an element to the sub-element list. 
     * Leaves with error KErrNotSupported, if sub-element is illegal.
     * Leaves with error KErrArgument, if the index in out of bounds.
     * @param aElem Pointer to the element to be added. Takes ownership of the pointer.
     * @param aIndex Sub-element array index where to add the element. -1 means that
     * the element is added to the end of the array.
     */
    virtual void AddSubElementL(CCreatorScriptElement* aElem, TInt aIndex = -1);
  
    /**
     * Returns the array of attributes.
     * @return Reference to the attribute array.
     */
    virtual RPointerArray<CCreatorScriptAttribute> const& Attributes() const;
        
    /**
     * Returns an attribute.
     * @param aIndex Attribute array index.
     * @return Pointer to a attribute
     */ 
    virtual CCreatorScriptAttribute* Attribute(TInt aIndex);
    
    /**
     * Removes all attributes.
     */
    virtual void RemoveAttributes();
    
    /**
     * Removes an attribute
     * @param aIndex Index of the attribute to be removed
     */
    virtual void RemoveAttributeL(TInt aIndex);
    
    /**
     * Adds an attribute to the attribute list. 
     * Leaves with error KErrNotSupported, if attribute is illegal.
     * @param aAttribute Pointer to the element to be added. Takes ownership of the pointer.
     * @param aIndex Attribute array index where to add the attribute. -1 means that
     * the attribute is added to the end of the array.
     */
    virtual void AddAttributeL(CCreatorScriptAttribute* aAttribute, TInt aIndex = -1);
    
    /**
     * Finds attribute with the given name. First match is returned.
     */
    virtual const CCreatorScriptAttribute* FindAttributeByName(const TDesC& aName) const;
    virtual CCreatorScriptAttribute* FindAttributeByName(const TDesC& aName);

    /**
     * Returns element name
     * @return Element name
     */
    virtual TPtrC Name() const;
    
    /**
     * Sets the element name.
     * @param aName Element name
     */
    virtual void SetNameL(const TDesC& aName);
    
    /**
     * Returns element content
     * @return Element content
     */
    virtual TPtrC Content() const;
    
    /**
     * Set the content of the element.
     * @param aContenct Element content
     */
    virtual void SetContentL(const TDesC& aContent);
    virtual void AppendContentL(const TDesC& aContent);
    
    /**
     * Returns element context
     * @return Element context
     */
    virtual TPtrC Context() const;
        
    /**
     * Set the context of the element.
     * @param aContenct Element context
     */
    virtual void SetContextL(const TDesC& aContext);
    
    /**
     * Tells whether the element should be cached for future use.
     * @return ETrue if the element should be cached for future use, EFalse otherwise.
     */
    virtual TBool IsCacheNeeded();
    virtual void AddToCacheL(CCreatorScriptElementCache& aCache);
    virtual void AddToCacheL();
    virtual TBool IsCommandElement() const;
    virtual void ExecuteCommandL();
    //virtual void SaveCommandResultsL();
    //virtual void DiscardCommandResultsL();
    virtual TBool IsRoot() const;
    virtual RPointerArray<CCreatorModuleBaseParameters>& CommandParameters();
    virtual const RPointerArray<CCreatorModuleBaseParameters>& CommandParameters() const;
    
protected:

    /**
     * Constructors. 
     */    
    CCreatorScriptElement(CCreatorEngine* aEngine = 0);
    
    /**
     * @param aName Name of the element.
     */
    virtual void ConstructL(const TDesC& aName, const TDesC& aContext = KNullDesC);
    
    /**
     * Tells whethet the sub-element is allowed or not.
     * @param aElem Sub-element to be tested.
     * @return Boolean value telling whether the sub-element is allowed 
     * to be added or not.
     */
    virtual TBool IsSubElementSupported(const CCreatorScriptElement& aElem) const;
    
    virtual MCreatorRandomDataField::TRandomLengthType ResolveRandomDataTypeL(const CCreatorScriptAttribute& aAttr, TInt& aRandomLen ) const;
    
    virtual TBool ConvertStrToBooleanL(const TDesC& aStr) const;
    virtual TInt ConvertStrToIntL(const TDesC& aStr) const;
    virtual TUint ConvertStrToUintL(const TDesC& aStr) const;
    virtual void ConvertStrToReal64L(const TDesC& aStr, TReal64& aVal) const;
    virtual void ConvertStrToReal32L(const TDesC& aStr, TReal32& aVal) const;
    
    virtual void AppendContactSetReferenceL(const CCreatorScriptElement& aContactSetRefElem, RArray<TLinkIdParam>& aLinkArray ) const;    
    
    virtual void SetContentToTextParamL(HBufC*& aPtr, const TDesC& aContent );
    
    virtual TTime ConvertToDateTimeL(const TDesC& aDtStr) const;
    
    virtual TInt CompareIgnoreCase(const TDesC& aStr1, const TDesC& aStr2 ) const;
    
    /**
     * Increases phonenumber by aDelta.
     * 
     * Special cases, that are handled:
     * +9          -> +9, +10, +11...
     * +3584098#99 -> +3584098#99, +3584098#100, +3584098#101...
     * #           -> #0, #1, #2...
     * 123#        -> 123#0, 123#1, 123#2...
     * 099         -> 099, 100, 101...
     * 
     * @param aOriginal original phonenumber
     * @param aDelta number to be added to original number. Must be >= 0.
     * @param aIncreased on return contains the increased number.
     *        The buffer must be allocated by the caller.
     */
    void IncreasePhoneNumL( const TDesC& aOriginal, TInt aDelta, HBufC* aIncreased ) const;
    
protected:
    
    // Sub-element array
    RPointerArray<CCreatorScriptElement> iSubElements;
    // Attribute array
    RPointerArray<CCreatorScriptAttribute> iAttributes;
    // Element name (e.g. "contact")
    HBufC* iName;
    // Element content
    HBufC* iContent;
    // Context
    HBufC* iContext;
    
    TBool iIsCommandElement;
    TBool iIsRoot;
    CCreatorEngine* iEngine;
    RPointerArray<CCreatorModuleBaseParameters> iParameters;
};

/**
 * Script element
 */
class CCreatorScriptRoot : public CCreatorScriptElement
{
public:
    static CCreatorScriptRoot* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
protected:
    CCreatorScriptRoot(CCreatorEngine* aEngine);
};

/**
 * Calendar elements
 */

/**
 * Base class for calendar elements
 */
class CCreatorCalendarElementBase : public CCreatorScriptElement
{
public:
    static CCreatorCalendarElementBase* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
protected:
    CCreatorCalendarElementBase(CCreatorEngine* aEngine);
};

/**
 * Message elements
 */

/**
 * Base class for message elements
 */
class CCreatorMessageElementBase : public CCreatorScriptElement
{
public:
    static CCreatorMessageElementBase* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
protected:
    CCreatorMessageElementBase(CCreatorEngine* aEngine);
};

#endif /*CREATORSCRIPTELEMENT_H_*/
