/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "engine.h"
#include "enginewrapper.h"
#include "creator_file.h" 
#include "creator_traces.h"

using namespace ContentAccess;

static const TInt KFilesFieldLength = 256;

//_LIT(KCreatorFilesPrefixName, "CR_");
//_LIT(KCreatorFilesPrefixFolderName, "CR_FLDR_");

//----------------------------------------------------------------------------

CFilesParameters::CFilesParameters()
    {
    LOGSTRING("Creator: CFilesParameters::CFilesParameters");
    iFullFilePath = HBufC::New(KFilesFieldLength);
    }

CFilesParameters::CFilesParameters( CFilesParameters& aCopy )
    {
    LOGSTRING("Creator: CFilesParameters::CFilesParameters");
    iFullFilePath = HBufC::New(KFilesFieldLength);
    iFullFilePath->Des().Copy( *aCopy.iFullFilePath );
    iFileCommand = aCopy.iFileCommand;
    iEncrypt = aCopy.iEncrypt;
    if ( aCopy.iPermission )
        {
        TRAP_IGNORE(    
            iPermission = CDRMPermission::NewL();
            iPermission->DuplicateL( *aCopy.iPermission );
            );
        }
    }

CFilesParameters::~CFilesParameters()
    {
    LOGSTRING("Creator: CFilesParameters::~CFilesParameters");
    delete iFullFilePath;
    delete iPermission;
    }

//----------------------------------------------------------------------------

CCreatorFiles* CCreatorFiles::NewL(CCreatorEngine* aEngine)
    {
    CCreatorFiles* self = CCreatorFiles::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorFiles* CCreatorFiles::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorFiles* self = new (ELeave) CCreatorFiles;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorFiles::CCreatorFiles() :
    iFs ( CEikonEnv::Static()->FsSession() )
    {
    }

void CCreatorFiles::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorFiles::ConstructL");

    iEngine = aEngine;

    User::LeaveIfError( iApaLs.Connect() );
    
    iFilePaths = new (ELeave) CDesCArrayFlat( 4 );
    
    // Restore file id 
    CDictionaryFileStore* store = iEngine->FileStoreLC();
    User::LeaveIfNull( store );
    if ( store->IsPresentL( KUidDictionaryUidFiles ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, KUidDictionaryUidFiles );
        TRAPD( err, iFileId = in.ReadInt32L() );
        if ( err )
            {
            iFileId = 1;
            }
        CleanupStack::PopAndDestroy(); // in
        }
    else
        {
        iFileId = 1;
        }
    CleanupStack::PopAndDestroy( store );
    }

CCreatorFiles::~CCreatorFiles()
    {
    LOGSTRING("Creator: CCreatorFiles::~CCreatorFiles");
    
    // this is done only once per file operation:
    if ( iFilePaths && iFilePaths->Count() )
        {
        TRAP_IGNORE( StorePathsForDeleteL( *iFilePaths ) );
        }
    delete iFilePaths;
    delete iParameters;
    delete iUserParameters;
    iApaLs.Close();
    }

//----------------------------------------------------------------------------

