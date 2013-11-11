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
* Description:  CapParser implementation
*
*/


// INCLUDE FILES

#include "capparser.h"
#include "caputils.h"

const char KFind1 = '&';
const char KFind2 = '<';
const char KFind3 = '>';
const char KFind4 = '"';
const char KFind5 = '\'';

_LIT( KReplace1, "&amp;" );
_LIT( KReplace2, "&lt;" );
_LIT( KReplace3, "&gt;" );
_LIT( KReplace4, "&quot;" );
_LIT( KReplace5, "&apos;" );

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CapParser::GetTagL(TDes& aText, TInt aId)
// Returns capability element tag (eg. "Memory")
// -----------------------------------------------------------------------------
//
void CapParser::GetTagL(TDes& aText, TInt aId)
    {
    aText=KNullDesC;
    
    TInt count=NUMXMLTOKENS;
    for (TInt i=0; i<count; i++)
        {
        TXmlToken t=KXmlTokens[i];
        if (t.id==aId)
            {
            aText=t.tag;
            return;
            }
        }
    User::Leave(KErrNotFound);
    }

// -----------------------------------------------------------------------------
// CapParser::ElementId(const TDesC& aText)
// Returns capability element id (eg. EMemory).
// -----------------------------------------------------------------------------
//
TInt CapParser::ElementId(const TDesC& aText)
    {
    TBuf<KTagSize>buf;
    TInt count=NUMXMLTOKENS;
    for (TInt i=0; i<count; i++)
        {
        TXmlToken t=KXmlTokens[i];
        buf=t.tag;
        if (buf.Compare(aText)==0)
            {
            return t.id;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CapParser::ParseElement(const TDesC& aText, TInt& aId, TInt& aType)
// Parses element's id and type (eg. "<Memory>" -> EMemory, EElementBegin)
// -----------------------------------------------------------------------------
//
void CapParser::ParseElement(const TDesC& aText, TInt& aId, TInt& aType)
    {
    TXmlParser parse;
    aId=KErrNotFound;
    
    parse.Set(aText);
    aType=parse.Type();
    if (aType==TXmlParser::EElementUnknown)
        {
        return;
        }

    TPtrC ptr=parse.Tag();
    aId=ElementId(ptr);
    }

// -----------------------------------------------------------------------------
// CapParser::MakeElementL(TDes& aText, TInt aId, const TDesC& aValue)
// Constructs element with value (eg. "<Free>23456</Free>").
// -----------------------------------------------------------------------------
//
void CapParser::MakeElementL(TDes& aText, TInt aId, const TDesC& aValue)
    {
    aText=KNullDesC;
    ReplaceSpecialCharsL( aText, aValue );
    
    TBuf<KTagSize> buf;
    MakeElementL( buf, aId, TXmlParser::EElementBegin );
    
    TInt len = aText.Length() + buf.Length() + buf.Length() + 1;
    if ( len > aText.MaxLength())
        {
        User::Leave(KErrTooBig);
        }
    
    aText.Insert( 0, buf );
    MakeElementL( buf, aId, TXmlParser::EElementEnd );
    aText.Append( buf );
    }

// -----------------------------------------------------------------------------
// CapParser::MakeElementL(TDes& aText, TInt aId, TInt aType)
// Constructs element without value (eg. "<Memory>" or "</Memory>").
// -----------------------------------------------------------------------------
//
void CapParser::MakeElementL(TDes& aText, TInt aId, TInt aType)
    {
    _LIT(KFormat1, "<%S>");
    _LIT(KFormat2, "</%S>");
    _LIT(KFormat3, "<%S/>");

    aText=KNullDesC;
    TBuf<KTagSize> buf;
    GetTagL(buf, aId);
    
    switch(aType)
        {
        case TXmlParser::EElementBegin:
            aText.Format(KFormat1, &buf);
            break;
        case TXmlParser::EElementEnd:
            aText.Format(KFormat2, &buf);
            break;
        case TXmlParser::EElementEmpty:
            aText.Format(KFormat3, &buf);
            break;
        default:
            CapUtil::Panic(KErrArgument);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CapParser::MakeElementL(TDes& aText, TInt aId, const TDesC& aVersion,
// const TDesC& aDate)
// Constructs element with attributes "Version" and "Date"(eg. 
// "<FW Version="2.2"/>").
// -----------------------------------------------------------------------------
//
void CapParser::MakeElementL(TDes& aText, TInt aId, const TDesC& aVersion, 
                            const TDesC& aDate)
    {
    _LIT(KFormat1, "<%S %S=\"%S\" %S=\"%S\"/>");
    _LIT(KFormat2, "<%S %S=\"%S\"/>");

    aText=KNullDesC;
    
    TBuf<KTagSize> element;
    TBuf<KTagSize> versionTag;
    TBuf<KTagSize> dateTag;
    
    GetTagL(element, aId);
    GetTagL(versionTag, EAttVersion);
    GetTagL(dateTag, EAttDate);

    if (aDate.Length()>0)
        {
        aText.Format(KFormat1, &element, &versionTag, &aVersion, &dateTag, 
        &aDate);
        }       
    else
        {
        aText.Format(KFormat2, &element, &versionTag, &aVersion);
        }
    }

// -----------------------------------------------------------------------------
// CapParser::ReplaceSpecialCharsL( TDes& aText, const TDesC& aValue )
// Replaces special characters to xml compliant.
// -----------------------------------------------------------------------------
//
void CapParser::ReplaceSpecialCharsL( TDes& aText, const TDesC& aValue )
    {
    if ( aValue.Length() > aText.MaxLength() )
        {
        User::Leave(KErrTooBig);
        }
    aText.Copy( aValue );
    
    //Replace special characters
    for( TInt i = 0; i < aText.Length(); i++ )
        {
        switch( aText[i] )
            {
            case KFind1:
                if ( aText.Length()+KReplace1().Length()-1 > aText.MaxLength() )
                    {
                    User::Leave(KErrTooBig);
                    }
                aText.Replace( i, 1, KReplace1 );
                i+=KReplace1().Length()-1;
                break;
            case KFind2:
                if ( aText.Length()+KReplace2().Length()-1 > aText.MaxLength() )
                    {
                    User::Leave(KErrTooBig);
                    }
                aText.Replace( i, 1, KReplace2 );
                i+=KReplace2().Length()-1;
                break;
            case KFind3:
                if ( aText.Length()+KReplace3().Length()-1 > aText.MaxLength() )
                    {
                    User::Leave(KErrTooBig);
                    }
                aText.Replace( i, 1, KReplace3 );
                i+=KReplace3().Length()-1;
                break;
            case KFind4:
                if ( aText.Length()+KReplace4().Length()-1 > aText.MaxLength() )
                    {
                    User::Leave(KErrTooBig);
                    }
                aText.Replace( i, 1, KReplace4 );
                i+=KReplace4().Length()-1;
                break;
            case KFind5:
                if ( aText.Length()+KReplace5().Length()-1 > aText.MaxLength() )
                    {
                    User::Leave(KErrTooBig);
                    }
                aText.Replace( i, 1, KReplace5 );
                i+=KReplace5().Length()-1;
                break;
            default:
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// TXmlParser::Set(const TDesC& aText)
// Sets a tag
// -----------------------------------------------------------------------------
//
void TXmlParser::Set(const TDesC& aText)
    {
    iOriginal.Set(aText);
    TrimOriginal();
    Init();
    Parse();
    }

// -----------------------------------------------------------------------------
// TXmlParser::Init()
// Inits the parser
// -----------------------------------------------------------------------------
//
void TXmlParser::Init()
    {
    iType=EElementUnknown;
    iTag.Set(KNullDesC);
    iValue.Set(KNullDesC);
    }

// -----------------------------------------------------------------------------
// TXmlParser::GetTag(TDes& aText) const
// Gets the tag
// -----------------------------------------------------------------------------
//
void TXmlParser::GetTag(TDes& aText) const
    {
    StrCopy(aText, iTag);
    }

// -----------------------------------------------------------------------------
// TXmlParser::Tag() const
// Returns a pointer to a tag
// -----------------------------------------------------------------------------
//
TPtrC TXmlParser::Tag() const
    {
    return iTag;
    }

// -----------------------------------------------------------------------------
// TXmlParser::Type() const
// Returns type
// -----------------------------------------------------------------------------
//
TInt TXmlParser::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// TXmlParser::Parse()
// Parses xml-element.  
// -----------------------------------------------------------------------------
//
void TXmlParser::Parse()
    {
    iType=EElementUnknown;
    
    // this must be the first check
    if (ElementUnknown())
        {
        return;
        }       
    
    if (ElementBegin())
        {
        return;
        }
    
    if (ElementEnd())
        {
        return;
        }
        
    if (ElementValue())
        {
        return;
        }
            
    if (ElementComment())
        {
        return;
        }
        
    if (ElementDocType())
        {
        return;
        }
        
    if (ElementVersion())
        {
        return;
        }
        
    if (ElementEmpty())
        {
        return;
        }
        
    if (ElementAtt())
        {
        return;
        }
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementUnknown()
// Parses unknown elements
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementUnknown()
    {
    TPtrC str=Str();
    TInt len=str.Length();
    const TInt KXmlElementMinLength = 3;
    if ( len < KXmlElementMinLength )
        {
        return ETrue;  // too short to be xml element
        }
        
    
    iFirst=str.Locate('<');
    iLast=str.LocateReverse('>');
    iSlash=str.LocateReverse('/');

    if (iFirst!=0 || iLast!=len-1)
        {
        return ETrue;  // first char must be "<" and last ">"
        }
        
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementBegin()
// Parses beginning tag (eg. <General>)
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementBegin()
    {
    TPtrC str=Str();
        
    if (iSlash!=KErrNotFound)
        {
        return EFalse;
        }
        
    // remove possible attributes
    TInt pos=str.Locate(' ');
    if (pos==KErrNotFound)
        {
        pos=iLast;
        }
        
    TPtrC ptr=SubStr(iFirst, pos);

    if (!ValidTag(ptr))
        {
        return EFalse;
        }       

    iType=EElementBegin;
    iTag.Set(ptr);

    return ETrue;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementEnd()
// Parses ending tag (eg. </General>)
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementEnd()
    {
    if (iSlash!=iFirst+1)
        {
        return EFalse;
        }
        
    TPtrC ptr=SubStr(iSlash, iLast);
    if (!ValidTag(ptr))
        {
        return EFalse;
        }       

    iType=EElementEnd;
    iTag.Set(ptr);
    return ETrue;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementValue()
// Parses value (eg. <Language>en</Language>)
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementValue()
    {
    TPtrC str=Str();

    TInt pos1=str.Locate('>');
    TInt pos2=str.LocateReverse('<');
    
    if (pos1<pos2 && iSlash==pos2+1)
        {
        TPtrC ptr1=SubStr(iFirst, pos1);
        TPtrC ptr2=SubStr(iSlash, iLast);
        if (!ValidTag(ptr1) || ptr1.Compare(ptr2)!=0)
            {
            return EFalse;
            }
                    
        iType=EElementValue;
        iTag.Set(ptr1);
        iValue.Set(SubStr(pos1, pos2));
        return ETrue;
        }
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementEmpty()
// Parses empty element (eg. <CaseSenN/>)
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementEmpty()
    {
    if (iSlash!=iLast-1)
        {
        return EFalse;
        }       
    
    TPtrC ptr=SubStr(iFirst, iSlash);
    if (!ValidTag(ptr))
        {
        return EFalse;
        }
        
    iType=EElementEmpty;
    iTag.Set(ptr);
    return ETrue;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementVersion()
// Identifies version element ("<?xml version="1.0" ?>") 
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementVersion()
    {
    TPtrC str=Str();

    if ( str.Length()>=3 && str[1]=='?' && str[2]=='x')
        {
        iType=EElementVersion;
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementDocType()
// Identifies document type element 
// ("<!DOCTYPE Capability SYSTEM "obex-capability.dtd">").
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementDocType()
    {
    TPtrC str=Str();
    const TInt KElementMinLength = 10;
    if ( str.Length() < KElementMinLength )
        {
        return EFalse;
        }
        
    if (str[1]=='!' && str[2]=='D' && str[3]=='O' && str[4]=='C')
        {
        iType=EElementDocType;
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementComment()
// Identifies comment element ("<!-- OBEX Capability Object -->").
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementComment()
    {
    TPtrC str=Str();
    
    if (str.Length()<7)
        {
        return EFalse;
        }
        
    if (str[1]=='!' && str[2]=='-' && str[3]=='-')
        {
        iType=EElementComment;
        return ETrue;
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ElementAtt()
// Parses attribute element (eg. <HW Version="1.5"/>)
// Note: Attribute values cannot contain equals (=) or quotations (")
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ElementAtt()
    {
    TPtrC str=Str();
    TInt num1=Count(str, '=');
    TInt num2=Count(str, '"');
    TInt pos=str.Locate(' ');  // find end of tag
    
    if (iSlash==iLast-1 && num1>0 && num2==(2*num1) && pos!=KErrNotFound)
        {
        TPtrC ptr1=SubStr(iFirst, pos);
        if (!ValidTag(ptr1))
            {
            return EFalse;
            }
            
        iType=EElementAtt;
        iTag.Set(ptr1);
        TPtrC ptr2=SubStr(pos, iSlash);
        iValue.Set(ptr2);
        return ETrue;
        }
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// TXmlParser::Str() const
// Locates the position of the given character
// -----------------------------------------------------------------------------
//
TPtrC TXmlParser::Str() const
    {
    return iText;
    }

// -----------------------------------------------------------------------------
// TXmlParser::FirstNonSpace(const TDesC& aText)
// Returns the position of the first non space character
// -----------------------------------------------------------------------------
//
TInt TXmlParser::FirstNonSpace(const TDesC& aText)
    {
    TInt len=aText.Length();
    for (TInt i=0; i<len; i++)
        {
        TChar c=aText[i];
        if (!c.IsSpace())
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// TXmlParser::LastNonSpace(const TDesC& aText)
// Returns the position of the last non space character
// -----------------------------------------------------------------------------
//
TInt TXmlParser::LastNonSpace(const TDesC& aText)
    {
    TInt last=aText.Length()-1;
    for (TInt i=last; i>=0; i--)
        {
        TChar c=aText[i];
        if (!c.IsSpace())
            {
            return i;
            }           
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// TXmlParser::SubStr(const TDesC& aText, TInt pos1, TInt pos2)
// Function returns a sub-string between aPos1 and aPos2
// -----------------------------------------------------------------------------
//
TPtrC TXmlParser::SubStr(const TDesC& aText, TInt pos1, TInt pos2)
    {
    if (pos1<0 || pos2>=aText.Length() || pos1>=pos2)
        {
        return TPtrC(); // return empty descriptor
        }
        
    TInt len=pos2-pos1;
    return aText.Mid(pos1+1, len-1); 
    }

// -----------------------------------------------------------------------------
// TXmlParser::SubStr(TInt pos1, TInt pos2)
// Function returns a sub-string between aPos1 and aPos2
// -----------------------------------------------------------------------------
//
TPtrC TXmlParser::SubStr(TInt pos1, TInt pos2)
    {
    TPtrC str=Str();
    return SubStr(str, pos1, pos2);
    }

// -----------------------------------------------------------------------------
// TXmlParser::StrCopy(TDes& aTarget, const TDesC& aSource)
// Function performs string copy with length checking.
// -----------------------------------------------------------------------------
//
void TXmlParser::StrCopy(TDes& aTarget, const TDesC& aSource)
    {
    TInt len=aTarget.MaxLength();
    if(len<aSource.Length()) 
        {
        aTarget.Copy(aSource.Left(len));
        }        
    else
        {
        aTarget.Copy(aSource);
        }
    }

// -----------------------------------------------------------------------------
// TXmlParser::Count(const TDesC& aText, const TChar aChar)
// Returns the number of characters (aChar)
// -----------------------------------------------------------------------------
//
TInt TXmlParser::Count(const TDesC& aText, const TChar aChar)
    {
    TInt count=0;
    TInt len=aText.Length();

    for (TInt i=0; i<len; i++)
        {
        TChar ch=aText[i];
        if (ch == aChar)
            {
            count++;
            }
        }
    return count;
    }

// -----------------------------------------------------------------------------
// TXmlParser::TrimOriginal()
// Trimmer function
// -----------------------------------------------------------------------------
//
void TXmlParser::TrimOriginal()
    {
    // trim iOriginal and place into iText
    iText.Set(KNullDesC);
    TInt first=FirstNonSpace(iOriginal);
    TInt last=LastNonSpace(iOriginal);
    if(first!=KErrNotFound && last!=KErrNotFound && last>first+1)
        {
        TPtrC ptr=iOriginal.Mid(first, last-first+1);
        iText.Set(ptr);
        }
    }

// -----------------------------------------------------------------------------
// TXmlParser::ValidChar(const TChar aChar)
// Validates the character
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ValidChar(const TChar aChar)
    {
    if (aChar >= 'A' && aChar <= 'Z')
        {
        return ETrue;
        }
    if (aChar >= 'a' && aChar <= 'z')
        {
        return ETrue;
        }
        
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TXmlParser::ValidTag(const TDesC& aText)
// Validates the tag
// -----------------------------------------------------------------------------
//
TBool TXmlParser::ValidTag(const TDesC& aText)
    {
    TInt len=aText.Length();
    for (TInt i=0; i<len; i++)
        {
        if (!ValidChar(aText[i]))
            {
            return EFalse;
            }           
        }
    return ETrue;
    }
    
// End of file
