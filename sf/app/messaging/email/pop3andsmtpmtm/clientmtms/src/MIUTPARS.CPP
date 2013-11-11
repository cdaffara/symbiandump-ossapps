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

#include <e32std.h>
#include "MIUTPARS.H"

/** Tests if the specified string contains a valid email address.

@param aAddress String to test
@return ETrue if aAddress contains one valid email address. The email address 
can contain aliases and comments. EFalse otherwise.
*/
EXPORT_C TBool TImMessageField::ValidInternetEmailAddress(const TDesC16& aAddress)
	{
	TInt leftEdge=0,rightEdge=0;
	TBool result = (isValidEmailAddress(aAddress, leftEdge, rightEdge));
	result &= hasAngledBrackets(aAddress);
	return result;
	}

/** Tests if the specified string contains a valid email address.

@param aAddress String to test
@param rFirstBadCharPos On return, if the test has succeeded, the character position in
aAddress where the email address begins. If the test fails, the position of the 
character that caused the test to fail.
 
@return ETrue if aAddress contains one valid email address; EFalse otherwise.
*/
EXPORT_C TBool TImMessageField::ValidInternetEmailAddress(const TDesC16& aAddress, TInt& rFirstBadCharPos)
	{
	TInt discardPos;
	TBool result = (isValidEmailAddress(aAddress, rFirstBadCharPos, discardPos));
	result &= hasAngledBrackets(aAddress);
	return result;
	}

/** Tests if the specified string contains a valid email address.

@param aAddress String to test
@param rFirstChar On return, if the test has succeeded, the character position in
aAddress where the email address begins. If the test fails, the position of the 
character that caused the test to fail.
@param rLastChar On return, if the test has succeeded, the character position in
aAddress where the email address ends.
@return ETrue if aAddress contains one valid email address; EFalse otherwise.
*/
EXPORT_C TBool TImMessageField::ValidInternetEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar)
	{
	TBool result = (isValidEmailAddress(aAddress, rFirstChar, rLastChar));
	result &= hasAngledBrackets(aAddress);
	return result;
	}

/** Tests if the specified string contains a valid email address.

@param aAddress String to test
@param rFirstChar On return, if the test has succeeded, the character position in
aAddress where the email address begins. If the test fails, the position of the 
character that caused the test to fail.
@param rLastChar On return, if the test has succeeded, the character position in
aAddress where the email address ends.
@param rFirstBadCharPos On return, if the test has succeeded, KErrNotFound. If the test 
fails, the position of the character that caused the test to fail.
@return ETrue if aAddress contains one valid email address; EFalse otherwise.
*/
EXPORT_C TBool TImMessageField::ValidInternetEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar, TInt& rFirstBadCharPos)
	{
	TBool result = isValidEmailAddress(aAddress, rFirstChar, rLastChar);
	// if test failed, then aLeft is the first bad character found in the email address
	rFirstBadCharPos = (result) ? KErrNotFound : rFirstChar;
	result &= hasAngledBrackets(aAddress);
	return result;
	}

/** Tests if the specified character is valid (in ASCII range 32-127) for use in an email address.

@param aChar Character to test
@return ETrue if test succeeds; EFalse otherwise.
*/
EXPORT_C TBool TImMessageField::ValidInternetEmailAddressChar(const TChar& aChar)	
	{
	return isValidChar(aChar);
	}

/** This function always returns true, so can be ignored.

@param aSubjectLine Unused
@return Always ETrue
*/
EXPORT_C TBool TImMessageField::ValidSubjectLine(const TDesC16& /*aSubjectLine*/)
	{
	return ETrue;
	}

/** This function always returns true, so can be ignored.

@param aSubjectLine Unused
@param rFirstBadCharPos Unused
@return Always ETrue
*/
EXPORT_C TBool TImMessageField::ValidSubjectLine(const TDesC16& /*aSubjectLine*/, TInt& /*rFirstBadCharPos*/)
	{
	return ETrue;
	}

/** This function always returns true, so can be ignored.

@param aChar Unused
@return Always ETrue
*/
EXPORT_C TBool TImMessageField::ValidSubjectLineChar(const TChar& /*aChar*/)
	{
	// return ( (aChar>=0 &&aChar<128) || (aChar>=160 &&aChar<256));
	// All chars are valid UNICODE world.
	return ETrue;
	}