void CCreatorFiles::QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
    {
    LOGSTRING("Creator: CCreatorFiles::QueryDialogClosedL");
    
    if( aPositiveAction == EFalse )
        {
        iEngine->ShutDownEnginesL();
        return;
        }
    
    const TDesC* showText = &KSavingText;
    TBool finished(EFalse);
    TBool retval(ETrue);
    switch(aUserData)
        {
        case ECreatorFilesDelete:
            showText = &KDeletingText;
            iEntriesToBeCreated = 1;
            finished = ETrue;
            break;
        case ECreatorFilesStart:
            {
            // set a default directory  (eg. c:\Nokia\Images\)
            iEngine->SetDefaultPathForFileCommandL(iCommand, iDirectoryQueriedFromUser);
            TBuf<50> promptText;
            if (iCommand == ECmdCreateFileEntryEmptyFolder)
                promptText.Copy( _L("Specify the folder path and name") );
            else
                promptText.Copy( _L("Specify the directory") );
            
            // show directory query dialog
            retval = iEngine->GetEngineWrapper()->DirectoryQueryDialog(promptText, iDirectoryQueriedFromUser, this, ECreatorFilesGetDirectory );
            }
            break;
        case ECreatorFilesGetDirectory:
            // check that the root folder is correct
            if ( iDirectoryQueriedFromUser.Length() < 3  ||  BaflUtils::CheckFolder( iFs, iDirectoryQueriedFromUser.Left(3) ) != KErrNone )
                {
                iEngine->GetEngineWrapper()->ShowErrorMessage(_L("Invalid path"));
                retval = EFalse;
                }        
            else
                {
                // check the directory contains a trailing backlash
                if ( iDirectoryQueriedFromUser.Right(1) != _L("\\") )
                    {
                    iDirectoryQueriedFromUser.Append(_L("\\"));
                    }
                // copy the directory name to a class member
                if ( iCommand == ECmdCreateFileEntryEmptyFolder )
                    {
                    finished = ETrue;
                    }
                else
                    {
                    retval = AskDRMDataFromUserL();
                    }
                }
            break;
        case ECreatorFilesAskDRMData:
            if ( iDummy > 0 )
                {
                iUserParameters->iEncrypt = ETrue;
                }
            if ( iDummy == 2 )
                {
                iUserParameters->iPermission = CDRMPermission::NewL();
                CDRMPermission* perm = iUserParameters->iPermission; 
                perm->iTopLevel->iActiveConstraints = EConstraintNone;
                perm->iPlay->iActiveConstraints = EConstraintNone;
                perm->iDisplay->iActiveConstraints = EConstraintNone;
                perm->iPrint->iActiveConstraints = EConstraintNone;
                perm->iExecute->iActiveConstraints = EConstraintNone;
                perm->iUniqueID = 0;
                // DRM Combined Delivery
                iDummy = 0;
                retval = iEngine->GetEngineWrapper()->EntriesQueryDialog( &iDummy, _L("How many counts(0=unlimited)?"), ETrue, this, ECreatorFilesAskDRM_CD_Counts );
                }
            else
                {
                finished = ETrue;
                }
            break;
        case ECreatorFilesAskDRM_CD_Counts:
            if ( iDummy > 0 )
                {
                TInt count = iDummy;
                CDRMPermission* perm = iUserParameters->iPermission;
                // apply constraints to all permission types
                // applied type will be selected by setting iAvailableRights
                // when determining the file type
                perm->iDisplay->iActiveConstraints |= EConstraintCounter;
                perm->iDisplay->iCounter = count;
                perm->iDisplay->iOriginalCounter = count;
    
                perm->iPlay->iActiveConstraints |= EConstraintCounter;
                perm->iPlay->iCounter = count;
                perm->iPlay->iOriginalCounter = count;
    
                perm->iPrint->iActiveConstraints |= EConstraintCounter;
                perm->iPrint->iCounter = count;
                perm->iPrint->iOriginalCounter = count;
                
                perm->iExecute->iActiveConstraints |= EConstraintCounter;
                perm->iExecute->iCounter = count;
                perm->iExecute->iOriginalCounter = count;
                }
            iDummy = 0;
            retval = iEngine->GetEngineWrapper()->EntriesQueryDialog( &iDummy, _L("How many minutes until expire(0=unlimited)?"), ETrue, 
                this, ECreatorFilesAskDRM_CD_Minutes
                );
            break;
        case ECreatorFilesAskDRM_CD_Minutes:
            if ( iDummy > 0 )
                {
                TInt minutes = iDummy;
                CDRMPermission* perm = iUserParameters->iPermission;            
                // apply constraints to all permission types
                // applied type will be selected by setting iAvailableRights
                // when determining the file type            
                perm->iDisplay->iActiveConstraints |= EConstraintInterval;
                perm->iDisplay->iInterval = TTimeIntervalSeconds( 60 * minutes );
                perm->iDisplay->iIntervalStart = Time::NullTTime();
                
                perm->iPlay->iActiveConstraints |= EConstraintInterval;
                perm->iPlay->iInterval = TTimeIntervalSeconds( 60 * minutes );
                perm->iPlay->iIntervalStart = Time::NullTTime();
    
                perm->iPrint->iActiveConstraints |= EConstraintInterval;
                perm->iPrint->iInterval = TTimeIntervalSeconds( 60 * minutes );
                perm->iPrint->iIntervalStart = Time::NullTTime();
    
                perm->iExecute->iActiveConstraints |= EConstraintInterval;
                perm->iExecute->iInterval = TTimeIntervalSeconds( 60 * minutes );
                perm->iExecute->iIntervalStart = Time::NullTTime();
                }
            finished = ETrue;
            break;
        default:
            //some error
            retval = EFalse;
            break;
        }
    if( retval == EFalse )
        {
        iEngine->ShutDownEnginesL();
        }
    else if( finished )
        {
        // add this command to command array
        iEngine->AppendToCommandArrayL(iCommand, NULL, iEntriesToBeCreated);
        // started exucuting commands
        iEngine->ExecuteFirstCommandL( *showText );
        }
    }

