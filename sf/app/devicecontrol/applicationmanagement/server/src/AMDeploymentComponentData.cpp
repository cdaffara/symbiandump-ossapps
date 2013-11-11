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
 * Description: Implementation of applicationmanagement components
 *
 */

#include <bautils.h>
#include <DRMLicenseManager.h>
#include <pathinfo.h>
#include <zipfilemember.h>
#include "amdeploymentcomponentdata.h"
#include "debug.h"

_LIT8( KSisxMimeType, "x-epoc/x-sisx-app" );
_LIT( KTempDir, "piptemp\\" );
_LIT8( KPipMimeType, "application/x-pip" );
_LIT8( KDrmMessageMimeType, "application/vnd.oma.drm.message" );
_LIT8( KDrmContentMimeType, "application/vnd.oma.drm.content" );
_LIT8( KSisMimeType, "application/vnd.symbian.install");
_LIT8( KJadMIMEType, "text/vnd.sun.j2me.app-descriptor" );
_LIT8( KJarMIMEType, "application/java-archive" );
_LIT8( KJavaMIMEType, "application/java" );

using namespace NApplicationManagement;

/**
 *  Data class to Data access
 */
CDeploymentComponentData::CDeploymentComponentData()
    {
    }

CDeploymentComponentData::CDeploymentComponentData(TType aType,
        const TDesC8 &aDataFile) :
    iDataFileName(aDataFile), iType(aType)
    {
    }

void CDeploymentComponentData::ConstructL(const TDesC8 &aData,
        const TDesC8 &aMime)
    {
    SetDataL(aData, aMime);
    }

CDeploymentComponentData &CDeploymentComponentData::operator=(
        const CDeploymentComponentData &aData )
    {
    if( &aData != this )
        {
        SetDataL( aData.Data(), aData.MimeType() );
        iType = aData.iType;
        iDataFileName = aData.iDataFileName;
        }
    return *this;
    }

void CDeploymentComponentData::ConstructLoadL(RReadStream &aStream)
    {
    TUint32 len(aStream.ReadUint32L() );
    aStream.ReadL(iDataFileName, len);

    len = aStream.ReadUint32L();
    aStream.ReadL(iMimeType, len);
    }

CDeploymentComponentData* CDeploymentComponentData::NewL(TType aType,
        const TDesC8 &aData, const TDesC8 &aMime, const TDesC8 &aDataFile)
    {
    CDeploymentComponentData *self = CDeploymentComponentData::NewLC(aType,
            aData, aMime, aDataFile);
    CleanupStack::Pop();
    return self;
    }

CDeploymentComponentData* CDeploymentComponentData::NewLC(TType aType,
        const TDesC8 &aData, const TDesC8 &aMime, const TDesC8 &aDataFile)
    {
    CDeploymentComponentData *self = new ( ELeave ) CDeploymentComponentData( aType, aDataFile);
    CleanupStack::PushL(self) ;
    self->ConstructL(aData, aMime);
    return self;
    }

CDeploymentComponentData* CDeploymentComponentData::LoadL(
        RReadStream &aBuffer)
    {
    CDeploymentComponentData *self =
            CDeploymentComponentData::LoadLC(aBuffer);
    CleanupStack::Pop();
    return self;
    }

CDeploymentComponentData* CDeploymentComponentData::LoadLC(
        RReadStream &aBuffer)
    {
    CDeploymentComponentData *self = new ( ELeave ) CDeploymentComponentData();
    CleanupStack::PushL(self) ;
    self->ConstructLoadL(aBuffer);
    return self;
    }

CDeploymentComponentData::~CDeploymentComponentData()
    {
    RDEBUG8_3("CDeploymentComponentData::~CDeploymentComponentData 0x%X - 0x%X", reinterpret_cast<TUint>(this),
            reinterpret_cast<TUint>(this)+sizeof( CDeploymentComponentData ) );

    delete iData;
    iData = NULL;
    }

TInt CDeploymentComponentData::DataLengthL() const
    {
    TInt ret( 0);
    if (iData == NULL)
        {
        if (iDataFileName.Length() > 0)
            {
            RFs fs;
            User::LeaveIfError(fs.Connect() );
            CleanupClosePushL(fs); // 1
            TEntry entry;
            TFileName fileName;
            fileName.Copy(iDataFileName);
            User::LeaveIfError(fs.Entry(fileName, entry) );
            ret = entry.iSize;
            CleanupStack::PopAndDestroy( &fs);
            }
        }
    else
        {
        ret = iData->Length();
        }
    return ret;
    }

