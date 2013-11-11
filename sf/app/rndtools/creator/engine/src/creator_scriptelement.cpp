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



#include "creator_traces.h"
#include "creator_scriptelement.h"
#include <xml/documentparameters.h>
#include <collate.h>
#include <etel3rdparty.h> // KMaxTelNumberSize

using namespace Xml;

CCreatorScriptElementCache* CCreatorScriptElementCache::NewL()
{
    CCreatorScriptElementCache* self = new (ELeave) CCreatorScriptElementCache;
    CleanupStack::PushL(self);
    
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CCreatorScriptElementCache::CCreatorScriptElementCache()
    {
    
    }

void CCreatorScriptElementCache::ConstructL()
    {
    
    }
 
CCreatorScriptElementCache::~CCreatorScriptElementCache()
    {
    iElementCache.Reset();
    iElementCache.Close();
    }
    
void CCreatorScriptElementCache::RemoveElements()
    {
    iElementCache.Reset();
    }

        
void CCreatorScriptElementCache::AddElementL(CCreatorScriptElement* aElement)
    {
    iElementCache.AppendL(aElement);
    }

/**
 * 
 */
CCreatorScriptAttribute* CCreatorScriptAttribute::NewLC(const TDesC& aName, const TDesC& aValue)
    {
    CCreatorScriptAttribute* self = new(ELeave) CCreatorScriptAttribute();
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValue);
    return self;
    }

CCreatorScriptAttribute* CCreatorScriptAttribute::NewL(const TDesC& aName, const TDesC& aValue)
    {
    CCreatorScriptAttribute* self = CCreatorScriptAttribute::NewLC(aName, aValue);
    CleanupStack::Pop(); // self    
    return self;
    }

CCreatorScriptAttribute::~CCreatorScriptAttribute()
    {
    delete iName;
    delete iValue;
    }
    
TPtrC CCreatorScriptAttribute::Name() const
    {
    return iName->Des();
    }

void CCreatorScriptAttribute::SetNameL(const TDesC& aName) 
    {
    if( iName )
        {
        delete iName;
        iName = 0;
        }    
    iName = HBufC::NewL(aName.Length());
    iName->Des() = aName; 
    }       
    
TPtrC CCreatorScriptAttribute::Value() const
    {
    return iValue->Des();
    }

void CCreatorScriptAttribute::SetValueL(const TDesC& aValue)
    {
    if( iValue )
        {
        delete iValue;
        iValue = 0;
        }
    iValue = HBufC::NewL(aValue.Length());
    iValue->Des() = aValue; 
    }   

CCreatorScriptAttribute::CCreatorScriptAttribute()
    {    
    }

void CCreatorScriptAttribute::ConstructL(const TDesC& aName, const TDesC& aValue)
    {
    SetNameL(aName);
    SetValueL(aValue);
    }

/**
 * CCreatorScriptElement
 */

CCreatorScriptElement* CCreatorScriptElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    CCreatorScriptElement* self = new (ELeave) CCreatorScriptElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }

CCreatorScriptElement::~CCreatorScriptElement()
    {
    LOGSTRING("Creator: CCreatorScriptElement::~CCreatorScriptElement");
    iSubElements.ResetAndDestroy();
    iAttributes.ResetAndDestroy();
    iSubElements.Close();
    iAttributes.Close();
    iParameters.Reset();
    iParameters.Close();
    delete iName;
    delete iContent;
    delete iContext;
    }

RPointerArray<CCreatorScriptElement> const& CCreatorScriptElement::SubElements() const
    {
    return iSubElements;
    }

RPointerArray<CCreatorScriptElement>& CCreatorScriptElement::SubElements()
    {
    return iSubElements;
    }
    
CCreatorScriptElement* CCreatorScriptElement::SubElement(TInt aIndex)
    {
    return iSubElements[aIndex];
    }

CCreatorScriptElement* CCreatorScriptElement::FindSubElement(const TDesC& aName)
    {
    for( TInt i = 0; i < iSubElements.Count(); ++i )
        {
        if( iSubElements[i]->Name() == aName )
            {
            return iSubElements[i];
            }            
        }
    return 0;
    }
    
