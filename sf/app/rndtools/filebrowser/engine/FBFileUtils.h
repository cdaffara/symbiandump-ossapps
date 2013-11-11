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

#ifndef FILEBROWSER_FILEUTILS_H
#define FILEBROWSER_FILEUTILS_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <w32std.h>
#include <badesca.h>
#include <coedef.h>
#include <msvapi.h>
#include <tz.h>
#include "FB.hrh"

_LIT(KIRAppPath, "z:\\sys\\bin\\irapp.exe");
_LIT(KBTAppPath, "z:\\sys\\bin\\btui.exe");
_LIT(KUSBAppPath, "z:\\sys\\bin\\usbclasschangeui.exe");
_LIT(KErrRdPath, "c:\\resource\\ErrRd");
_LIT(KErrRdDir, "c:\\resource\\");


// FORWARD DECLARATIONS
class CEngine;
class CFileBrowserFileOps;
class CDocumentHandler;
class CEikProgressInfo;
class CFBFileOpClient;
class CEikProgressInfo;
class CMessageDigest;

// CLASS DECLARATIONS

class TSearchAttributes
	{
public:
    TFileName       iSearchDir;
    TFileName       iWildCards;
    TFileName       iTextInFile;
    TUint           iMinSize;
    TUint           iMaxSize;
    TTime           iMinDate;
    TTime           iMaxDate;
    TBool           iRecurse;
    TBool           iDefaultWildCard;
	};

class TDriveEntry
	{
public:
    TChar           iLetter;
    TInt            iNumber;
    TVolumeInfo     iVolumeInfo;
    TBuf<64>        iMediaTypeDesc;
    TBuf<128>       iAttributesDesc;
    TInt            iIconId;
	};

class TFileEntry
	{
public:
    TFileName       iFullName;	
    TFileName       iPath;
    TEntry          iEntry;
    TInt            iDirEntries;
    TInt            iIconId;
	};

class TAppIcon
	{
public:
    TInt            iId;
    TUid            iUid;
	};

typedef CArrayFixSeg<TDriveEntry> CDriveEntryList;
typedef CArrayFixSeg<TFileEntry> CFileEntryList;
typedef CArrayFixSeg<TAppIcon> CAppIconList;

class TSearchResults
    {
public:
    TInt iNumberOfFoundFiles;
    //CFileEntryList iFoundFilesList;
    };

class TOverwriteOptions
    {
public:
    TBool iDoFileOperations/*(ETrue)*/;
    TInt iQueryIndex/*(0)*/;
    TFileName iPostFix;
    TInt iOverWriteFlags/* = CFileMan::EOverWrite*/;
    };

class CCommandParamsBase : public CBase
    {
    };

class CCommandParamsAttribs : public CCommandParamsBase
    {
public:
    TFileEntry iSourceEntry;
    TUint iSetMask;
    TUint iClearMask;
    TTime iTime;
    TUint iSwitch;
public:
    CCommandParamsAttribs(const TFileEntry& aSourceEntry, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch) : iSourceEntry(aSourceEntry), iSetMask(aSetMask), iClearMask(aClearMask), iTime(aTime), iSwitch(aSwitch) {}
    };
    
class CCommandParamsCopyOrMove : public CCommandParamsBase
    {
public:
    TFileEntry iSourceEntry;
    TFileName iTargetPath;
    TUint iSwitch;
public:
    CCommandParamsCopyOrMove(const TFileEntry& aSourceEntry, const TDesC& aTargetPath, TUint aSwitch) : iSourceEntry(aSourceEntry), iTargetPath(aTargetPath), iSwitch(aSwitch) {}
    };
        
class CCommandParamsRename : public CCommandParamsBase
    {
public:
    TFileEntry iSourceEntry;
    TFileName iTargetPath;
    TUint iSwitch;
public:
    CCommandParamsRename(const TFileEntry& aSourceEntry, const TDesC& aTargetPath, TUint aSwitch) : iSourceEntry(aSourceEntry), iTargetPath(aTargetPath), iSwitch(aSwitch) {}
    };