const TDesC8 &CDeploymentComponentData::Data() const
    {
    if (iData == NULL)
        {
        TRAPD( err, LoadDataL() )
        ;
        if (err != KErrNone)
            {
            return KNullDesC8();
            }
        }
    if (iData != NULL)
        {
        return *iData;
        }
    else
        {
        return KNullDesC8();
        }
    }

void CDeploymentComponentData::LoadDataL() const
    {

    if (iDataFileName.Length() > 0)
        {
        RFs fs;
        User::LeaveIfError(fs.Connect() );
        CleanupClosePushL(fs); // 1
        RFile file;
        TFileName fileName;
        fileName.Copy(iDataFileName);
        TInt err(file.Open(fs, fileName, EFileRead) );
        if (err == KErrNone)
            {
            CleanupClosePushL(file); // 2
            TInt fsize;
            User::LeaveIfError(file.Size(fsize) );
            iData = HBufC8::NewL(fsize);
            TPtr8 ptr(iData->Des() );
            User::LeaveIfError(file.Read(ptr) );
            CleanupStack::PopAndDestroy( &file);
            }
        else
            {
            RDEBUG_3( "ERROR Leaving CDeploymentComponentData::Data - SEVERE Could not open data file '%S': %d!",
                    &fileName, err );
            User::Leave(KErrNotFound) ;
            }
        CleanupStack::PopAndDestroy( &fs);
        }
    else
        {
        RDEBUG( "ERROR Leaving CDeploymentComponentData::LoadDataL() - No file to load!" );
        User::Leave(KErrNotFound) ;
        }
    }

TUid CDeploymentComponentData::SetDataL(const TDesC8& aMimeType)
    {
    RDEBUG8_2("CDeploymentComponentData::SetDataL() aMimeType: (%S)", &aMimeType);

    TUid ret(TUid::Null());
    iMimeType = aMimeType.Left(KMaxMimeLength);

    if (aMimeType.Length()!=NULL)
        {
        TUid ret(TUid::Null());
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        CFileMan *fm = CFileMan::NewL(fs);
        CleanupStack::PushL(fm);
        TFileName oldfilepath;
        oldfilepath.Copy(iDataFileName);
        
        
        RDEBUG("App Mgmt before copy start");
        TInt maxLength = iDataFileName.Length();
        TChar charvaldot = '.';
        TChar charvalslash = '\\';
        //TFileName oldfilepath;
        TInt pos = iDataFileName.LocateReverse(charvaldot);

        TInt lengthDeleted = maxLength - pos;

        iDataFileName.Delete(pos, lengthDeleted);
        
        if (iMimeType == KSisxMimeType)
            {
            _LIT16(KExt,".sisx");
            iExtn.Append(KExt);
            }
        if(iMimeType==KSisMimeType)
            {
            _LIT16(KExt,".sis");
            iExtn.Append(KExt);
            }
        if(iMimeType==KPipMimeType)
            {
            _LIT16(KExt,".pip");
            iExtn.Append(KExt);
            }
        if(iMimeType==KJadMIMEType)
            {
            _LIT16(KExt,".jad");
            iExtn.Append(KExt);
            }
        if(iMimeType==KJarMIMEType)
            {
            _LIT16(KExt,".jar");
            iExtn.Append(KExt);
            }
        if(iMimeType==KJavaMIMEType)
            {
            _LIT16(KExt,".jar");
            iExtn.Append(KExt);
            }
        iDataFileName.Append(iExtn);//file name with sisx extension
        TFileName newfilepath;
        newfilepath.Copy(iDataFileName);
        User::LeaveIfError(fm->Rename(oldfilepath, newfilepath));
        CleanupStack::PopAndDestroy(fm);
        CleanupStack::PopAndDestroy( &fs);
        //RDEBUG_2(" filename: %S", iDataFileName );
        }
    if (IsSISInstallFile(aMimeType) )
        {
        RFs fs;
        User::LeaveIfError(fs.Connect() );
        CleanupClosePushL(fs);     
        ret = ResolveUidL(fs);
        CleanupStack::PopAndDestroy( &fs);
        }
    RDEBUG8_2("CDeploymentComponentData::SetDataL() UID: (0x%x)", ret.iUid);
    return ret;
    }