/** This function always returns true, so can be ignored.

@param aAliasName Unused
@return Always ETrue
*/
EXPORT_C TBool TImMessageField::ValidAliasName(const TDesC16& /*aAliasName*/)
	{
	return ETrue;
	}

/** This function always returns true, so can be ignored.

@param aAliasName Unused
@param rFirstBadCharPos Unused
@return Always ETrue
*/
EXPORT_C TBool TImMessageField::ValidAliasName(const TDesC16& /*aAliasName*/, TInt& /*rFirstBadCharPos*/)
	{
	return ETrue;
	}

/** Gets a valid email address, if one exists, from the specified string.

@param aAddress String to parse
@return If a valid email address was found in aAddress, a pointer descriptor to the 
address part (without surrounding aliases or comments). If an address could not be found, 
the whole of the original aAddress string is returned.
*/
EXPORT_C TPtrC16 TImMessageField::GetValidInternetEmailAddressFromString(const TDesC16& aAddress)
	{
	// returns Email address (without the enclosing angled brackets) - if one can be found,
	// otherwise return whole string as best-guess

	// Best-fit = "<name@address>"
	TInt LeftEdge=0,RightEdge=0;
	if (isLegalEmailAddress(aAddress,LeftEdge,RightEdge))
		{
		// NB removes surrounding angled brackets
		return aAddress.Mid(1+LeftEdge,RightEdge-LeftEdge-1);
		}

	// Next best-fit = "name@address"
	if (isValidEmailAddress(aAddress, LeftEdge, RightEdge))
		{
		return aAddress.Mid(LeftEdge,RightEdge-LeftEdge+1);
		}

	// default-fit = "whole contents of aDesc"
	return aAddress;
	}

/** Gets a valid email address, if one exists, from the specified string.

This does not differ in functionality from the other overload of 
GetValidInternetEmailAddressFromString().

@param aAddress String to parse
@param rError On return, always KErrNone
@return If a valid email address was found in aAddress, a pointer descriptor to the 
address substring (without surrounding aliases or comments). If an address could not be found, 
the whole of the original aAddress string is returned.
*/
EXPORT_C TPtrC16 TImMessageField::GetValidInternetEmailAddressFromString(const TDesC16& aAddress, TInt& rError)
	{
	// NOTE: This needs a proper implementation!
	GetValidInternetEmailAddressFromString(aAddress);
	rError = KErrNone;
	return aAddress;
	}


/** This function overload is not implemented.

@param aAddress String to parse
@param rFirstChar Unused
@param rLastChar Unused
@return Original aAddress string
*/
EXPORT_C TPtrC16 TImMessageField::GetValidInternetEmailAddressFromString(const TDesC16& aAddress, TInt& /*rFirstChar*/, TInt& /*rLastChar*/)
	{
	return aAddress;
	}

/** This function overload is not implemented.

@param aAddress String to parse
@param rFirstChar Unused
@param rLastChar Unused
@param rError Unused
@return Original aAddress string
*/
EXPORT_C TPtrC16 TImMessageField::GetValidInternetEmailAddressFromString(const TDesC16& aAddress, TInt& /*rFirstChar*/, TInt& /*rLastChar*/, TInt& /*rError*/)
	{
	return aAddress;
	}

/** Gets a valid alias, if one exists, from the specified string.

A valid alias is defined here as being any substring that appears 
to the left of a legal email address.

This overload supplies an error code in rError if an alias was not found.

@param aAddress String to parse
@param rError On return, KErrNone if a valid alias was found; otherwise, KErrNotFound.
@return If a valid alias was found in aAddress, a pointer descriptor to the 
alias substring. If a valid alias could not be found, the whole of the original 
aAddress string is returned.
*/
EXPORT_C TPtrC16 TImMessageField::GetValidAlias(const TDesC16& aAddress, TInt& rError)
	{
	// the alias part of the string is defined here as being any
	//  substring which appears to the left of a legal email address.
	// If the string contains no Email address or no alias then the full address is returned
	// and rError is set to KErrNotFound
	rError=KErrNone;
	TInt LeftEdge=0,RightEdge=0;
	if (isLegalEmailAddress(aAddress,LeftEdge,RightEdge))
		{
		// skip space between alias and left edge of email address.
		LeftEdge--;
		while (LeftEdge>=0 && aAddress[LeftEdge]==' ')
			{
			LeftEdge--;
			}

		// left edge now points to the last character of the alias,
		// or -1 if  there were just spaces before the email address
		if(LeftEdge>=0)
			{
			// set right edge of alias to left edge of email address minus spaces
			RightEdge=LeftEdge;
			
			// find first non-space character at start of descriptor
			LeftEdge=0;
			while (LeftEdge<RightEdge && aAddress[LeftEdge]==' ')
				{
				LeftEdge++;
				}

			if (RightEdge>=LeftEdge)
				{
				return aAddress.Mid(LeftEdge,RightEdge-LeftEdge+1);
				}
			}
		}
	rError=KErrNotFound;
	return aAddress;
	}

