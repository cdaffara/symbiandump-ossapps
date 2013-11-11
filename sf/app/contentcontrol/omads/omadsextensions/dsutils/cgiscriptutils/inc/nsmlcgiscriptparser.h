/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __TNSMLCGISCRIPTPARSER_H__
#define __TNSMLCGISCRIPTPARSER_H__

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <e32std.h>
#include <badesca.h>

// CONSTANTS
_LIT( KNSmlCGIParserReservedChars, ";/?:@&=+,$[]" );
_LIT( KNSmlCGIParserDateTimeFormat, "%04u%02u%02uT%02u%02u%02uZ" );
_LIT( KNSmlCGIParserDateFormat, "%04u%02u%02u" );

_LIT(KNSmlCGIScriptComparatorEqualToCaseSensitiveStr, "&EQ;");
_LIT(KNSmlCGIScriptComparatorEqualToCaseInSensitiveStr, "&iEQ;");
_LIT(KNSmlCGIScriptComparatorNotEqualToCaseSensitiveStr, "&NE;");
_LIT(KNSmlCGIScriptComparatorNotEqualToCaseInSensitiveStr, "&iNE;");
_LIT(KNSmlCGIScriptComparatorGreaterThanCaseSensitiveStr, "&GT;");
_LIT(KNSmlCGIScriptComparatorGreaterThanCaseInSensitiveStr, "&iGT;");
_LIT(KNSmlCGIScriptComparatorGreaterThanOrEqualToCaseSensitiveStr, "&GE;");
_LIT(KNSmlCGIScriptComparatorGreaterThanOrEqualToCaseInSensitiveStr, "&iGE;");
_LIT(KNSmlCGIScriptComparatorLessThanCaseSensitiveStr, "&LT;");
_LIT(KNSmlCGIScriptComparatorLessThanCaseInSensitiveStr, "&iLT;");
_LIT(KNSmlCGIScriptComparatorLessThanOrEqualCaseSensitiveStr, "&LE;");
_LIT(KNSmlCGIScriptComparatorLessThanOrEqualCaseInSensitiveStr, "&iLE;");
_LIT(KNSmlCGIScriptComparatorContainsValueCaseSensitiveStr, "&CON;");
_LIT(KNSmlCGIScriptComparatorContainsValueCaseInSensitiveStr, "&iCON;");

_LIT(KNSmlCGIScriptLogicalSeparatorAndStr, "&AND;");
_LIT(KNSmlCGIScriptLogicalSeparatorOrStr, "&OR;");

_LIT(KNSmlCGIScriptNullValue, "&NULL;");
_LIT(KNSmlCGIScriptLuidValue, "&LUID;");

_LIT(KNSmlCGIScriptNullStr,"");

_LIT(KNSmlCGIScriptBoolTrue, "TRUE");
_LIT(KNSmlCGIScriptBoolFalse, "FALSE");

const TInt KNSmlCGIScriptLogSepCount = 2;
const TInt KNSmlCGIParserLogOpsCount = 15;
const TInt KNSmlCGIScriptSpecialUsageStringsCount = 2;

const TInt KNSmlCGIParserDateTimeLen = 16;
const TInt KNSmlCGIParserDateLen = 8;

// DATA TYPES
// ------------------------------------------------------------------------------------------------
// TNSmlCGIScriptDataType
// ------------------------------------------------------------------------------------------------
enum TNSmlCGIScriptDataType
	{
	ENSmlCGIScriptDataTypeHBufC,
	ENSmlCGIScriptDataTypeDate,
	ENSmlCGIScriptDataTypeDateTime,
	ENSmlCGIScriptDataTypeNumber,
	ENSmlCGIScriptDataTypeBoolean,
    ENSmlCGIScriptDataTypeNoValue,
	ENSmlCGIScriptDataTypeNull,
	ENSmlCGIScriptDataTypeUnKnown
	};

