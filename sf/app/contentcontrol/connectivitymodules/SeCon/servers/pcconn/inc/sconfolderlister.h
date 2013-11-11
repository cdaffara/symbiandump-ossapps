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
* Description:  CSconFolderLister declaration
*
*/


#ifndef CSCONFOLDERLISTER_H
#define CSCONFOLDERLISTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <s32mem.h> // For RMemReadStream
#include <f32file.h>
// CLASS DECLARATION

class TSconDriveInfoEntry
    {
public:
    TUint8 iDriveType;
    TUint8 iDriveTypeCount;
    TInt64 iSize;
    TInt64 iFree;
    };

class TSconDriveInfo : public TSconDriveInfoEntry
    {
public:
    TUint iDriveStatus;
    TBuf16<2> iDriveLetter;
    TUint iDriveAttr;
    TBufC<KMaxFileName> iVolumeName;
    };


class CSconFolderEntry : public CBase
    {
public:
    static CSconFolderEntry* NewLC();
    static CSconFolderEntry* NewLC( const TEntry& aEntry );
    ~CSconFolderEntry();
private:
    CSconFolderEntry();
    void ConstructL( const TEntry& aEntry );
    
public:
    TUint iAtt;
    TTime iModified;
    HBufC *iName;
    HBufC *iLabel;
    TBool iDriveInfoEntryExists;
    TSconDriveInfoEntry iDriveInfoEntry;
    };


class CDirectoryLocalizer;

/**
 *  CSconFolderLister
 * 
 */
class CSconFolderLister : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CSconFolderLister();
    
    /**
     * Two-phased constructor.
     */
    static CSconFolderLister* NewL( RFs& aFs );
    
    /**
     * 
     * @param aStream
     * @param aStartPath
     * @param aLevelsToSearch -1 = Search all levels, 0 = Search only current level, 1..N Search N levels
     */
    void GenerateFolderListL( RBufWriteStream& aStream, const TDesC& aStartPath, const TInt aLevelsToSearch );
    
private:  
    
    void ListAllDrivesL( RBufWriteStream& aStream, const TInt aLevelsToSearch );
    void ListDriveAndPathL( RBufWriteStream& aStream, TInt aDrive, const TDesC& aStartPath, const TInt aLevelsToSearch );
    
    void ExportDriveL( RBufWriteStream& aStream, const TSconDriveInfo& aDriveInfo,
            const TDesC& aPathName, const TInt aLevelsToSearch );
    
    /**
     * 
     * @param aStream
     * @param aPathName
     */
    void ExportPathL( RBufWriteStream& aStream, const TDesC& aPathName, 
            const CSconFolderEntry& aEntry, const TBool aLocalize, const TInt aLevelsToSearch );
    
    void GetDriveTypeNumberL( TInt aDrive , TUint driveStatus, TInt& aTypeNumber);
    
    /**
     * Constructor for performing 1st stage construction
     */
    CSconFolderLister( RFs& aFs );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    TBool IsDirectoryPrintable( const TDesC& aParentPath, const TEntry& aFolderEntry );
    
    void GetLocalizedVolumeNameL( CSconFolderEntry& aDriveEntry );
private:
    RFs&    iFs;
    TInt    iFolders;
    TInt    iFiles;
    TInt    iCriticalLevel;
    CDirectoryLocalizer* iLocalizer;
    };

#endif // CSCONFOLDERLISTER_H