void CCreatorScriptElement::RemoveSubElements()
    {
    iSubElements.ResetAndDestroy();
    }
    
void CCreatorScriptElement::RemoveSubElementL(TInt aIndex)
    {
    if( aIndex < iSubElements.Count())
        {
        delete iSubElements[aIndex];
        iSubElements.Remove(aIndex);
        }
    else
        {
        User::Leave(KErrArgument);
        }
    }
    
void CCreatorScriptElement::AddSubElementL(CCreatorScriptElement* aElem, TInt aIndex )
    {
    if( aIndex == -1 || aIndex == iSubElements.Count() )
        {
        iSubElements.AppendL(aElem);
        }
    else if( aIndex < iSubElements.Count())
        {
        iSubElements.InsertL(aElem, aIndex);
        }
    else
        {
        User::Leave(KErrArgument);
        }
    }
  
RPointerArray<CCreatorScriptAttribute> const& CCreatorScriptElement::Attributes() const
    {
    return iAttributes;
    }

        
 CCreatorScriptAttribute* CCreatorScriptElement::Attribute(TInt aIndex)
     {
     return iAttributes[aIndex];
     }
    
void CCreatorScriptElement::RemoveAttributes()
    {
    iAttributes.ResetAndDestroy();
    }
    
 
void CCreatorScriptElement::RemoveAttributeL(TInt aIndex)
    {
    if( aIndex < iAttributes.Count())
        {
        delete iAttributes[aIndex];
        iAttributes.Remove(aIndex);
        }
    else
        {
        User::Leave(KErrArgument);
        }
    }
 
void CCreatorScriptElement::AddAttributeL(CCreatorScriptAttribute* aAttribute, TInt aIndex )
    {
    if( aIndex == -1 || aIndex == iAttributes.Count() )
        {
        iAttributes.AppendL(aAttribute);
        }
    else if( aIndex < iAttributes.Count())
        {
        iAttributes.InsertL(aAttribute, aIndex);        
        }
    else
        {
        User::Leave(KErrArgument);
        }
    }

TPtrC CCreatorScriptElement::Name() const
    {
    if( iName )
        return iName->Des();
    return TPtrC();
    }

void CCreatorScriptElement::SetNameL(const TDesC& aName)
    {
    if( iName )
        {
        delete iName;
        iName = 0;
        }    
    iName = HBufC::NewL(aName.Length());
    iName->Des() = aName; 
    }

 
TPtrC CCreatorScriptElement::Content() const
    {
    if (iContent)
        return iContent->Des();
    return TPtrC();
    }
    
void CCreatorScriptElement::SetContentL(const TDesC& aContent)
    {
    if( iContent )
        {
        delete iContent;
        iContent = 0;
        }    
    iContent = HBufC::NewL(aContent.Length());
    iContent->Des() = aContent;
    }

void CCreatorScriptElement::AppendContentL(const TDesC& aContent)
    {
    if( iContent == 0 )
        {
        SetContentL(aContent);
        return;
        }
            
    iContent = iContent->ReAllocL(iContent->Length() + aContent.Length() );
    
    TPtr ptr(iContent->Des());
    ptr += aContent;
    }

TPtrC CCreatorScriptElement::Context() const
    {
    if( iContext )
        return iContext->Des();
    return TPtrC();
    }
    
void CCreatorScriptElement::SetContextL(const TDesC& aContext)
    {
    if( iContext )
        {
        delete iContext;
        iContext = 0;
        }    
    iContext = HBufC::NewL(aContext.Length());
    iContext->Des() = aContext;
    }

const CCreatorScriptAttribute* CCreatorScriptElement::FindAttributeByName(const TDesC& aName) const
    {
    for( TInt i = 0; i < iAttributes.Count(); ++i )
        {
        CCreatorScriptAttribute* attr = iAttributes[i];
        if( attr->Name() == aName )
            {
            return attr;
            }
        }
    return 0;
    }