/** Gets a valid alias, if one exists, from the specified string.

A valid alias is defined here as being any substring that appears 
to the left of a legal email address.

@param aAddress String to parse
@return If a valid alias was found in aAddress, a pointer descriptor to the 
alias substring. If a valid alias could not be found, the whole of the original 
aAddress string is returned.
*/
EXPORT_C TPtrC16 TImMessageField::GetValidAlias(const TDesC16& aAddress)
	{
	TInt discardedInt;
	return GetValidAlias(aAddress,discardedInt);
	}

/** Gets a valid comment, if one exists, from the specified string.

A valid comment is found if the string contains both: 1) a legal email address; and
2) a comment (text surrounded by parentheses).

This overload supplies an error code in rError if a comment was not found.

@param aAddress String to parse
@param rError On return, KErrNone if a valid alias was found; otherwise, KErrNotFound.
@return If a valid comment was found in aAddress, a pointer descriptor to the 
comment substring. If a valid comment could not be found, the whole of the original 
aAddress string is returned.
*/
EXPORT_C TPtrC16 TImMessageField::GetValidComment(const TDesC16& aAddress, TInt& rError)
	{
	// Returns text contained within round brackets

	// Note that string must contain a Legal email address
	rError=KErrNone;
	TInt LeftEdge=0,RightEdge=0;
	if (isLegalEmailAddress(aAddress,LeftEdge,RightEdge))
		{
		if (isSurroundedByRoundBrackets(aAddress,LeftEdge,RightEdge))
			{
			LeftEdge++;
			return aAddress.Mid(LeftEdge,RightEdge-LeftEdge);
			}
		}
	rError=KErrNotFound;
	return aAddress;
	}

/** Gets a valid comment, if one exists, from the specified string.

A valid comment is found if the string contains both: 1) a legal email address; and
2) a comment (text surrounded by parentheses).

@param aAddress String to parse
@return If a valid comment was found in aAddress, a pointer descriptor to the 
comment substring. If a valid comment could not be found, the whole of the original 
aAddress string is returned.
*/
EXPORT_C TPtrC16 TImMessageField::GetValidComment(const TDesC16& aAddress)
	{
	TInt discardedInt;
	return GetValidComment(aAddress,discardedInt);
	}

TBool TImMessageField::hasAngledBrackets(const TDesC16& anAddress)
	{
	TInt leftBracketPos=anAddress.LocateReverse('<');
	TInt rightBracketPos=0;
	TBool result = ETrue;
	if (leftBracketPos==KErrNotFound)
		{
		rightBracketPos=anAddress.LocateReverse('>');
		result &= (rightBracketPos==KErrNotFound);
		}
	else
		{
		rightBracketPos=anAddress.LocateReverse('>');
		result &= (rightBracketPos>leftBracketPos);
		}
	return result;
	}

/** Counts email addresses in string, returns true if specified max is reached.

This function is provided to allow truncation of strings of email addresses to be
included in the body header of reply/forwarded emails, hence only a simple check 
is made on the validity of contained email addresses.

@param  aDesc  String to parse
@param  aLimit  The number of emails to identify before truncation should occur.
@param  aLastChar The position in the passed string of the closing angled bracket 
                  of the last email address before truncation should occur.
@return ETrue if the truncation limit reached, EFalse otherwise.
*/
TBool TImMessageField::TruncateAddressString(const TDesC16& aDesc, TInt aLimit, TInt& aLastChar)
	{
	TInt firstChar=0;
	TInt startPos=0;
	TInt atPos;
	TInt length=aDesc.Length();
	TInt count=0;
	do	{
		TPtrC16 rightString=aDesc.Right(length-startPos);
		atPos=rightString.Locate('@');
		if (atPos>0)
			{
			TBool valid = isSurroundedByAngledBrackets(rightString,firstChar,aLastChar,atPos);
			if (valid)
				{
				++count;
				if (count<=aLimit)
					{
					firstChar+=startPos;
					aLastChar+=startPos;
					startPos=aLastChar+1;
					}
				}
			else
				{
				startPos+=atPos+1; // allows loop to skip "@" in aliases
				}
			}
		}
	while (atPos>=0 && startPos<length && count<aLimit);
	aLastChar+=1; // update to position of closing angled bracket

	// return true if truncate limit reached
	return (!(count<aLimit));
	}

