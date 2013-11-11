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




#ifndef __LAUNCHERE32IMAGE_H__
#define __LAUNCHERE32IMAGE_H__

#include <e32base.h>
#include <badesca.h>
#include <eikenv.h>
#include <e32ldr_private.h>

class E32ImageHeader;
class E32ImportSection;
class E32RelocSection;
class CEikonEnv;


NONSHARABLE_CLASS(E32ImageReader) : public CBase, public TProcessCreateInfo
	{
public:
	static E32ImageReader* NewLC();
	~E32ImageReader();

public:
    CDesCArray* ListOfDLLsL(const TDesC& aFullPathToE32Image);

private:
	E32ImageReader();
	void ConstructL();
    TInt LoadData();
    static TUint8* MemoryMove(TAny* aDestination, const TAny* aSource, TInt aNumberofBytes);
    TInt LoadFile(TUint32 aCompression);
    TInt LoadFileNoCompress();
    void LoadFileInflateL();
    TInt Read(TText8* aDest, TInt aSize);
    TInt Read(TInt aPos, TText8* aDest, TInt aSize);
    TInt ReadImportData();

private:
	CEikonEnv*      iEnv;

public:
	E32ImageHeader* iHeader;
	E32ImportSection* iImportSection;
	E32RelocSection* iCodeRelocSection;
	E32RelocSection* iDataRelocSection;
	TUint32* iImportData;
	TUint8* iRestOfFileData;
	TUint32 iRestOfFileSize;
	TUint32 iConversionOffset;
	RFile iFile;
	TUint32 iCodeDelta;
	TUint32 iDataDelta;
	TUint32 iExportDirLoad;
	TBool isDll;
	TBool iAlreadyLoaded;
	TAny* iCloseCodeSeg;
	TInt iCurrentImportCount;
	TInt iNextImportPos;
	TUint32* iCurrentImportList;
	TUint8 iCurrentImportListSorted;
	TUint8 iExportDirLoadAllocated;
	TUint8 iPadding1;
	TUint8 iPadding2;
	};

#endif // __LAUNCHERE32IMAGE_H__
