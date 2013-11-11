// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#if !defined (__MIUTPARS_H__)
#define __MIUTPARS_H__

#include <e32base.h>

// comment out line below to switch ON Internet message checking
//#define __NO_EMAIL_ADDRESS_CHECKING__

class TImMessageField
/** Parses email message header fields for valid Internet email addresses, 
comments and aliases.

A comment is a string surrounded by parentheses, as defined in RFC822, 3.1.4. 

An alias is defined as any substring which appears to the left of a legal email address:  
for example, the string "this is an alias" in "this is an alias <an.email@address.com>".  

For email addresses, the string being parsed: 
1) must contain an @ character, surrounded by valid address characters; 
2) may not contain more than one address or @ character; 
3) may contain aliases and comments.

Note the following about the implementation of this class:

1. Functions that test subject lines, ValidSubjectLine(),  and alias names, ValidAliasName(), were 
initially written to test that the strings did not contain characters outside a limited 
ASCII range. Unicode characters are now allowed, so these functions now always return true.

2. Of the four overloads of GetValidInternetEmailAddressFromString(), only the first has a meaningful
implementation. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C TBool ValidInternetEmailAddress(const TDesC16& aAddress);
	IMPORT_C TBool ValidInternetEmailAddress(const TDesC16& aAddress, TInt& rFirstBadCharPos);	
	IMPORT_C TBool ValidInternetEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar);
	IMPORT_C TBool ValidInternetEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar, TInt& rFirstBadCharPos);

	IMPORT_C TBool ValidInternetEmailAddressChar(const TChar& aChar);	

	IMPORT_C TBool ValidSubjectLine(const TDesC16& aSubjectLine);
	IMPORT_C TBool ValidSubjectLine(const TDesC16& aSubjectLine, TInt& rFirstBadCharPos);
	
	IMPORT_C TBool ValidSubjectLineChar(const TChar& aChar);
	
	IMPORT_C TBool ValidAliasName(const TDesC16& aAliasName);
	IMPORT_C TBool ValidAliasName(const TDesC16& aAliasName, TInt& rFirstBadCharPos);

	IMPORT_C TPtrC16 GetValidInternetEmailAddressFromString(const TDesC16& aAddress);
	IMPORT_C TPtrC16 GetValidInternetEmailAddressFromString(const TDesC16& aAddress, TInt& rError);
	IMPORT_C TPtrC16 GetValidInternetEmailAddressFromString(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar);
	IMPORT_C TPtrC16 GetValidInternetEmailAddressFromString(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar, TInt& rError);

	IMPORT_C TPtrC16 GetValidAlias(const TDesC16& aAddress);
	IMPORT_C TPtrC16 GetValidAlias(const TDesC16& aAddress, TInt& rError);

	IMPORT_C TPtrC16 GetValidComment(const TDesC16& aAddress);
	IMPORT_C TPtrC16 GetValidComment(const TDesC16& aAddress, TInt& rError);
	TBool TruncateAddressString(const TDesC16& aDesc, TInt aLimit, TInt& aLastChar);
private:
	TBool isValidEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar);
	TBool isLegalEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar);
	TBool isSurroundedByRoundBrackets(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar);
	TBool isSurroundedByAngledBrackets(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar, TInt aAtPos);
	TBool isEnclosedSubString(const TDesC16& anAddress, const TChar& aLeftBracket, const TChar& aRightBracket,TInt& aLeftPos, TInt& aRightPos);
    TBool isValidString(const TDesC16& anAddress, TInt& aFirstBadCharPos);
	TBool isValidChar(const TChar& aChar);
    TBool isValidEmailString(const TDesC16& anAddress, TInt& aFirstBadCharPos);
	TBool isValidEmailChar(const TChar& aChar);
	TBool LocateSubString(const TDesC16& anAddress, const TInt atPos, TInt& rFirstChar, TInt& rLastChar);	
	TBool hasAngledBrackets(const TDesC16& anAddress);
	TBool isValid_ISO88591String(const TDesC16& aString,TInt& aPos);
	TBool isValidRoutedEmailAddress(const TDesC16& anAddress);
	TBool isValidDomainNameChar(const TChar& aChar);
	TBool isValidMailboxChar(const TChar& aChar);
	TBool isDotChar(const TChar& aChar);
	};


#endif // !defined __MIUTPARS_H__

