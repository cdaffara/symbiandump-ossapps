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


#include "cimapbodystructure.h"

#include "cimapenvelope.h"


CImapBodyStructure* CImapBodyStructure::NewL()
// static method
// There is no non-trivial construction required, so no need for a ConstructL()
// iRfc822EnvelopeStructure is only constructed when needed.
	{
	return new(ELeave)CImapBodyStructure;
	}

CImapBodyStructure::CImapBodyStructure()
	{}
	
CImapBodyStructure::~CImapBodyStructure()
	{
	iEmbeddedBodyStructureList.ResetAndDestroy();
	
	iParameterList.Close();
	iExtDispositionParameterList.Close();
	iExtLanguageList.Close();
	
	delete iRfc822EnvelopeStructure;
	}

/**
@return The type of this bodystructure
*/	
EXPORT_C CImapBodyStructure::TBodyStructureType CImapBodyStructure::BodyStructureType() const
	{
	return iBodyStructureType;
	}
/**
Multipart bodystructures can have many sub-parts.
Message/RFC822 bodystructures will have a single sub-part.
This method gives access to those sub-parts.
@return The list of sub-part bodystructures.
*/
EXPORT_C const CImapBodyStructure::RBodyStructureList CImapBodyStructure::EmbeddedBodyStructureList() const
	{
	return iEmbeddedBodyStructureList;
	}

/**
@return The MIME type for this message part
*/
EXPORT_C const TDesC8& CImapBodyStructure::Type() const
	{
	return iType;
	}
/**
@return The MIME sub-type for this message part
*/
EXPORT_C const TDesC8& CImapBodyStructure::SubType() const
	{
	return iSubType;
	}

/**
@return A list of parameter-value pairs, corresponding to a top level "body-fld-param"
*/	
EXPORT_C const CImapBodyStructure::RAttributeValuePairList& CImapBodyStructure::ParameterList() const
	{
	return iParameterList;
	}
/**
@return The "body-fld-id" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::BodyId() const
	{
	return iBodyId;
	}
/**
@return The "body-fld-desc" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::BodyDescription() const
	{
	return iBodyDescription;
	}
/**
@return The "body-fld-enc" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::BodyEncoding() const
	{
	return iBodyEncoding;
	}
/**
@return The "body-fld-octets" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::BodySizeOctets() const
	{
	return iBodySizeOctets;
	}
/**
@return The "body-fld-lines" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::BodyLines() const
	{
	return iBodyLines;
	}
/**
@return The "body-fld-md5" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::ExtMD5() const
	{
	return iExtMD5;
	}
/**
@return The "string" part the "body-fld-dsp" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::ExtDispositionName() const
	{
	return iExtDispositionName;
	}
/**
@return A list of parameter-value pairs, corresponding the "body-fld-param" part of a "body-fld-dsp" field
*/	
EXPORT_C const CImapBodyStructure::RAttributeValuePairList& CImapBodyStructure::ExtDispositionParameterList() const
	{
	return iExtDispositionParameterList;
	}
/**
@return A list of the strings returned in the "body-fld-lang" field
*/	
EXPORT_C const CImapBodyStructure::RArrayTPtrC8& CImapBodyStructure::ExtLanguageList() const
	{
	return iExtLanguageList;
	}
/**
@return The "body-fld-loc" field
*/	
EXPORT_C const TDesC8& CImapBodyStructure::ExtLocation() const
	{
	return iExtLocation;	
	}	

/**
Most CImapBodyStructure objects will not need to store an envelope, as these are only 
required by MESSAGE/RFC822.  So this method constructs the envelope on demand.
@return The CImapEnvelope object for this class.  An object is constructed if one does not already exist.
*/
EXPORT_C CImapEnvelope& CImapBodyStructure::GetRfc822EnvelopeStructureL()
	{
	if (iRfc822EnvelopeStructure == NULL)
		{
		iRfc822EnvelopeStructure = CImapEnvelope::NewL();
		}
		
	return *iRfc822EnvelopeStructure;
	}
	
void CImapBodyStructure::SetBodyStructureType(TBodyStructureType aBodyStructureType)
	{
	iBodyStructureType = aBodyStructureType;
	}
void CImapBodyStructure::AppendEmbeddedBodyStructureL(const CImapBodyStructure& aBodyStructure)
	{
	iEmbeddedBodyStructureList.AppendL(&aBodyStructure);
	}
	
void CImapBodyStructure::SetType(const TDesC8& aType)
	{
	iType.Set(aType);
	}
void CImapBodyStructure::SetSubType(const TDesC8& aSubType)
	{
	iSubType.Set(aSubType);
	}
	
void CImapBodyStructure::AppendParameterListL(const TAttributeValuePair& aAttributeValuePair)
	{
	iParameterList.AppendL(aAttributeValuePair);
	}

void CImapBodyStructure::SetBodyId(const TDesC8& aBodyId)
	{
	iBodyId.Set(aBodyId);
	}
void CImapBodyStructure::SetBodyDescription(const TDesC8& aBodyDescription)
	{
	iBodyDescription.Set(aBodyDescription);
	}
void CImapBodyStructure::SetBodyEncoding(const TDesC8& aBodyEncoding)
	{
	iBodyEncoding.Set(aBodyEncoding);
	}
void CImapBodyStructure::SetBodySizeOctets(const TDesC8& aBodySizeOctets)
	{
	iBodySizeOctets.Set(aBodySizeOctets);
	}
void CImapBodyStructure::SetBodyLines(const TDesC8& aBodyLines)
	{
	iBodyLines.Set(aBodyLines);
	}

void CImapBodyStructure::SetExtMD5(const TDesC8& aExtMD5)
	{
	iExtMD5.Set(aExtMD5);
	}
void CImapBodyStructure::SetExtDispositionName(const TDesC8& aExtDispositionName)
	{
	iExtDispositionName.Set(aExtDispositionName);
	}
	
void CImapBodyStructure::AppendExtDispositionParameterListL(const TAttributeValuePair& aAttributeValuePair)
	{
	iExtDispositionParameterList.AppendL(aAttributeValuePair);
	}

void CImapBodyStructure::AppendExtLanguageListL(const TDesC8& aExtLanguage)
	{
	iExtLanguageList.AppendL(aExtLanguage);
	}
void CImapBodyStructure::SetExtLocation(const TDesC8& aExtLocation)
	{
	iExtLocation.Set(aExtLocation);
	}