// ------------------------------------------------------------------------------------------------
// TNSmlCGIScriptLogicalOperator
// ------------------------------------------------------------------------------------------------
enum TNSmlCGIScriptLogicalOperator
	{
	ENSmlCGIScriptLogicalOperatorAnd,
	ENSmlCGIScriptLogicalOperatorOr,
    ENSmlCGIScriptLogicalOperatorNull
	};

// ------------------------------------------------------------------------------------------------
// TNSmlCGIScriptComparator
// ------------------------------------------------------------------------------------------------
enum TNSmlCGIScriptComparator
	{
	ENSmlCGIScriptComparatorEqualToCaseSensitive,
	ENSmlCGIScriptComparatorEqualToCaseInSensitive,
	ENSmlCGIScriptComparatorNotEqualToCaseSensitive,
	ENSmlCGIScriptComparatorNotEqualToCaseInSensitive,
	ENSmlCGIScriptComparatorGreaterThanCaseSensitive,
	ENSmlCGIScriptComparatorGreaterThanCaseInSensitive,
	ENSmlCGIScriptComparatorGreaterThanOrEqualToCaseSensitive,
	ENSmlCGIScriptComparatorGreaterThanOrEqualToCaseInSensitive,
	ENSmlCGIScriptComparatorLessThanCaseSensitive,
	ENSmlCGIScriptComparatorLessThanCaseInSensitive,
	ENSmlCGIScriptComparatorLessThanOrEqualCaseSensitive,
	ENSmlCGIScriptComparatorLessThanOrEqualCaseInSensitive,
	ENSmlCGIScriptComparatorContainsValueCaseSensitive,
	ENSmlCGIScriptComparatorContainsValueCaseInSensitive,
    ENSmlCGIScriptComparatorNull
	};

// FORWARD DECLARATIONS

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// TNSmlCGIScriptPart
// ------------------------------------------------------------------------------------------------
struct TNSmlCGIScriptPart
    {
    HBufC* iName;
	TAny* iData;
	TNSmlCGIScriptDataType iDataType;
	TNSmlCGIScriptComparator iComparator;
    };

// ------------------------------------------------------------------------------------------------
// TNSmlDataTypesForCGIScriptNames
// ------------------------------------------------------------------------------------------------
struct TNSmlDataTypesForCGIScriptNames
	{
	HBufC* iKeywordOrProperty;
	TNSmlCGIScriptDataType iDataType;
	};

// ------------------------------------------------------------------------------------------------
// CNSmlCGIScript
// ------------------------------------------------------------------------------------------------
class CNSmlCGIScript : public CBase
	{
public:
	/**
    * CNSmlCGIScript NewL.
    */
	IMPORT_C static CNSmlCGIScript* NewL();

	/**
    * CNSmlCGIScript NewLC.
    */
	IMPORT_C static CNSmlCGIScript* NewLC();
	
	/**
    * CNSmlCGIScript destructor.
	* 
    */
	~CNSmlCGIScript();

	/**
    * Returns logical Separator of CNSmlCGIScript.
    */
	IMPORT_C TNSmlCGIScriptLogicalOperator LogicalOperator() const;

	/**
    * Sets logical Separator.
	* @param aSeparator sets logical Separator of CNSmlCGIScript.
    */
	IMPORT_C void SetLogicalOperator( const TNSmlCGIScriptLogicalOperator aSeparator );

	/**
    * Returns script of CNSmlCGIScript.
    */
	IMPORT_C HBufC* CGIScript() const;

	/**
    * Sets script of CNSmlCGIScript.
	* @param aCGIScript sets script of CNSmlCGIScript.
    */
	IMPORT_C void SetCGIScriptL( const TDesC& aCGIScript );

	/**
    * Returns number of records in list.
	* @return TInt Number of records in list.
    */
	IMPORT_C TInt Count() const;

	/**
    * Returns script part at index aIndex from list.
    * @param aIndex - value between 0 - Count()-1
	* @return TNSmlCGIScriptPart pointer to script part at index aIndex.
    */
	IMPORT_C const TNSmlCGIScriptPart* Get( TInt aIndex ) const;

	/**
    * Adds new script part to the list.
	* @param aScriptPart Pointer to script part to add to the list.
    */
	IMPORT_C void AddL( TNSmlCGIScriptPart* aScriptPart );

	/**
    * Removes script part at index aIndex from list.
	* @param aIndex Index to script part to be removed.
	* @return TNSmlCGIScriptPart* Pointer to removed script part.
    */
	IMPORT_C TNSmlCGIScriptPart* Remove( TInt aIndex );

    /**
    * Removes all script parts.
    */
    IMPORT_C void Clear();

protected:
	CNSmlCGIScript();
	void ConstructL();
	
private:
	TNSmlCGIScriptLogicalOperator iSeparator;
	HBufC* iCGIScript;
	CArrayPtrFlat<TNSmlCGIScriptPart>* iScriptParts;
	TAny* iReserved;
	};