//----------------------------------------------------------------------------

TBool CCreatorFiles::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorFiles::AskDataFromUserL");
    
    CCreatorModuleBase::AskDataFromUserL( aCommand );
    
    if ( aCommand == ECmdDeleteCreatorFiles )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all files created with Creator?"), this, ECreatorFilesDelete );
        }
    
    delete iUserParameters;
    iUserParameters = NULL;
    iUserParameters = new(ELeave) CFilesParameters();
    
    iDirectoryQueriedFromUser.Copy( KNullDesC );

    return iEngine->GetEngineWrapper()->EntriesQueryDialog(&iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorFilesStart );
    }


//----------------------------------------------------------------------------

TInt CCreatorFiles::CreateFileEntryL(CFilesParameters *aParameters, TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorFiles::CreateFileEntryL");

    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    TFileName directoryToBeCreated;
            
    CFilesParameters* parameters = aParameters;
    
    if (!parameters)
        {
        if ( iUserParameters )
            {
            iParameters = new (ELeave) CFilesParameters( *iUserParameters );
            // iUserParameters = NULL;
            }
        else
            {
            // random data needed if no predefined data available
            iParameters = new (ELeave) CFilesParameters;            
            }
        parameters = iParameters;
        }

    TInt err = KErrNone;

    // if we just create directories
    if ( aCommand == ECmdCreateFileEntryEmptyFolder)
        {
        // strip the last backslash from the path
        if( iDirectoryQueriedFromUser.Length() > 0)
        	directoryToBeCreated = iDirectoryQueriedFromUser;
        else if( parameters->iFullFilePath && parameters->iFullFilePath->Des().Length() > 0 )
        	directoryToBeCreated = parameters->iFullFilePath->Des();
        else 
        	return err;
        
        _LIT(KSlash, "\\");
        if( directoryToBeCreated.Right(1) == KSlash )
        	directoryToBeCreated.SetLength ( directoryToBeCreated.Length() - 1 ); 
            
        // generate a unique file name
        err = CApaApplication::GenerateFileName( iFs, directoryToBeCreated);
        if (err != KErrNone)
            return err;

        // now append the backslah back
        directoryToBeCreated.Append( _L("\\") );
        
        // now create the new directory
        err = iFs.MkDirAll( directoryToBeCreated );
        
        // Add directoryToBeCreated to store
        iFilePaths->AppendL( directoryToBeCreated );

        LOGSTRING3("Creator: CCreatorFiles::CreateFileEntryL creating empty directory %S returns err", &directoryToBeCreated, err);
        }

    else  // files
        {
        LOGSTRING2("Creator: CCreatorFiles::CreateFileEntryL file id is %d", aCommand);

        // get source
        TFileName fullSourcePath;
        switch (aCommand)
            {
	        case ECmdCreateFileEntryJPEG_25kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EJPEG_25kB );  break; }
	        case ECmdCreateFileEntryJPEG_200kB:     { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EJPEG_200kB );  break; }
	        case ECmdCreateFileEntryJPEG_500kB:     { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EJPEG_500kB );  break; }
	        case ECmdCreateFileEntryPNG_15kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EPNG_15kB );  break; }
	        case ECmdCreateFileEntryGIF_2kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EGIF_2kB );  break; }
	        case ECmdCreateFileEntryRNG_1kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ERNG_1kB );  break; }
	        case ECmdCreateFileEntryMIDI_10kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EMIDI_10kB );  break; }
	        case ECmdCreateFileEntryWAV_20kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EWAVE_20kB );  break; }
	        case ECmdCreateFileEntryAMR_20kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EAMR_20kB );  break; }
	        case ECmdCreateFileEntryXLS_15kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EExcel_15kB );  break; }
	        case ECmdCreateFileEntryDOC_20kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EWord_20kB );  break; }
	        case ECmdCreateFileEntryPPT_40kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EPowerPoint_40kB );  break; }
	        case ECmdCreateFileEntryTXT_10kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EText_10kB );  break; }
	        case ECmdCreateFileEntryTXT_70kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EText_70kB );  break; }
	        case ECmdCreateFileEntry3GPP_70kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::E3GPP_70kB );  break; }
	        case ECmdCreateFileEntryMP3_250kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EMP3_250kB );  break; }
	        case ECmdCreateFileEntryAAC_100kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EAAC_100kB );  break; }
	        case ECmdCreateFileEntryRM_95kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ERM_95kB );  break; }
	        case ECmdCreateFileEntryBMP_25kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EBMP_25kB );  break; }
	        case ECmdCreateFileEntryDeck_1kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ESavedDeck_1kB );  break; }
	        case ECmdCreateFileEntryHTML_20kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EHTML_20kB );  break; }
	        case ECmdCreateFileEntryJAD_1kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EJAD_1kB );  break; }
	        case ECmdCreateFileEntryJAR_10kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EJAR_10kB );  break; }
	        case ECmdCreateFileEntryJP2_65kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EJP2_65kB );  break; }
	        case ECmdCreateFileEntryMP4_200kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EMP4_200kB );  break; }
	        case ECmdCreateFileEntryMXMF_40kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EMXMF_40kB );  break; }
	        case ECmdCreateFileEntryRAM_1kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ERAM_1kB );  break; }
	        case ECmdCreateFileEntrySVG_15kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ESVG_15kB );  break; }
	        case ECmdCreateFileEntrySWF_15kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ESWF_15kB );  break; }
	        case ECmdCreateFileEntryTIF_25kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ETIF_25kB );  break; }
	        case ECmdCreateFileEntryVCF_1kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EVCF_1kB );  break; }
	        case ECmdCreateFileEntryVCS_1kB:        { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EVCS_1kB );  break; }
	        case ECmdCreateFileEntrySISX_10kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::ESISX_10kB );  break; }
	        case ECmdCreateFileEntryWMA_50kB:       { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EWMA_50kB );  break; }
	        case ECmdCreateFileEntryWMV_200kB:      { fullSourcePath = iEngine->TestDataPathL( CCreatorEngine::EWMV_200kB );  break; }
	        case ECmdCreateFileEntryEmptyFolder:    { User::Panic(_L("EmptyFolder"), 801); break; }
	        default:                                { return KErrPathNotFound; }
            }


        // define the full target path
        TFileName fullTargetPath;

        if ( parameters->iFullFilePath && parameters->iFullFilePath->Des().Length() > 3 )
            {
            _LIT(KSlash, "\\");
            // check the path is ok
            fullTargetPath = parameters->iFullFilePath->Des();
            
            if(fullTargetPath.Right(1) == KSlash)
                {
                // Remove '\' from the end, because the filename check does not work with it:
                fullTargetPath.SetLength ( fullTargetPath.Length() - 1 );
                }
            
            if (!iFs.IsValidName( fullTargetPath ))
		        User::Leave(KErrBadName);

            // target path = directory + the file name from source path
            TParse parser;
            parser.Set(fullSourcePath, NULL, NULL);
            
            // Add '\' to the end:
            fullTargetPath.Append(KSlash);            
            // Add filename:
            fullTargetPath.Append( parser.NameAndExt() );

            LOGSTRING2("Creator: CCreatorFiles::CreateFileEntryL iFullFilePath used, fullTargetPath: %S", &fullTargetPath);
            }
    
        else if ( iDirectoryQueriedFromUser.Length() > 0 )
            {
            // target path = directory + the file name from source path
            TParse parser;
            parser.Set(fullSourcePath, NULL, NULL);

            fullTargetPath = iDirectoryQueriedFromUser;
            fullTargetPath.Append( parser.NameAndExt() );

            LOGSTRING2("Creator: CCreatorFiles::CreateFileEntryL iDirectoryQueriedFromUser used, fullTargetPath: %S", &fullTargetPath);
            }

        else
            {   
            LOGSTRING("Creator: CCreatorFiles::CreateFileEntryL leaving with KErrPathNotFound");
            User::Leave(KErrPathNotFound);
            }
    
        TBool encrypt = parameters->iEncrypt; 
        if ( encrypt ) fullTargetPath.Append( KOma2DcfExtension );
        // check the target path has a unique filename ie we won't overwrite existing files
        // also generates any missing directories
        if ( BaflUtils::FileExists( iFs, fullTargetPath ) )
            {
            GenerateFileNameL( fullTargetPath );
            }
        User::LeaveIfError(CApaApplication::GenerateFileName( iFs, fullTargetPath ) );
        
        if ( encrypt )
            {
            EncryptFileL( fullSourcePath, fullTargetPath, parameters );
            }
        else
            {
            // copy the file (synchronous function)
            err = BaflUtils::CopyFile(iFs, fullSourcePath, fullTargetPath);
            LOGSTRING4("Creator: CCreatorFiles::CreateFileEntryL copy %S to %S, err=%d", &fullSourcePath, &fullTargetPath, err);            
            }
        
        if (err != KErrNone)
            User::Leave(err);  // leave because copying failed

        // Add fullTargetPath to store
        iFilePaths->AppendL( fullTargetPath );
        
		// make sure that the file won't have a read only attribute
		TEntry fileEntry;
		iFs.Entry(fullTargetPath, fileEntry);
		iFs.SetEntry(fullTargetPath, fileEntry.iModified, NULL, KEntryAttReadOnly);

        // clear variables
		parameters->iFullFilePath->Des().Copy ( KNullDesC );
        }

    return err;
    }

