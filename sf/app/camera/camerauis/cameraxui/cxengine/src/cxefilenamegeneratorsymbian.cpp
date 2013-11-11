/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <pathinfo.h>
#include <pathconfiguration.hrh>
#include <coemain.h> // For CCoeEnv
#include <QDate>

#include "cxefilenamegeneratorsymbian.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxutils.h"
#include "cxesysutil.h"
#include "cxesettings.h"
#include "cxeerror.h"
#include "cxeerrormappingsymbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxefilenamegeneratorsymbianTraces.h"
#endif


using namespace Cxe;

// ===========================================================================
// Local constants

static const int KMultipleFolderNumberChars = 2;
static const int KMaxMonthFolders           = 260;
static const int KBase10                    = 10;
static const int KMaxFilesPerFolder         = 100;

const char* CAMERA_FOLDER   = "Camera";
const char* FOLDER_SEPARATOR   = "\\";
const char* WILDCARD_CHARACTER = "?";

const char CHAR_OFFSET = 'A';
const char DIGIT_OFFSET = '0';
const char MAX_CHAR = 'Z';

const TInt64 KMinRequiredSpaceImage = 2000000;

// Whether there's enough space for video or not is handled lower in the SW stack
// so this is set to 0 to fix an error
const TInt64 KMinRequiredSpaceVideo = 0;



// ===========================================================================
// Local functions

/**
* Convert QString to TPtrC16.
*/
inline TPtrC16 convertToTPtrC16(const QString& string)
{
    return TPtrC16(reinterpret_cast<const TUint16*>(string.utf16()));
}


// ===========================================================================
// Class functions

/**
 * Constructor
 */
CxeFilenameGeneratorSymbian::CxeFilenameGeneratorSymbian(CxeSettings &settings, CameraMode mode)
       : mFs(CCoeEnv::Static()->FsSession()),
         mSettings(settings),
         mCurrentMode(mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEFILENAMEGENERATOR_1, "msg: e_CX_FILENAMEGENERATOR_NEW 1");

    // Set default values (used in case of error retrieving values)
    mCurrentMonth = "";
    mMonthCounterImage = -1;
    mMonthCounterVideo = -1;
    mImageCounter = 0;
    mVideoCounter = 0;

    // Retrieve last used counter values from settings
    mCurrentMonth = mSettings.get<QString>(CxeSettingIds::FNAME_MONTH_FOLDER, "");
    mImageCounter = mSettings.get<int>(CxeSettingIds::FNAME_IMAGE_COUNTER, 0);
    mVideoCounter = mSettings.get<int>(CxeSettingIds::FNAME_VIDEO_COUNTER, 0);

    OstTrace0(camerax_performance, CXEFILENAMEGENERATOR_2, "msg: e_CX_FILENAMEGENERATOR_NEW 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Destructor
 */
CxeFilenameGeneratorSymbian::~CxeFilenameGeneratorSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * This must be called for every burst capture.
 */
void CxeFilenameGeneratorSymbian::startNewImageFilenameSequence()
{
    CX_DEBUG_ENTER_FUNCTION();

    /*  NOTES
    When the client calls this function, we should start a new "sequence" of filenames...
    What I mean here is that for burst capture, we have filename sequences like...
        Filename1
        Filename1(002)
        Filename1(003)
        and so on...

    So this function says "Ok, I'm starting a new capture sequence here... so reset your variables"
    This should be done below...
    Development efforts need to be aware of Edrive vs. Cdrive, and also multiple drive support.
    */

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Generates the next file name in the sequence.
 * @param   filename: A QString reference to hold the filename
 * @return  error id, Id::None if no error
 */
CxeError::Id CxeFilenameGeneratorSymbian::nextImageFilenameInSequence(QString &qfName, const QString &fileExt)
{
    CX_DEBUG_IN_FUNCTION();

    /** @todo: When the client calls this function, the next filename in the
     * sequence is retrieved. The entire path should be included... e.g
     * C//PHOTOS//SOMETHING//HELLO//Filename001.jpg
     */
    return generateFilename(qfName, fileExt);
}

/**
 * Generates image/video file name depending on the current active mode.
 * @param   filename: A QString reference to hold the filename
 * @return  error id, CxeError::None if no error.
 */
CxeError::Id CxeFilenameGeneratorSymbian::generateFilename(QString &qfName, const QString &fileExt)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_GENERATENAME_1, "msg: e_CX_GENERATE_FILENAME 1");

    // Make sure that the path for images/videos exists
    QString path;
    int err = selectFolder(path);

    // Continue generating filename, if no errors encountered yet
    if (!err) {
        // Generate and append the file name to construct the full path
        QString fileName;
        err = generateUniqueFileName(path, fileName, fileExt);
        if (!err)  {
            qfName = path + fileName;
        }
    }

    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_GENERATENAME_2, "msg: e_CX_GENERATE_FILENAME 0");
    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}

