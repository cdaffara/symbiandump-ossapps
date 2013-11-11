/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Internal header for ApplicationManagementOptions
*
*/


#ifndef __AMINSTALLOPTIONSINT_H__
#define __AMINSTALLOPTIONSINT_H__


#include <parser.h>

#include <contenthandler.h>
#include <documentparameters.h>
#include <SWInstDefs.h>


class CAMInstOptsParserBase : public CBase, public Xml::MContentHandler
	{
public:
	virtual ~CAMInstOptsParserBase();
	void ParseL();
protected:
	const TDesC8 &iBuffer;
	Xml::CParser *iParser ;
	TInt iCustIndex ;
	SwiUI::TPolicy iDefaultPolicy;
	
	CAMInstOptsParserBase( 
		const TDesC8 &aBuffer, 
		SwiUI::TPolicy aDefaultPolicy = SwiUI::EPolicyUserConfirm 
	  )	: iBuffer( aBuffer ), iCustIndex(-1), iDefaultPolicy ( aDefaultPolicy )
		{
		
		}

	void SetPolicyValue( SwiUI::TPolicy &aPolicy, const TDesC8& aValue ) const;
	void ConstructL();
	

	virtual void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
	virtual void OnEndDocumentL(TInt aErrorCode);
	virtual void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	virtual void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
	virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	virtual void OnError(TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);
	
	};	
	
class CAMInstOptsParser : public CAMInstOptsParserBase//, public Xml::MContentHandler
	{
	//SwiUI::TInstallOptions &iResult;
	NApplicationManagement::TAMInstallOptions &iResult;

	//void ParseL();
	public:
		
			
		static CAMInstOptsParser * NewL( const TDesC8 &aBuffer, NApplicationManagement::TAMInstallOptions &aResult );
		static CAMInstOptsParser * NewLC( const TDesC8 &aBuffer, NApplicationManagement::TAMInstallOptions &aResult );
		
		CAMInstOptsParser( const TDesC8 &aBuffer, NApplicationManagement::TAMInstallOptions &aResult ) 
			: CAMInstOptsParserBase ( aBuffer ), iResult( aResult )
			{
			
			}
		
		virtual ~CAMInstOptsParser();
	
    	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	private:
	};    			

class CAMUninstOptsParser : public CAMInstOptsParserBase
	{
	SwiUI::TUninstallOptions &iResult; 
	public:
		static CAMUninstOptsParser * NewL( const TDesC8 &aBuffer, SwiUI::TUninstallOptions &aResult );
		static CAMUninstOptsParser * NewLC( const TDesC8 &aBuffer, SwiUI::TUninstallOptions &aResult );
		
		CAMUninstOptsParser( const TDesC8 &aBuffer, SwiUI::TUninstallOptions &aResult ) 
			: CAMInstOptsParserBase( aBuffer ), iResult( aResult )
			{
			
			}
		virtual ~CAMUninstOptsParser();
	
    	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	private:
	};    

#endif // __AMINSTALLOPTIONSINT_H__

//  End of File