//----------------------------------------------------------------------------

void CCreatorFiles::EncryptFileL( const TDesC& aInFileName, const TDesC& aOutFileName, CFilesParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorFiles::EncryptFileL");
    TBuf8<64> mime;
    SetMimeTypeL( aInFileName, mime, aParameters );
    CSupplier* supplier = CSupplier::NewLC();

    CMetaDataArray* metaData = CMetaDataArray::NewLC();
    
    // Tell the agent which MIME type to use for the encrypted data
    metaData->AddL( KOmaImportMimeTypeField, mime );

    if ( aParameters && aParameters->iPermission )
        {
        // Combined Delivery file will be created
        SetPermissionsL( metaData, aOutFileName, aParameters );
        }
    
    supplier->SetOutputDirectoryL( iDirectoryQueriedFromUser );
    
    // The KOmaImportContentType is a OMA DRM agent specific MIME type which
    // indicates that plain content is to be encrypted
    CImportFile* importFile = supplier->ImportFileL( KOmaImportContentType,
                                                     *metaData,
                                                     aOutFileName );
    CleanupStack::PushL( importFile );
    
    // Peek the source file size:
    TInt fileLen( 0 );
    RFile file;
    User::LeaveIfError( file.Open( iFs, aInFileName, EFileRead ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( fileLen ) );
    CleanupStack::PopAndDestroy( &file );
    
    // Read the source file to inmemory buffer
    RFileReadStream rs;    
    User::LeaveIfError( rs.Open( iFs,
                                 aInFileName,
                                 EFileStream | EFileRead ) ); 
    CleanupClosePushL( rs );
    HBufC8* fileBuf = HBufC8::NewLC( fileLen );
    TPtr8 p = fileBuf->Des();
    rs.ReadL( p, fileLen );
    
    // Start encryption
    TInt err = importFile->WriteData( p );
    if ( err == KErrCANewFileHandleRequired )
        {
        RFile file;
        User::LeaveIfError( file.Create( iFs, aOutFileName, EFileWrite ) );
        CleanupClosePushL( file );
        User::LeaveIfError( importFile->ContinueWithNewOutputFile( file, aOutFileName ) );
        CleanupStack::PopAndDestroy( &file );
        }
    else
        {
        User::LeaveIfError( err );
        }
    User::LeaveIfError( importFile->WriteDataComplete() );
    CleanupStack::PopAndDestroy( fileBuf );
    CleanupStack::PopAndDestroy( &rs );    
    CleanupStack::PopAndDestroy( importFile );
    CleanupStack::PopAndDestroy( metaData );
    CleanupStack::PopAndDestroy( supplier );
    }

//----------------------------------------------------------------------------

void CCreatorFiles::SetPermissionsL( CMetaDataArray* aMetaData, const TDesC& aOutFileName, CFilesParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorFiles::SetPermissionsL");
    CDRMRights* rights = CDRMRights::NewL();
    CleanupStack::PushL( rights );
    
    HBufC8* cnturi = HBufC8::NewL( KMaxFileName );
    cnturi->Des().Copy( aOutFileName );
    
    CDRMAsset* asset = CDRMAsset::NewLC();
    asset->iUid = cnturi;
    // Set the asset to the rights class, it will duplicate the asset
    rights->SetAssetL( *asset );
    CleanupStack::PopAndDestroy( asset );
    
    rights->SetPermissionL( *aParameters->iPermission );
    
    // Construct externalized presentation of the rights object
    TInt rightsSize = 1024 *100;
    HBufC8* rightBuf = HBufC8::NewLC( rightsSize ); 
    TPtr8 bptr = rightBuf->Des();
    bptr.SetLength( rightsSize );
    RMemWriteStream iWriteStream;
    iWriteStream.Open( (TAny*)(rightBuf->Ptr() ), rightsSize );
    CleanupClosePushL( iWriteStream );
    iWriteStream << *rights;
    iWriteStream.CommitL();
    TPtr8 rp = rightBuf->Des();
    
    // Add rights to metadata
    aMetaData->AddL( KOmaImportRightsField, rp );
    CleanupStack::PopAndDestroy( &iWriteStream );
    CleanupStack::PopAndDestroy( rightBuf );
    CleanupStack::PopAndDestroy( rights );
    }

//----------------------------------------------------------------------------

void CCreatorFiles::SetMimeTypeL( const TDesC& aFileName, TDes8& aMime, CFilesParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorFiles::SetMimeTypeL");
    TUid appUid;
    TDataType dataType;
    User::LeaveIfError( iApaLs.AppForDocument( aFileName, appUid, dataType ) );
    if ( dataType.Des().Length() )
        {
        aMime.Copy( dataType.Des() );    
        }
    else
        {
        // set default mime, because it was not recognized by iApaLs
        aMime.Copy( _L("text/plain") );
        }

    // set DRM permissions according the type of the file
    if ( aParameters->iPermission )
        {
        if ( dataType.Des().FindF( _L("image") ) > KErrNotFound )
            {
            aParameters->iPermission->iAvailableRights = ERightsDisplay | ERightsPrint;
            }
        else if ( dataType.Des().FindF( _L("audio") ) > KErrNotFound ||
                  dataType.Des().FindF( _L("video") ) > KErrNotFound ||
                  dataType.Des().FindF( _L("tone") ) > KErrNotFound || // e.g. application/vnd.nokia.ringing-tone
                  dataType.Des().FindF( _L("realmedia") ) > KErrNotFound )
            {
            // media files
            aParameters->iPermission->iAvailableRights = ERightsPlay;
            }
        else if ( dataType.Des().FindF( _L("archive") ) > KErrNotFound ||
                  dataType.Des().FindF( _L("x-sis") ) > KErrNotFound )
            {
            // application/java-archive
            // x-epoc/x-sisx-app
            aParameters->iPermission->iAvailableRights = ERightsExecute;
            }
        else if ( dataType.Des().FindF( _L("application") ) > KErrNotFound ||
                  dataType.Des().FindF( _L("text") ) > KErrNotFound )
            {
            // application/msexcel
            // application/msword
            // text/plain
            // etc.
            aParameters->iPermission->iAvailableRights = ERightsDisplay | ERightsPrint;
            }
        else
            {
            // other filetype
            aParameters->iPermission->iAvailableRights = ERightsDisplay;
            }
        }
    }

//----------------------------------------------------------------------------

TBool CCreatorFiles::AskDRMDataFromUserL()
    {
    LOGSTRING("Creator: CCreatorFiles::AskDRMDataFromUserL");
    // Encryption -dialog
    CDesCArrayFlat* items = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL(items);

    // Add entires to list
    items->AppendL( _L("None") );
    items->AppendL( _L("DRM Forward Lock") );
    items->AppendL( _L("DRM Combined Delivery") );

    
	// create a popup list
    iDummy = 0;
    TBool retval = iEngine->GetEngineWrapper()->PopupListDialog(_L("Encryption"), items, &iDummy, this, ECreatorFilesAskDRMData );
    CleanupStack::PopAndDestroy( items );
    return retval;
    }

//----------------------------------------------------------------------------
void CCreatorFiles::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorFiles::DeleteAllL");
    User::Leave( KErrNotSupported ); // will not be supported
    }

