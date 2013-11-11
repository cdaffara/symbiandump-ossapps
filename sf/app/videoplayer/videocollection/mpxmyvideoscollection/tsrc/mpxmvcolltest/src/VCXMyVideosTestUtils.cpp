/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*/

// INCLUDE FILES
#include "VCXMyVideosTestUtils.h"
#include "VCXTestLog.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::NewL
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestUtils* CVCXMyVideosTestUtils::NewL()
    {
    VCXLOGLO1(">>>CVCXMyVideosTestUtils::NewL");
    CVCXMyVideosTestUtils* self = new (ELeave) CVCXMyVideosTestUtils();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVCXMyVideosTestUtils::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::CVCXMyVideosTestUtils
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestUtils::CVCXMyVideosTestUtils()
    {
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::~CVCXMyVideosTestUtils
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestUtils::~CVCXMyVideosTestUtils( )
    {
    delete iFileMan;
    iFileMan = NULL;
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestUtils::ConstructL( )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestUtils::ConstructL");
    User::LeaveIfError( iFs.Connect() );
    iFileMan = CFileMan::NewL( iFs );
    VCXLOGLO1("<<<CVCXMyVideosTestUtils::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::EnsureFileIsNotInUse
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosTestUtils::EnsureFileIsNotInUse( const TPtrC& aFileName )
    {
    TInt error( KErrNone );
    RFile file;
 
    for(TInt retry = 0; retry < 6; retry++)
        {
        error = file.Open(iFs, aFileName, EFileShareExclusive);
        if( error == KErrInUse )
            {
            User::After( 1000000 * 10 );
            }
        else
            {
            break;
            }
        }

    file.Close();
    VCXLOGLO2("CVCXTestCommon::EnsureFileIsNotInUse: %d", error);
    return error;
    }


// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::CreateVideoFileL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestUtils::CreateVideoFileL( TVcxTestVideoType aVideoType, TDesC& aFileName, TInt aSize, TBool aUniqueName )
    {
    VCXLOGLO1(">>>CVcxTestVideoCreator::CreateVideoL");

    // Resolve source filename
    TBuf<256> srcFileName;

    GetVideoFile( srcFileName, aVideoType, _L("C") );
    if( !BaflUtils::FileExists(iFs, srcFileName) )
        {
        VCXLOGLO2("CVcxTestVideoCreator:: %S does not exist.", &srcFileName);
        GetVideoFile( srcFileName, aVideoType, _L("E") );
        if( !BaflUtils::FileExists(iFs, srcFileName) )
            {
            VCXLOGLO2("CVcxTestVideoCreator:: %S does not exist.", &srcFileName);
            VCXLOGLO2("CVcxTestVideoCreator:: test video file %S missing! PANIC.", &srcFileName);
            User::Panic(_L("Video files missing!"), KErrNotFound);
            }
        }
    
    BaflUtils::EnsurePathExistsL( iFs, aFileName.Left( aFileName.LocateReverse('\\') ) );

    HBufC* newFileName = HBufC::NewL( 256 );
    CleanupStack::PushL( newFileName );
    
    if(!aUniqueName)
    {
        newFileName->Des().Copy( aFileName );
        BaflUtils::DeleteFile( iFs, aFileName );
    }
    else
    {
        newFileName->Des().Copy( aFileName );
        TInt dotPos = aFileName.LocateReverse('.');
        int count = 0;
        while( BaflUtils::FileExists( iFs, *newFileName ) )
        {
            newFileName->Des().Copy( aFileName.Left( aFileName.LocateReverse('\\') ) );
            newFileName->Des().Copy( aFileName.Left( dotPos ) );
            newFileName->Des().Append( _L("_") );
            newFileName->Des().AppendNum( count++ );
            newFileName->Des().Append( aFileName.Right( aFileName.Length() - dotPos ) );
        }
    }
    
    VCXLOGLO2("CVcxTestVideoCreator:: aSize = %d", aSize);
    
    TInt64 wantedSize( 0 );
    
    // Check the size
    if( aSize == KVcxTestLargeFile3GB ) {
        wantedSize = 1024*1024*1024 * 3;
    }
    else {
        wantedSize = aSize;
    }
    
    //wantedSize = wantedSize == 0 ? wantedSize -1 : wantedSize;
    
    VCXLOGLO2("CVcxTestVideoCreator:: Wanted file size: %Ld", wantedSize);

    // Read source file into memory, won't work on huge files.
    RFile64 srcFile;
    VCXLOGLO2("CVcxTestVideoCreator:: Opening %S", &srcFileName);
    User::LeaveIfError( srcFile.Open( iFs, srcFileName, EFileRead ) );
    CleanupClosePushL( srcFile );

    TInt64 srcSize(0);
    VCXLOGLO2("CVcxTestVideoCreator:: Getting size of %S", &srcFileName);
    User::LeaveIfError( srcFile.Size( srcSize ) );

    HBufC8* data = HBufC8::NewL( srcSize );
    TPtr8 ptr( data->Des() );
    srcFile.Read( ptr, srcSize );
    CleanupStack::PopAndDestroy( &srcFile );

    CleanupStack::PushL( data );

    // Write new file.
    RFile64 dstFile;
    VCXLOGLO1("CVcxTestVideoCreator:: Replace");
    User::LeaveIfError( dstFile.Replace( iFs, *newFileName, EFileWrite ) );
    CleanupClosePushL(dstFile);

    if( wantedSize <= srcSize )
        {
        if( wantedSize == -1 )
            {
            VCXLOGLO2("CVcxTestVideoCreator:: Writing %Ld", srcSize);
            User::LeaveIfError( dstFile.Write( *data, srcSize ) );
            }
        else
            {
            VCXLOGLO2("CVcxTestVideoCreator:: Writing %Ld", wantedSize);
            User::LeaveIfError( dstFile.Write( *data, wantedSize ) );
            }
        }
    else
        {
        VCXLOGLO2("CVcxTestVideoCreator:: Writing %Ld", srcSize);
        User::LeaveIfError( dstFile.Write( *data, srcSize ) );

        const TInt KVcxTest200Kilos = 1024*200;
        HBufC8* buff = HBufC8::NewL( KVcxTest200Kilos );
        buff->Des().SetLength( KVcxTest200Kilos );
        CleanupStack::PushL( buff );
        TInt64 bytesToWrite = wantedSize - srcSize;
        while( bytesToWrite > 0 )
            {
            if( bytesToWrite >= KVcxTest200Kilos )
                {
                bytesToWrite -= KVcxTest200Kilos;
                User::LeaveIfError( dstFile.Write( *buff ) );
                }
            else
                {
                User::LeaveIfError( dstFile.Write( *buff, bytesToWrite ) );
                bytesToWrite = 0;
                }
            }
        CleanupStack::PopAndDestroy( buff );
        }

    CleanupStack::PopAndDestroy( &dstFile );
    CleanupStack::PopAndDestroy( data );
    
    CleanupStack::PopAndDestroy( newFileName );

    VCXLOGLO1("<<<CVcxTestVideoCreator::CreateVideoL");
    }


// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::CreateVideoFilesL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestUtils::CreateVideoFilesL( TVcxTestVideoType aVideoType, TDesC& aFileName, TInt aCount, RPointerArray<HBufC>& aFileArray )
    {
    VCXLOGLO1(">>>CVcxTestVideoCreator::CreateVideosL");

    // Resolve source filename
    TBuf<256> srcFileName;

    GetVideoFile( srcFileName, aVideoType, _L("C") );

    if( !BaflUtils::FileExists(iFs, srcFileName) )
        {
        VCXLOGLO2("CVcxTestVideoCreator:: %S does not exist.", &srcFileName);
        GetVideoFile( srcFileName, aVideoType, _L("E") );
        if( !BaflUtils::FileExists(iFs, srcFileName) )
            {
            VCXLOGLO2("CVcxTestVideoCreator:: %S does not exist.", &srcFileName);
            VCXLOGLO2("CVcxTestVideoCreator:: test video file %S missing! PANIC.", &srcFileName);
            // Comment next line if you want dummy files to be created. They won't be recognized by MDS.
            User::Panic(_L("Video files missing!"), KErrNotFound);
            }
        }

    TBool fileExists = BaflUtils::FileExists(iFs, srcFileName);

    HBufC* newFileName = HBufC::NewL( 256 );
    CleanupStack::PushL( newFileName );

    newFileName->Des().Copy( aFileName.Left( aFileName.LocateReverse('\\') ) );
    BaflUtils::EnsurePathExistsL(iFs, *newFileName);

    TInt dotPos = aFileName.LocateReverse('.');

    for( TInt i = 0; i < aCount; i++ )
        {
        newFileName->Des().Copy( aFileName.Left( dotPos ) );
        newFileName->Des().Append( _L("_") );
        newFileName->Des().AppendNum( i );
        newFileName->Des().Append( aFileName.Right( aFileName.Length() - dotPos ) );

        HBufC* fileForClient = newFileName->Des().AllocL();
        aFileArray.Append( fileForClient );

        if( fileExists )
            {
            User::LeaveIfError( iFileMan->Copy(srcFileName, *newFileName) );
            VCXLOGLO2("CVcxTestVideoCreator:: copy file: '%S'", newFileName);
            }
        else
            {
            VCXLOGLO2("CVcxTestVideoCreator:: new fake file: '%S'", newFileName);
            RFile file;
            CleanupClosePushL(file);
            User::LeaveIfError( file.Replace(iFs, *newFileName, EFileWrite) );
            User::LeaveIfError( file.SetSize( 1024*10 ) );
            CleanupStack::PopAndDestroy( &file );
            }
        User::After( 100000 ); // Wait tenth of a second.
        }

    CleanupStack::PopAndDestroy( newFileName );

    VCXLOGLO1("<<<CVcxTestVideoCreator::CreateVideosL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestUtils::GetVideoFile
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestUtils::GetVideoFile( TDes& aFileName, CVCXMyVideosTestUtils::TVcxTestVideoType aVideoType, const TDesC& aDrive )
    {
    VCXLOGLO1(">>>CVcxTestVideoCreator::GetVideoFile");
    aFileName.Copy( aDrive );

    _LIT(KVcxTestVideoBasePath, ":\\testing\\data\\");

    aFileName.Append( KVcxTestVideoBasePath );
    
    VCXLOGLO2("CVcxTestVideoCreator::GetVideoFile -- using %S", &aFileName);

    switch (aVideoType)
        {
        case VcxTestVideo3Gp:
            {
            aFileName.Append( _L("video_3gp.xxx") );
            }
            break;

        case VcxTestVideoMpeg4:
            {
            aFileName.Append( _L("video_mpeg4.xxx") );
            }
            break;

        case VcxTestVideoWmv:
            {
            aFileName.Append( _L("video_wmv.xxx") );
            }
            break;

        default:
            User::Panic(_L("VcxMyVideosApiTest: Unknown video type!"), KErrCorrupt);
        }
    VCXLOGLO1("<<<CVcxTestVideoCreator::GetVideoFile");
    }

//  End of File
