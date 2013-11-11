/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PC Connectivity server
*
*/


#ifndef __SCONPCCONNSERVER_H__
#define __SCONPCCONNSERVER_H__

#include "sconconmlhandler.h"
#include "sconconmltask.h"


class CSConPCD;
class CSConCSC;
class CSConConMLParser;

const TUint KSConDefaultHeapSize=0x10000;
const TInt KSConFirstDrive = 65;

_LIT8 ( KParamNameSilent, "Silent" );


// PCD Handler Dll
_LIT(KSConPCDLibName, "sconpcd.dll");
const TInt KSConPCDUidValue = 0x10009D8D;
const TUid KSConPCDUid = {KSConPCDUidValue};

// CSC Handler Dll
_LIT(KSConCSCLibName, "sconcsc.dll");
const TInt KSConCSCUidValue = 0x10009D8D;
const TUid KSConCSCUid = {KSConCSCUidValue};

// ConML Handler Dll
_LIT (KSConConMLHandlerLibName, "sconconmlhandler.dll");
const TInt KSConConMLHandlerUidValue = 0x10009D8D;
const TUid KSConConMLHandlerUid = {KSConConMLHandlerUidValue};

// debug file for put and get messages
_LIT ( KSConConMLDebugFile, "c:\\data\\handler_result.txt");

// Secure ids
_LIT_SECURE_ID(KSConPCConnClientSecureId,0x101F7C87);

// PCD types
_LIT8( KSConPCDXMLObjectType, "application/vnd.nokia.conml+xml" );
_LIT8( KSConPCDXMLObjectType2, "application/vnd.nokia.pcd+xml" );
_LIT8( KSConPCDWBXMLObjectType, "application/vnd.nokia.conml+wbxml" );
_LIT8( KSConPCDWBXMLObjectType2, "application/vnd.nokia.pcd+wbxml" );

// CSC types
_LIT8( KSConCapabilityObjectType, "x-obex/capability" );

// Drives
_LIT( KSConNoDrive, "\x0" );
_LIT( KSConDriveExists, "\x1" );

// user permissions
_LIT8( KSConPermReadOnly, "R" );
_LIT8( KSConPermNormal, "RW" );
// Java hash id for ConML
_LIT8( KSConJavaHashId, "JAVA_");

_LIT8( KSConAppInfoSeparator, "#" );

//**********************************
//CSConPCConnServer - PC Connectivity Server 
//**********************************

class CSConPCConnServer : public CServer2
    {
public:
        // From CServer2
    IMPORT_C static CSConPCConnServer* NewLC();
    IMPORT_C ~CSConPCConnServer();
    
    /**
     * Function to start the server
     * @param none
     * @return Error code
     */
    static TInt RunServer();
    
    /**
     * Function to start the server
     * @param none
     * @return none
     */
    static void RunServerL();
    
    /**
     * Increments session count.
     * @param none
     * @return none
     */
    void IncSessionCount();
    
    /**
     * Decrements session count. 
     * When count reaches zero, stops active scheduler
     * @param none
     * @return none
     */ 
    void DecSessionCount();
    

protected:
    virtual CSession2* NewSessionL( const TVersion &aVersion, 
                                    const RMessage2& aMessage) const;

private:
    CSConPCConnServer();
    
    TInt iSessionCount;

    };


class CSconFolderLister;
class CSconBtEngine;

