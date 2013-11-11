/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ConformanceChecker.h"

#include <gmxmldocument.h>
#include <gmxmlelement.h>

#include "smilliterals.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CConformanceChecker::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C
CConformanceChecker* CConformanceChecker::NewL()
    {
    CConformanceChecker* self = new( ELeave ) CConformanceChecker;
    return self;
    }


// Destructor
CConformanceChecker::~CConformanceChecker()
    {
    }


//
// ----------------------------------------------------------
// CConformanceChecker::Check(CMDXMLDocument* aXmldoc)
//
// Checks if the DOM tree's content is legacy content or not.
// ----------------------------------------------------------
//

TBool CConformanceChecker::Check(CMDXMLDocument* aXmldoc, TMmsSmilVersion aVersion, TInt aFlags)
	{
	//Initializing the flags according to the CF document version
	switch ( aVersion )
		{
		case EMmsSmil_v2_0:
			iFlags = 0;
			break;
		case EMmsSmil_v3_0: //Not implemented yet
			return EFalse;
		default:
			iFlags = 0;
		}
	//Combine the implicit flags  with the explicitly specified flags
	iFlags=iFlags | aFlags;

	if (!aXmldoc) return EFalse;

	CMDXMLElement* root = aXmldoc->DocumentElement();
	if (!root) return EFalse;

	CMDXMLNode* smil = NULL;
	CMDXMLNode* head = NULL;
	CMDXMLNode* body = NULL;

	CMDXMLNode* node = root->FirstChild();
	while (node && node->NodeName() != KSmilTag)
		{
		node=node->NextSibling();
		}

	if ( node ) smil = node;
	else return EFalse;

	node = smil->FirstChild();
	while (node && node->NodeType() != CMDXMLNode::EElementNode)
		{
		node = node->NextSibling();
		}

	if (node && node->NodeName() == KHeadTag)
		{
		head = node;
		if (!CheckHeadStructure(head))
			return EFalse;
		//Going forward in the DOM to find next element
		node = head->NextSibling();
		while (node && node->NodeType() != CMDXMLNode::EElementNode)
			{
			node = node->NextSibling();
			}
		}
	if (node && node->NodeName() == KBodyTag)
		{
		body = node;
		if (!CheckBodyStructure(body))
			return EFalse;
		}
	else if ( node ) return EFalse; //there is some other element (not head or body in the smil element)
	//Checking the head content
	if ( head )
		{
		node = head->FirstChild();
		}
	else
		{
		node = NULL;
		}

	while (node && node->NodeName() != KLayoutTag)
		{
		node = node->NextSibling();
		}
	//Checking the content of the layout
	if (!CheckLayoutContent(node))
		return EFalse;
	//Checking the body content
	if (!CheckBodyContent(body))
		return EFalse;

	return ETrue;
	} //End of Check()

//
// ----------------------------------------------------------
// CConformanceChecker::CheckHeadStructure(CMDXMLNode* aHead)
//
// Checks the structure of the head element: allowed to contain only layout and meta element
// ----------------------------------------------------------
//

TBool CConformanceChecker::CheckHeadStructure(CMDXMLNode* aHead)
	{
	//Checking that the head has only layout and meta element
	if (!aHead)
		{
		return ETrue;
		}
	CMDXMLNode* node = aHead->FirstChild();
	while (node)
		{
		if (node->NodeType() == CMDXMLNode::EElementNode
			&& node->NodeName() != KLayoutTag
			&& node->NodeName() != KMetaTag)
			{
			return EFalse;
			}
		else
			{
			node = node->NextSibling();
			}
		}
	return ETrue;
	}