/**
 * Generates a unique file name in the given path.
 * @param filePath          Path for the file (reference)
 * @param fileName          Name of the file (reference)
 * @return                  Symbian error code (used internally)
 */
int CxeFilenameGeneratorSymbian::generateUniqueFileName(QString &filePath, QString &fileName, const QString &ext)
{
    CX_DEBUG_ENTER_FUNCTION();

    // At this point the path must exist.
    // Remote possibility / security threat that it doesn't exist or is not a folder.
    TEntry entry;
    int ferr = mFs.Entry(convertToTPtrC16(filePath), entry);
    if (ferr != KErrNone || !entry.IsDir()) {
        return ferr;
    }

    // Initialize counters and file extension
    int *fileCounter = &mImageCounter;

    if (mCurrentMode==VideoMode) {
        fileCounter  = &mVideoCounter;
    }

    QString fnFormat("%1%2%3");
    QString number;

    // get the file name suffix
    QString baseFileName;
    mSettings.get(CxeSettingIds::FNAME_FOLDER_SUFFIX, baseFileName);

    while (true) {
        // Generate new name
        number = number.sprintf("%04d", *fileCounter);
        fileName = fnFormat.arg(number, baseFileName, ext);

        CX_DEBUG(("Checking filename [%s]", fileName.toAscii().constData()));

        // Check if it exists
        ferr = checkExistence(filePath + fileName);
        if (ferr == KErrNone) {
            // Filename has already been used in this location, try the next number
            CX_DEBUG(("File exists already"));
            (*fileCounter)++;
        } else if (ferr == KErrNotFound) {
            // No file with this name, use it.
            CX_DEBUG(("Filename free, using it"));
            ferr = KErrNone;
            break;
        } else {
            // Filename could not be reserved. Log actual error and report as general error.
            CX_DEBUG(("[WARNING] Error %d generating filename!", ferr));
            ferr = KErrGeneral;
            break;
        }
    }

    // Increment counter if image mode. Video counter is incremented when
    // video has been actually recorded
    if (mCurrentMode == ImageMode) {
        raiseCounterValue();
    }

    CX_DEBUG_EXIT_FUNCTION();
    return ferr;
}

/**
 * Computes the suffix to be used in destination folder name.
 */
void CxeFilenameGeneratorSymbian::computeFolderSuffix(int folderNumber, QString &suffix)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Clear any old content.
    suffix = "";

    // Example:
    // if folderNumber is 15
    // 15/10 = 1 (int)
    // A + 1  = B
    suffix += (CHAR_OFFSET + folderNumber/KBase10);
    // 15%10 = 5
    // 0 + 5 = 5
    // => suffix = B5
    suffix += (DIGIT_OFFSET + folderNumber%KBase10);
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Compute the month counter value (0 to 259 => A0 to Z9).
 * @param
 * @return   Symbian error code (internal to class)
 */