TUid CDeploymentComponentData::SetDataL(const TFileName &aData,
        const TDesC8& aMimeType)
    {
    RDEBUG_2("CDeploymentComponentData::SetDataL() TFileName: (%S)", &aData);
    
    _LIT(KNewPath, "c:\\private\\200267FB\\");

    TUid ret(TUid::Null());
    iMimeType = aMimeType.Left(KMaxMimeLength) ;
    RFs fs;
    User::LeaveIfError(fs.Connect() );
    CleanupClosePushL(fs);
    CFileMan *fm = CFileMan::NewL(fs);
    CleanupStack::PushL(fm);
    TFileName fn;
    fn.Copy(iDataFileName);

    RDEBUG("App Mgmt before copy start");

    
    TInt maxLength = iDataFileName.Length();
    TChar charvaldot = '.';
    TChar charvalslash = '\\';
    
    TInt pos = iDataFileName.LocateReverse(charvaldot);
    
    TInt lengthDeleted = maxLength-pos;
    
    iDataFileName.Delete(pos, lengthDeleted);
    
    TInt srcpos = aData.LocateReverse(charvaldot);
    
    TBuf<15> extn(aData.Mid(srcpos));
    
    iDataFileName.Append(extn);
    
    TFileName newfilepath;
    newfilepath.Copy(iDataFileName);
    
    User::LeaveIfError(fm->Move(aData,KNewPath()));
    
    TFileName oldfilepath(KNewPath());
    oldfilepath.Append(aData.Mid(aData.LocateReverse(charvalslash)));
    
    User::LeaveIfError(fm->Rename(oldfilepath, newfilepath));
   
    
    //User::LeaveIfError(fm->Copy(aData, fn) );

    RDEBUG("App Mgmt before copy End");


    if (IsSISInstallFile(aMimeType) )
        {
        ret = ResolveUidL(fs);
        }

    CleanupStack::PopAndDestroy(fm);
    CleanupStack::PopAndDestroy( &fs);
    RDEBUG_2("CDeploymentComponentData::SetDataL() (%d)", ret.iUid);

    return ret;
    }

TUid CDeploymentComponentData::SetDataL(const TDesC8 &aData,
        const TDesC8 &aMimeType)
    {
    TUid ret(TUid::Null());
    if (iData)
        {
        delete iData;
        iData = NULL;
        }
    iData = aData.AllocL();
    ret = SetDataL(aMimeType);
    return ret;
    }

TBool CDeploymentComponentData::IsSISInstallFile(const TDesC8 &aMimeType)
    {
    TBool isSIS(EFalse);

    if (aMimeType == KSisxMimeType || aMimeType == KPipMimeType || aMimeType
            == KDrmMessageMimeType || aMimeType == KDrmContentMimeType || aMimeType == KSisMimeType)
        {
        isSIS = ETrue;
        }

    return isSIS;
    }

TUid CDeploymentComponentData::ResolveUidL(RFs& aFs)
    {
    RDEBUG("CDeploymentComponentData::ResolveUidL()");

    TUid ret(TUid::Null() );
    HBufC* buf = HBufC::NewLC(KMaxFileName);
    TPtr16 ptr = buf->Des();
    ptr.Copy(iDataFileName);

    // if PIP/DRM package, we need to use license manager to extract the sis file
    if (iMimeType == KPipMimeType || iMimeType == KDrmMessageMimeType
            || iMimeType == KDrmContentMimeType)
        {
        RDEBUG8_2("	-> mime: %S", &iMimeType );

        RFile originalFile;
        RFile decryptedFile;
        TFileName decryptedTempFileName;

        RDEBUG_2("	-> opening original file: %S", &ptr );
        // leave if can not open the original file
        User::LeaveIfError(originalFile.Open(aFs, ptr, EFileWrite) );
        RDEBUG("	-> done");

        // First construct the temp path
        User::LeaveIfError(aFs.PrivatePath(decryptedTempFileName) );
        // set drive letter into the path
        decryptedTempFileName.Insert( 0, TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive() );
        // append "piptemp\\"	
        decryptedTempFileName.Append(KTempDir);
        // create the folder
        aFs.MkDir(decryptedTempFileName);

        // Use license manager to extract files from the pip package
        CDRMLicenseManager* licenseMgr = CDRMLicenseManager::NewL();
        CleanupStack::PushL(licenseMgr);
        // decryp from the original file into the temp file   
        RDEBUG_2("	-> extracting SIS file into: %S", &decryptedTempFileName);
        User::LeaveIfError(licenseMgr->ExtractSISFileL(originalFile,
                decryptedTempFileName) );
        RDEBUG("	-> done");

        // Get the sis file name 
        decryptedTempFileName.Append( *(licenseMgr->GetSISMemberL()->Name() ));
        // open temporary handle to it.
        RDEBUG_2("	-> opening decrypted file: %S", &decryptedTempFileName );
        User::LeaveIfError(decryptedFile.Open(aFs, decryptedTempFileName,
                EFileShareAny) );
        RDEBUG("	-> done");
        // parse the uid from the file
        ret = ParseUidFromSisFileL(decryptedFile);

        // no use anymore for the decrypted file
        decryptedFile.Close();
        // delete the temp file
        TInt err = aFs.Delete(decryptedTempFileName);
        if (err != KErrNone)
            {
            RDEBUG_2("**** ERROR, unable to delete temporary file: %S", &decryptedTempFileName );
            }

        CleanupStack::PopAndDestroy(licenseMgr);
        decryptedFile.Close();
        originalFile.Close();
        }
    else
        if (iMimeType == KSisxMimeType || iMimeType == KSisMimeType )
            {
            RDEBUG("	-> mime: x-epoc/x-sisx-app");
            RFile originalFile;
            RDEBUG_2("	-> opening file: %S", &ptr );
            User::LeaveIfError(originalFile.Open(aFs, ptr, EFileRead) );
            RDEBUG("	-> opened ok");
            ret = ParseUidFromSisFileL(originalFile);
            originalFile.Close();
            }

        else
            {
            RDEBUG8_2( "**** ERROR - CDeploymentComponentData::ResolveUidL( ) - cannot get uid from mime type: %S", &iMimeType );
            }

    CleanupStack::PopAndDestroy(buf);
    return ret;
    }

