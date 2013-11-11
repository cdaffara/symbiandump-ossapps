/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONCONMLGENERATOR_H__
#define __SCONCONMLGENERATOR_H__

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "sconxmlelement.h"
#include "sconconmldtd.h"
#include "sconconmltags.h"
#include "sconwbxmldefs.h"
#include "sconxmlworkspace.h"

// -----------------------------------------------------------------------------
// MWBXMLConMLCallback - Call back for parsed data
// -----------------------------------------------------------------------------
class MWBXMLConMLCallback
    {
public:
     /**
    * Call back function for delivering the parsed data
    * @param aContent contains the parsed data
    * @return none
    */

    virtual void ConMLL( ConML_ConMLPtr_t aContent ) = 0;
    };

// -----------------------------------------------------------------------------
// TWBXMLContentFormat
// -----------------------------------------------------------------------------
enum TWBXMLContentFormat
    {
    EWBXMLContentFormatOpaque,
    EWBXMLContentFormatInlineString
    };
    
// -----------------------------------------------------------------------------
// CSConConMLGenerator - Generates XBXML and XML documents
// -----------------------------------------------------------------------------
NONSHARABLE_CLASS( CSConConMLGenerator ) : public CBase
    {
public:

    static CSConConMLGenerator* NewL();
    ~CSConConMLGenerator();
        
    /**
    * Returns the generated WBXML document
    * @param none 
    * @return the document as TPtrC8
    */
    TPtrC8 WBXMLDocument();

    /**
    * Returns the generated XML document
    * @param none 
    * @return the document as TPtrC8
    */
    TPtrC8 XMLDocument();
    
    
    // WBXML Parser
    
    /**
    * Sets the callback function to call after parsing
    * @param aCallback callback function to call when parsing finished
    * @return none
    */
    void SetCallback ( MWBXMLConMLCallback* aCallbakc );
    
    /**
    * Starts the document after header has been parsed 
    * @param aVersion - version of WBXML document
    * @param aPublicId - public ID of WBXML document as known public ID
    * @param aCharset - Character set of WBXML document
    * @return none
    */
    void StartDocument( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset );

    /**
    * Starts the document after header has been parsed 
    * @param aVersion - version of WBXML document
    * @param aPublicIdStr - public ID of WBXML document as a inline string 
    * @param aCharset - Character set of WBXML document
    * @return none
    */
    void StartDocument( 
        TUint8 aVersion, const TDesC8& aPublicIdStr, TUint32 aCharset );
    
    /**
    * Starts a new element in the document
    * @param aTag - Tag of the element
    * @return none
    */
    void StartElementL( TWBXMLTag aTag );

    /**
    * Starts a character data in the document
    * @param aBuffer - Character data
    * @return none
    */
    void CharactersL( const TDesC8& aBuffer );

    /**
    * Document has been parsed - the end of document
    * @param none
    * @return none
    */
    void EndDocument();
        
    /**
    * End tag of an element has been found
    * @param aTag - tag of the element to be closed 
    * @return none
    */
    void EndElementL( TWBXMLTag aTag );
    
    // Generator
    
    /**
    * Starts the generation of a document
    * @param aContent - data for the document
    * @return none
    */
    TInt GenerateConMLDocument ( ConML_ConMLPtr_t aContent );

private:

    // WBXML Generator
    void WriteMUint32L( TUint32 aValue );
    void WriteOpaqueDataL( const TDesC8& aData );
    void WriteInlineStringL( const TDesC8& aData );
    
    // XML Generator
    void IndentL();
    TPtrC8 TranslateElement( TUint8 aElement );

    // WBXML / XML Generator
    void BeginDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset, 
        const TDesC8& aStringTbl = TPtrC8() );
    void BeginElementL( TUint8 aElement, TBool aHasContent = EFalse, 
        TBool aHasAttributes = EFalse );
    void EndElementL();
    void AddElementL( 
        TUint8 aElement, const TDesC8& aContent, 
        const TWBXMLContentFormat aFormat = EWBXMLContentFormatOpaque );
    void AppendPCDataL( TUint8 aElement, pcdataPtr_t aContent );
    void AppendConMLL( ConML_ConMLPtr_t aContent );
    void AppendExecuteL( ConML_ExecutePtr_t aContent );
    void AppendSupplyDataL( ConML_SupplyDataPtr_t aContent );
    void AppendInstallL( ConML_InstallPtr_t aContent );
    void AppendCancelL ( ConML_CancelPtr_t aContent );
    void AppendStatusL ( ConML_StatusPtr_t aContent );
    void AppendGetStatusL( ConML_GetStatusPtr_t aContent );
    void AppendRebootL( ConML_RebootPtr_t aContent );
    void AppendTaskL( ConML_TaskPtr_t aContent );
    void AppendTaskListL( ConML_TaskListPtr_t aContent );
    void AppendUnInstallL( ConML_UnInstallPtr_t aContent );
    void AppendListInstalledAppsL ( ConML_ListInstalledAppsPtr_t aContent );
    void AppendListDataOwnersL ( ConML_ListDataOwnersPtr_t aContent );
    void AppendBUROptionsL( ConML_BUROptionsPtr_t aContent );
    void AppendSetBURModeL( ConML_SetBURModePtr_t aContent );
    void AppendGetDataSizeL( ConML_GetDataSizePtr_t aContent );
    void AppendRequestDataL( ConML_RequestDataPtr_t aContent );
    void AppendUpdateDeviceInfoL( ConML_UpdateDeviceInfoPtr_t aContent );
    void AppendListPublicFilesL( ConML_ListPublicFilesPtr_t aContent );
    void AppendApplicationL( ConML_ApplicationPtr_t aContent );
    void AppendApplicationListL( ConML_ApplicationListPtr_t aContent );
    void AppendApplicationsL( ConML_ApplicationsPtr_t aContent );
    void AppendParamL( ConML_ParamPtr_t aContent );
    void AppendParamListL( ConML_ParamListPtr_t aContent );
    void AppendInstParamsL( ConML_InstParamsPtr_t aContent );
    void AppendProgressL( ConML_ProgressPtr_t aContent );
    void AppendResultsL( ConML_ResultsPtr_t aContent );
    void AppendDriveL( ConML_DrivePtr_t aContent );
    void AppendDriveListL( ConML_DriveListPtr_t aContent );
    void AppendDrivesL( ConML_DrivesPtr_t aContent );
    void AppendDataOwnersL( ConML_DataOwnersPtr_t aContent );
    void AppendPackageInfoL ( ConML_PackageInfoPtr_t aContent );
    void AppendSIDL( ConML_SIDPtr_t aContent );
    void AppendSIDListL( ConML_SIDListPtr_t aContent );
    void AppendDeviceInfoL( ConML_DeviceInfoPtr_t aContent );
    void AppendFilesL( ConML_FilesPtr_t aContent );
    void AppendSupportedMethodsL( ConML_SupportedMethodsPtr_t aContent );
    void AppendFileListL( ConML_FileListPtr_t  aContent );
    void AppendFileL( ConML_FilePtr_t aContent );
    void AppendGetDataOwnerStatusL( ConML_GetDataOwnerStatusPtr_t aContent );
    void AppendGetMetadataL( ConML_GetMetadataPtr_t aContent );
    
    TInt HandleResult( TInt aResult, TInt aTreshold = 0 );
    
private:
    void ConstructL();
    CSConConMLGenerator();
    void AddElementL( CXMLElement* aElement );

private:
    // Parser
    CXMLStack<CXMLElement>* iCmdStack;
    CXMLStack<CXMLElement>* iCleanupStack;
    MWBXMLConMLCallback* iCallback;

    // Generator workspaces 
    CXMLWorkspace* iWBXMLWorkspace;
    CXMLWorkspace* iXMLWorkspace;

    // XML
    RArray<TUint8> iElemStack;
    TInt iInitialIndentLevel;
    TBool iDontNewLine;
    TBool iDontIndent;
    }; 

#endif // __SCONCONMLGENERATOR_H__