TBool TImMessageField::isLegalEmailAddress(const TDesC16& aDesc, TInt& rFirstChar, TInt& rLastChar)
	{
	// search through string for sub-string contained within angled brackets
	// if found, see whether substring is a valid email address
	TInt startPos=0;
	TInt atPos;
	TInt length=aDesc.Length();
	do	{
		TPtrC16 rightString=aDesc.Right(length-startPos);
		atPos=rightString.Locate('@');
		if (atPos>0)
			{
			TBool valid = isSurroundedByAngledBrackets(rightString,rFirstChar,rLastChar,atPos);
			if (valid)
				{
				if (rLastChar>rFirstChar)
					{
					if (rFirstChar + 1 < rLastChar)
						{
						TPtrC rightStringWithoutAngles = rightString.Mid(rFirstChar+1, rLastChar-rFirstChar-1);
						if (isValidRoutedEmailAddress(rightStringWithoutAngles))
							return ETrue;
						}
					}
				}

			if (valid && atPos>rFirstChar+1 && atPos<rLastChar-1)
				{
				rFirstChar+=startPos;
				rLastChar+=startPos;
				TInt discardedChar;

#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
				discardedChar=0;
				return ETrue;
#else
				return isValidString(aDesc.Mid(rFirstChar,rLastChar-rFirstChar+1),discardedChar);	// finally, check for illegal chars in substring
#endif
				}
			}
		startPos+=atPos+1;
		}
	while (atPos>=0 && startPos<length);

#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
	return ETrue;
#else
	return EFalse;
#endif
	}

TBool TImMessageField::isValidEmailAddress(const TDesC16& aAddress, TInt& rFirstChar, TInt& rLastChar)
	{
	TInt startPos=0;
	TInt atPos;
	TInt length=aAddress.Length();
	do	{
		TPtrC16 rightString=aAddress.Right(length-startPos);
		atPos=rightString.Locate('@');
		if (isValidRoutedEmailAddress(aAddress))
			{
			rFirstChar = 0;
			rLastChar = aAddress.Length() - 1;
			return ETrue;
			}
		else if (LocateSubString(rightString,atPos,rFirstChar,rLastChar))
			{
			// Assume that this is an Internet Email address as it contains an '@' character
			if (rLastChar-rFirstChar>1 && rFirstChar<atPos && rLastChar>atPos)		// string must be at least 3 chars long, eg "a@c" and must not contain an '@' char at either end
				{
				rFirstChar+=startPos;
				rLastChar+=startPos;
				TInt discardedChar;
				TPtrC16 address=aAddress.Mid(rFirstChar,rLastChar-rFirstChar+1);
				TInt result=address.LocateReverse('@');	// search for any more bogus '@' characters in remainder of email address

#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
				discardedChar=0;
				return ETrue;
#else
				return ((rFirstChar+result==atPos+startPos) && isValidEmailString(address,discardedChar));	// finally, check for multiple '@' chars, and also for illegal chars in substring 
#endif
				}
			}
		startPos+=atPos+1;
		}
	while (atPos>=0 && startPos<length);

#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
	return ETrue;
#else
	return EFalse;
#endif
	}

TBool TImMessageField::LocateSubString(const TDesC16& aAddress, const TInt atPos, TInt& rFirstChar, TInt& rLastChar)
	{
	// extract a substring containing '@' character from aDesc
	// The substring must contain an '@' character, but this character must not be 1st or last char in substring.
	// and which is at least three characters long (NB an arbitrary-chosen criterion to preven lone '@' chars being tested)
	TInt LeftEdge=0,RightEdge=0;
	TInt endPos=aAddress.Length()-1;
	rFirstChar=atPos;
	while (rFirstChar>0 && aAddress[rFirstChar]!=' ')
		{
		rFirstChar--;
		}
	if (rFirstChar>=0 && aAddress[rFirstChar]==' ')
		{
		rFirstChar++;	// point to 1st character in substring
		}
	if (rFirstChar<atPos)
		{
		rLastChar=atPos;
		while (rLastChar<endPos && aAddress[rLastChar]!=' ')
			{
			rLastChar++;
			}

		if (rLastChar<endPos && aAddress[rLastChar]==' ')
			{
			rLastChar--;	// point to last char in substring
			}

		if (atPos<rLastChar+1)
			{
			// check whether substring contains embedded legal email address eg "x<name@com>z@"
			TInt left,right;
			if (isSurroundedByAngledBrackets(aAddress,left,right,atPos))
				{
				rFirstChar=left+1;
				rLastChar=right-1;
				}
			return (aAddress[LeftEdge]!='@' && aAddress[RightEdge]!='@');	// email address mustn't start or end with '@' character
			}
		}
	return EFalse;
	}

