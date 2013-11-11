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


#ifndef __CREATORENGINE_H__
#define __CREATORENGINE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32hal.h>
#include <s32file.h>
#include <s32mem.h>
#include <w32std.h>

//#include <aknenv.h>
#include <coeutils.h>
#include <apparc.h>
#include <eikenv.h>
//#include <eikprogi.h>
#include <bautils.h>
#include <commdb.h>
#include <imageconversion.h> //TFrameInfo

#include "creator.hrh"
#include "creator_std.h"
#include "creator_randomdataparser.h"

//class CCreatorAppUi;
class TCommand;
class CCreatorBrowser;
class CCreatorCalendarBase;
class CCreatorNotepad;
class CCreatorLogs;
//class CCreatorAccessPoints;
class CCreatorMailboxes;
class CCreatorIMPS;
class CCreatorFiles;
class CCreatorMessages;
class CCreatorLandmarks;
class MCreatorModuleBase;
class CCreatorPhonebookBase;
class CCreatorModuleBaseParameters;
//class CCreatorConnectionSettingsBase;
class CDictionaryFileStore;
class CImageDecoder;
class CFbsBitmap;
class CBitmapScaler;
class CCommandParser;
class EngineWrapper;
class CCreatorCmdScriptRun;

static const TUid KUidCreatorApp = {0x20011383};

enum TListQueryId
	{
	R_GROUP_CREATION_TYPE_QUERY,
	R_CONTACT_CREATION_TYPE_QUERY,
	R_MESSAGE_TYPE_QUERY,
	R_FOLDER_TYPE_QUERY,
	R_UNREAD_QUERY,
	R_ATTACHMENT_MULTI_SELECTION_QUERY,
	R_AMS_ATTACHMENT_SINGLE_SELECTION_QUERY,
	R_ATTACHMENT_SINGLE_SELECTION_QUERY
	};

/*
 * Interface for UI dialogs
 */
class MUIObserver
    {
public:
    /**
     * Called when some dialog in UI is closed
     *
     * @since S60 10.1
     * @param aPositiveAction ETrue if "Ok", "Yes" or other "positive" button was pressed
     * @param aUserData number that was passed to UI before dialog was opened
     * @return None.
     */
    virtual void QueryDialogClosedL(TBool aPositiveAction, TInt aUserData) = 0;

    };

/*
 * Interface for CCommandParser
 */
class MCommandParserObserver
    {
public:
    /**
     * Called when CCommandParser user has choosen script file
     *
     * @since S60 10.1
     * @param aSuccess ETrue if "Ok", "Yes" or other "positive" button was pressed
     * @param aFileName filename chosen by user
     * @return None.
     */
    virtual void ScriptChosenL(TBool aSuccess, const TDesC& aFileName = KNullDesC) = 0;

    /**
     * Called when CCommandParser user has choosen random data file
     *
     * @since S60 10.1
     * @param aSuccess ETrue if "Ok", "Yes" or other "positive" button was pressed
     * @param aFileName filename chosen by user
     * @return None.
     */
    virtual void RandomDataFileChosenL(TBool aSuccess, const TDesC& aFileName = KNullDesC) = 0;
    };

/**
 * Device memory information
 */
class TMemoryDetails
	{
public:
	
	TBuf<16> iFree;
	TBuf<16> iSize;
	TChar iDriveLetter;

    TBuf<16> iRamFree;
    TBuf<16> iRomFree;
    
	TBool    iENotAvailable;

	TBuf<16> iRamSize;
	TBuf<16> iRomSize;
	};
// Common constant for undefined integer value:
const TInt KUndef = KErrNotFound;