int CxeFilenameGeneratorSymbian::computeMonthCounter(QString &path, QString& monthFolder, int &monthCounter)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool done = false;

    // Query the month folder: path\YYYYMM??
    QString queryPath = path + monthFolder;
    for (int charCount = 0; charCount < KMultipleFolderNumberChars; charCount++) {
        queryPath += WILDCARD_CHARACTER;
    }
    CX_DEBUG(("Listing folders with wildcards: [%s]", queryPath.toAscii().constData()));

    // Get a list of folders for this month, sorted in descending alphabetical order.
    // The first entry should be the latest used folder.
    CDir* dirList;
    int err = mFs.GetDir(convertToTPtrC16(queryPath),
                         KEntryAttMatchExclusive|KEntryAttDir,
                         ESortByName|EDescending,
                         dirList);

    // On errors (or empty list of folders), let's default to zero.
    // Errors here should be recoverable when we start to
    // count files within current directory.
    monthCounter = 0;

    // If directory listing worked fine, then figure out the last month folder number
    if (err == KErrNone) {
        int monthFolderCount = dirList->Count();
        CX_DEBUG(("Matching folder count: %d", monthFolderCount));
        int index = 0;
        // Look through the list of folders in the month for the highest numbered folder
        // with the format YYYYMMAX Where YYYY is the year MM is the month A is an alphabetical
        // character in the range a-z or A-Z and X is a digit 0-9
        while ( index < monthFolderCount && !done ) {
            // The list is sorted in descending order. Get the last 2 characters from
            // the first directory in the list these indicate the highest folder number
            TPtrC name = ( *dirList )[index].iName;
            int nameLength = name.Length();

            // Check the first character is in the range a-z or A-Z
            // and the second character is in the range 0-9
            TChar firstChar = name[nameLength - KMultipleFolderNumberChars];
            firstChar.UpperCase();
            TChar secondChar = name[nameLength - 1];
            int secondCharVal = secondChar.GetNumericValue();

            // If 1st character is NOT in the range(A-Z) then disregard this folder
            if (firstChar < CHAR_OFFSET || firstChar > MAX_CHAR) {
                done = false;
            }
            // If 2nd character is NOT in the range(0-9) then disregard this folder
            else if (secondCharVal < 0 || secondCharVal > KBase10-1) {
                done = false;
            }
            else {
                done = true;
            }

            // If no problems were encountered, then calculate the folder number
            if (done) {
                // 10's part of folder number is represented by characters A-Z
                // convert the character into a decimal value (*10) and add on the units.
                monthCounter = ((int)firstChar-(int)CHAR_OFFSET)*KBase10 + secondCharVal;
                CX_DEBUG(("Calculated month counter value: %d", monthCounter));
                // Make sure that month counter doesn't exceed Z9
                if (monthCounter >= KMaxMonthFolders) {
                    monthCounter = KMaxMonthFolders - 1;
                }
            }
            index++;
        }
    } else {
        CX_DEBUG(("[WARNING] Error %d listing month folders!", err));
    }

    delete dirList;
    CX_DEBUG_EXIT_FUNCTION();
    return err;
}

/**
 * Checks that the month folder exists: creates the folder if required.
 * Returns error code if the folder can't be created.
 */
int CxeFilenameGeneratorSymbian::selectFolder(QString &suggestedPath)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_SELECTFOLDER_1, "msg: e_CX_SELECT_FOLDER 1");

    // Compose the path string and select counter based on mode.
    QString basePath = "%1%2\\";
    int *monthCounter = &mMonthCounterImage;

    if (ImageMode == mCurrentMode) {
        basePath = basePath.arg(mImagesPath, mCurrentMonth);
    }
    else { // VideoMode
        basePath = basePath.arg(mVideosPath, mCurrentMonth);
        monthCounter = &mMonthCounterVideo;
    }
    CX_DEBUG(("Base path [%s]", basePath.toAscii().constData()));

    QString suffix;
    QString newPath;
    int status(KErrNone);
    bool created(false);

    while (true) {
        // Construct the complete path
        computeFolderSuffix(*monthCounter, suffix);
        newPath = basePath + mCurrentMonth + suffix + FOLDER_SEPARATOR;

        // Check if the folder exists, and create if not.
        CX_DEBUG(("Checking new path [%s] ..", newPath.toAscii().constData()));
        status = ensureExists(newPath, created);

        // Check if the folder can be used.
        if (status == KErrNone) {
            // If the folder was newly created, it's empty and can be used.
            // If the folder is last available month folder,
            // we need to use it even if it has "too many" files.
            if (created || *monthCounter >= (KMaxMonthFolders-1)) {
                CX_DEBUG(("Newly created folder, or the last month folder available, using it"));
                suggestedPath = newPath;
                break;
            } else {
                // For other folders, if the folder already exists then make sure
                // that it doesn't contain maximum number of files already.
                CX_DEBUG(("Folder already existed, check the amount of files.."));
                CDir* fileList;
                status = mFs.GetDir(convertToTPtrC16(newPath),
                                    KEntryAttMaskSupported,
                                    ESortNone,
                                    fileList);
                CX_DEBUG(("Folder contains %d files", fileList->Count()));

                bool spaceRemaining(fileList->Count() < KMaxFilesPerFolder);

                delete fileList;
                fileList = NULL;

                if (status == KErrNone && spaceRemaining) {
                    // Not too many files on the folder, use it.
                    CX_DEBUG(("File count acceptable, using this folder"));
                    suggestedPath = newPath;
                    break;
                } else {
                    // Need to check the next folder, this one is full,
                    // or we had error listing it's contents.
                    CX_DEBUG(("Folder full, continue search"));
                    (*monthCounter)++;
                }
            }
        } else {
            // Unknown error encountered.
            // LOG the error and use the basePath!
            // Base path's existence has been checked when init was called.
            CX_DEBUG(("[WARNING] - error %d encountered. Using base path as fallback.", status));
            suggestedPath = basePath;
            break;
        }
    }

    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_SELECTFOLDER_2, "msg: e_CX_SELECT_FOLDER 0");
    CX_DEBUG_EXIT_FUNCTION();

    // We fallback to basePath in case of unknown errors,
    // so no error will be reported here.
    return KErrNone;
}



