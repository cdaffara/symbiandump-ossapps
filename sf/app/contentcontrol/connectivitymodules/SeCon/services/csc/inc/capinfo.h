/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Capability service controller
*
*/


#ifndef _CAPINFO_H_
#define _CAPINFO_H_

#include <e32base.h>
#include <f32file.h>

#include "caputils.h"

//
// CCapInfo creates capability document
// 
//

class CCapList;
class CStringList;
class TSConSolutionInfo;
class CSconVersionInfo;

NONSHARABLE_CLASS( CCapInfo ) : public CBase
{
public:
    static CCapInfo* NewL();        
    ~CCapInfo();
private:
    CCapInfo();
    void ConstructL();

public:
    void CapabilityDocumentL( CBufFlat* aBuf );

private:
    CCapList* CapList();
    void SetCapabilityBuf( CBufFlat* aBuf );
    
    void GeneralInfoL();
    void MemoryInfoL();
    void WriteMemoryInfoL( TMemoryInfo& aInfo );
    void ServiceInfoL( const TDesC& aDir, const RArray<TFileName>& aExcludeList,
        RArray<TFileName>& aFilesAdded );
    void FolderServiceL();
    
    void SyncSolutionsServiceL();
    void WriteSyncSolutionsServiceL( const TSConSolutionInfo& aCalendar,
    		const TSConSolutionInfo& aContact,
    		const TSConSolutionInfo& aSms,
    		const TSConSolutionInfo& aBookmark );
    void WriteSolutionTagL( const TDesC& aContentName, const TSConSolutionInfo& aSolution );
    
    void WriteFolderL( const TDesC& aType, const TDesC& aFullName, const TDesC& aMemory );

    void WriteValueL( TInt aId, const TDesC& aValue );
    void WriteBigValueL( TInt aId, const TDesC& aValue );
    void WriteValueL( TInt aId, TInt64 aValue );
    void WriteTagL( TInt aId, TInt aType );
    void WriteAttributeL( TInt aId, const TDesC& aVersion, const TDesC& aDate );
    void WriteL( const TDesC& aText );

    void WriteFromListL( CStringList* aList );
    void WriteNewLineL();

    void FormatElement( TDes& aText );
    void AddFormatText( TDes& aText, TInt aNum ) const;

private:
    RFs             iFsSession;     // has
    CCapList*       iCapList;       // has
    HBufC8*         iHeapBuf;       // has
    TBuf<KMaxSize>  iBuf;
    TIdStack        iIdStack;

    CBufFlat*       iCapabilityBuf; // uses
    CSconVersionInfo* iSconInfo;
};

#endif // CCapability