CCreatorScriptAttribute* CCreatorScriptElement::FindAttributeByName(const TDesC& aName)
    {    
    for( TInt i = 0; i < iAttributes.Count(); ++i )
        {
        CCreatorScriptAttribute* attr = iAttributes[i];
        if( attr->Name() == aName )
            {
            return attr;
            }
        }
    return 0;
    }

TBool CCreatorScriptElement::IsCacheNeeded()
    {
    _LIT(KIDAttrName, "id");
    const CCreatorScriptAttribute* attr = FindAttributeByName(KIDAttrName);
    return attr != 0;
    }

void CCreatorScriptElement::AddToCacheL(CCreatorScriptElementCache& aCache)
    {
    aCache.AddElementL(this);
    }

RPointerArray<CCreatorModuleBaseParameters>& CCreatorScriptElement::CommandParameters()
    {
    return iParameters;
    }

const RPointerArray<CCreatorModuleBaseParameters>& CCreatorScriptElement::CommandParameters() const
    {
    return iParameters;
    }

void CCreatorScriptElement::AddToCacheL()
    {    
    }

TBool CCreatorScriptElement::IsCommandElement() const
    {
    return iIsCommandElement;
    }

void CCreatorScriptElement::ExecuteCommandL()
    {
    
    }
   
TBool CCreatorScriptElement::IsRoot() const
    {
    _LIT(KRootName, "creatorscript");
    if(iName->Des() == KRootName)
        return ETrue;
    return EFalse;
    }

CCreatorScriptElement::CCreatorScriptElement(CCreatorEngine* aEngine)
: 
iIsCommandElement(EFalse),
iIsRoot(EFalse),
iEngine(aEngine)
    {}
    
void CCreatorScriptElement::ConstructL(const TDesC& aName, const TDesC& aContext)
    {
    SetNameL(aName);
    SetContextL(aContext);
    }
    
TBool CCreatorScriptElement::IsSubElementSupported(const CCreatorScriptElement& /*aElem*/) const
    {
    return ETrue;
    }

TBool CCreatorScriptElement::ConvertStrToBooleanL(const TDesC& aStr) const
    {
    TBool boolVal = EFalse;
    _LIT(KYes, "yes");
    _LIT(KTrue, "true");
    if( CompareIgnoreCase(aStr, KYes) == 0 ||
        CompareIgnoreCase(aStr, KTrue) == 0 )
        {
        boolVal = ETrue;
        }    
    return boolVal;
    }

TInt CCreatorScriptElement::ConvertStrToIntL(const TDesC& aStr) const
    {
    TInt intVal = 0;
    TLex lex(aStr);
    TInt errorCode=lex.Val(intVal);
    User::LeaveIfError(errorCode);
    return intVal;
    }

TUint CCreatorScriptElement::ConvertStrToUintL(const TDesC& aStr) const
	{
	TUint uintVal = 0;
	TLex lex(aStr);
	TInt errorCode=lex.Val(uintVal);
	User::LeaveIfError(errorCode);
	return uintVal;
	}
void CCreatorScriptElement::ConvertStrToReal64L(const TDesC& aStr, TReal64& aVal) const
    {    
    TLex lex(aStr);
    TInt errorCode=lex.Val(aVal);
    User::LeaveIfError(errorCode);    
    }

void CCreatorScriptElement::ConvertStrToReal32L(const TDesC& aStr, TReal32& aVal) const
    {    
    TLex lex(aStr);
    TInt errorCode=lex.Val(aVal);
    User::LeaveIfError(errorCode);    
    }

MCreatorRandomDataField::TRandomLengthType CCreatorScriptElement::ResolveRandomDataTypeL(const CCreatorScriptAttribute& aAttr, TInt& aRandomLen) const
    {    
    TPtrC attrVal = aAttr.Value();
    if(attrVal == KMax )
        return MCreatorRandomDataField::ERandomLengthMax;
    if(attrVal == KDefault)
        return MCreatorRandomDataField::ERandomLengthDefault;
    
    // Let's see if the value is numeric:
    TInt val = 0;
    TLex lex(attrVal);
    TInt errorCode=lex.Val(val);
    if( errorCode == KErrNone )   
        {
        aRandomLen = val;
        return MCreatorRandomDataField::ERandomLengthExplicit;
        }
    return MCreatorRandomDataField::ERandomLengthUndefined;
    }