//----------------------------------------------------------------------------
void CCreatorFiles::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorFiles::DeleteAllCreatedByCreatorL");
    iFileId = 1;
    CDictionaryFileStore* store = iEngine->FileStoreLC();
    User::LeaveIfNull( store );
        
    if ( store->IsPresentL( KUidDictionaryUidFiles ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, KUidDictionaryUidFiles );
        CFileMan* fileMan = CFileMan::NewL( iFs );
        CleanupStack::PushL( fileMan );
        TFileName fullPath;
        // ignore return value, don't update iFileId here:
        TRAPD( err, in.ReadInt32L() );
        while ( !err )
            {
            TInt len( KErrNotFound );
            TRAP( err, len = in.ReadInt8L() );  // will leave with KErrEof
            if ( !err )
                {
                TRAP( err, in.ReadL( fullPath, len ) );                    
                }
            if ( !err )
                {
                TEntry fileEntry;
                iFs.Entry( fullPath, fileEntry );
                if ( fileEntry.IsDir() )
                    {
                    fileMan->RmDir( fullPath ); // ignore return value
                    }
                else
                    {
                    iFs.Delete( fullPath ); // ignore return value    
                    }                    
                }
            }
        CleanupStack::PopAndDestroy( fileMan );
        CleanupStack::PopAndDestroy( &in );
        
        // files deleted, remove the Creator internal file registry
        store->Remove( KUidDictionaryUidFiles );
        store->CommitL();
        }
    CleanupStack::PopAndDestroy( store );
    }