//
// ----------------------------------------------------------
// CConformanceChecker::CheckBodyStructure(CMDXMLNode* aBody)
//
// Checks the structure of the body element: allowed to contain only par elements
//											(one including seq element right after the body is allowed)
// ----------------------------------------------------------
//
TBool CConformanceChecker::CheckBodyStructure(CMDXMLNode* aBody)
	{
	//Checking that the body has only par elements
	if (!aBody)
		{
		return ETrue;
		}
	CMDXMLNode* node = aBody->FirstChild();
	while (node && node->NodeType() != CMDXMLNode::EElementNode)
		{
		node = node->NextSibling();
		}

	//One seq element right after the body is allowed if EAllowSeqTag is set
	if ((iFlags & EAllowSeqTag)!=0 && node && node->NodeName() == KSeqTag)
		{
		if (node->NextSibling())
			{
			return EFalse;
			}
		else
			{
			node = node->FirstChild();
			}
		}
	while (node)
		{
		if (node->NodeType() == CMDXMLNode::EElementNode && node->NodeName() != KParTag)
			{
			return EFalse;
			}
		else
			{
			node = node->NextSibling();
			}
		}
	return ETrue;
	}

//
// ----------------------------------------------------------
// CConformanceChecker::CheckLayoutContent(CMDXMLNode* aLayout)
//
// Checks the content of the layout element
//			- root-layout and max. 2 region elements are allowed
//			- checks the width/height values for root-layout (pixel is allowed)
//			- checks the width/height/top/left values for region
//			  if EAllowMixedRegionDimensions is not set no mixed pixel/percent allowed
// ----------------------------------------------------------
//

TBool CConformanceChecker::CheckLayoutContent(CMDXMLNode* aLayout)
	{
	TInt rl_nr = 0;
	TInt r_nr = 0;
	if (!aLayout)
		{
		return ETrue;
		}
	CMDXMLNode* node = aLayout->FirstChild();
	TBool foundImage = EFalse;
	TBool foundText = EFalse;
	while (node)
		{
		if (node->NodeType() == CMDXMLNode::EElementNode)
			{
			if (node->NodeName() == KRootLayoutTag)
				{
				if (++rl_nr > 1)
					return EFalse;
				//Check the attributes
				CMDXMLElement* elem = static_cast<CMDXMLElement*>(node);
				TInt n = elem->NumAttributes();
				for (TInt i = 0; i < n; ++i)
					{
					TPtrC name, value;
					elem->AttributeDetails(i,name,value);
					if	( (iFlags & EAllowAllAttributes) == 0 &&
						name != KWidthAttr && name != KHeightAttr )
						{
						return EFalse;
						}
					if (name == KWidthAttr || name == KHeightAttr)
						{
						TBool pixel = EFalse;  // WB!
						TBool percent = EFalse;
						if (!CheckMixedRegionAttribute(value,percent,pixel))
							{
							return EFalse;
							}
						}
					}
				}
			else if (node->NodeName() == KRegionTag)
				{
				if (++r_nr > 2)
					{
					return EFalse;
					}
				//Check the attributes
				TBool foundPixel = EFalse;
				TBool foundPercent = EFalse;

				CMDXMLElement* elem = static_cast<CMDXMLElement*>(node);
				TInt n = elem->NumAttributes();
				for (TInt i = 0; i < n; ++i)
					{
					TPtrC name, value;
					elem->AttributeDetails(i,name,value);

					if ((iFlags & EAllowAllAttributes) == 0 && name != KIdAttr
						&& name != KWidthAttr && name != KHeightAttr
						&& name != KFitAttr && name != KLeftAttr && name != KTopAttr)
						{
						return EFalse;
						}
					if ((iFlags & EAllowMixedRegionDimensions)==0 &&
						(name == KWidthAttr || name == KHeightAttr
						|| name == KTopAttr || name == KLeftAttr)
						&& !CheckMixedRegionAttribute(value,foundPercent,foundPixel))
						{
						return EFalse;
						}
					if ((iFlags & EAllowAnyRegionNames)==0 && name == KIdAttr
						&& !CheckRegionNames(value,foundImage,foundText))
						{
						return EFalse;
						}

					}//for
				}
			else
				{
				return EFalse;
				}

			}
		node = node->NextSibling();
		}//while
	return ETrue;
	}