TBool TImMessageField::isEnclosedSubString(const TDesC16& aDesc, const TChar& aLeftBracket, const TChar& aRightBracket,TInt& aLeftPos, TInt& aRightPos)
	{
	//Checking if the email address has  "< "  in the username part : ex: <xxxxxxx<ssss@sdsds.com>
	aLeftPos=aDesc.LocateReverse(aLeftBracket);
	if (aLeftPos>=0)
		{
 		TInt aPos = aLeftPos - 1 ; 
 		aRightPos=aDesc.LocateReverse(aRightBracket);		
 		if(aPos >= 0)
 			{
 			TPtrC16 aChar = aDesc.Mid(aPos);// To extract email address at specific position.
 			if(aPos < aChar.Length())
 				{
 				if(aChar[aPos] == '<' && aChar[aLeftPos] == '<' )
 					{
 					return EFalse;
 					}			
 				}	
 			}
 		return (aLeftPos<aRightPos);
		}
	return(EFalse);
	}

TBool TImMessageField::isSurroundedByAngledBrackets(const TDesC16& aAddress, TInt& aLeftEnd, TInt& aRightEnd, TInt aAtPos)
	{	
	// find the '>' position in the string		
	TInt length=aAddress.Length();
	TPtrC16 rightString=aAddress.Right(length-aAtPos-1);
	aRightEnd=rightString.Locate('>');	
	if(aRightEnd<0)
		{
		aRightEnd=aAddress.Locate('>');			
		}
	else
		{
		aRightEnd=aRightEnd+aAtPos+1;
		}				
	// find the '<' position in the string		
	TPtrC16 leftString=aAddress.Left(aAtPos);
	aLeftEnd=leftString.LocateReverse('<');			
	if(aLeftEnd<0)
		{
		aLeftEnd=aAddress.Locate('<');
		if(aLeftEnd<0) 
			{
			// no existance of '<' in the string
			return(EFalse);
			}
		}		
	return(aLeftEnd<aRightEnd);
	}

TBool TImMessageField::isValidChar(const TChar& aChar)
	{
	// Is character within RFC822 limits ie 32<=ASC(aChar)<128
#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
	return (aChar>=0 && aChar<65536);
#else
	return (aChar>31 && aChar<128);
#endif
	}

TBool TImMessageField::isValidString(const TDesC16& aString,TInt& aPos)
	{
	aPos=0;
	TInt end=aString.Length()-1;
	if (end<0)
		{
		return ETrue;	// nothing in string - so cannot be bogus!
		}

	while (aPos<end && isValidChar(aString[aPos]))
		{
		aPos++;
		}

#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
	aPos=0;
	return ETrue;
#else
	return (aPos==end && isValidChar(aString[end]));
#endif
	}


