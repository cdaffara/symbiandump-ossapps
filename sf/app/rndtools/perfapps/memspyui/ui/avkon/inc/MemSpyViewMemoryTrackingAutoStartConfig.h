/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYVIEWMEMORYTRACKINGAUTOSTARTCONFIG_H
#define MEMSPYVIEWMEMORYTRACKINGAUTOSTARTCONFIG_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>
#include <ecom/ecom.h>
#include <xml/contenthandler.h> // MContentHandler mix in class
#include <xml/parser.h>

// User includes
#include "MemSpyViewBase.h"

// Literal constants
_LIT( KMemSpyProcessMemoryTrackingAutoStartConfigSearchPath, "E:\\MemSpy\\" );
_LIT( KMemSpyProcessMemoryTrackingAutoStartConfigFileName, "MemSpyProcessMemoryTrackingAutoStartConfig.xml" );

// Classes referenced
class CCnvCharacterSetConverter;

// Namespaces referenced
using namespace Xml;


class CMemSpyViewMemoryTrackingAutoStartConfig : public CMemSpyViewBase, public MContentHandler
    {
public:
    CMemSpyViewMemoryTrackingAutoStartConfig( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewMemoryTrackingAutoStartConfig();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    TBool HandleCommandL( TInt aCommand );
    void HandleListBoxItemActionedL( TInt aIndex );
    void SetListBoxModelL();
    void RefreshL();

public: // Menu framework
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    TUint MenuCascadeResourceId() const;
    TInt MenuCascadeCommandId() const;

private: // Command handlers
    void OnCmdItemAddL();
    void OnCmdItemEditL();
    void OnCmdItemDeleteL();
    void OnCmdItemDeleteAllL();
    void OnCmdItemImportL();

private: // Internal methods
    TUid ShowDialogL( TUid aUid );
    void SaveChangesL();
    static TUid ValidateProcessUid( const TDesC& aUid );

private: // Internal XML methods
    void FindXmlInstallTimeL();
    void FindXmlUserSuppliedL();
    HBufC* FindEComXmlFileNameLC();
    void ConvertL( const TDesC8& aInput, TDes16& aOutput );
    void ParseL( const TDesC& aFileName );

private: // XML helper functions
    void OnSectionProcessL( const RAttributeArray& aAttributes );

private: // From MContentHandler
	void OnStartDocumentL( const RDocumentParameters& aDocParam, TInt aErrorCode );
	void OnEndDocumentL( TInt aErrorCode );
	void OnStartElementL( const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode );
	void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );
	void OnContentL( const TDesC8& aBytes, TInt aErrorCode );
	void OnStartPrefixMappingL( const RString& aPrefix, const RString& aUri, TInt aErrorCode );
	void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );
	void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );
	void OnSkippedEntityL( const RString& aName, TInt aErrorCode );
	void OnProcessingInstructionL( const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode );
	void OnError( TInt aErrorCode );
	TAny* GetExtendedInterface( const TInt32 aUid );

private: // Data members
    RArray< TUid > iProcessUids;
    TInt iParserErrorCode;
    TInt iCharconvConverterState;
    TBool iSeenMasterSection;
    CParser* iParser;
    CCnvCharacterSetConverter* iConverter;
    HBufC* iXMLFileNameInstallTime;
    HBufC* iXMLFileNameUserSupplied;
    };


#endif