//
// ----------------------------------------------------------
// CConformanceChecker::CheckMixedRegionAttribute
// Checks if the given value is ending with percent or pixel
// and if this is conforming to the values of aPixel, aPercent.
// Returns true - if the value is not mixed
//		   false - if mixed
// ----------------------------------------------------------
//
TBool CConformanceChecker::CheckMixedRegionAttribute(TDesC& aValue,
													 TBool& aPercent, TBool& aPixel) const
	{
	TInt valueLength = aValue.Length();
	TInt j = 0;
	TBool success = ETrue;
	while (j < valueLength && static_cast<TChar>(aValue[j]).IsDigit())
		{
		j++;
		}
	HBufC* ending = aValue.Right(valueLength-j).Alloc();
	if (!ending)
		{
		success = EFalse;
		}
	else
		{
		TPtr endingPtr = ending -> Des();
		endingPtr.LowerCase();
		endingPtr.TrimRight();
		if (endingPtr==KPercent)
			{
			aPercent = ETrue;
			if (aPixel)
				{
				success = EFalse;
				}
			}
		else if (endingPtr==KNullDesC || endingPtr==KPx)
			{
			aPixel = ETrue;
			if (aPercent)
				{
				success = EFalse;
				}
			}
		else
			success = EFalse; //Cannot be other then pixel or percent
		}
	delete ending;
	return success;
	}


//
// ----------------------------------------------------------
// CConformanceChecker::CheckRegionNames
// Checks if the given value is exactly "Image" or "Text" and if it
// is duplicated (checking the transmitted boolean parameters).
// Returns true - if the value is "Image" or "Text" and it's not duplicated
//		   false - otherwise
// ----------------------------------------------------------
//
TBool CConformanceChecker::CheckRegionNames(TDesC& aValue,
											TBool& aFoundImage, TBool& aFoundText) const
{
	if (aValue == KImage)
		{
		if (aFoundImage)
			{
			return EFalse;
			}
		else
			{
			aFoundImage = ETrue;
			}
		}
	else if (aValue == KText)
		{
		if (aFoundText)
			{
			return EFalse;
			}
		else
			{
			aFoundText = ETrue;
			}
		}
	else
		{
		return EFalse;
		}
	return ETrue;
}

//
// ----------------------------------------------------------
// CConformanceChecker::CheckBodyContent(CMDXMLNode* aBody)
//
// Checks the content of the body element
// ----------------------------------------------------------
//
TBool CConformanceChecker::CheckBodyContent(CMDXMLNode* aBody)
	{
	if (!aBody)
		{
		return ETrue;
		}
	CMDXMLNode* node = aBody->FirstChild();
	//One seq element right after the body is allowed if EAllowSeqTag is set
	if ((iFlags & EAllowSeqTag)!=0 && node && node->NodeName() == KSeqTag)
		{
		node = node->FirstChild();
		}
	while (node)
		{
		if (node->NodeName() == KParTag)
			{
			//Checking the attributes of par element
			//only dur is allowed, dur='indefinite' illegal
			//checking dur is in ms
			CMDXMLElement* elem = static_cast<CMDXMLElement*>(node);
			if (elem->NumAttributes() > 1 && (iFlags & EAllowAllAttributes) == 0)
				{
				return EFalse;
				}
			for (TInt i = 0; i < elem->NumAttributes(); ++i)
				{
				TPtrC name, value;
				elem->AttributeDetails(i,name,value);

				if (name == KDurAttr && value == KIndefiniteVal)
					{
					return EFalse;
					}
				if ((iFlags & EAllowAllAttributes) == 0 && (name != KDurAttr ))
					{
					return EFalse;
					}
				if (name == KDurAttr && (iFlags & EAllowNonMilliseconds) == 0
					&& !IsInMilliseconds(value))
					{
					return EFalse;
					}

				}
			if (!CheckParContent(node))
				return EFalse;
			}
		node = node->NextSibling();
		}
	return ETrue;
	}


//
// ----------------------------------------------------------
// CConformanceChecker::CheckParContent(CMDXMLNode* aPar)
//
// Checks the content of the par element
// ----------------------------------------------------------
//