// ------------------------------------------------------------------------------------------------
//  CGI parser/generator
// ------------------------------------------------------------------------------------------------
class TNSmlCGIScriptParser
    {
    public:
// ------------------------------------------------------------------------------------------------
// TNSmlCGIParserError
// ------------------------------------------------------------------------------------------------
        enum TNSmlCGIParserError
	        {
	        ENSmlCGIParserErrorOk,
	        ENSmlCGIParserErrorDataTypeNotFound,
            ENSmlCGIParserErrorConversion,
	        ENSmlCGIParserErrorWrongOperator,
            ENSmlCGIParserErrorWrongSeparator,
            ENSmlCGIParserErrorParsing,
            ENSmlCGIParserErrorNoKeyword
	        };


    private:
        // Private helper classes:
// ------------------------------------------------------------------------------------------------
// TNSmlCGIScriptParseState
// ------------------------------------------------------------------------------------------------
        enum TNSmlCGIScriptParseState 
            {
            ENSmlCGIScriptParseStateKeyWord, 
            ENSmlCGIScriptParseStateLogOp, 
            ENSmlCGIScriptParseStateValue, 
            ENSmlCGIScriptParseStateLogSep
            };

        // ----------------------------------------------------------------------------------------
        // TNSmlCGIScriptParseStateInfo
        // ----------------------------------------------------------------------------------------
        struct TNSmlCGIScriptParseStateInfo
            {
            TNSmlCGIScriptParseStateInfo(
                const TDesC* aCGIScript,
                const CArrayPtr<TNSmlDataTypesForCGIScriptNames>* aDatatypes);
            TNSmlCGIScriptParseState iState;
            TNSmlCGIScriptParseState iNextState;
            const TDesC* iCGIScript;
            const CArrayPtr<TNSmlDataTypesForCGIScriptNames>* iDatatypes;
            TInt iStartPos;
            TInt iCurrPos;
            };
        // ----------------------------------------------------------------------------------------
        // TNSmlCGIScriptParseData
        // ----------------------------------------------------------------------------------------
        struct TNSmlCGIScriptParseData
            {
            TNSmlCGIScriptParseData();
            TPtrC iKeyword;
            TNSmlCGIScriptDataType iDataType;
            TPtrC iValue;
            TNSmlCGIScriptComparator iComparator;
            TNSmlCGIScriptLogicalOperator iSeparator;
            TBool iParseDataReady;
            };

        friend struct TNSmlCGIScriptParseStateInfo;

    public:  // Constructors and destructor

        
        /**
        * constructor.
        */
        IMPORT_C TNSmlCGIScriptParser();
        
    public: // New functions
        
        /**
        * Converts cgi-script to a data structure.
        * @param aSp contains cgi-script (=source) and methods to add parsed data structure (=dest.) there.
        * @param aDatatypes contains all acceptable keyword and property names and their datatypes.
        */
        IMPORT_C void ParseL(CNSmlCGIScript& aSp, 
            const CArrayPtr<TNSmlDataTypesForCGIScriptNames>& aDatatypes) const;

        /**
        * Converts data structure to cgi-script
        * @param aS contains the data structure (=source) and method to set the generated cgi-script.
        */
        IMPORT_C void GenerateL(CNSmlCGIScript& aS) const;
        
    protected:  // New functions

    private:

        /**
        * In parsing cgi-script, determines whether to split cgi-script at a certain point or not. 
        * @param aCGIScript - cgi-script
        * @param aStartFrom - index to cgi-script
        * @return ETrue if there is split point at aCGIScript[aStartFrom], EFalse otherwise.
        */
        TBool IsParseSplitPoint(const TDesC& aCGIScript, TInt aStartFrom) const;

        /**
        * Does the parsing of cgi-script.
        * @param aPSInfo contains state information needed for parsing (=source)
        * @param aPD is filled with data, that is parsed (=dest.)
        * @return EFalse, if ready for handling of next split point, ETrue, if must be called again before next split point.
        */
        TBool ParseStateL(TNSmlCGIScriptParseStateInfo& aPSInfo, TNSmlCGIScriptParseData& aPD) const;

        /**
        * adds script part to aSp
        * @param aSp - script to add script part. (=dest)
        * @param aPD - parsed data, has the data needed for script part.(=source)
        * @return ETrue, if script part was added, EFalse otherwise.
        */
        static TBool AddScriptPartL(CNSmlCGIScript& aSp, TNSmlCGIScriptParseData& aPD);

        /**
        * parses the script's value.
        * @param aSp - struct's iData - member receives the parsed value.
        * @param aValue - value to parse.
        * @return ETrue, if aSp should be used, EFalse if it should be ignored.
        */
        static TBool ParseScriptPartDataLC(TNSmlCGIScriptPart& aSp, const TPtrC& aValue);

        /**
        * finds datatype for a keyword (=script's keyword or propertyname)
        * @param aDatatypes - array where to search.
        * @param aKeyword - keyword to search
        * @return datatype, leaves if not found
        */
        static TNSmlCGIScriptDataType FindDataTypeL(const CArrayPtr<TNSmlDataTypesForCGIScriptNames>& aDatatypes, const TPtrC& aKeyword);

        /**
        * similar to FindDataTypeL, but doesn't leave if datatype not found, instead returns TNSmlCGIScriptDataTypeUnKnown.
        * @param aDatatypes - array where to search.
        * @param aKeyword - keyword to search
        * @return datatype, leaves if not found
        */
        static TNSmlCGIScriptDataType FindDataType(const CArrayPtr<TNSmlDataTypesForCGIScriptNames>& aDatatypes, const TPtrC& aKeyword);

        /**
        * converts script part to string.
        * @param aSp - script part to convert
        * @return converted string.
        */
        HBufC* GenerateScriptPartL(const TNSmlCGIScriptPart& aSp) const;

        /**
        * generates the script's value.
        * @param aDataType - datatype for value.
        * @param aValue - value to convert. (=source)
        * @param aPtr receives the generated value (=dest.)
        */
        static void GenerateScriptPartValueL(TNSmlCGIScriptDataType aDataType, const TAny* aValue, TPtr& aPtr);

        /**
        * collects script-parts to one cgi-script.
        * @param aSp - where to set the script (=dest)
        * @param aBufParts - scripts parts to collect (=source)
        */
        void SetCGIScriptL(CNSmlCGIScript& aSp, CArrayPtr<HBufC>& aBufParts) const;

        /**
        * converts cgi-script date to TDateTime.
        * @param aDateTime receives the parsed datetime(=dest.)
        * @param aDes - descriptor where date is to be parsed (= source)
        * @param aStartFrom - index to aDes, where date-string should start.
        * @return how many letters from aDes[aStartFrom] was needed to get the datetime. 
        */
        static TInt ParseDateTimeL(TDateTime& aDateTime, const TDesC& aDes, TInt aStartFrom = 0);

        /**
        * converts TDateTime to string-format used by cgi-script.
        * @param aDateTime - the date to convert(=source.)
        * @param aDes receives the converted date-string. (=dest.)
        * @param aUseTimePart is ETrue, if timepart (= hour, minute, second) should be converted too.
        */
        static void GenerateDateTimeValue(const TDateTime& aDateTime, TDes& aDes, TBool aUseTimePart = ETrue);

        /**
        * converts boolean value to string-format used by cgi-script.
        * @param aBool - source boolean value.
        * @return boolean value as descriptor.
        */
        static const TDesC& GenerateBoolValue(TBool aBool);

        /**
        * converts number as descriptor to TInt.
        * @param aDes - source number.
        * @param aStartFrom - index to aDes, where the number should be.
        * @param aLength - how many numbers follows aDes[aStartFrom].
        * @return the number as integer.
        */
        static TInt ParseIntL(const TDesC& aDes, TInt aStartFrom, TInt aLength);

        /**
        * converts boolean-value as descriptor to TBool
        * @param aDes - source boolean-value.
        * @param aStartFrom - index to aDes, where the boolean-value should be.
        * @return the boolean-value as TBool.
        */
        static TBool ParseBoolL(const TDesC& aDes, TInt aStartFrom = 0);
        
        /**
        * get logical operator as descriptor.
        * @param aComp - comparator 
        * @return comparator as string.
        */
        const TDesC& LogOpL(TNSmlCGIScriptComparator aComp) const;

        /**
        * gets comparator from descriptor.
        * @param aDes - descriptor, where comparator should be.
        * @param aInd - index to aDes where comparator should be. aInd should point to next letter following '&'.
        * @return comparator at aDes[aInd] or else leaves.
        */
        TNSmlCGIScriptComparator LogOpL(const TDesC& aDes, TInt aInd) const;

        /**
        * get logical separator as descriptor.
        * @param aLogSep - separator
        * @return separator as string.
        */
        const TDesC& LogSepL(TNSmlCGIScriptLogicalOperator aLogSep) const;

        /**
        * gets separator from descriptor.
        * @param aDes - descriptor, where comparator should be.
        * @param aInd - index to aDes, where comparator should be. aInd should point to next letter following '&'.
        * @return separator at aDes[aInd] or else leaves.
        */
        TNSmlCGIScriptLogicalOperator LogSepL(const TDesC& aDes, TInt aInd) const;

        /**
        * compares comparator to one of equality-operators.
        * @param aComp - comparator to compare.
        * @return ETrue, if aComp was one of equality-operators, EFalse otherwise.
        */
        static TBool IsEqualityOperator(TNSmlCGIScriptComparator aComp);

        /**
        * checks, if scriptPart-data is valid. Leaves if not.
        * @param aSp - scriptpart to check.
        */
        static void CheckScriptPartValidityL(const TNSmlCGIScriptPart& aSp);

        /**
        * compares descriptors 
        * @param aWhat - what to compare.
        * @param aWhere - where to compare
        * @param aStartFrom - index to aWhere. 
        * @return ETrue if aWhat is found exactly at aWhere[aStartFrom], but aWhere can contain letters after the found string.
        */
        static TBool Compare(const TDesC& aWhat, const TDesC& aWhere, TInt aStartFrom);

    public:     // Data
    
    protected:  // Data
        
    private:    // Data
        const TDesC* KNSmlCGIScriptLogSep[KNSmlCGIScriptLogSepCount];
        const TDesC* KNSmlCGIScriptLogOps[KNSmlCGIParserLogOpsCount];
        const TDesC* KNSmlCGIScriptSpecialUsageStrings[KNSmlCGIScriptSpecialUsageStringsCount];        
    };

#endif      // __TNSMLCGISCRIPTPARSER_H__


// End of File