TBool TImMessageField::isValidEmailString(const TDesC16& aString,TInt& aPos)
	{
	aPos = 0;
	TInt atCharPos = aString.Locate('@'); //redo to take a proper constant character
	TInt stringLength = aString.Length();
	
	// Perform dot checking first i.e. that it is not the first or last character of the mailbox or domain
	TInt mailboxEnd = atCharPos-1;
	TInt domainBegin = atCharPos+1;
	TInt domainEnd = stringLength-1;

	if(aString[0] == '.' || aString[mailboxEnd] == '.' || aString[domainBegin] == '.' || aString[domainEnd] == '.')
		{
		return EFalse;
		}
	
	// This first while-loop checks if the mailbox contains valid characters
	while(aPos < atCharPos)
		{
		if(isValidMailboxChar(aString[aPos]))
			{
			aPos++;
			}
		else
			{
			return EFalse;
			}
		}

	// If we reach this point in the code then the local host is OK
	aPos = atCharPos + 1; //increment past the '@' as we don't need to check this
	

	// This second while - loop checks if the domain name is valid, this also includes checking for dot
	// repetitions
	TBool lastCharDot = EFalse;

	while(aPos < stringLength)
		{
		if(lastCharDot)
			{
			if(isDotChar(aString[aPos]))
				{
				// We have two dots after each other therefore address is invalid
				return EFalse;
				}
			else if(isValidDomainNameChar(aString[aPos]))
				{
				lastCharDot = EFalse;
				aPos++;
				}
			else
				{
				return EFalse;
				}
			}
		else
			{
			if(isDotChar(aString[aPos]))
				{				
				lastCharDot = ETrue;
				aPos++;
				}
			else if(isValidDomainNameChar(aString[aPos]))
				{
				lastCharDot = EFalse; // excessive but just in case
				aPos++;
				}
			else
				{
				return EFalse;
				}
			}
		}

	// If we reach this point then we have not encountered an invalid character so we can return ETrue
	return ETrue;
	}


TBool TImMessageField::isSurroundedByRoundBrackets(const TDesC16& aDesc, TInt& aLeftEnd, TInt& aRightEnd)
	{
	return (isEnclosedSubString(aDesc,'(',')',aLeftEnd,aRightEnd));
	}

TBool TImMessageField::isValidEmailChar(const TChar& aChar)
	{
#if defined(__NO_EMAIL_ADDRESS_CHECKING__)
	return (aChar>=0 && aChar<65536);
#else
	return (aChar>32 && aChar<128 && aChar!=44 && aChar!=91 && aChar!=92 && aChar!=93);
#endif
	}

TBool TImMessageField::isValidRoutedEmailAddress(const TDesC16& anAddress)
	{
	TBool valid = EFalse;

	if (anAddress.Length() > 1)
		{
		if ((anAddress[0] == '@') && (anAddress.Locate(':') != KErrNotFound))
			// Presume that an email address starting with '@' is a routed address
			// but only if it also contains a ':' character
			{
			valid = ETrue;
			}
		}

	return valid;
   	}


TBool TImMessageField::isValidDomainNameChar(const TChar& aChar)
	{
	// The following characters are valid for a Domain name
	// 45 = - hyphen
	// 48 to 57 = 0 to 9
	// 65 to 90 = A to Z
	// 97 to 122 = a to z
	// N.B. Whilst dot is a valid char it is considered a special case as
	// two cannot follow each other
	 
	return ( ( aChar >= 'a' && aChar <= 'z') ||   // 97 - 122
	         ( aChar >= '0' && aChar <= '9') ||	  // 48 - 57	 
             ( aChar >= 'A' && aChar <= 'Z') ||   // 65 - 90 
			   aChar == '.' ||                    // 46 
	           aChar == '-'                       // 45
	       );
	}


TBool TImMessageField::isValidMailboxChar(const TChar& aChar)
	{
	// The following characters are valid for a mailbox name
	// 33 = ! excalamation
	// 35 - 39 = #, $, %, &, ' : Hash, dollar, percent, ampersand, single quote
	// 42 - 43 = *, + : Star, plus
	// 45 - 57 = -, ., /, 0 - 9 : Dash, dot, fwd slash, 0 to 9
	// 61 = = : Equals
	// 63 = ? : Question mark
	// 65 - 90 = A - Z : A to Z
	// 94 - 126 = ^, _, `, a - z, {, |, }, ~ : caret, underscore, single opening quote, a to z, open curly braces, line, closed curly braces, tilda
	// N.B. Dot is considered valid but not a special case as although RFC does not allow the following sequence '..'
	// in a mailbox too many commercial email clients and server seem to allow this
	
	return ( ( aChar >= 'A' && aChar <= 'Z') ||   // 65 - 90 
	         ( aChar >= '^' && aChar <= '~') ||   // 94 - 126
	         ( aChar >= '-' && aChar <= '9') ||	  // 45 - 57	
	         ( aChar >= '#' && aChar <= '\'') ||  // 35 - 39
	           aChar == '!' ||                    // 33 
	           aChar == '*' ||                    // 42  
	           aChar == '+' ||                    // 43
	           aChar == '=' ||                    // 61
	           aChar == '?'                       // 63 	       
	       ); 
	}

TBool TImMessageField::isDotChar(const TChar& aChar)
	{
	return (aChar == '.');
	}