TBool CConformanceChecker::CheckParContent(CMDXMLNode* aPar)
	{
	if (!aPar)
		{
		return ETrue;
		}
	TBool hasImage=EFalse;
	TBool hasText=EFalse;
	TBool hasAudio=EFalse;
	TBool hasRef=EFalse;
	TBool hasVideo=EFalse;

	RArray<TPtrC> regionNames;
	CMDXMLNode* innernode = aPar->FirstChild();
	TBool legacy = ETrue;
	while (legacy && innernode)
		{
		if (innernode->NodeType() == CMDXMLNode::EElementNode)
			{
			//Checking one type of media per par element
			if (innernode->NodeName() == KImageTag)
				if (hasImage)
					{
					legacy = EFalse;
					}
				else
					{
					hasImage = ETrue;
					}
			else if (innernode->NodeName() == KTextTag)
				if (hasText)
					{
					legacy = EFalse;
					}
				else
					{
					hasText = ETrue;
					}
			else if (innernode->NodeName() == KAudioTag)
				if (hasAudio)
					{
					legacy = EFalse;
					}
				else
					{
					hasAudio = ETrue;
					}
			else if (innernode->NodeName() == KRefTag)
				if (hasRef)
					{
					legacy = EFalse;
					}
				else
					{
					hasRef = ETrue;
					}
			else if (innernode->NodeName() == KVideoTag)
				if ((iFlags & EAllowVideoTag) == 0)
					{
					legacy = EFalse;
					}
				else
					{
					if (hasVideo)
						{
						legacy = EFalse;
						}
					else
						{
						hasVideo=ETrue;
						}
					}
			else legacy = EFalse; //No other elements are allowed then img,text, audio, ref, video
				//Check attributes
			CMDXMLElement* elem = static_cast<CMDXMLElement*>(innernode);
			TInt n = elem->NumAttributes();
			for (TInt i = 0; legacy && (i < n); ++i)
				{
				TPtrC name, value;
				elem->AttributeDetails(i,name,value);
				//Check one region is used only once per par element
				if (name == KRegionAttr)
					{
					if ((iFlags & EAllowAnyRegionNames) == 0
						&& value != KImage && value != KText)
						{
						legacy = EFalse;
						}
					TInt nr = regionNames.Count();
					TInt j = 0;
					while (j < nr && legacy)
						{
						if (regionNames[j]==value)
							{
							legacy = EFalse;
							}
						else
							{
							++j;
							}
						}
					if (legacy)
						{
						regionNames.Append(value);
						if (regionNames.Count()>2)
							{
							legacy = EFalse;
							}
						}

					}
				else if ((iFlags & EAllowAllAttributes) == 0 && name != KSrcAttr
					&& name != KAltAttr && name != KBeginAttr && name != KEndAttr)
					{
					legacy = EFalse;
					}
					//checking the src attribute
				if (name == KSrcAttr && (iFlags & EAllowNonUsAscii) == 0)
					{
					TInt valueLength=value.Length();
					for (TInt i = 0; legacy && (i < valueLength); ++i)
						{
						TInt v = value[i];
						if (v < KUSAsciiMinCode || v > KUSAsciiMaxCode)
							{
							legacy = EFalse;
							}
						}
					}
				//checking begin and end in ms
				if ((name == KBeginAttr || name == KEndAttr)
					&& (iFlags & EAllowNonMilliseconds) == 0
					&& !IsInMilliseconds(value))
					{
						legacy = EFalse;
					}
				}//for
			}
		innernode = innernode->NextSibling();
		} //while
    regionNames.Close();
	return legacy;
	}

//
// ----------------------------------------------------------
// CConformanceChecker::IsInMilliseconds(TDesC aValue);
//
// Checks if  aValue is ending with 'ms'
// ----------------------------------------------------------
//

TBool CConformanceChecker::IsInMilliseconds(TDesC& aValue) const
	{
	TBuf<2> ending;
	TInt j = 0;
	TInt n = aValue.Length();
	TInt trimmedEndingLength = 2;
	//Processing till all the white spaces are eliminated from the end
	do
	{
	j+=2-trimmedEndingLength;
	if (n>=j+2)
		ending = aValue.Mid(n-2-j,2);
	ending.TrimRight();
	trimmedEndingLength = ending.Length();
	}
	while (trimmedEndingLength!=2 && n>j+2);
	ending.LowerCase();
	if (ending != KMs) return EFalse;
	return ETrue;
	}

//  End of File
