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



#include "e32image.h"
#include "e32imageheaders.h"
#include "launchertraces.h"


const TInt KMaxHeaderSize = sizeof(E32ImageHeaderV) + 65536/8;

SCapabilitySet AllCapabilities;
SCapabilitySet DisabledCapabilities;

// ---------------------------------------------------------------------------

E32ImageReader* E32ImageReader::NewLC()
    {
    E32ImageReader* self = new(ELeave) E32ImageReader;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------

E32ImageReader::E32ImageReader()
    {
    }

// ---------------------------------------------------------------------------

void E32ImageReader::ConstructL()
    {
    LOGSTRING("Launcher: E32ImageReader::ConstructL");

    iEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------------------------

CDesCArray* E32ImageReader::ListOfDLLsL(const TDesC& aFullPathToE32Image)
    {
    LOGSTRING("Launcher: E32ImageReader::ListOfDLLsL");

    // open the file for reading
    User::LeaveIfError( iFile.Open(iEnv->FsSession(), aFullPathToE32Image, EFileRead) );
    CleanupClosePushL(iFile);
    
    // get local copies of capability sets
    TCapabilitySet caps;
    caps.SetAllSupported();
    AllCapabilities=*(SCapabilitySet*)&caps;
    caps.SetDisabled();
    DisabledCapabilities=*(SCapabilitySet*)&caps;

    // read the header of the image
    User::LeaveIfError( E32ImageHeader::New(iHeader, iFile) );

    // set stuff
    iHeapSizeMin = iHeader->iHeapSizeMin;
    iHeapSizeMax = iHeader->iHeapSizeMax;
    iStackSize = iHeader->iStackSize;
    iPriority = iHeader->ProcessPriority();
    iDepCount = iHeader->iDllRefTableCount;
    iExportDirCount = iHeader->iExportDirCount;
    iExportDir = iHeader->iExportDirOffset-iHeader->iCodeOffset;
    iTextSize = iHeader->iTextSize;
    iCodeSize = iHeader->iCodeSize;
    iDataSize = iHeader->iDataSize;
    iBssSize = iHeader->iBssSize;
    iTotalDataSize = iDataSize+iBssSize;
    iFileEntryPoint = iHeader->iEntryPoint;
    iEntryPtVeneer = 0;
    iExceptionDescriptor = iHeader->ExceptionDescriptor();
    if (iHeader->iExportDirOffset)
        iExportDirLoad=iExportDir;
    
    // allocate data for the code area
    iCodeLoadAddress = (TUint32)User::Alloc(iCodeSize);
    
    // load data from the binary
    User::LeaveIfError( LoadData() );

    // create an array for the dll names from the import list
    CDesCArray* dllArray = new(ELeave) CDesCArrayFlat(100);
    CleanupStack::PushL(dllArray);

    // get the dll names from the import section
    E32ImportSection* importSection = (E32ImportSection *)iImportData;
    E32ImportBlock* block;
    if(importSection)
        block = (E32ImportBlock*)(importSection+1);
    else
        block = NULL;
    iNextImportPos = 0;

    // loop through all import data blocks
    for (TInt i=0; i<iDepCount; i++)
        {
        // get a pointer to the dll name
        TPtrC8 dllname = (const TText8*)((TUint32)iImportData + block->iOffsetOfDllName);
        TBuf8<KMaxKernelName> rootname;
        if (dllname.Length() > KMaxKernelName)
            User::Leave( KErrNotSupported );
        
        // parse the filename
        TFileNameInfo fni;
        User::LeaveIfError( fni.Set(dllname, TFileNameInfo::EAllowUid) );
        fni.GetName(rootname, TFileNameInfo::EIncludeBaseExt);

        // append the name to the dll name array
        TFileName dllName16;
        dllName16.Copy(rootname);
        dllArray->AppendL(dllName16);

        // get the next block
        TUint impfmt = iHeader->ImportFormat();
        block = (E32ImportBlock*)block->NextBlock(impfmt);
        }

    CleanupStack::Pop();  //dllArray
    CleanupStack::PopAndDestroy(); //iFile

    // free memory
    delete iHeader;
    iHeader = NULL;
    delete iImportSection;
    iImportSection = NULL;
    delete iCodeRelocSection;
    iCodeRelocSection = NULL;
    delete iDataRelocSection;
    iDataRelocSection = NULL;
    
    User::Free((TAny*)iCodeLoadAddress);
    User::Free(iRestOfFileData);
    User::Free(iCurrentImportList);
    if (iExportDirLoadAllocated)
        User::Free((TAny*)iExportDirLoad);

    return dllArray;
    }

// ---------------------------------------------------------------------------

E32ImageReader::~E32ImageReader()
    {
    LOGSTRING("Launcher: E32ImageReader::~E32ImageReader");
    }


// ---------------------------------------------------------------------------

TInt FileRead(RFile& aFile, TUint8* aDest, TInt aSize)
    {
    TPtr8 p(aDest,aSize,aSize);
    return aFile.Read(p,aSize);
    }

// ---------------------------------------------------------------------------

TInt E32ImageHeader::New(E32ImageHeader*& aHdr, RFile& aFile)
    {
    LOGSTRING("Launcher: E32ImageHeader::New");

    E32ImageHeader* h = NULL;
    TInt hdrsz = 0;
    TInt filesize;
    TInt r = aFile.Size(filesize);
    if (r!=KErrNone)
        return r;
    E32ImageHeader bh;
    r = FileRead(aFile, (TUint8*)&bh, sizeof(E32ImageHeader));
    if (r==KErrNone)
        {
        hdrsz = bh.TotalSize();
        if ( (TUint(hdrsz) > TUint(KMaxHeaderSize)) || (TUint(hdrsz) < TUint(sizeof(bh))) )
            return KErrCorrupt;
        }
    h = (E32ImageHeader*)User::Alloc(hdrsz);
    if (!h)
        return KErrNoMemory;
    wordmove(h, &bh, sizeof(E32ImageHeader));
    if (hdrsz > (TInt)sizeof(E32ImageHeader))
        r = FileRead(aFile, (TUint8*)(h + 1), hdrsz - sizeof(E32ImageHeader));
    if (r==KErrNone)
        r = h->IntegrityCheck(filesize);
    if (r==KErrNone)
        {
        if (h->HeaderFormat() >= KImageHdrFmt_V)
            {
            // Overide capabilities in image
            for(TInt i=0; i<SCapabilitySet::ENCapW; i++)
                {
                ((E32ImageHeaderV*)h)->iS.iCaps[i] |= DisabledCapabilities[i];
                ((E32ImageHeaderV*)h)->iS.iCaps[i] &= AllCapabilities[i];
                }
            }
        aHdr = h;
        }
    else
        {
        delete h;
        aHdr = NULL;
        }
    return r;
    }

// ---------------------------------------------------------------------------

TInt E32ImageHeader::IntegrityCheck(TInt aFileSize)
    {
    LOGSTRING("Launcher: E32ImageHeader::IntegrityCheck");
    
    TInt hdrsz = TotalSize();
    TUint hdrfmt = HeaderFormat();
    TUidType uids = *(const TUidType*)&iUid1;
    E32ImageHeaderV* v = NULL;
    TCheckedUid chkuid(uids);
    const TUint32* pChkUid = (const TUint32*)&chkuid;

    if (pChkUid[3] != iUidChecksum)
        return KErrCorrupt;    

    if (hdrfmt == KImageHdrFmt_V)
        {
        TUint32 supplied_crc = iHeaderCrc;
        iHeaderCrc = KImageCrcInitialiser;
        TUint32 crc = 0;
        Mem::Crc32(crc, this, hdrsz);
        iHeaderCrc = supplied_crc;
        if (crc != supplied_crc)
            return KErrCorrupt;    
        v = (E32ImageHeaderV*)this;
        TUint total_eds = v->iExportDescSize + sizeof(v->iExportDescSize) + sizeof(v->iExportDescType);
        total_eds = (total_eds + 3) &~ 3;
        if (total_eds + (TUint)_FOFF(E32ImageHeaderV, iExportDescSize) != (TUint)hdrsz)
            return KErrCorrupt;
        if (TUint(v->iExportDescType) > KImageHdr_ExpD_SparseBitmap8)
            return KErrNotSupported;
        }
    else if (hdrfmt >= KImageHdrFmt_V)
        return KErrNotSupported; 

    TCpu cpu = CpuIdentifier();
    TUint abi = ABI();
    TUint impfmt = ImportFormat();
    TUint compression = CompressionType();

    TUint uncompressed_size = compression ? UncompressedFileSize() : (TUint)aFileSize;

    if (iSignature != 0x434f5045) // 'EPOC'
        return KErrCorrupt;
    if (iCodeSize<0)
        return KErrCorrupt;
    if (iDataSize<0)
        return KErrCorrupt;
    if (iHeapSizeMin<0)
        return KErrCorrupt;
    if (iHeapSizeMax<iHeapSizeMin)
        return KErrCorrupt;
    if (iStackSize<0)
        return KErrCorrupt;
    if (iBssSize<0)
        return KErrCorrupt;
    if (iDllRefTableCount<0)
        return KErrCorrupt;
    if (iExportDirOffset >= uncompressed_size)
        return KErrCorrupt;
    if (TUint(iExportDirCount)>65535)
        return KErrCorrupt;
    if (iTextSize<0)
        return KErrCorrupt;
    if (iCodeSize<iTextSize)
        return KErrCorrupt;
    if (iCodeOffset >= uncompressed_size)
        return KErrCorrupt;
    if (iDataOffset >= uncompressed_size)
        return KErrCorrupt;
    if (iImportOffset >= uncompressed_size)
        return KErrCorrupt;
    if (iCodeRelocOffset >= uncompressed_size)
        return KErrCorrupt;
    if (iDataRelocOffset >= uncompressed_size)
        return KErrCorrupt;
    if (TUint(iCodeSize+iDataSize) > uncompressed_size)
        return KErrCorrupt;

    if (abi>KImageABI_EABI)
        return KErrNotSupported;
    if (impfmt>KImageImpFmt_PE2)
        return KErrNotSupported;
    if (iUid1 != (TUint32)KExecutableImageUidValue && iUid1 != (TUint32)KDynamicLibraryUidValue)
        return KErrNotSupported;
    TUint32 mv = ModuleVersion();
    if (mv >= 0x80000000u || (mv & 0x0000ffffu) > 0x8000u)
        return KErrNotSupported;

    return KErrNone;
    }
// ---------------------------------------------------------------------------

TInt E32ImageReader::LoadData()
    {
    LOGSTRING("Launcher: E32ImageReader::LoadData");

    TInt remainder;
    iFile.Size(remainder);
    
    remainder -= iHeader->TotalSize();
    TUint32 compression = iHeader->CompressionType();
    if (compression != KMyFormatNotCompressed)
        {
        remainder = iHeader->UncompressedFileSize() - iHeader->iCodeOffset;
        }

    remainder -= iHeader->iCodeSize;
    
    if (remainder > 0)
        {
        iRestOfFileData = (TUint8*)User::Alloc(remainder);
        if (iRestOfFileData)
            iRestOfFileSize=remainder;
        else
            return KErrNoMemory;
        }

    iConversionOffset = iHeader->iCodeOffset + iHeader->iCodeSize;

    TInt r = LoadFile(compression);
    if (r !=KErrNone)
        return r;


    TUint8* source=NULL;
    if (iHeader->iImportOffset)
        {
        TUint bufferOffset=iHeader->iImportOffset-iConversionOffset;
        
        if(bufferOffset>iRestOfFileSize || bufferOffset+sizeof(E32ImportSection)>iRestOfFileSize)
            return KErrCorrupt;
        
        source=iRestOfFileData+bufferOffset;
        
        iImportSection = new E32ImportSection;
        
        if (iImportSection)
            Mem::Move((TText8*)iImportSection, source, sizeof(E32ImportSection));
        else
            return KErrNoMemory;
        }

    iCodeDelta = iCodeRunAddress-iHeader->iCodeBase;
    iDataDelta = iDataRunAddress-iHeader->iDataBase;

    if (r==KErrNone)
        r = ReadImportData();

    return r;
    }

// ---------------------------------------------------------------------------

TUint8* E32ImageReader::MemoryMove(TAny* aDestination, const TAny* aSource, TInt aNumberofBytes)
    {
    return Mem::Move(aDestination, aSource, aNumberofBytes);
    }

// ---------------------------------------------------------------------------

GLDEF_C TInt svRelocateExports(TAny* aPtr)
    {
    E32ImageReader* pI=(E32ImageReader*)aPtr;
    TUint32* destExport=(TUint32*)pI->iExportDirLoad;
    TInt i=pI->iExportDirCount;
    TUint32 codeBase=pI->iCodeRunAddress;
    while (i-->0)
        *destExport+++=codeBase;
    return 0;
    }

// ---------------------------------------------------------------------------

TInt E32ImageReader::LoadFile(TUint32 aCompression)
    {
    LOGSTRING("Launcher: E32ImageReader::LoadFile");

    TInt r(KErrNone);

    if (aCompression == KMyFormatNotCompressed)
        r=LoadFileNoCompress();
    
    else if (aCompression == KMyUidCompressionDeflate)
        {
        TRAP(r, LoadFileInflateL());
        }
    
    else
        r = KErrNotSupported;
    
    return r;
    }

// ---------------------------------------------------------------------------

TInt E32ImageReader::LoadFileNoCompress()
    {
    LOGSTRING("Launcher: E32ImageReader::LoadFileNoCompress");

    TInt r(KErrNone);

    if (iHeader->iCodeSize)
        {
        r = Read(iHeader->iCodeOffset, (TText8*)iCodeLoadAddress, iCodeSize);
        }

    if (r != KErrNone)
        {
        return r;
        }

    if (iRestOfFileSize)
        {
        r = Read(iConversionOffset, (TText8*)iRestOfFileData, iRestOfFileSize);
        }
    
    return r;
    }

// ---------------------------------------------------------------------------

void FileCleanup(TAny* aPtr)
    {
    TFileInput* f=(TFileInput*)aPtr;
    f->Cancel();
    delete f;
    }

// ---------------------------------------------------------------------------

void E32ImageReader::LoadFileInflateL()
    {
    LOGSTRING("Launcher: E32ImageReader::LoadFileInflateL");

    TInt pos = iHeader->TotalSize();
    User::LeaveIfError(iFile.Seek(ESeekStart,pos));

    TFileInput* file = new (ELeave) TFileInput(iFile);
    CleanupStack::PushL(TCleanupItem(&FileCleanup,file));
    CInflater* inflater = CInflater::NewLC(*file);
    
    if (iHeader->iCodeSize)
        {
        TInt count = inflater->ReadL((TUint8*)iCodeLoadAddress, iCodeSize, &MemoryMove);
        
        if(count!=iCodeSize)
            User::Leave(KErrCorrupt);
        }

    if (iRestOfFileSize)
        {
        TUint32 count = inflater->ReadL(iRestOfFileData, iRestOfFileSize, &Mem::Move);
        
        if(count!=iRestOfFileSize)
            User::Leave(KErrCorrupt);
        }
    
    CleanupStack::PopAndDestroy(2,file);
    }

// ---------------------------------------------------------------------------

TInt E32ImageReader::Read(TText8* aDest, TInt aSize)
    {
    TPtr8 p(aDest,aSize,aSize);
    return iFile.Read(p,aSize);
    }

// ---------------------------------------------------------------------------

TInt E32ImageReader::Read(TInt aPos, TText8* aDest, TInt aSize)
    {
    TPtr8 p(aDest,aSize,aSize);
    if (aPos<0)
        return KErrCorrupt;
    return iFile.Read(aPos,p,aSize);
    }

// ---------------------------------------------------------------------------

TInt E32ImageReader::ReadImportData()
    {
    LOGSTRING("Launcher: E32ImageReader::ReadImportData");

    if (!iHeader->iImportOffset)
        return KErrNone;
    
    TUint32 bufferOffset = iHeader->iImportOffset-iConversionOffset;
    
    if(bufferOffset>iRestOfFileSize || bufferOffset+iImportSection->iSize>iRestOfFileSize)
        return KErrCorrupt;
    
    iImportData = (TUint32*)(iRestOfFileData+bufferOffset);
    
    E32ImportSection* s = (E32ImportSection*)iImportData;
    E32ImportBlock* b = (E32ImportBlock*)(s + 1);
    TUint impfmt = iHeader->ImportFormat();
    TInt i;
    TInt n = 0;
    
    for (i=0; i<iDepCount; ++i)
        {
        if (b->iNumberOfImports > n)
            n = b->iNumberOfImports;
        b = (E32ImportBlock*)b->NextBlock(impfmt);
        }
    
    iCurrentImportList = (TUint32*)User::Alloc(n * sizeof(TUint32));
    
    if (!iCurrentImportList)
        return KErrNoMemory;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------

inline CInflater::CInflater(TBitInput& aInput)
    :iBits(&aInput),iEncoding(0),iOut(0)
    {}

// ---------------------------------------------------------------------------

void CInflater::ConstructL()
    {
    iEncoding=new(ELeave) TEncoding;
    InitL();
    iLen=0;
    iOut=new(ELeave) TUint8[KDeflateMaxDistance];
    iAvail=iLimit=iOut;
    }

// ---------------------------------------------------------------------------

CInflater* CInflater::NewLC(TBitInput& aInput)
    {
    CInflater* self=new(ELeave) CInflater(aInput);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------

CInflater::~CInflater()
    {
    delete iEncoding;
    delete [] iOut;
    }

// ---------------------------------------------------------------------------

TInt CInflater::ReadL(TUint8* aBuffer,TInt aLength, TMemoryMoveFunction aMemMovefn)
    {
    TInt tfr=0;
    for (;;)
        {
        TInt len=Min(aLength,iLimit-iAvail);
        if (len && aBuffer)
            {
            aMemMovefn(aBuffer,iAvail,len);
            aBuffer+=len;
            }
        aLength-=len;
        iAvail+=len;
        tfr+=len;
        if (aLength==0)
            return tfr;
        len=InflateL();
        if (len==0)
            return tfr;
        iAvail=iOut;
        iLimit=iAvail+len;
        }
    }

// ---------------------------------------------------------------------------

TInt CInflater::SkipL(TInt aLength)
    {
    return ReadL(0,aLength,Mem::Move);
    }

// ---------------------------------------------------------------------------

void CInflater::InitL()
    {
    Huffman::InternalizeL(*iBits,iEncoding->iLitLen,KDeflationCodes);

    if (!Huffman::IsValid(iEncoding->iLitLen,TEncoding::ELitLens) ||
        !Huffman::IsValid(iEncoding->iDistance,TEncoding::EDistances))
        User::Leave(KErrCorrupt);

    Huffman::Decoding(iEncoding->iLitLen,TEncoding::ELitLens,iEncoding->iLitLen);
    Huffman::Decoding(iEncoding->iDistance,TEncoding::EDistances,iEncoding->iDistance,KDeflateDistCodeBase);
    }

// ---------------------------------------------------------------------------

TInt CInflater::InflateL()
    {
    TUint8* out=iOut;
    TUint8* const end=out+KDeflateMaxDistance;
    const TUint32* tree=iEncoding->iLitLen;
    if (iLen<0)    // EOF
        return 0;
    if (iLen>0)
        goto useHistory;

    while (out<end)
        {
        // get a huffman code
        {
        TInt val=iBits->HuffmanL(tree)-TEncoding::ELiterals;
        if (val<0)
            {
            *out++=TUint8(val);
            continue;        
            }
        if (val==TEncoding::EEos-TEncoding::ELiterals)
            {    
            iLen=-1;
            break;
            }
        
        TInt code=val&0xff;
        if (code>=8)
            {    
            TInt xtra=(code>>2)-1;
            code-=xtra<<2;
            code<<=xtra;
            code|=iBits->ReadL(xtra);
            }
        if (val<KDeflateDistCodeBase-TEncoding::ELiterals)
            {
            iLen=code+KDeflateMinLength;
            tree=iEncoding->iDistance;
            continue;        
            }
        
        iRptr=out-(code+1);
        if (iRptr+KDeflateMaxDistance<end)
            iRptr+=KDeflateMaxDistance;
        }
useHistory:
        TInt tfr=Min(end-out,iLen);
        iLen-=tfr;
        const TUint8* from=iRptr;
        do
            {
            *out++=*from++;
            if (from==end)
                from-=KDeflateMaxDistance;
            } while (--tfr!=0);
        iRptr=from;
        tree=iEncoding->iLitLen;
        };
    
    return out-iOut;
    }

// ---------------------------------------------------------------------------

TFileInput::TFileInput(RFile& aFile)
    :iFile(aFile),iReadBuf(iBuf1),iPtr(iBuf1,KBufSize)
    {
    aFile.Read(iPtr,iStat);
    }

// ---------------------------------------------------------------------------

void TFileInput::Cancel()
    {
    if (iReadBuf)
        User::WaitForRequest(iStat);
    }

// ---------------------------------------------------------------------------

void TFileInput::UnderflowL()
    {
    TUint8* b=iReadBuf;
    ASSERT(b!=NULL);
    User::WaitForRequest(iStat);
    iReadBuf=0;
    User::LeaveIfError(iStat.Int());
    if(iPtr.Length()==0)
        User::Leave(KErrCorrupt);
    Set(b,iPtr.Length()*8);
    
    b = b==iBuf1 ? iBuf2 : iBuf1;
    iPtr.Set(b,0,KBufSize);
    iFile.Read(iPtr,iStat);
    iReadBuf=b;
    }

// ---------------------------------------------------------------------------

TFileNameInfo::TFileNameInfo()
    {
    memclr(this, sizeof(TFileNameInfo));
    }

// ---------------------------------------------------------------------------

TInt TFileNameInfo::Set(const TDesC8& aFileName, TUint aFlags)
    {
    iUid = 0;
    iVersion = 0;
    iPathPos = 0;
    iName = aFileName.Ptr();
    iLen = aFileName.Length();
    iExtPos = aFileName.LocateReverse('.');
    if (iExtPos<0)
        iExtPos = iLen;
    TInt osq = aFileName.LocateReverse('[');
    TInt csq = aFileName.LocateReverse(']');
    if (!(aFlags & EAllowUid) && (osq>=0 || csq>=0))
        {
        return KErrBadName;
        }
    if (osq>=iExtPos || csq>=iExtPos)
        {
        return KErrBadName;
        }
    TInt p = iExtPos;
    if ((aFlags & EAllowUid) && p>=10 && iName[p-1]==']' && iName[p-10]=='[')
        {
        TPtrC8 uidstr(iName + p - 9, 8);
        TLex8 uidlex(uidstr);
        TUint32 uid = 0;
        TInt r = uidlex.Val(uid, EHex);
        if (r==KErrNone && uidlex.Eos())
            iUid = uid, p -= 10;
        }
    iUidPos = p;
    TInt ob = aFileName.LocateReverse('{');
    TInt cb = aFileName.LocateReverse('}');
    if (ob>=iUidPos || cb>=iUidPos)
        {
        return KErrBadName;
        }
    if (ob>=0 && cb>=0 && p-1==cb)
        {
        TPtrC8 p8(iName, p);
        TInt d = p8.LocateReverse('.');
        TPtrC8 verstr(iName+ob+1, p-ob-2);
        TLex8 verlex(verstr);
        if (ob==p-10 && d<ob)
            {
            TUint32 ver = 0;
            TInt r = verlex.Val(ver, EHex);
            if (r==KErrNone && verlex.Eos())
                iVersion = ver, p = ob;
            }
        else if (d>ob && p-1>d && (aFlags & EAllowDecimalVersion))
            {
            TUint32 maj = 0;
            TUint32 min = 0;
            TInt r = verlex.Val(maj, EDecimal);
            TUint c = (TUint)verlex.Get();
            TInt r2 = verlex.Val(min, EDecimal);
            if (r==KErrNone && c=='.' && r2==KErrNone && verlex.Eos() && maj<32768 && min<32768)
                iVersion = (maj << 16) | min, p = ob;
            }
        }
    iVerPos = p;
    if (iLen>=2 && iName[1]==':')
        {
        TUint c = iName[0];
        if (c!='?' || !(aFlags & EAllowPlaceholder))
            {
            c |= 0x20;
            if (c<'a' || c>'z')
                {
                return KErrBadName;
                }
            }
        iPathPos = 2;
        }
    TPtrC8 pathp(iName+iPathPos, iVerPos-iPathPos);
    if (pathp.Locate('[')>=0 || pathp.Locate(']')>=0 || pathp.Locate('{')>=0 || pathp.Locate('}')>=0 || pathp.Locate(':')>=0)
        {
        return KErrBadName;
        }
    iBasePos = pathp.LocateReverse('\\') + 1 + iPathPos;

    return KErrNone;
    }

// ---------------------------------------------------------------------------

void TFileNameInfo::GetName(TDes8& aName, TUint aFlags) const
    {
    if (aFlags & EIncludeDrive)
        aName.Append(Drive());
    if (aFlags & EIncludePath)
        {
        if (PathLen() && iName[iPathPos]!='\\')
            aName.Append('\\');
        aName.Append(Path());
        }
    if (aFlags & EIncludeBase)
        aName.Append(Base());
    if ((aFlags & EForceVer) || ((aFlags & EIncludeVer) && VerLen()) )
        {
        aName.Append('{');
        aName.AppendNumFixedWidth(iVersion, EHex, 8);
        aName.Append('}');        
        }
    if ((aFlags & EForceUid) || ((aFlags & EIncludeUid) && UidLen()) )
        {
        aName.Append('[');
        aName.AppendNumFixedWidth(iUid, EHex, 8);
        aName.Append(']');
        }
    if (aFlags & EIncludeExt)
        aName.Append(Ext());
    }

// ---------------------------------------------------------------------------
