// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_sock.h>
#include <utf.h>
#include "pbapappheader.h"

#include "btaccesshostlog.h"


/*static*/ CPbapAppHeader* CPbapAppHeader::NewL()
	{
	LOG_STATIC_FUNC
	return new(ELeave) CPbapAppHeader;
	}


CPbapAppHeader::CPbapAppHeader()
	{
	LOG_FUNC
	Reset();
	}	

CPbapAppHeader::~CPbapAppHeader()
	{
	LOG_FUNC
	iSearchValue.Close();
	}	

/**
Set values to defaults defined in the PBAP specification
*/
void CPbapAppHeader::Reset()
	{
	LOG_FUNC
	
	iTagsPresent.Reset();
	iMaxListCount = KMaxTUint16;
	iListStartOffset = 0;
	iAttributeMask = SymbianPBAP::KPbapAttributeAll;
	iVCardVersion = EPBAPVCard21;
	iOrder = SymbianPBAP::EIndexed;
	iSearchValue.Close();
	iSearchAttribute = SymbianPBAP::EName;
	iOperation = EUnknownOperation;
	iIsAbsolutePathOp = EFalse;
	}
	
/**
Parse the application parameters header for the underlying "PBAP PDU"
this PBAP "PDU" is composed of the TLVs in the App Params Header
*/
void CPbapAppHeader::ParseL(const TDesC8& aObexApplicationHeader)
	{
	LOG_FUNC
	if (aObexApplicationHeader!=KNullDesC8)
		{
		TUint8 parseIndex = 0;		
		TUint8 appHeaderLength = aObexApplicationHeader.Length();
		while (parseIndex < appHeaderLength)
			{
			// get 'tag' from the header
			TApplicationHeaderTagId tag = static_cast<TApplicationHeaderTagId>(aObexApplicationHeader[parseIndex]);
			
			// check weather  'length' is present, then get 'length'
			if(++parseIndex == appHeaderLength)
				{
				User::Leave(KErrArgument);
				}
			TUint8 length = static_cast<TUint8>(aObexApplicationHeader[parseIndex]);
			// check weather 'length' is longer than the remaining header
			if (appHeaderLength-parseIndex <= length)
				{
				User::Leave(KErrArgument);
				}
				
			TPtrC8 nextValue = aObexApplicationHeader.Mid(++parseIndex,length);

			if (nextValue.Length() != length)
				{
				User::Leave(KErrArgument);
				}

			switch (tag)
				{
				case EOrder:
					{	
					LOG(_L("Order tag present"));
					// check whether the parameters header is valid
					if (length!=1)
						{
						User::Leave(KErrArgument);
						}
					iOrder = ConvertToOrder(nextValue[0]);
					LOG1(_L("Order = %d"), iOrder);
					if (iOrder == SymbianPBAP::EUnknownOrder)
						{
						User::Leave(KErrArgument);
						}
					}
					break;
				case ESearchValue:
					{
					LOG(_L("SearchValue tag present"));			

					// the search string may have a null terminator, if so remove it
					TPtrC8 searchValue(nextValue);
					if (length && searchValue[length-1]=='\0')
						{
						LOG(_L("Removing null terminator from search value"));	
						searchValue.Set(searchValue.Left(length-1));
						}
											
					// convert the search string from UTF-8 to unicode
					iSearchValue.Assign(CnvUtfConverter::ConvertToUnicodeFromUtf8L(searchValue)); // ownership transferred
					LOG1(_L("SearchValue = %S"), &iSearchValue);
					}
					break;
				case ESearchAttribute:
					{
					LOG(_L("SearchAttribute tag present"));
					if (length!=1)
						{
						User::Leave(KErrArgument);
						}
					iSearchAttribute = ConvertToSearchAttribute(nextValue[0]);
					LOG1(_L("SearchAttribute = %d"), iSearchAttribute);	
					if (iSearchAttribute == SymbianPBAP::EUnknownSearchAttribute)
						{
						User::Leave(KErrArgument);
						}
					}
					break;
				case EMaxListCount:
					{
					LOG(_L("MaxListCount tag present"));
					if (length!=2)
						{
						User::Leave(KErrArgument);
						}
					iMaxListCount = BigEndian::Get16(nextValue.Ptr());
					LOG1(_L("MaxListCount=%d"), iMaxListCount);
					}
					break;
				case EListStartOffset:
					{
					LOG(_L("ListStartOffset tag present"));
					if (length!=2)
						{
						User::Leave(KErrArgument);
						}
					iListStartOffset = BigEndian::Get16(nextValue.Ptr());
					LOG1(_L("ListStartOffset = %d"), iListStartOffset);
					}
					break;
				case EFilter:
					{
					LOG(_L("Filter tag present"));
					if (length!=8)
						{
						User::Leave(KErrArgument);
						}					
					iAttributeMask = ConvertToFilter(nextValue.Ptr());
					LOG1(_L("Filter tag = %d"), iAttributeMask);
					}
					break;
				case EFormat:
					{
					LOG(_L("Format tag present"));
					if (length!=1)
						{
						User::Leave(KErrArgument);
						}
					iVCardVersion = ConvertToVCardVersionL(nextValue[0]);
					LOG1(_L("vCard version = %d"), iVCardVersion);			
					}
					break;
				default:
					// ignore unknown tag
					break;
				}

			// completely ignore any unspecified tags
			if (tag < EBeginningOfUnspecifiedTags && tag > 0)
				{
				iTagsPresent[tag] = ETrue;
				}

			parseIndex+=length;
			}
		}
	}