/**
 * Initializes the month folders.
 * @return Symbian error code (internal to class)
 */
int CxeFilenameGeneratorSymbian::initMonthFolders()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_INITFOLDERS_1, "msg: e_CX_INIT_MONTH_FOLDER 1");

    // Month folder: YYYYMM, with suffix: YYYYMMXX
    QString monthFolder = QDate::currentDate().toString("yyyyMM");

    // If the month folder name is different from the last used month folder name
    // this indicates that a new month has been started.
    if ( monthFolder.compare(mCurrentMonth) != 0 ) {
        resetCounters(monthFolder);
    }

    // Check/create that the month folder exists
    int *monthCounter = &mMonthCounterImage;
    QString path = "%1%2\\";
    if (ImageMode==mCurrentMode) {
        path = path.arg(mImagesPath, monthFolder);
    }
    else {
        path = path.arg(mVideosPath, monthFolder);
        monthCounter = &mMonthCounterVideo;
    }

    CX_DEBUG(("Check if exists, create if not [%s]", path.toAscii().constData()));
    bool created(false);
    int status(ensureExists(path, created));

    if (status == KErrNone) {
        if (created) {
            // New folder, new month -> counter starts from zero.
            *monthCounter = 0;
        } else {
            // If the month counter is un-initialised it needs to be set up.
            if (*monthCounter < 0) {
                status = computeMonthCounter(path, monthFolder, *monthCounter);
                if (status != KErrNone) {
                    CX_DEBUG(("[WARNING] - Error setting month counter: %d", status));
                    *monthCounter = 0;
                }
            }
        }
    } else {
        //! @todo: Review error handling
        // Report error - can't continue without 'camera' and camera\YYYYMM folders
        CX_DEBUG(("[FATAL] - Could not create month folder, error %d", status));
    }

    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_INITFOLDERS_2, "msg: e_CX_INIT_MONTH_FOLDER 0");
    CX_DEBUG_EXIT_FUNCTION();
    return status;
}

/**
 * Checks if the passed amount of disk space is available on the passed drive.
 * @return   true if space available, false otherwise
 */