void CDeploymentComponentData::SerializedFormL(RWriteStream &aBuffer) const
    {
    aBuffer.WriteUint32L(iDataFileName.Length() );
    aBuffer.WriteL(iDataFileName);

    aBuffer.WriteUint32L(iMimeType.Length() );
    aBuffer.WriteL(iMimeType);

    }

void CDeploymentComponentData::DestroyL(RFs &aFs) const
    {
    TFileName fn;
    fn.Copy(iDataFileName);
    aFs.Delete(fn) ;
    }

const TDesC8 &CDeploymentComponentData::DataFileName() const
    {
    return iDataFileName;
    }

void CDeploymentComponentData::PersistL(RFs &aFs)
    {
    if (iData)
        {
        RFile file;
        TFileName aFile;
        aFile.Copy(iDataFileName) ;
        RDEBUG_3( "CDeploymentComponentData::PersistL() - Saving '%S', dataLenght %d", &aFile, (iData ? iData->Length() : 0) );
        User::LeaveIfError(file.Replace(aFs, aFile, EFileWrite) );
        CleanupClosePushL(file) ;

        file.Write( *iData) ;
        file.Flush();
        delete iData;
        iData = NULL;
        CleanupStack::PopAndDestroy( &file) ; // file
        }
    else
        {
        RDEBUG( "CDeploymentComponentData::PersistL() - Already persisted" );
        }
    RDEBUG( "CDeploymentComponentData::PersistL() - Done" );
    }

TInt CDeploymentComponentData::SerializedFormLength() const
    {
    return iDataFileName.Length() + (2 * 4) + iMimeType.Length();
    }

void CDeploymentComponentData::SetDataFileNameL(const TDesC8 &aNewFileName)
    {
    if (aNewFileName.Length() > 0)
        {
        RFs fs;
        User::LeaveIfError(fs.Connect() );
        CleanupClosePushL(fs);
        TFileName newfile;
        newfile.Copy(aNewFileName);
        TFileName oldfile;
        oldfile.Copy(iDataFileName);
        User::LeaveIfError(BaflUtils::RenameFile(fs, oldfile, newfile) );
        CleanupStack::PopAndDestroy( &fs);
        iDataFileName = aNewFileName;
        }
    else
        {
        RDEBUG( "CDeploymentComponentData::SetDataFileNameL - ERROR Cannot set empty filename!" );
        User::Leave(KErrArgument);
        }
    }
void CDeploymentComponentData::SetMimeTypeL(const TDesC8 &aMimeType)
{
	iMimeType = aMimeType.Left(KMaxMimeLength) ;
}
void CDeploymentComponentData::ResetData(RFs &aFs)
    {
    TRAP_IGNORE(DestroyL( aFs ));
    iDataFileName = KNullDesC8();
    delete iData;
    iData = NULL;
    }

TUid CDeploymentComponentData::ParseUidFromSisFileL(RFile& aHandleToFile)
    {
    RDEBUG("CDeploymentComponentData::ParseUidFromSisFileL");

    TUid appUid;
    TInt uidLen = sizeof(TInt32);
    TInt seekLen = sizeof(TInt32) + sizeof(TInt32);
    User::LeaveIfError(aHandleToFile.Seek(ESeekStart, seekLen));
    TPckg<TInt32> uid1(appUid.iUid);
    User::LeaveIfError(aHandleToFile.Read(uid1, uidLen));

    if (uid1.Length() != uidLen)
        {
        RDEBUG("**** ERROR - uid length inconsistency - underflow");
        User::Leave(KErrUnderflow);
        }

    RDEBUG_2("	-> returning SIS UID: %d", appUid.iUid );
    return appUid;
    }

//  End of File