void CCreatorScriptElement::AppendContactSetReferenceL(
        const CCreatorScriptElement& aContactSetRefElem, 
        RArray<TLinkIdParam>& aLinkArray ) const
        {
        const TDesC& eName = aContactSetRefElem.Name();
        if( eName != KContactSetRef )
            return;
        
        const CCreatorScriptAttribute* linkIdAttr = aContactSetRefElem.FindAttributeByName(KId);
        if( linkIdAttr )
            {
            TInt linkId = ConvertStrToIntL(linkIdAttr->Value());
            if( linkId > 0 )
                {
                TLinkIdParam linkParams;
                // Add contact-set-reference id to links
                linkParams.iLinkId = linkId;
                linkParams.iLinkAmount = KUndef; // undefined
                // Resolve maxamount:
                const CCreatorScriptAttribute* maxAmount = aContactSetRefElem.FindAttributeByName(KMaxAmount);
                if( maxAmount )
                    {                                
                    TInt maxAmountVal = ConvertStrToIntL(maxAmount->Value());
                    if( maxAmountVal > 0 )
                        {
                        linkParams.iLinkAmount = maxAmountVal;
                        }
                    }
                aLinkArray.AppendL(linkParams);
                }
            }
        }       

TTime CCreatorScriptElement::ConvertToDateTimeL(const TDesC& aDtStr) const
    {
    _LIT(KDateFieldSeparator, "-");
    _LIT(KTimeFieldSeparator, ":");    
    _LIT(KDateTimeSeparator, "T");    
    _LIT(KTimeSuffix, ".");
    _LIT(KDateSuffix, ":");
    // Format date-time string:
    HBufC* formatted = HBufC::NewLC(aDtStr.Length());
    formatted->Des().Copy(aDtStr);
    
    TBool hasTimePart = EFalse; 
    TBool hasDateTimeSeparator = EFalse;
    TInt pos = 0;
    
    // Date and month numbering starts from 0 in Symbian, so first
    // we need to decrease the date and month by one. Script format is following:
    // yyyy-mm-ddThh:mm:ss
    // Remove also date field separators ('-')
    while( (pos = formatted->Find(KDateFieldSeparator)) != KErrNotFound )
        {
        // decrease month or date by one
        
        // First char. Can be zero also:
        TInt newValue = 0;
        const TPtrC& char1 = formatted->Des().Mid(pos+1).Left(1);
        newValue = 10 * ConvertStrToIntL(char1);
        // Next char:
        const TPtrC& char2 = formatted->Des().Mid(pos+2).Left(1);
        newValue += ConvertStrToIntL(char2);
       
        if( newValue > 0 )
            {
            --newValue;
            }
        
        _LIT(KTemp, "%d");
        HBufC* formatBuf = KTemp().AllocLC();
        HBufC* buf = 0;
        if( newValue < 10 )
            buf = HBufC::NewLC(1);            
        else
            buf = HBufC::NewLC(2);
                        
        TPtr temp(buf->Des());
        temp.Format(*formatBuf, newValue);
        if( newValue < 10 )
            {
            formatted->Des()[pos+1] = TChar('0');
            formatted->Des()[pos+2] = buf->Des()[0];
            }
        else
            {
            formatted->Des()[pos+1] = buf->Des()[0];
            formatted->Des()[pos+2] = buf->Des()[1];
            }
        // Finally, delete the '-' separator:
        formatted->Des().Delete(pos, 1);        
        CleanupStack::PopAndDestroy(2);
        }
    
    while( (pos = formatted->Find(KTimeFieldSeparator)) != KErrNotFound )
        {
        formatted->Des().Delete(pos, 1);
        hasTimePart = ETrue;
        }
    
    // Replace 'T' with ':':
    if( (pos = formatted->Find(KDateTimeSeparator)) != KErrNotFound )
        {
        formatted->Des().Replace(pos, 1, KDateSuffix);
        hasDateTimeSeparator = ETrue;
        }
    
    if( hasTimePart )
        formatted->Des().Append(KTimeSuffix);
    else if( !hasDateTimeSeparator )
        formatted->Des().Append(KDateSuffix);
    
    
    TTime ret;
    ret.Set(*formatted);    
    CleanupStack::PopAndDestroy(); // formatted
    return ret;
    }