class CCreatorEngine : public CActive, public MUIObserver, public MCommandParserObserver
	{
enum ECreatorEngineState{
    ECreatorEngineDeleteAllEntries,
    ECreatorEngineDeleteAllCreatorEntries,
};
public:
    
    enum TRandomStringType
        {
        EFirstName = 0,
        ESurname,
        ECompany,
        EAddress,
        EJobTitle,
        EPhoneNumber,
        EGroupName,
        EMeetingReason,
        EMeetingPlace,
        EMemoText,
        EAnniversaryReason,
        EToDoText,
        EReminderText,
        EMessageSubject,
        EMessageText,
        ECity,
        ECountry,
        EPostcode,        
        EState,
        EPobox,
        EPrefix,
        ESuffix,
        ELandmarkName,
        ELandmarkDescription
        };


// note! changing order will cause regression in other modules

    enum TTestDataPath
        {
        EJPEG_25kB = 0,
        EJPEG_200kB,
        EJPEG_500kB,
        EPNG_15kB,
        EGIF_2kB,
        ERNG_1kB,
        EMIDI_10kB,
        EWAVE_20kB,
        EAMR_20kB,
        EExcel_15kB,
        EWord_20kB,
        EPowerPoint_40kB,
        EText_10kB,
        EText_70kB,
        E3GPP_70kB,
        EMP3_250kB,
        EAAC_100kB,
        ERM_95kB,
        EBMP_25kB,
        EHTML_20kB,
        EJAD_1kB,
        EJAR_10kB,
        EJP2_65kB,
        EMP4_200kB,
        EMXMF_40kB,
        ERAM_1kB,
        ESVG_15kB,
        ESWF_15kB,
        ETIF_25kB,
        EVCF_1kB,
        EVCS_1kB,
        ESISX_10kB,
        EWMA_50kB,
        EWMV_200kB,
        ESavedDeck_1kB,
        LAST_FILE_ID//When you add new items to enum, add them before last item, so random number generations does not need to be updated also         
        };
        
    enum TRandomDateType
        {
        EDatePast,
        EDateFuture    
        };

public:
    /**
     * Constructor: NewL
     */
    static CCreatorEngine* NewL(EngineWrapper *aEngineWrapper);
    /**
     * Constructor: NewLC
     */
    static CCreatorEngine* NewLC(EngineWrapper *aEngineWrapper);	
    ~CCreatorEngine();
	
    inline const RPointerArray<TMemoryDetails>& GetMemoryDetailsList(){ return iMemoryDetailsList; };
	inline const TMemoryDetails& GetMemoryDetails(){ return iMemoryDetails; };
	inline EngineWrapper* GetEngineWrapper(){ return iEngineWrapper; };
	inline CCreatorPhonebookBase* GetPhonebook(){ return  iPhonebook; };
	
private:
    CCreatorEngine();
    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(EngineWrapper *aEngineWrapper);
	
	void RequestMemoryDetails();
    TBool MMC_OK() const;
    void Init();
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

    void ExecuteCommand();
    void StartEnginesL();
    void CheckForMoreCommandsL();
    
    TBool IsDeleteCommand( TInt aCommand );
    
    TInt ReadRandomDataL(const TRandomStringType aRandomStringType,
    		             const TInt aResourceId,
    		             const TDesC& aFilename,
    		             const CCreatorRandomDataParser::TRandomDataType aRandomDataType);
    void GenerateSourceImageFileL( const TTestDataPath aFileType, const TDesC& aFileName );
    void GenereteSourceTextFileL( const TDesC& aFileName, TInt aSize );

public: // from MUIObserver
    /**
     * Called when some dialog in UI is closed
     *
     * @since S60 10.1
     * @param aPositiveAction ETrue if "Ok", "Yes" or other "positive" button was pressed
     * @param aUserData number that was passed to UI before dialog was opened
     * @return ?description
     */
    virtual void QueryDialogClosedL(TBool aPositiveAction, TInt aUserData);
    
public: // from MCommandParserObserver
    /**
     * Called when CCommandParser user has choosen script file
     *
     * @since S60 10.1
     * @param aSuccess ETrue if "Ok", "Yes" or other "positive" button was pressed
     * @param aFileName filename chosen by user
     * @return None.
     */
    virtual void ScriptChosenL(TBool aSuccess, const TDesC& aFileName = KNullDesC);
    
    /**
     * Called when CCommandParser user has choosen random data file
     *
     * @since S60 10.1
     * @param aSuccess ETrue if "Ok", "Yes" or other "positive" button was pressed
     * @param aFileName filename chosen by user
     * @return None.
     */
    virtual void RandomDataFileChosenL(TBool aSuccess, const TDesC& aFileName = KNullDesC);
    
public:
    void ExecuteOptionsMenuCommandL(TInt aCommand);
    void RunScriptL();
    TInt RunScriptL(const TDesC& aScriptFile);
    void ShutDownEnginesL();
    void ExecuteFirstCommandL(const TDesC& aText);
    
    void AppendToCommandArrayL(TInt aCommand, CCreatorModuleBaseParameters* aParameters, TInt aNumberOfEntries = 1);
    TInt CommandArrayCount();
 
    TPtrC TestDataPathL(enum TTestDataPath aTestDataPath);
    TPtrC RandomSoundFileL(); // Returns temporary path, which will be removed on shutdown 
    void RandomPictureFileL(TDes& aPath);  // aPath temporary path, which will be removed on shutdown
    TPtrC CreateRandomSoundFileL(); // Copies file to new location and returns the new path
    TPtrC CreateSoundFileL(TTestDataPath aId); // Copies file to new location and returns the new path
    
    void CopyFileL(const TFileName& aSourceFile, const TFileName& aTargetFile, TBool aOverwrite = EFalse );
    /*
     * Gets attacment file id based on the file id string
     * @return Attachment file id
     * @param aAttachmentIdStr Attachment file id string
     */
    TInt GetAttachmentIdL( const TDesC& aAttachmentIdStr ) const;    

    TUint32 GetDefaultIAPL();
    TUint32 AccessPointNameToIdL(const TDesC& aAPName, TBool aAnyIfNotFound = EFalse );

    TPtrC RandomString(enum TRandomStringType aRandomStringType);    
    TInt RandomNumber(TInt aRange);
    TInt RandomNumber(TInt aMin, TInt aMax);
    TTime RandomDate(TRandomDateType type, TInt aMinDays = 0, TInt aMaxDays = 360 );
    TTime RandomDate(TTime baseTime, TRandomDateType type, TInt aMinDays = 0, TInt aMaxDays = 360);  
    TTime RandomTime(TTime baseTime, TRandomDateType type, TInt aMinHours=0, TInt aMaxHours=24, TInt aMinMinutes=0, TInt aMaxMinutes=60 );
    CDesCArrayFlat* SoundFilesL();
    CDesCArrayFlat* PictureFilesL();
    
    HBufC* CreateEmailAddressLC();
    HBufC* CreateHTTPUrlLC();
    HBufC* CreateRandomStringLC(TInt aStrLen);
    
    HBufC* CreateEmailAddressLC(
            const TDesC& firstname,
            const TDesC& lastname,
            const TDesC& domain,
            const TDesC& country ) const;
    
    HBufC* CreateUrlLC(
            const TDesC& protocol,
            const TDesC& prefix,
            const TDesC& domain,
            const TDesC& country) const;
    
    void SetDefaultPathForFileCommandL(TInt aCommand, TFileName& aPath);

    TBool GetRandomDataL();
    TBool GetRandomDataFromFileL(const TDesC& aFilename);
    void CancelComplete();
    CDictionaryFileStore* FileStoreLC();
    void ReadEntryIdsFromStoreL( RArray<TInt>& aEntryIds, const TUid aModuleUid );
    void WriteEntryIdsToStoreL( RArray<TInt>& aEntryIds, const TUid aModuleUid );
    void ReadEntryIdsFromStoreL( RArray<TUint32>& aEntryIds, const TUid aModuleUid );
    void WriteEntryIdsToStoreL( RArray<TUint32>& aEntryIds, const TUid aModuleUid );
    void RemoveStoreL( const TUid aModuleUid );
	void ProgressDialogCancelledL();
	
	void SortCommands();
    
private:
    // needed by the engine itself
    RTimer iTimer;
    CEikonEnv* iEnv;
    //CCreatorAppUi* iAppUi;
    TInt iCurrentEntry;
    TInt iFailedCommands;

    CDesCArrayFlat* iSoundFileArray;
    CDesCArrayFlat* iPictureFileArray;
    CCommandArray* iCommandArray;
    RPointerArray<CCreatorModuleBaseParameters> iParameterArray;
    // progress bar
    //CAknProgressDialog* iProgressDialog;
    //CEikProgressInfo* iProgressInfo;
        
    // for the random data generator
    TInt64 iSeed;
    
    class TStringArrayPtr
        {
        public:
        TStringArrayPtr(TRandomStringType aType, CDesCArray* aPtr)
            {
            iStringType = aType;
            iArrayPtr = aPtr;
            };
        TRandomStringType iStringType;
        CDesCArray* iArrayPtr;
        }; 
    
    RArray<TStringArrayPtr> iStringArrays;
    
    // modules
    CCreatorBrowser* iBrowser;
    CCreatorCalendarBase* iCalendar;
    CCreatorPhonebookBase* iPhonebook;
    CCreatorNotepad* iNotepad;
    CCreatorLogs* iLogs;
//    CCreatorConnectionSettingsBase* iAccessPoints;
    CCreatorMailboxes* iMailboxes;
    CCreatorIMPS* iIMPS;
    CCreatorFiles* iFiles;
    CCreatorMessages* iMessages;
    CCreatorLandmarks* iLandmarks;
	CCreatorCmdScriptRun* iCmdScriptRun;
	CCommandParser* iCommandParser;

    // options menu command home module
    MCreatorModuleBase* iUsedOptionsMenuModule;
    
    HBufC* iTempPath;
    
    RPointerArray<HBufC> iAllocatedFilePaths;
    TBool iUserCancelled;
    
    // For generating missing bitmaps
    HBufC8* iBitmapData;
    CFbsBitmap* iBitmap;
    CFbsBitmap* iScaledBitmap;
    CImageDecoder* iDecoder;
    TFrameInfo iFrameinfo;
    CImageEncoder* iEncoder;
    CFrameImageData* iFrameImageData;
    CBitmapScaler* iScaler;

    EngineWrapper* iEngineWrapper; // Enginewrapper that is used for communicating between QT and Symbian
	TMemoryDetails iMemoryDetails;
	
	RPointerArray<TMemoryDetails> iMemoryDetailsList;
	
	TInt iResourceFileId;

	HBufC* iCommandLineScriptName;
	HBufC* iCommandLineRandomDataFileName;
	};