/**
 convert sort order parameter values defined in PBAP specification to internal enum
*/
SymbianPBAP::TOrder CPbapAppHeader::ConvertToOrder(TUint8 aValue) const
	{
	LOG_FUNC
	SymbianPBAP::TOrder order;
	switch (aValue)
		{
		case 0x00:
			order = SymbianPBAP::EIndexed;
			break;
		case 0x01:
			order = SymbianPBAP::EAlphabetical;
			break;
		case 0x02:
			order = SymbianPBAP::EPhonetical;
			break;
		default:
			order = SymbianPBAP::EUnknownOrder;
			break;	
		}
	return order;
	}


/**
 convert search attribute parameter values defined in PBAP specification to internal enum
*/
SymbianPBAP::TSearchAttribute CPbapAppHeader::ConvertToSearchAttribute(TUint8 aValue) const
	{
	LOG_FUNC
	SymbianPBAP::TSearchAttribute searchAttrib;
	switch (aValue)
		{
		case 0x00:
			searchAttrib = SymbianPBAP::EName;
			break;
		case 0x01:
			searchAttrib = SymbianPBAP::ENumber;
			break;
		case 0x02:
			searchAttrib = SymbianPBAP::ESound;
			break;
		default:
			searchAttrib = SymbianPBAP::EUnknownSearchAttribute;
			break;	
		}
	return searchAttrib;
	}

/**
 convert format parameter values defined in PBAP specification to internal enum
*/	
TVCardVersion CPbapAppHeader::ConvertToVCardVersionL(TUint8 aValue) const
	{
	LOG_FUNC
	TVCardVersion version = EPBAPVCard21;
	switch (aValue)
		{
		case 0x00:
			version = EPBAPVCard21;
			break;
		case 0x01:
			version = EPBAPVCard30;
			break;
		default:
			User::Leave(KErrArgument);
			break;	
		}
	return version;
	}

/** 
 Converts big-endian formatted descriptor to 64-bit filter mask
*/
TUint64 CPbapAppHeader::ConvertToFilter(const TUint8* aPtr) const
	{
	LOG_FUNC
	return ((static_cast<TUint64>(aPtr[0])<<56) |
			(static_cast<TUint64>(aPtr[1])<<48) |
			(static_cast<TUint64>(aPtr[2])<<40) |
			(static_cast<TUint64>(aPtr[3])<<32) |
			(static_cast<TUint64>(aPtr[4])<<24) |
			(static_cast<TUint64>(aPtr[5])<<16) |
			(static_cast<TUint64>(aPtr[6])<<8)  |
			(static_cast<TUint64>(aPtr[7]))); 	
	}
	
/**
Determine the actual PBAP operation from the type header and the parsed
application parameter header 
*/		
TPbapOperation CPbapAppHeader::DeterminePBAPOperationL(const TDesC8& aTypeHeader)
	{
	LOG_FUNC

#ifdef __FLOG_ACTIVE
	RBuf buf;
	buf.CreateL(aTypeHeader.Length());
	buf.Copy(aTypeHeader);
	LOG1(_L("Type header: %S"), &buf);
	buf.Close();
#endif

	TPtrC8 typeHeader(aTypeHeader);
	TInt length = aTypeHeader.Length();
	
	// obex may append null terminator to type header, if so remove it
	if (length && aTypeHeader[length-1]=='\0')
		{	
		typeHeader.Set(aTypeHeader.Left(length-1));
		}
	
	if (typeHeader.CompareF(KObexTypeBTvCardPhoneBook)==0)
		{
		if (!iTagsPresent[EMaxListCount])
			{
			LOG(_L("Missing mandatory MaxListCount tag in App Header"));
			// mandatory to have the tag present in this operation
			User::Leave(KErrArgument);
			}		
		if (iMaxListCount==0)
			{
			iOperation = ERequestPhoneBookSize;
			LOG(_L("PBAP operation is PullPhoneBook - MaxListCount = 0"));
			}
		else
			{
			iOperation = EPullPhoneBook;
			LOG(_L("PBAP operation is PullPhoneBook"));
			}
			
		// this type of request operates on absolute paths	
		iIsAbsolutePathOp = ETrue;
		}
	else if (typeHeader.CompareF(KObexTypeBTvCardListing)==0)
		{
		if (!iTagsPresent[EMaxListCount])
			{
			LOG(_L("Missing MaxListCount tag in App Header, continuing due to spec (v1.0) inconsistency"));
			}		
		if (iMaxListCount==0)
			{
			iOperation = ERequestPhoneBookSize;
			LOG(_L("PBAP operation is PullvCardListing - MaxListCount = 0"));
			}
		else
			{
			iOperation = EPullVCardListing;
			LOG(_L("PBAP operation is PullvCardListing"));
			}
		}
	else if (typeHeader.CompareF(KObexTypeBTvCard)==0)
		{
		// leave to folderparser to determine if handle is ok
		iOperation = EPullVCard;
		LOG(_L("PBAP operation is PullvCard"));		
		}		
	else
		{
		// incorrect or nonexistent Type header. Bad.
		LOG(_L("Cannot determine PBAP operation"));
		User::Leave(KErrCorrupt);
		}

	return iOperation;
	}