class CCommandParamsDelete : public CCommandParamsBase
    {
public:
    TFileEntry iSourceEntry;
    TUint iSwitch;
public:
    CCommandParamsDelete(const TFileEntry& aSourceEntry, TUint aSwitch) : iSourceEntry(aSourceEntry), iSwitch(aSwitch) {}
    };

class CCommandParamsDriveSnapShot : public CCommandParamsBase
    {
public:
    TInt iSourceDriveLetter;
    TInt iTargetDriveLetter;
public:
    CCommandParamsDriveSnapShot(TChar aSourceDriveLetter, TChar aTargetDriveLetter) : iSourceDriveLetter(aSourceDriveLetter), iTargetDriveLetter(aTargetDriveLetter) {}
    };
    
class TCommand
    {
public:
    TInt iCommandId;
    CCommandParamsBase* iParameters;
public:
    TCommand(TInt aCommandId, CCommandParamsBase* aParameters) : iCommandId(aCommandId), iParameters(aParameters) {}
    };

typedef CArrayFixSeg<TCommand> CCommandArray;



class CFileBrowserFileUtils : public CActive, public MMsvSessionObserver
	{
private:
    enum TState // active object states
    	{
    	EIdle = 0,              // do nothing
    	};

//    enum TClipBoardMode
//    	{
//    	EClipBoardModeCut = 0,
//    	EClipBoardModeCopy
//    	};

public:
	static CFileBrowserFileUtils* NewL(CEngine* aEngine);
	~CFileBrowserFileUtils();

private:
	CFileBrowserFileUtils(CEngine* aEngine);
	void ConstructL();

private: // from CActive
	void RunL();
        TInt RunError(TInt aError);
	void DoCancel();

private: // from MMsvSessionObserver
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

public:  //from MProgressDialogCallback
    void DialogDismissedL(/*TInt aButtonId*/);
    
public: // command handling
    void StartExecutingCommandsL(const TDesC& aLabel);
private: // command handling
    void ExecuteCommand();
    void CheckForMoreCommandsL();
    void AppendToCommandArrayL(TInt aCommand, CCommandParamsBase* aParameters);
    TInt CommandArrayCount() const;
    void ResetCommandArray();       

private: // misc functionality
    void GenerateDirectoryDataL();
    void GetDriveListL();
    void GetDirectoryListingL();
//    CAknIconArray* GenerateIconArrayL(TBool aGenerateNewBasicIconArray=EFalse);
//    void AppendGulIconToIconArrayL(CAknIconArray* aIconArray, const TDesC& aIconFile, TInt aIconId, TInt aMaskId, const TAknsItemID aAknsItemId);
    TInt AppIconIdForUid(TUid aUid);
    TUid GetAppUid(TFileEntry aFileEntry);
//    CDesCArray* GenerateItemTextArrayL();
//    TInt GetSelectedItemsOrCurrentItemL(CFileEntryList* aFileEntryList);
    TInt SetSelectedItemsOrCurrentItemL(const CArrayFix<TInt>* selectionIndexes,
                                        CFileEntryList* aFileEntryList);

//    TBool IsDestinationEntriesExists(const CFileEntryList* aEntryList, const TDesC& aTargetDir);
    void DoCopyToFolderL(CFileEntryList* aEntryList, const TDesC& aTargetDir, const TOverwriteOptions &aOverwriteOptions, TBool aDeleteSource);
    TInt DoSearchFiles(const TDesC& aFileName, const TDesC& aPath);
    TInt DoSearchFilesRecursiveL(const TDesC& aFileName, const TDesC& aPath);
    TInt DoFindFiles(const TDesC& aFileName, const TDesC& aPath);
    TInt DoFindFilesRecursiveL(const TDesC& aFileName, const TDesC& aPath);
    void ReadAttachmentPathsRecursiveL(CMsvSession* aSession, CMsvEntry* aContext, CDesCArray* aAttPaths);
    void WriteMessageEntryInfoRecursiveL(CMsvSession* aSession, CMsvEntry* aContext, RFile& aFile, TInt& aLevel);
    void DoWriteMessageEntryInfoL(CMsvEntry* aContext, RFile& aFile, TInt aLevel);
    void ConvertCharsToPwd(const TDesC& aWord, TDes8& aConverted) const;
    HBufC8* MessageDigestInHexLC(CMessageDigest* aMD, RFile& aFile);
    void OpenCommonFileActionQueryL(TInt aCurrentItemIndex);
    
public: // public interfaces
    TKeyResponse HandleOfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandleSettingsChangeL();
    void SetSortModeL(TInt aSortMode);
    void SetOrderModeL(TInt aOrderMode);
    void RefreshViewL();
    TBool IsCurrentDriveReadOnly();
    TBool IsItemDirectory(TInt aCurrentItemIndex);
    void MoveUpOneLevelL();
    void MoveDownToDirectoryL(TInt aIndex);
    TInt ClipboardCutL(const CArrayFix<TInt>* aSelectionIndices);
    TInt ClipboardCopyL(const CArrayFix<TInt>* aSelectionIndices);
    void ClipboardPasteL(const TOverwriteOptions &aOverwriteOptions);
    TInt SetCurrentSelection(const CArrayFix<TInt>* aSelectionIndices);
    void CopyToFolderL(TFileName aTargetDir, const TOverwriteOptions &aOverwriteOptions, TBool aMove=EFalse);
    void DeleteL();
    TBool SelectionHasDirs();
    void TouchL(TBool aRecurse);
    void RenameL(const TInt aIndex, const TFileName &newName);
    void SetAttributesL(TUint &aSetAttMask, TUint &aClearAttMask, TBool &aRecurse);
    void SearchL();
    void NewFileL(const TFileName &aNewFileName);
    void NewDirectoryL(const TFileName &aNewDirectoryName);
    void SendToL();
    void CompressL();
    void DecompressL();
    bool PropertiesL(TInt currentItemIndex, CDesCArray* entryLines, TFileName &titleText);
    void OpenWithApparcL(TFileName aFileName);
    void OpenWithDocHandlerL(TFileName aFileName, TBool aEmbed);
    void OpenWithFileServiceL(TInt aCurrentItemIndex);
    TBool FileExists(const TDesC& aPath);
    TInt LaunchProgramL(const TDesC& aPath);
    void MemoryInfoPopupL();
    void ShowFileCheckSumsL(TInt aCurrentItemIndex, TInt aType);
    void SetErrRdL(TBool aEnable);
//    void EnableAvkonIconCacheL(TBool aEnable);
    void SimulateLeaveL(TInt aLeaveCode);
    void SimulatePanicL(const TDesC& aPanicCategory, TInt aPanicCode);
    void SimulateExceptionL(TInt aExceptionCode);
    TUint32 GetDebugMask();
    void SetDebugMaskL(TUint32 aDbgMask);
    void WriteAllAppsL();
    void WriteAllFilesL();
    void ListOpenFilesL();
    void ListMessageAttachmentsL(TInt aType);
    void WriteMsgStoreWalkL();
    void FileEditorL(TInt aCurrentItemIndex, TInt aType);
    void SetDrivePasswordL(TInt aIndex, const TFileName &aOldPassword, const TFileName &aNewPassword);
    void UnlockDriveL(TInt aIndex, const TFileName &aOldPassword);
    void ClearDrivePasswordL(TInt aIndex, const TFileName &aOldPassword);
    void EraseDrivePasswordL(TInt aIndex);
    void FormatDriveL(TInt aIndex, TBool aQuickFormat);
    void CheckDiskL(TInt aIndex);
    void ScanDriveL(TInt aIndex);
    void SetDriveNameL(TInt aIndex, const TFileName &aDriveName);
    void SetDriveVolumeLabelL(TInt aIndex, const TFileName &aVolumeLabel);
    void EjectDriveL(TInt aIndex);
    void DismountFileSystemL(TInt aIndex);
    void EraseMBRL(TInt aIndex);
    void PartitionDriveL(TInt aIndex, TBool aEraseMBR, TInt aAmountOfPartitions);
    TBool DriveSnapShotPossible();
    void DriveSnapShotL();
    void EditDataTypesL();
    void SecureBackupL(TInt aType);

    TBool IsDestinationEntriesExists(const TDesC& aTargetDir);
    TBool TargetExists(const TInt aIndex, const TFileName &newName);
    void GetDriveName(TInt aIndex, TFileName &aDriveName);
    void GetDriveVolumeLabel(TInt aIndex, TFileName &aVolumeLabel);

    CFileEntryList* FileEntries() const;
    CDriveEntryList* DriveEntries() const;
    
public:    
    inline TInt SortMode() { return iSortMode; }
    inline TInt OrderMode() { return iOrderMode; }
    inline CFileEntryList* ClipBoardList() { return iClipBoardList; }
    inline CFileEntryList* CurrentSelectionList() { return iCurrentSelectionList; }
    inline TBool IsDriveListViewActive() { return iCurrentPath==KNullDesC && iListingMode==ENormalEntries; }
    inline TBool IsNormalModeActive() { return iListingMode==ENormalEntries; }
    inline TListingMode ListingMode() { return iListingMode; }
    inline TFileName CurrentPath() { return iCurrentPath; }
	
    inline TSearchAttributes GetSearchAttributes(){ return iSearchAttributes; };
    inline void ChangeAttributes(TSearchAttributes attributes) { iSearchAttributes = attributes; };
    inline TSearchResults SearchResults(){ return iFileSearchResults; };
    inline CFileEntryList* FoundFiles() { return iFileEntryList; };    
    inline void SetAllowProcessing(TBool aAllowProcessing) { iAllowProcessing = aAllowProcessing; }
    TClipBoardMode GetClipBoardMode() { return iClipBoardMode; }
	
private:
    TState                          iState;
    CEngine*                        iEngine;
    CFileBrowserFileOps*            iFileOps;

    TBool                           isWaitDialog;
    TBool                           isProgressDialog;

    //CEikProgressInfo*               iProgressInfo;
    CCommandArray*                  iCommandArray;
    TInt                            iCurrentEntry;
    TInt                            iSucceededOperations;
    TInt                            iFailedOperations;
    TInt                            iLastError;
    RTimer                          iTimer;
    RFs                             iFs;
    TListingMode                    iListingMode;
    CFileMan*                       iFileMan;
    TInt                            iViewMode;
    TFileName                       iCurrentPath;
    TInt                            iSortMode;
    TInt                            iOrderMode;
//    TInt                            iClipboardMode;
    CDesCArray*                     iClipboardPaths;
    CDriveEntryList*                iDriveEntryList;
    CFileEntryList*                 iFileEntryList;
    CFileEntryList*                 iFindFileEntryList;
    CAppIconList*                   iAppIconList;
    TClipBoardMode                  iClipBoardMode;
    CFileEntryList*                 iClipBoardList;
    CFileEntryList*                 iCurrentSelectionList;
    TSearchAttributes               iSearchAttributes;
    CDocumentHandler*               iDocHandler;

    RFile                           iMsgStoreWalkFile;
    TInt                            iPrevFolderIndex;
    TFileName                       iPrevFolderName;
    RTz                             iTz;
    TSearchResults                  iFileSearchResults;
    CDesCArray*                     iTextArray;
    TBool                           iAllowProcessing;
    };


// utility class for waiting for asychronous requests
class CAsyncWaiter : public CActive
	{
public:
	static CAsyncWaiter* NewL( TInt aPriority = EPriorityStandard );
	static CAsyncWaiter* NewLC( TInt aPriority = EPriorityStandard );
	~CAsyncWaiter();
	
	void StartAndWait();
	TInt Result() const;
	
private:
	CAsyncWaiter( TInt aPriority );
	
	// from CActive
	void RunL();
	void DoCancel();
	
private:
    CActiveSchedulerWait iWait;
	TInt iError;
	};
	
	
#endif

// End of File