TInt CCreatorScriptElement::CompareIgnoreCase(const TDesC& aStr1, const TDesC& aStr2 ) const
    {
    // Get default collation method:
    TCollationMethod defaultCollationMethod = *Mem::CollationMethodByIndex(0); 
    
    // Add ignore case flag:
    defaultCollationMethod.iFlags |= TCollationMethod::EFoldCase;
    
    return aStr1.CompareF(aStr2);
    }

void CCreatorScriptElement::SetContentToTextParamL(HBufC*& aPtr, const TDesC& aContent )
    {
    delete aPtr;
    aPtr = 0;
    aPtr = HBufC::NewL(aContent.Length());
    aPtr->Des().Copy(aContent);
    }

/**
 * Increases phonenumber by aDelta.
 */
void CCreatorScriptElement::IncreasePhoneNumL( const TDesC& aOriginal, TInt aDelta, HBufC* aIncreased ) const
    {
    LOGSTRING("Creator: CCreatorMessageElement::IncreasePhoneNumL");
    __ASSERT_ALWAYS( aDelta >= 0, User::Panic( _L("IncreasePhoneNumL"), KErrArgument ) );
    
    // special cases, that are handled:
    // +9          -> +9, +10, +11...
    // +3584098#99 -> +3584098#99, +3584098#100, +3584098#101...
    // #           -> #0, #1, #2...
    // 123#        -> 123#0, 123#1, 123#2... 
    // 099         -> 099, 100, 101...
    
    // find out if there are any special characters, like # p or *, in the original number
    TInt startIndex( aOriginal.Length() -1 );
    while ( startIndex >= 0 && 
            aOriginal[startIndex] >= '0' &&
            aOriginal[startIndex] <= '9' )
        {
        startIndex--;
        }
    startIndex++;
    
    // append original head that may contain any non number characters 
    aIncreased->Des().Append( aOriginal.Left( startIndex ) );
    
    TBuf<CTelephony::KMaxTelNumberSize> tailBuf;
    if ( aOriginal.Length() > startIndex )
        {
        tailBuf.Copy( aOriginal.Right( aOriginal.Length() -startIndex ) );
        }
    
    // parse the tail part of the original number
    TInt64 intVal = 0;
    if ( aOriginal.Length() > startIndex )
        {
        TLex lex( tailBuf );
        User::LeaveIfError( lex.Val( intVal ) ); // this loses leading zeros
        }
    
    // increase
    intVal += aDelta;
    
    // restore leading zeros to tail, if any
    TBuf<CTelephony::KMaxTelNumberSize> resultTailNoZeros;
    resultTailNoZeros.AppendNum( intVal );
    TInt tailLeadingZerosToRestore = tailBuf.Length() - resultTailNoZeros.Length();
    for ( TInt i = 0; i < tailLeadingZerosToRestore; i++ )
        {
        aIncreased->Des().AppendNum( 0 );
        }

    // and finally, append the increased value as tail part of the result
    aIncreased->Des().Append( resultTailNoZeros );    
    }

CCreatorScriptRoot* CCreatorScriptRoot::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    CCreatorScriptRoot* self = new (ELeave) CCreatorScriptRoot(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorScriptRoot::CCreatorScriptRoot(CCreatorEngine* aEngine)
:
CCreatorScriptElement(aEngine)
    {
    iIsRoot=ETrue;
    }


CCreatorCalendarElementBase* CCreatorCalendarElementBase::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    CCreatorCalendarElementBase* self = new (ELeave) CCreatorCalendarElementBase(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorCalendarElementBase::CCreatorCalendarElementBase(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    { }

CCreatorMessageElementBase* CCreatorMessageElementBase::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    CCreatorMessageElementBase* self = new (ELeave) CCreatorMessageElementBase(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorMessageElementBase::CCreatorMessageElementBase(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    { }