//**********************************
//CSConPCConnSession - Server session class, handles the requested operations
//**********************************
class CSConPCConnSession : public CSession2, public MWBXMLConMLCallback
    {

public:
    static CSConPCConnSession* NewL(CSConPCConnServer& aServer);
    
    /**
    * From CSession2
    */
    void ServiceL( const RMessage2 &aMessage);

private:
    void DispatchRequestL( const RMessage2 &aMessage);
    void LoadPCDDllL();
    void LoadCSCDllL();
    void LoadConMLHandlerDllL();

protected:
    // Callback funktion for WBXML parsing
    void ConMLL ( ConML_ConMLPtr_t aContent );

private:
    void ConstructL();
    CSConPCConnSession ( CSConPCConnServer& aServer);
    ~CSConPCConnSession();

    TInt HandlePutMessageL();
    TInt HandleGetMessageL();
    TInt HandleWBXMLGetRequestL( const TDesC& aFileName );
    TInt HandleResetMessage();
    TInt HandleChunkMessage( const RMessage2& aMessage );
    TInt OptionsFromExecuteL ( ConML_ExecutePtr_t aContent );
    TInt TaskCancelL(ConML_CancelPtr_t aContent);
    TInt TaskGetStatusL(ConML_GetStatusPtr_t aContent );
    TInt TaskInstallL( ConML_InstallPtr_t aContent );
    TInt TaskUnInstallL( ConML_UnInstallPtr_t aContent );
    void SplitL( const TDesC8& aText, const TChar aSeparator, RArray<TPtrC8>& aArray );
    TInt TaskListInstalledAppsL( ConML_ListInstalledAppsPtr_t aContent );
    TInt TaskListDataOwnersL();
    TInt TaskSetBURModeL( ConML_SetBURModePtr_t aContent );
    TInt TaskGetDataSizeL( ConML_GetDataSizePtr_t aContent );
    TInt TaskRequestDataL( ConML_RequestDataPtr_t aContent );
    TInt TaskUpdateDeviceInfoL( ConML_UpdateDeviceInfoPtr_t aContent );
    TInt TaskListPublicFilesL( ConML_ListPublicFilesPtr_t aContent );
    TInt TaskGetDataOwnerStatusL( ConML_GetDataOwnerStatusPtr_t aContent );
    TInt TaskSupplyDataL ( ConML_SupplyDataPtr_t aContent );
    TInt TaskRebootL();
    TInt TaskGetMetadataL( ConML_GetMetadataPtr_t aContent );
    void AppendStatusL( ConML_ConMLPtr_t aContent, CSConStatusReply* reply );
    void AppendUpdateDeviceInfoResultsL( ConML_ResultsPtr_t aContent, 
                                         CSConUpdateDeviceInfo* aResult );
    void AppendSetBURModeResultsL ( ConML_ResultsPtr_t aContent, 
                                    CSConSetBURMode* aResult );
    void AppendListInstalledAppsResultsL ( ConML_ResultsPtr_t aContent, 
                                           CSConListInstApps* aResult );
    void AppendListPublicFilesResultsL ( ConML_ResultsPtr_t aContent, 
                                         CSConListPublicFiles* aResult );
    void AppendRequestDataResultsL( ConML_ResultsPtr_t aContent, 
                                    CSConRequestData* aResult );
    void AppendSupplyDataResultsL( ConML_ResultsPtr_t aContent, 
                                   CSConSupplyData* aResult );
    void AppendInstallResultsL( ConML_ResultsPtr_t aContent, 
                                CSConInstall* aResult );
    void AppendUninstallResultsL( ConML_ResultsPtr_t aContent, 
                                CSConUninstall* aResult );                          
    void AppendListDataOwnersResultsL ( ConML_ResultsPtr_t aContent, 
                                        CSConListDataOwners* aResult  );
    void AppendGetDataOwnerStatusResultsL ( ConML_ResultsPtr_t aContent, 
                                            CSConGetDataOwnerStatus* aResult );
    void AppendGetDataSizeResultsL ( ConML_ResultsPtr_t aContent, 
                                     CSConGetDataSize* aResult );
    void AppendGetMetadataResultsL ( ConML_ResultsPtr_t aContent, 
                                     CSConGetMetadata* aResult );
    void AppendProgressL ( ConML_ResultsPtr_t aContent, TInt progress );
    void AppendDrivesL( ConML_DrivesPtr_t aContent, TDriveList aDrives );
    TDriveList DriveList( ConML_DriveListPtr_t aContent );
    TDesC8& IntToDesLC( const TInt& anInt);
    TDesC8& DriveNumToDesLC( const TInt& anInt );
    TDesC8& BufToDesLC( const TDesC& aBuf );
    TDesC8& UidToDesLC( const TUid& aUid );
    TUid DesToUid ( const TDesC8& aDes );
    TInt DesToInt( const TDesC8& aDes);
    TBool HasDrives( TDriveList& aDrive );
    TDesC8& HashToDesLC( const TDesC& aBuf );
    TPtr DesToHashLC( const TDesC8& aDes );
    TBool IsJavaHash( const TDesC8& aDes );
    
    TInt ReadOviAccountInfoL( RWriteStream& aAccountInfoStream );
    
// data
private:
    CSConPCConnServer& iServer;
    
    RLibrary iPCDlib;
    RLibrary iCSClib;
    RLibrary iConMLHandlerLib;
    
    CSConPCD* iPCDHandler;
    CSConCSC* iCSCHandler;
    CSConConMLHandler* iConMLHandler;
    
    RChunk iChunk;
    CBufFlat* iBuffer;  
    TInt iResult;
    
    CSconFolderLister* iFolderLister;
    RFs iFs;
    CSconBtEngine*  iBtEngine;
    };

#endif // __SCONPCCONNSERVER_H__