//----------------------------------------------------------------------------
void CCreatorFiles::StorePathsForDeleteL( CDesCArray& aPaths )
    {
    LOGSTRING("Creator: CCreatorFiles::StorePathsForDeleteL");
    CDictionaryFileStore* store = iEngine->FileStoreLC();
    User::LeaveIfNull( store );
    
    // backup previous filepaths from store
    // otherwise they would be overwritten when calling out.WriteL
    CDesCArray* previousPaths = new (ELeave) CDesCArrayFlat( 4 );
    CleanupStack::PushL( previousPaths );
    
    TFileName fullPath;
    
    if ( store->IsPresentL( KUidDictionaryUidFiles ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, KUidDictionaryUidFiles );
        // ignore return value, don't update iFileId here:
        TRAPD( err, in.ReadInt32L() );
        while ( !err ) 
            {
            TRAP( err,
                TInt len = in.ReadInt8L(); // will leave with KErrEof
                in.ReadL( fullPath, len );
                previousPaths->AppendL( fullPath );
                );
            }
        CleanupStack::PopAndDestroy(); // in
        }
    
    RDictionaryWriteStream out;       
    out.AssignLC( *store, KUidDictionaryUidFiles );
    
    // write latest file id to store
    out.WriteInt32L( iFileId );
    
    // restore previous paths to store
    for ( TInt i = 0; i < previousPaths->Count(); i++ )
        {
        out.WriteInt8L( (*previousPaths)[i].Length() );
        out.WriteL( (*previousPaths)[i] );            
        }

    // write new paths to store
    for ( TInt i = 0; i < aPaths.Count(); i++ )
        {
        out.WriteInt8L( aPaths[i].Length() );
        out.WriteL( aPaths[i] );            
        }
    
    out.CommitL();
    CleanupStack::PopAndDestroy(); // out
    
    store->CommitL();
    CleanupStack::PopAndDestroy( previousPaths );
    CleanupStack::PopAndDestroy( store );
    }

//----------------------------------------------------------------------------
void CCreatorFiles::GenerateFileNameL( TFileName& aRootName )
    {
    LOGSTRING("Creator: CCreatorFiles::GenerateFileNameL");
    if ( iFileId )
        {
        TBuf<16> extension;
        if ( iFileId < 10 )
            {
            extension.Format( _L("(0%d)"), iFileId );     
            }
        else
            {
            extension.Format( _L("(%d)"), iFileId );
            }
        aRootName.Insert( aRootName.Locate( '.' ), extension );
        }
    iFileId++;
    }

// End of file
