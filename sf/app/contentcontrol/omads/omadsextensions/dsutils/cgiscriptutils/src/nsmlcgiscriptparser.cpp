/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CGI parser and generator
*
*/



// INCLUDE FILES
#include <escapeutils.h>

#include "nsmlcgiscriptparser.h"
#include "nsmlconstants.h" //PtrArrCleanupItem


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::TNSmlCGIScriptParser
// -----------------------------------------------------------------------------
//
EXPORT_C TNSmlCGIScriptParser::TNSmlCGIScriptParser()
    {
    //logical separator table initialization
    KNSmlCGIScriptLogSep[ENSmlCGIScriptLogicalOperatorAnd] = &KNSmlCGIScriptLogicalSeparatorAndStr;
    KNSmlCGIScriptLogSep[ENSmlCGIScriptLogicalOperatorOr] = &KNSmlCGIScriptLogicalSeparatorOrStr;

    //logical operator table initialization
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorEqualToCaseSensitive] = &KNSmlCGIScriptComparatorEqualToCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorEqualToCaseInSensitive] = &KNSmlCGIScriptComparatorEqualToCaseInSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorNotEqualToCaseSensitive] = &KNSmlCGIScriptComparatorNotEqualToCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorNotEqualToCaseInSensitive] = &KNSmlCGIScriptComparatorNotEqualToCaseInSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorGreaterThanCaseSensitive] = &KNSmlCGIScriptComparatorGreaterThanCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorGreaterThanCaseInSensitive] = &KNSmlCGIScriptComparatorGreaterThanCaseInSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorGreaterThanOrEqualToCaseSensitive] = &KNSmlCGIScriptComparatorGreaterThanOrEqualToCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorGreaterThanOrEqualToCaseInSensitive] = &KNSmlCGIScriptComparatorGreaterThanOrEqualToCaseInSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorLessThanCaseSensitive] = &KNSmlCGIScriptComparatorLessThanCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorLessThanCaseInSensitive] = &KNSmlCGIScriptComparatorLessThanCaseInSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorLessThanOrEqualCaseSensitive] = &KNSmlCGIScriptComparatorLessThanOrEqualCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorLessThanOrEqualCaseInSensitive] = &KNSmlCGIScriptComparatorLessThanOrEqualCaseInSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorContainsValueCaseSensitive] = &KNSmlCGIScriptComparatorContainsValueCaseSensitiveStr; 
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorContainsValueCaseInSensitive] = &KNSmlCGIScriptComparatorContainsValueCaseInSensitiveStr;
    KNSmlCGIScriptLogOps[ENSmlCGIScriptComparatorNull] = &KNSmlCGIScriptNullStr;

    //special usage table initialization
    KNSmlCGIScriptSpecialUsageStrings[0] = &KNSmlCGIScriptNullValue;
    KNSmlCGIScriptSpecialUsageStrings[1] = &KNSmlCGIScriptLuidValue;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::ParseL
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlCGIScriptParser::ParseL(
    CNSmlCGIScript& aSp, 
    const CArrayPtr<TNSmlDataTypesForCGIScriptNames>& aDatatypes) const
    {

    aSp.Clear();

    TNSmlCGIScriptParseStateInfo psInfo(aSp.CGIScript(), &aDatatypes);
    
    if (psInfo.iCGIScript == NULL)
        {
        return;
        }
    
    TNSmlCGIScriptParseData pd;

    //Main parse loop
    for ( psInfo.iCurrPos = 0; psInfo.iCurrPos < psInfo.iCGIScript->Length();
        psInfo.iCurrPos++ )
        {
        if ( IsParseSplitPoint(*psInfo.iCGIScript, psInfo.iCurrPos) )
            {
            for (psInfo.iState = psInfo.iNextState; ParseStateL(psInfo, pd); 
                 psInfo.iState = psInfo.iNextState )
                {
                }

            if ( pd.iParseDataReady )
                {
                AddScriptPartL( aSp, pd );
                pd.iParseDataReady = EFalse;
                }
            psInfo.iStartPos = psInfo.iCurrPos + 1;
            }
        }

    // parsing script's last value
    psInfo.iState = psInfo.iNextState;
    ParseStateL( psInfo, pd );
    if ( pd.iParseDataReady )
        {
        AddScriptPartL( aSp, pd );
        }
    else{
        User::Leave( ENSmlCGIParserErrorParsing );
        }

    aSp.SetLogicalOperator( pd.iSeparator );
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::GenerateL
// -----------------------------------------------------------------------------
//
EXPORT_C void TNSmlCGIScriptParser::GenerateL(CNSmlCGIScript& aSp) const
    {
    typedef CArrayPtrFlat<HBufC> CBufParts;
    CBufParts* bufParts = new (ELeave) CBufParts(aSp.Count());
    CleanupStack::PushL( PtrArrCleanupItem(HBufC, bufParts) );
    TInt i;

    //generates all script parts and appends them to array.
    for (i = 0;i < aSp.Count();i++)
        {
        const TNSmlCGIScriptPart* sp = aSp.Get(i);
        HBufC* bufTmp = GenerateScriptPartL(*sp);

        CleanupStack::PushL(bufTmp);
        bufParts->AppendL(bufTmp);
        CleanupStack::Pop(bufTmp);
        }

    //collects all parts to a single CGI script.
    SetCGIScriptL(aSp, *bufParts);
        
    CleanupStack::PopAndDestroy(bufParts);
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::IsParseSplitPoint
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::IsParseSplitPoint(const TDesC& aCGIScript, TInt aStartFrom) const
    {
    if (aCGIScript[aStartFrom] == '&')
        {
        for (TInt i(0); i < KNSmlCGIScriptSpecialUsageStringsCount; i++)
            {
            if (Compare(*KNSmlCGIScriptSpecialUsageStrings[i], aCGIScript, aStartFrom))
                {
                return EFalse;
                }
            }
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::ParseStateL
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::ParseStateL(
    TNSmlCGIScriptParseStateInfo & aPSInfo, 
    TNSmlCGIScriptParseData & aPD) const
    {
    TBool retV(ETrue);

    switch (aPSInfo.iState)
        {
        case ENSmlCGIScriptParseStateKeyWord:
            aPD.iKeyword.Set(aPSInfo.iCGIScript->Mid(aPSInfo.iStartPos, 
                aPSInfo.iCurrPos - aPSInfo.iStartPos));
            aPD.iDataType = FindDataTypeL(*aPSInfo.iDatatypes, aPD.iKeyword);

            if (aPD.iDataType == ENSmlCGIScriptDataTypeNoValue)
                {
                aPSInfo.iNextState = ENSmlCGIScriptParseStateLogSep;
                aPD.iValue.Set(TPtrC());
                aPD.iComparator = ENSmlCGIScriptComparatorNull;
                aPD.iParseDataReady = ETrue;
                }
            else
                {
                aPSInfo.iNextState = ENSmlCGIScriptParseStateLogOp;
                }
            break;
        case ENSmlCGIScriptParseStateLogOp:
            aPD.iComparator = LogOpL(*aPSInfo.iCGIScript,
                aPSInfo.iCurrPos + 1);
            aPSInfo.iCurrPos += LogOpL(aPD.iComparator).Length() - 1;
            aPSInfo.iNextState = ENSmlCGIScriptParseStateValue;
            retV = EFalse;
            break;
        case ENSmlCGIScriptParseStateValue:
            {
            aPD.iValue.Set(aPSInfo.iCGIScript->Mid(aPSInfo.iStartPos, 
                aPSInfo.iCurrPos - aPSInfo.iStartPos));

            aPSInfo.iNextState = ENSmlCGIScriptParseStateLogSep;
            aPD.iParseDataReady = ETrue;
            break;
            }
        case ENSmlCGIScriptParseStateLogSep:
            {
            TNSmlCGIScriptLogicalOperator loTmp(
                LogSepL(*aPSInfo.iCGIScript, aPSInfo.iCurrPos + 1) );

            if ( aPD.iSeparator != ENSmlCGIScriptLogicalOperatorNull && 
                (loTmp != aPD.iSeparator) )
                {
                //Error: query may contain only 1 type of logical separator
                User::Leave(ENSmlCGIParserErrorWrongSeparator);
                }
            aPD.iSeparator = loTmp;

            aPSInfo.iCurrPos += LogSepL( aPD.iSeparator ).Length() - 1;
            aPSInfo.iNextState = ENSmlCGIScriptParseStateKeyWord;
            retV = EFalse;
            }
            break;
        default:
            User::Leave(ENSmlCGIParserErrorParsing);
            break;
        }
    return retV;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::AddScriptPartL
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::AddScriptPartL(CNSmlCGIScript& aSp, 
    TNSmlCGIScriptParseData& aPD)
    {

    TNSmlCGIScriptDataType datatype(aPD.iDataType);

    TNSmlCGIScriptPart* sp = new (ELeave) TNSmlCGIScriptPart;
    CleanupStack::PushL(sp);

    sp->iData = NULL;
    sp->iName = HBufC::NewLC(aPD.iKeyword.Length());
    *(sp->iName) = aPD.iKeyword;

    sp->iComparator = aPD.iComparator;
    sp->iDataType = datatype;

    TBool retV = ParseScriptPartDataLC(*sp, aPD.iValue);

    CheckScriptPartValidityL(*sp);

    if (retV)
        {
        aSp.AddL(sp);
        CleanupStack::Pop(3); //ParseScriptPartDataLC:stä, sp->iName,sp;
        }
    else
        {
        CleanupStack::PopAndDestroy(2); //sp->iName, sp
        sp->iName = NULL;
        sp = NULL;
        }
    return retV;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::ParseScriptPartDataLC
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::ParseScriptPartDataLC(
    TNSmlCGIScriptPart& aSp, 
    const TPtrC& aValue)
    {
    TNSmlCGIScriptDataType datatype( (aValue == KNSmlCGIScriptNullValue) 
        ? ENSmlCGIScriptDataTypeNull : aSp.iDataType );

    TBool retV(ETrue);

    // in each case - sentence, either push something to cleanupstack, 
    // set retV to EFalse or leave
    switch (datatype)
        {
        case ENSmlCGIScriptDataTypeHBufC:
            {
            aSp.iData = EscapeUtils::EscapeDecodeL(aValue);
            CleanupStack::PushL(aSp.iData);
            }
            break;
        case ENSmlCGIScriptDataTypeDateTime:
        case ENSmlCGIScriptDataTypeDate:
            {
            TDateTime* dt = new (ELeave) TDateTime();
            CleanupStack::PushL(dt);
            TInt num = ParseDateTimeL(*dt, aValue);

            switch(num)
                {
                case KNSmlCGIParserDateTimeLen:
                    aSp.iDataType = ENSmlCGIScriptDataTypeDateTime;
                    break;
                case KNSmlCGIParserDateLen:
                    aSp.iDataType = ENSmlCGIScriptDataTypeDate;
                    break;
                default:
                    User::Leave(ENSmlCGIParserErrorConversion);
                    break;
                }
            aSp.iData = dt;
            }
            break;
        case ENSmlCGIScriptDataTypeNumber:
            {
            TInt* num = new (ELeave) TInt();
            CleanupStack::PushL(num);
            *num = ParseIntL(aValue, 0, aValue.Length());
            aSp.iData = num;
            }
            break;
        case ENSmlCGIScriptDataTypeBoolean:
            {
            TBool* bln = new (ELeave) TBool;
            CleanupStack::PushL(bln);
            *bln = ParseBoolL(aValue);
            aSp.iData = bln;
            }
            break;
        case ENSmlCGIScriptDataTypeNoValue:
        case ENSmlCGIScriptDataTypeNull:
            aSp.iData = NULL;
            CleanupStack::PushL(aSp.iData);
            break;
        default: // TNSmlCGIScriptDataTypeUnKnown
            User::Leave(ENSmlCGIParserErrorConversion);
            break;
        }

    return retV;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::FindDataTypeL
// -----------------------------------------------------------------------------
//
TNSmlCGIScriptDataType TNSmlCGIScriptParser::FindDataTypeL(
    const CArrayPtr<TNSmlDataTypesForCGIScriptNames>& aDatatypes, 
    const TPtrC& aKeyword)
    {
    TNSmlCGIScriptDataType dt(FindDataType(aDatatypes, aKeyword));
    if (dt == ENSmlCGIScriptDataTypeUnKnown)
        {
        User::Leave(ENSmlCGIParserErrorDataTypeNotFound);
        }
    return dt;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::FindDataType
// -----------------------------------------------------------------------------
//
TNSmlCGIScriptDataType TNSmlCGIScriptParser::FindDataType(
    const CArrayPtr<TNSmlDataTypesForCGIScriptNames>& aDatatypes, 
    const TPtrC& aKeyword)
    {
    TNSmlDataTypesForCGIScriptNames* tmp;

    for (TInt i = 0; i < aDatatypes.Count(); i++)
        {
        tmp = aDatatypes[i];
        if (*tmp->iKeywordOrProperty == aKeyword)
            {
            return tmp->iDataType;
            }
        }
    return ENSmlCGIScriptDataTypeUnKnown;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::GenerateScriptPartL
// -----------------------------------------------------------------------------
//
HBufC* TNSmlCGIScriptParser::GenerateScriptPartL(
    const TNSmlCGIScriptPart& aSp) const
    {

    CheckScriptPartValidityL(aSp);

    TInt len(LogOpL(aSp.iComparator).Length() + aSp.iName->Length());
    TAny* value = NULL;
    TBool destroyValue(EFalse);
    TNSmlCGIScriptDataType dataType(aSp.iDataType);

    if (aSp.iData == NULL && dataType != ENSmlCGIScriptDataTypeNoValue)
        {
        dataType = ENSmlCGIScriptDataTypeNull;
        }

    if (dataType == ENSmlCGIScriptDataTypeHBufC)
        {
        HBufC* buf = EscapeUtils::EscapeEncodeL( 
            *reinterpret_cast<HBufC*>(aSp.iData), KNSmlCGIParserReservedChars );
        CleanupStack::PushL(buf);
        len += buf->Length();
        destroyValue = ETrue;
        value = buf;
        }
    else
        {
        len += 32;              //enough for datetime, number, boolean etc.
        value = reinterpret_cast<TAny*>(aSp.iData);
        }

    HBufC* bufTmp = HBufC::NewLC(len);
    TPtr ptr(bufTmp->Des());

    ptr.Append(*aSp.iName);
    ptr.Append( LogOpL(aSp.iComparator) );
    GenerateScriptPartValueL(dataType, value, ptr);

    CleanupStack::Pop(bufTmp);

    if (destroyValue)
        {
        CleanupStack::PopAndDestroy(value);
        }

    return bufTmp;

    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::GenerateScriptPartValueL
// -----------------------------------------------------------------------------
//
void TNSmlCGIScriptParser::GenerateScriptPartValueL(
    TNSmlCGIScriptDataType aDataType, 
    const TAny *aValue,
    TPtr & aPtr)
    {

    switch (aDataType)
        {
        case ENSmlCGIScriptDataTypeHBufC:
            {
            aPtr.Append( *reinterpret_cast<const HBufC*>(aValue) );
            break;
            }
        case ENSmlCGIScriptDataTypeDate:
            {
            const TDateTime* dt = reinterpret_cast<const TDateTime*>(aValue);
            TBuf<KNSmlCGIParserDateLen> des;
            GenerateDateTimeValue(*dt, des, EFalse);
            aPtr.Append(des);
            }
            break;
        case ENSmlCGIScriptDataTypeDateTime:
            {
            const TDateTime* dt = reinterpret_cast<const TDateTime*>(aValue);
            TBuf<KNSmlCGIParserDateTimeLen> des;
            GenerateDateTimeValue(*dt, des, ETrue);
            aPtr.Append(des);
            }
            break;
        case ENSmlCGIScriptDataTypeNumber:
            {
            TBuf<32> des;
            const TInt* num = reinterpret_cast<const TInt*>(aValue);
            des.Num(*num);
            aPtr.Append(des);
            }
            break;
        case ENSmlCGIScriptDataTypeBoolean:
            {
            const TBool* bln = reinterpret_cast<const TBool*>(aValue);
            aPtr.Append(GenerateBoolValue(*bln));
            }
            break;
        case ENSmlCGIScriptDataTypeNull:
            aPtr.Append(KNSmlCGIScriptNullValue);
            break;
        case ENSmlCGIScriptDataTypeNoValue:
            //empty
            break;
        default: //TNSmlCGIScriptDataTypeUnKnown
            User::Leave(ENSmlCGIParserErrorConversion);
            break;
        }
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::SetCGIScriptL
// -----------------------------------------------------------------------------
//
void TNSmlCGIScriptParser::SetCGIScriptL(
    CNSmlCGIScript& aSp, 
    CArrayPtr<HBufC>& aBufParts ) const
    {

    TInt bufPartsCount(aBufParts.Count());
    TInt i(0);
    TInt scriptLen(0);

    if (bufPartsCount > 0)
        {

        //length of entire script
        for (i = 0; i < bufPartsCount; i++)
            {
            scriptLen += aBufParts.At(i)->Length();
            }

        //adds length of separator between two scriptparts
        if (bufPartsCount > 1)
            {
            scriptLen += (bufPartsCount - 1) * 
                LogSepL(aSp.LogicalOperator()).Length();
            }

        //reserves needed memory
        HBufC* bufTmp = HBufC::NewLC(scriptLen);
        TPtr ptr(bufTmp->Des());

        //appends all parts to a full script.
        for (i = 0; i < bufPartsCount;i++)
            {
            ptr.Append(*aBufParts.At(i));
            if (i < (bufPartsCount - 1))
                {
                ptr.Append(LogSepL(aSp.LogicalOperator()));
                }
            }

        aSp.SetCGIScriptL(*bufTmp);
        CleanupStack::PopAndDestroy(bufTmp);
        }
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::ParseDateTimeL
// -----------------------------------------------------------------------------
//
TInt TNSmlCGIScriptParser::ParseDateTimeL(
    TDateTime& aDateTime, 
    const TDesC& aDes, 
    TInt aStartFrom)
    {

    enum TDateParseState{ 
        EDateParseStateDate, 
        EDateParseStateTime, 
        EDateParseStateCheckUTC, 
        EDateParseStateDone};

    TDateParseState stateAfterDate(EDateParseStateTime);

    switch (aDes.Length())
        {
        case KNSmlCGIParserDateLen:
            stateAfterDate = EDateParseStateDone;
            break;
        case KNSmlCGIParserDateTimeLen:
            stateAfterDate = EDateParseStateTime;
            break;
        default:
            User::Leave(ENSmlCGIParserErrorConversion);
        }

    TInt ind(aStartFrom);
    const TInt8 DateItemLengths[] = {4, 2, 2, 2, 2, 2};
    TInt DateValues[6] = {0, 0, 0, 0, 0, 0};

    TDateParseState state(EDateParseStateDate);
    TDateParseState nextState(state);

    TInt i(0);
    TInt upto(0);

    while (state != EDateParseStateDone)
        {
        switch (state)
            {
            case EDateParseStateDate:
                i = 0;
                upto = 3;
                nextState = stateAfterDate;
                break;
            case EDateParseStateTime:
                if (aDes[ind] != 'T')
                    {
                    User::Leave(ENSmlCGIParserErrorConversion);
                    }
                ind++;
                upto = 6;
                nextState = EDateParseStateCheckUTC;
                break;
            case EDateParseStateCheckUTC:

                // must be UTC, marked by Z
                if (aDes[ind] != 'Z')
                    {
                    User::Leave(ENSmlCGIParserErrorConversion);
                    }
                ind++;
                state = EDateParseStateDone;
                break;
            default:
                User::Leave(KErrGeneral);
                break;
            }

        if (state != EDateParseStateDone)
            {
            for (; i < upto;i++)
                {
                TInt len(DateItemLengths[i]);
                DateValues[i] = ParseIntL(aDes, ind, len);
                ind += len;
                }
            state = nextState;
            }
        }
    
    TInt err(aDateTime.Set(DateValues[0], 
        static_cast<TMonth>(DateValues[1] - 1), DateValues[2], 
        DateValues[3], DateValues[4], DateValues[5], 0));

    User::LeaveIfError(err);
    return(ind - aStartFrom);
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::GenerateDateTimeValue
// -----------------------------------------------------------------------------
//
void TNSmlCGIScriptParser::GenerateDateTimeValue(
    const TDateTime& aDateTime, 
    TDes& aDes, 
    TBool aUseTimePart)
    {

    if (aUseTimePart)
        {
        aDes.Format(KNSmlCGIParserDateTimeFormat, aDateTime.Year(), 
            aDateTime.Month() + 1, aDateTime.Day(), 
            aDateTime.Hour(), aDateTime.Minute(), aDateTime.Second());
        }
    else
        {
        aDes.Format(KNSmlCGIParserDateFormat, aDateTime.Year(),
            aDateTime.Month() + 1, aDateTime.Day());
        }
}

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::GenerateBoolValue
// -----------------------------------------------------------------------------
//
const TDesC & TNSmlCGIScriptParser::GenerateBoolValue(TBool aBool)
    {

    if (aBool)
        {
        return KNSmlCGIScriptBoolTrue;
        }
    else
        {
        return KNSmlCGIScriptBoolFalse;
        }
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::ParseIntL
// -----------------------------------------------------------------------------
//
TInt TNSmlCGIScriptParser::ParseIntL(
    const TDesC& aDes, 
    TInt aStartFrom, 
    TInt aLength)
    {

    TInt num(0);
    TPtrC des(aDes.Mid(aStartFrom, aLength));
    TLex lex(des);    
    TInt t = lex.Val(num);
    if (t != KErrNone)
        {
        User::Leave(t);
        }
    if (!lex.Eos())
        {
        User::Leave(ENSmlCGIParserErrorConversion);
        }
    return num;

    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::ParseBoolL
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::ParseBoolL(
    const TDesC& aDes, 
    TInt aStartFrom)
    {
    TBool bln(EFalse);
    const TDesC* cmp = NULL;

    if ( Compare(*(cmp = &GenerateBoolValue(ETrue)), aDes, aStartFrom) )
        {
        bln = ETrue;
        }
    else if ( Compare(*(cmp = &GenerateBoolValue(EFalse)), aDes, aStartFrom) )
        {
        bln = EFalse;
        }
    else
        {
        User::Leave(ENSmlCGIParserErrorConversion);
        }

    if ( cmp->Length() != (aDes.Length() - aStartFrom) )
        {
        User::Leave(ENSmlCGIParserErrorConversion);
        }
    
    return bln;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::LogOpL
// -----------------------------------------------------------------------------
//
const TDesC& TNSmlCGIScriptParser::LogOpL(TNSmlCGIScriptComparator aComp) const
    {
    if ( aComp < KNSmlCGIParserLogOpsCount) //range check
        {
        return *KNSmlCGIScriptLogOps[aComp];
        }
    else
        {
        User::Leave(ENSmlCGIParserErrorWrongOperator);
        }
    return KNSmlCGIScriptNullStr;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::LogOpL
// -----------------------------------------------------------------------------
//
TNSmlCGIScriptComparator TNSmlCGIScriptParser::LogOpL(
    const TDesC& aDes, 
    TInt aInd) const
    {

     for (TInt i = 0; i < KNSmlCGIParserLogOpsCount; i++)
        {
        TNSmlCGIScriptComparator lo(
            static_cast<TNSmlCGIScriptComparator>(i));

        if (Compare(LogOpL(lo), aDes, aInd-1))
            {
            return lo;
            }
        }
    User::Leave(ENSmlCGIParserErrorWrongOperator);

    return ENSmlCGIScriptComparatorNull;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::LogSepL
// -----------------------------------------------------------------------------
//
const TDesC & TNSmlCGIScriptParser::LogSepL(
    TNSmlCGIScriptLogicalOperator aLogSep) const
    {
    
    if ( aLogSep < KNSmlCGIScriptLogSepCount) //range check
        {
        return *KNSmlCGIScriptLogSep[aLogSep];
        }
    else
        {
        User::Leave(ENSmlCGIParserErrorWrongSeparator);
        }

    return KNSmlCGIScriptNullStr;

    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::LogSepL
// -----------------------------------------------------------------------------
//
TNSmlCGIScriptLogicalOperator TNSmlCGIScriptParser::LogSepL(
    const TDesC& aDes, 
    TInt aInd) const
    {

    for (TInt i = 0; i < KNSmlCGIScriptLogSepCount; i++)
        {
        TNSmlCGIScriptLogicalOperator lo(
            static_cast<TNSmlCGIScriptLogicalOperator>(i));

        if (Compare(LogSepL(lo), aDes, aInd-1))
            {
            return lo;
            }
        }
    User::Leave(ENSmlCGIParserErrorWrongSeparator);
    return ENSmlCGIScriptLogicalOperatorNull;

    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::IsEqualityOperator
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::IsEqualityOperator(TNSmlCGIScriptComparator comp)
    {
    if (comp >= ENSmlCGIScriptComparatorEqualToCaseSensitive 
        && comp <= ENSmlCGIScriptComparatorNotEqualToCaseInSensitive)
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::CheckScriptPartValidityL
// -----------------------------------------------------------------------------
//
void TNSmlCGIScriptParser::CheckScriptPartValidityL(const TNSmlCGIScriptPart & aSp)
    {
    if (aSp.iData == NULL)
        {
        if (aSp.iDataType == ENSmlCGIScriptDataTypeNoValue)
            {
            if (aSp.iComparator != ENSmlCGIScriptComparatorNull)
                {
                User::Leave(ENSmlCGIParserErrorWrongOperator);
                }
            }
        else if ( !IsEqualityOperator(aSp.iComparator) )
            {
            User::Leave(ENSmlCGIParserErrorWrongOperator);
            }
        }
    else
        {
        if (aSp.iDataType == ENSmlCGIScriptDataTypeNoValue)
            {
            User::Leave(ENSmlCGIParserErrorConversion);
            }
        if (aSp.iComparator == ENSmlCGIScriptComparatorNull)
            {
            User::Leave(ENSmlCGIParserErrorWrongOperator);
            }
        }
    if (aSp.iName == NULL)
        {
        User::Leave(ENSmlCGIParserErrorNoKeyword);
        }
    }

// -----------------------------------------------------------------------------
// TNSmlCGIScriptParser::Compare
// -----------------------------------------------------------------------------
//
TBool TNSmlCGIScriptParser::Compare(
    const TDesC& aWhat, 
    const TDesC& aWhere, 
    TInt aStartFrom)
    {

    if((aWhere.Length() - aStartFrom) < aWhat.Length())
        {
        return EFalse; //aWhat is longer than what's left in aWhere. No match
        }
    TBuf<32> buf( aWhere.Mid(aStartFrom, aWhat.Length()) );
    return ( buf.Compare(aWhat) == 0 );

    }

//  End of File  