bool CxeFilenameGeneratorSymbian::spaceAvailable(int &driveIndex, TInt64 minSpaceInBytes)
{
    CX_DEBUG_ENTER_FUNCTION();

    TVolumeInfo vInfo;

    // Get the drive/volume details
    int status(KErrNone);
    driveIndex = CxeSysUtil::getCameraDrive(mFs);
    if (driveIndex >= 0) {
        status = mFs.Volume(vInfo, driveIndex);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return (status == KErrNone) ? vInfo.iFree >= minSpaceInBytes : false;

}

/**
 * Selects the drive to use based on preference and availability.
 * @param   index of the drive to be used (reference)
 * @return  Symbian error code (internal to class)
 */
int CxeFilenameGeneratorSymbian::selectDrive(int &drive)
{
    CX_DEBUG_ENTER_FUNCTION();
    int err = KErrNone;
    TInt64 minDiskSpace = KMinRequiredSpaceImage;

	if (Cxe::VideoMode == mCurrentMode) {
	    minDiskSpace = KMinRequiredSpaceVideo;
    }

    // Check the available space.
    // Drive index is set here also.
    if ( !spaceAvailable(drive, minDiskSpace)) {
        // All drives are full or inaccessible
        err = KErrDiskFull;
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}

/**
 * Initializes the value of base path for the given mode and drive.
 */
void CxeFilenameGeneratorSymbian::initBasePath(QString &path, int drive)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Get the root path for the given drive.
    TFileName tPath;
    PathInfo::GetRootPath(tPath, drive);

    if (VideoMode == mCurrentMode) {
        tPath.Append( PathInfo::VideosPath() );
    }
    else {
        tPath.Append( PathInfo::ImagesPath() );
    }

    //! @todo: Fetch localized Camera folder name, if/when applicable.
    path = QString::fromUtf16(tPath.Ptr(), tPath.Length());
    path = path + CAMERA_FOLDER + FOLDER_SEPARATOR;

    CX_DEBUG(("Path: %s", path.toAscii().constData()));
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Resets the image and video counters used for file name generation.
 */
void CxeFilenameGeneratorSymbian::resetCounters(QString &monthFolder)
{
    CX_DEBUG_ENTER_FUNCTION();

    mCurrentMonth = monthFolder;
    mMonthCounterImage = 0;
    mMonthCounterVideo = 0;

    // Save the setting values
    // Errors (if any) encountered here are not handled, since these indicate
    // that settings are'nt saved. If settings are not fetched next time,
    // then default settings are used.
    mSettings.set(CxeSettingIds::FNAME_MONTH_FOLDER, mCurrentMonth);

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Initializes the base folder names and counters
 * NB: Video mode init requires file name. So this should be called
 * before init is called for image/video capture controls.
 * Initializes only for appropriate mode (image/video).
 */
CxeError::Id CxeFilenameGeneratorSymbian::init(Cxe::CameraMode mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_INIT_1, "msg: e_CX_FILENAMEGENERATOR_INIT 1");

    mCurrentMode = mode;
    int err = KErrNone;
    bool initialized = (mode==ImageMode && !mImagesPath.isEmpty()) ||
                       (mode==VideoMode && !mVideosPath.isEmpty());

    if (!initialized) {
        // Select a drive based on available disk space
        int drive;
        err = selectDrive(drive);
        if (!err) {
            if (mode == ImageMode) {
                initBasePath(mImagesPath, drive);
            }
            else {
                initBasePath(mVideosPath, drive);
            }

            // Initialize the month folders and counters so that
            // file names can be generated quickly when requested.
            err = initMonthFolders();
        }
    }

    OstTrace0(camerax_performance, CXEFILENAMEGENERATORSYMBIAN_INIT_2, "msg: e_CX_FILENAMEGENERATOR_INIT 0");
    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}

/**
 * Raises file name counter value by one
 */
void CxeFilenameGeneratorSymbian::raiseCounterValue()
{
    if (VideoMode==mCurrentMode) {
        mSettings.set(CxeSettingIds::FNAME_VIDEO_COUNTER, ++mVideoCounter);
    }
    else {
        mSettings.set(CxeSettingIds::FNAME_IMAGE_COUNTER, ++mImageCounter);
    }
}

/**
 * Check if the given file or directory exists.
 * @return KErrNone, if given file/directory exists, KErrNotFound if not.
 *         Other Symbian error code on "real" errors.
 */
int CxeFilenameGeneratorSymbian::checkExistence(const QString& path)
{
    CX_DEBUG_ENTER_FUNCTION();

    unsigned int placeHolder(0);
    int status(mFs.Att(convertToTPtrC16(path), placeHolder));

    CX_DEBUG_EXIT_FUNCTION();
    return status;
}


/**
 * Check if directory exists and create if not.
 * @param created When returned, holds if the folder needed to be created or not.
 * @return KErrNone, if folder existed or was created successfully.
 *         Other Symbian error code if there were errors.
 */
int CxeFilenameGeneratorSymbian::ensureExists(const QString& path, bool& created)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Default to "not created" if errors are encountered.
    created = false;

    unsigned int placeHolder(0);
    TPtrC16 pathDesc(convertToTPtrC16(path));
    CX_DEBUG_SYMBIAN((_L("Checking [%S]"), &pathDesc));

    // Check if it already exists
    CX_DEBUG(("CxeFilenameGeneratorSymbian::ensureExists - check existence.."));
    int status = mFs.Att(pathDesc, placeHolder);
    CX_DEBUG(("CxeFilenameGeneratorSymbian::ensureExists - ..check existence"));

    if( status == KErrPathNotFound || status == KErrNotFound ) {
        // Create path if doesn't exist yet.
        CX_DEBUG(("CxeFilenameGeneratorSymbian::ensureExists - create path.."));
        status = mFs.MkDirAll(pathDesc);
        CX_DEBUG(("CxeFilenameGeneratorSymbian::ensureExists - ..create path"));
        // Set "created" flag only if successfull
        created = (status == KErrNone);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return status;
}


// end of file