// this class contains a command which will be executed
class TCommand
    {
public:
    TInt iCommandId;
    CCreatorModuleBaseParameters* iParameters;
public:
    TCommand(TInt aCommandId, CCreatorModuleBaseParameters* aParameters);
    ~TCommand();

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
	
	
/*
 * Maps file id string (file name) to TTestDataPath (file id) and TCreatorIds (command id)
 */
// File ids:
	
	_LIT(K3GPP_70KB, "3GPP-70kB");
	_LIT(KAAC_100KB, "AAC-100kB");
	_LIT(KAMR_20KB, "AMR-20kB");
	_LIT(KXLS_15KB, "XLS-15kB");
	_LIT(KGIF_2KB, "GIF-2kB");
	_LIT(KJPEG_200KB, "JPEG-200kB");
	_LIT(KJPEG_25KB, "JPEG-25kB");
	_LIT(KJPEG_500KB, "JPEG-500kB");
	_LIT(KMIDI_10KB, "MIDI-10kB");
	_LIT(KMP3_250KB, "MP3-250kB");
	_LIT(KPNG_15KB, "PNG-15kB");
	_LIT(KPPT_40KB, "PPT-40kB");
	_LIT(KRM_95KB, "RM-95kB");
	_LIT(KRNG_1KB, "RNG-1kB");
	_LIT(KTXT_10KB, "TXT-10kB");
	_LIT(KTXT_70KB, "TXT-70kB");
	_LIT(KWAV_20KB, "WAV-20kB");
	_LIT(KDOC_20KB, "DOC-20kB");
	_LIT(KSWF_15KB, "SWF-15kB");
	_LIT(KJAD_1KB, "JAD-1kB");
	_LIT(KJAR_10KB, "JAR-10kB");
	_LIT(KTIF_25KB, "TIF-25kB");
	_LIT(KMXMF_40KB, "MXMF-40kB");
	_LIT(KBMP_25KB, "BMP-25kB");
	_LIT(KJP2_25KB, "JP2-25kB");
	_LIT(KSVG_15KB, "SVG-15kB");
	_LIT(KHTML_20KB, "HTML-20kB");
	_LIT(KVCF_1KB, "VCF-1kB");
	_LIT(KVCS_1KB, "VCS-1kB");
	_LIT(KMP4_200KB, "MP4-200kB");
	_LIT(KSISX_10KB, "SISX-10kB");
	_LIT(KRAM_1KB, "RAM-1kB");
    _LIT(KDECK_1KB, "DECK-1kB");
    _LIT(KWMA_50KB, "WMA-50kB");
    _LIT(KWMV_200KB, "WMV-200kB");
	
class FileMapping
    {
	public:
	FileMapping(const TDesC& aFileName, TInt aFileId, TInt aCommandId );
	const TDesC& FileName() const;
	TInt FileId() const;
	TInt CommandId() const;

	private:
	const TDesC& iFileName;
	TInt  iFileId;
	TInt  iCommandId;
	};
	
	/*
	 * This table contains the string <-> TTestDataPath mappings for the file types
	 */
	static FileMapping FileMap[] = 
	    {
      
	    FileMapping(K3GPP_70KB, CCreatorEngine::E3GPP_70kB, ECmdCreateFileEntry3GPP_70kB),
	    FileMapping(KAAC_100KB, CCreatorEngine::EAAC_100kB, ECmdCreateFileEntryAAC_100kB),
	    FileMapping(KAMR_20KB, CCreatorEngine::EAMR_20kB, ECmdCreateFileEntryAMR_20kB),
	    FileMapping(KXLS_15KB, CCreatorEngine::EExcel_15kB, ECmdCreateFileEntryXLS_15kB),
	    FileMapping(KGIF_2KB, CCreatorEngine::EGIF_2kB, ECmdCreateFileEntryGIF_2kB),
	    FileMapping(KJPEG_200KB, CCreatorEngine::EJPEG_200kB, ECmdCreateFileEntryJPEG_200kB),
	    FileMapping(KJPEG_25KB, CCreatorEngine::EJPEG_25kB, ECmdCreateFileEntryJPEG_25kB),
	    FileMapping(KJPEG_500KB, CCreatorEngine::EJPEG_500kB, ECmdCreateFileEntryJPEG_500kB),
	    FileMapping(KMIDI_10KB, CCreatorEngine::EMIDI_10kB, ECmdCreateFileEntryMIDI_10kB),
	    FileMapping(KMP3_250KB, CCreatorEngine::EMP3_250kB, ECmdCreateFileEntryMP3_250kB),
	    FileMapping(KPNG_15KB, CCreatorEngine::EPNG_15kB, ECmdCreateFileEntryPNG_15kB),
	    FileMapping(KPPT_40KB, CCreatorEngine::EPowerPoint_40kB, ECmdCreateFileEntryPPT_40kB),
	    FileMapping(KRM_95KB, CCreatorEngine::ERM_95kB, ECmdCreateFileEntryRM_95kB),
	    FileMapping(KRNG_1KB, CCreatorEngine::ERNG_1kB, ECmdCreateFileEntryRNG_1kB),
	    FileMapping(KTXT_10KB, CCreatorEngine::EText_10kB, ECmdCreateFileEntryTXT_10kB),
	    FileMapping(KTXT_70KB, CCreatorEngine::EText_70kB, ECmdCreateFileEntryTXT_70kB),
	    FileMapping(KWAV_20KB, CCreatorEngine::EWAVE_20kB, ECmdCreateFileEntryWAV_20kB),
	    FileMapping(KDOC_20KB, CCreatorEngine::EWord_20kB, ECmdCreateFileEntryDOC_20kB),
	    FileMapping(KBMP_25KB, CCreatorEngine::EBMP_25kB, ECmdCreateFileEntryBMP_25kB),
	    FileMapping(KDECK_1KB, CCreatorEngine::ESavedDeck_1kB, ECmdCreateFileEntryDeck_1kB),
	    FileMapping(KHTML_20KB, CCreatorEngine::EHTML_20kB, ECmdCreateFileEntryHTML_20kB),
	    FileMapping(KJAD_1KB, CCreatorEngine::EJAD_1kB, ECmdCreateFileEntryJAD_1kB),
	    FileMapping(KJAR_10KB, CCreatorEngine::EJAR_10kB, ECmdCreateFileEntryJAR_10kB),
	    FileMapping(KJP2_25KB, CCreatorEngine::EJP2_65kB, ECmdCreateFileEntryJP2_65kB),
	    FileMapping(KMP4_200KB, CCreatorEngine::EMP4_200kB, ECmdCreateFileEntryMP4_200kB),
	    FileMapping(KMXMF_40KB, CCreatorEngine::EMXMF_40kB, ECmdCreateFileEntryMXMF_40kB),
	    FileMapping(KRAM_1KB, CCreatorEngine::ERAM_1kB, ECmdCreateFileEntryRAM_1kB),
	    FileMapping(KSVG_15KB, CCreatorEngine::ESVG_15kB, ECmdCreateFileEntrySVG_15kB),
	    FileMapping(KSWF_15KB, CCreatorEngine::ESWF_15kB, ECmdCreateFileEntrySWF_15kB),
	    FileMapping(KTIF_25KB, CCreatorEngine::ETIF_25kB, ECmdCreateFileEntryTIF_25kB),
	    FileMapping(KVCF_1KB, CCreatorEngine::EVCF_1kB, ECmdCreateFileEntryVCF_1kB),
	    FileMapping(KVCS_1KB, CCreatorEngine::EVCS_1kB, ECmdCreateFileEntryVCS_1kB),
	    FileMapping(KSISX_10KB, CCreatorEngine::ESISX_10kB, ECmdCreateFileEntrySISX_10kB),
	    FileMapping(KWMA_50KB, CCreatorEngine::EWMA_50kB, ECmdCreateFileEntryWMA_50kB),
	    FileMapping(KWMV_200KB, CCreatorEngine::EWMV_200kB, ECmdCreateFileEntryWMV_200kB)
	    };

#endif // __CREATORENGINE_H__

