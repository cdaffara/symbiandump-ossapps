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

#include "enginewrapper.h"
#include "engine.h"
#include "FBFileUtils.h"
#include "notifications.h"
#include "fbfileview.h"
#include "fbsearchview.h"
#include "filebrowsersettings.h"
//#include "fbsettingsview.h"

#include <HbProgressDialog>

#include <QString>
#include <QFileInfo>
#include <QModelIndex>

// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper()
    : mEngine(0),
    mFilesFound(),
    mSettings(0),
    mProgressDialog(0),
    mWaitDialog(0)
{
}

// ---------------------------------------------------------------------------

EngineWrapper::~EngineWrapper()
{
    if(mEngine != NULL) {
        TRAP_IGNORE(mEngine->DeActivateEngineL());
        delete mEngine;
    } 
    if (mProgressDialog)
        delete mProgressDialog;

    if (mWaitDialog)
        delete mWaitDialog;
}

// ---------------------------------------------------------------------------

bool EngineWrapper::init()
{
    TRAPD(err, mEngine = CEngine::NewL(this));
    if (err != KErrNone) {
        return false;
    } else {
        TRAP_IGNORE(mEngine->ActivateEngineL());
        mSettings = FileBrowserSettings(&mEngine->Settings());
        return true;
    }
}

// ---------------------------------------------------------------------------
// Functions that are called from UI
// ---------------------------------------------------------------------------
bool EngineWrapper::searchFiles()
{
    TRAPD(err, mEngine->SearchL() );
    if(err != KErrNone) {
        return false;
    }
    else {
        return true;
    }
}

// ---------------------------------------------------------------------------
SearchAttributes EngineWrapper::getFileSearchAttributes()
{
    TSearchAttributes tAttributes = mEngine->GetSearchAttributes();
    SearchAttributes attributes;

    // Convert TSearchAttributes to SearchAttributes 
    attributes.mSearchDir  = QString((QChar*)tAttributes.iSearchDir.Ptr(),tAttributes.iSearchDir.Length());
    attributes.mWildCards  = QString((QChar*)tAttributes.iWildCards.Ptr(),tAttributes.iWildCards.Length()); 
    attributes.mTextInFile = QString((QChar*)tAttributes.iTextInFile.Ptr(),tAttributes.iTextInFile.Length()); 
    attributes.mMinSize    = tAttributes.iMinSize;
    attributes.mMaxSize    = tAttributes.iMaxSize;
    attributes.mRecurse    = tAttributes.iRecurse;
    attributes.mDefaultWildCard = tAttributes.iDefaultWildCard;
    
    // TTime to QDate
    TBuf<20> timeString;
    _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    TRAP_IGNORE( tAttributes.iMinDate.FormatL(timeString, KDateString) );
    QString temp = QString::fromUtf16(timeString.Ptr(), timeString.Length());
    temp.replace(QChar('/'), QChar('-'));
    attributes.mMinDate = QDate::fromString(temp, "dd-MM-yyyy");
    
    
    TRAP_IGNORE( tAttributes.iMaxDate.FormatL(timeString, KDateString) );
    temp = QString::fromUtf16(timeString.Ptr(), timeString.Length());
    temp.replace(QChar('/'), QChar('-'));
    attributes.mMaxDate = QDate::fromString(temp, "dd-MM-yyyy");
    
    return attributes;

}

// ---------------------------------------------------------------------------
int EngineWrapper::setFileSearchAttributes(SearchAttributes attributes)
{
    TSearchAttributes tAttributes;
    // Convert SearchAttributes to TSearchAttributes 
    
    //convert QString to TFilename:
    tAttributes.iSearchDir = TFileName(attributes.mSearchDir.utf16());
    tAttributes.iWildCards = TFileName(attributes.mWildCards.utf16());
    tAttributes.iTextInFile = TFileName(attributes.mTextInFile.utf16());
    
    tAttributes.iMinSize    = attributes.mMinSize;
    tAttributes.iMaxSize    = attributes.mMaxSize;    
    tAttributes.iRecurse    = attributes.mRecurse;
    tAttributes.iDefaultWildCard = attributes.mDefaultWildCard;
    
    // QDate to TTime for both min- and max Date
    QString temp = attributes.mMinDate.toString("yyyy-MM-dd");
    QStringList dateList = temp.split("-");
    int month = dateList[1].toInt() - 1;
    int day = dateList[2].toInt() - 1;
    temp = dateList[0];
    if (month == 0) {
        temp.append("00");
    }
    else {
        temp.append(QString::number(month));
    }
    if (day == 0) {
        temp.append("00");
    }
    else { 
        temp.append(QString::number(day));
    }
    temp.append(":");
        
    TBuf<24> dateString(temp.utf16());
    tAttributes.iMinDate.Set(dateString);
    
    temp = attributes.mMaxDate.toString("yyyy-MM-dd");
    dateList = temp.split("-");
    month = dateList[1].toInt() - 1;
    day = dateList[2].toInt() - 1;
    temp = dateList[0];
    if (month == 0) {
        temp.append("00");
    }
    else {
        temp.append(QString::number(month));
    }
    if (day == 0) {
        temp.append("00");
    }
    else { 
        temp.append(QString::number(day));
    }
    temp.append(":");
    dateString.Copy(temp.utf16());
    tAttributes.iMaxDate.Set(dateString);
    
    mEngine->ChangeAttributes(tAttributes);
    return KErrNone;
}


// ---------------------------------------------------------------------------
SearchResults EngineWrapper::getSearchResults()
{
    TSearchResults tResults = mEngine->SearchResults();
    SearchResults results;
    results.mNumberOfFoundFiles = tResults.iNumberOfFoundFiles;
    CFileEntryList* foundFilesResult = mEngine->FoundFiles();
    if (!mFilesFound.isEmpty()) {
        mFilesFound.clear();
        }
    // copy file names and convert them from TFileName format to QStringList items type
    for (int i = 0; i < foundFilesResult->Count(); i++) {
            mFilesFound.append(
                    QString((QChar*)foundFilesResult->At(i).iFullName.Ptr(), 
                    foundFilesResult->At(i).iFullName.Length()) 
                    );
        }
    results.mFoundFilesList = &mFilesFound;
    return results;
}

// ---------------------------------------------------------------------------

void EngineWrapper::saveSettings(bool aNotifyModules)
{
    return mEngine->SaveSettingsL(aNotifyModules);;
}

// ---------------------------------------------------------------------------

bool EngineWrapper::isDriveListViewActive()
{
    // TODO check return value
    if (mEngine->FileUtils() && mEngine->FileUtils()->IsDriveListViewActive()) {
        return true;
    } else {
        return false;
    }
}

bool EngineWrapper::isCurrentDriveReadOnly()
{
    // TODO check return value
    if (mEngine->FileUtils() && mEngine->FileUtils()->IsCurrentDriveReadOnly()) {
        return true;
    } else {
        return false;
    }
}

bool EngineWrapper::isClipBoardListInUse()
{
    if (mEngine->FileUtils() && mEngine->FileUtils()->ClipBoardList() && mEngine->FileUtils()->ClipBoardList()->Count() != 0)
        return true;
    else
        return false;
}

// ---------------------------------------------------------------------------
void EngineWrapper::startExecutingCommands(const QString &aCommandsExecutionMessage)
{
    TPtrC commandsExecutionMessage(reinterpret_cast<const TText*>(aCommandsExecutionMessage.constData()));
    TRAPD(err, mEngine->FileUtils()->StartExecutingCommandsL(commandsExecutionMessage) );
    Q_UNUSED(err); //TODO
}

// ---------------------------------------------------------------------------

void EngineWrapper::refreshView()
{
    if (mEngine->FileUtils()) {
        TRAPD(err, mEngine->FileUtils()->RefreshViewL() );
        Q_UNUSED(err); //TODO
    }
}

void EngineWrapper::moveUpOneLevel()
{
    if (mEngine->FileUtils()) {
        TRAPD(err, mEngine->FileUtils()->MoveUpOneLevelL() );
        Q_UNUSED(err); //TODO
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::moveDownToDirectory(const QModelIndex& aIndex)
{
    if (mEngine->FileUtils()) {
        TRAPD(err, mEngine->FileUtils()->MoveDownToDirectoryL(aIndex.row()) );
        Q_UNUSED(err); //TODO
    }
}

// ---------------------------------------------------------------------------

int EngineWrapper::clipboardCut(const QModelIndexList& aSelectionIndices)
{
    TInt operations = 0;
    const CArrayFix<TInt> *selectionIndexes = convertSelectionList(aSelectionIndices);
    if (mEngine->FileUtils()) {
        TRAPD(err, operations = mEngine->FileUtils()->ClipboardCutL(selectionIndexes) );
        Q_UNUSED(err); //TODO
    }
    delete selectionIndexes;
    return operations;
}

// ---------------------------------------------------------------------------
int EngineWrapper::clipboardCopy(const QModelIndexList& aSelectionIndices)
{
    TInt operations = 0;
    const CArrayFix<TInt> *selectionIndexes = convertSelectionList(aSelectionIndices);

    if (mEngine->FileUtils()) {
        TRAPD(err, operations = mEngine->FileUtils()->ClipboardCopyL(selectionIndexes));
        Q_UNUSED(err); //TODO
    }
    delete selectionIndexes;
    return operations;
}

// ---------------------------------------------------------------------------

void EngineWrapper::clipboardPaste(const OverwriteOptions &aOverwriteOptions)
{
    TOverwriteOptions tOverwriteOptions = convertOverwriteOptions(aOverwriteOptions);
    if (mEngine->FileUtils()) {
        TRAPD(err, mEngine->FileUtils()->ClipboardPasteL(tOverwriteOptions));
        Q_UNUSED(err); //TODO
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::copyToFolder(const QString &aTargetDir, const OverwriteOptions &aOverwriteOptions, bool aMove)
{
    TFileName targetDir = TFileName(aTargetDir.utf16());
    TOverwriteOptions tOverwriteOptions = convertOverwriteOptions(aOverwriteOptions);


    TRAPD(err, mEngine->FileUtils()->CopyToFolderL(targetDir, tOverwriteOptions, aMove ? ETrue : EFalse) );
    Q_UNUSED(err); //TODO
}

// ---------------------------------------------------------------------------

void EngineWrapper::createNewFile(const QString &aNewFileName)
{
    TFileName fileName = TFileName(aNewFileName.utf16());
    TRAPD(err, mEngine->FileUtils()->NewFileL(fileName) );
    Q_UNUSED(err); //TODO
}

// ---------------------------------------------------------------------------

void EngineWrapper::createNewDirectory(const QString &aNewDirectoryName)
{
    TFileName newDirectoryName = TFileName(aNewDirectoryName.utf16());
    TRAPD(err, mEngine->FileUtils()->NewDirectoryL(newDirectoryName) );
    Q_UNUSED(err); //TODO
}

// ---------------------------------------------------------------------------

void EngineWrapper::deleteItems(const QModelIndexList& aSelectionIndices)
{
    setCurrentSelection(aSelectionIndices);
    TRAPD(err, mEngine->FileUtils()->DeleteL() );
    Q_UNUSED(err); //TODO
}

// ---------------------------------------------------------------------------
bool EngineWrapper::selectionHasDirs()
{
    return mEngine->FileUtils()->SelectionHasDirs();
}

// ---------------------------------------------------------------------------

void EngineWrapper::rename(const QModelIndex& aIndex, const QString aNewName)
{
    if (mEngine->FileUtils()) {
        const TFileName newName = TFileName(aNewName.utf16());
        TRAPD(err, mEngine->FileUtils()->RenameL(aIndex.row(), newName) );
        Q_UNUSED(err); //TODO
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::touch(bool aRecurse)
{
    if (mEngine->FileUtils()) {
        TRAPD(err, mEngine->FileUtils()->TouchL(aRecurse) );
        Q_UNUSED(err); //TODO
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::properties(const QModelIndex &aCurrentItemIndex, QStringList &aPropertyList, QString &aTitleText)
{
    if (mEngine->FileUtils()) {

        // create an array for the items
        CDesCArray* entryLines = new(ELeave) CDesCArrayFlat(16);
//        CleanupStack::PushL(entryLines);
        TFileName titleText;

        TRAPD(err, mEngine->FileUtils()->PropertiesL(aCurrentItemIndex.row(), entryLines, titleText));
        Q_UNUSED(err); //TODO

        aTitleText = QString::fromUtf16(titleText.Ptr(), titleText.Length());
        QString textItem;
        for (TInt i = 0; i < entryLines->Count(); ++i) {
            textItem = QString::fromUtf16((*entryLines)[i].Ptr(), (*entryLines)[i].Length());
            aPropertyList.append(textItem);
        }
//        CleanupStack::PopAndDestroy(); //entryLines
        delete entryLines;
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::setAttributes(quint32 &setAttributesMask, quint32 &clearAttributesMask, bool &recurse)
{
    if (mEngine->FileUtils()) {
        TBool tRecurse = recurse ? ETrue : EFalse;
        TRAPD(err, mEngine->FileUtils()->SetAttributesL(setAttributesMask, clearAttributesMask, tRecurse));
        Q_UNUSED(err); //TODO
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::openAppArc(QString fileName)
{

    //convert QString to TFilename:
    fileName.replace("/", "\\");
    TFileName fileToOpen = TFileName(fileName.utf16());

    TRAPD(err, mEngine->OpenWithApparcL(fileToOpen) );
    if(err != KErrNone) {
        return false;
    } else {
        return true;
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::openDocHandler(QString fileName, bool embeddedVal)
{
    //convert QString to TFilename:
    fileName.replace("/", "\\");
    TFileName fileToOpen = TFileName(fileName.utf16());

    TRAPD(err, mEngine->OpenWithDocHandlerL(fileToOpen, embeddedVal) );
    if(err != KErrNone) {
        return false;
    } else {
        return true;
    }
}

// ---------------------------------------------------------------------------

int EngineWrapper::itemCount() const
{
    if (mEngine->FileUtils()->IsDriveListViewActive()) {
        return mEngine->FileUtils()->DriveEntries()->Count();
    } else {
        return mEngine->FileUtils()->FileEntries()->Count();
    }
}

// ---------------------------------------------------------------------------

FbDriveEntry EngineWrapper::getDriveEntry(const QModelIndex& aIndex) const
{
    TDriveEntry driveEntry;
    if (mEngine->FileUtils()->DriveEntries()->Count() > aIndex.row() && aIndex.row() >= 0) {
        driveEntry = mEngine->FileUtils()->DriveEntries()->At(aIndex.row());
    }
    return FbDriveEntry(driveEntry);
}

// ---------------------------------------------------------------------------

FbFileEntry EngineWrapper::getFileEntry(const QModelIndex& aIndex) const
{
    TFileEntry fileEntry;
    if (mEngine->FileUtils()->FileEntries()->Count() > aIndex.row() && aIndex.row() >= 0) {
        fileEntry = mEngine->FileUtils()->FileEntries()->At(aIndex.row());
    }
    return FbFileEntry(fileEntry);
}

// ---------------------------------------------------------------------------

const CArrayFix<TInt> *EngineWrapper::convertSelectionList(const QModelIndexList &aSelectionIndices)
{
    CArrayFixFlat<TInt>* selectionIndexes = 0;
    TRAPD(err, selectionIndexes = new(ELeave)CArrayFixFlat<TInt>(4));
    if (err != KErrNone) {
        return 0;
    }

    for (int i=0; i< aSelectionIndices.count(); ++i) {
        TRAPD(err, selectionIndexes->AppendL(aSelectionIndices.at(i).row()) );
        Q_UNUSED(err); //TODO
    }
    return selectionIndexes;
}


// ---------------------------------------------------------------------------

void EngineWrapper::setCurrentSelection(const QModelIndexList &aSelectionIndices)
{
    const CArrayFix<TInt> *selectionIndexes = convertSelectionList(aSelectionIndices);
    mEngine->FileUtils()->SetCurrentSelection(selectionIndexes);
    delete selectionIndexes;
}

// ---------------------------------------------------------------------------

bool EngineWrapper::isDestinationEntriesExists(const QModelIndexList &aSelectionIndices, QString aTargetDir)
{
    TFileName targetDir = TFileName(aTargetDir.utf16());
    //setCurrentSelection(aSelectionIndices);
    Q_UNUSED(aSelectionIndices);

    TBool someEntryExists = mEngine->FileUtils()->IsDestinationEntriesExists(targetDir);
    return someEntryExists;
}

// ---------------------------------------------------------------------------
bool EngineWrapper::targetExists(const QModelIndex& aIndex, const QString aNewName)
{
    const TFileName newName = TFileName(aNewName.utf16());
    return mEngine->FileUtils()->TargetExists(aIndex.row(), newName);
}

// ---------------------------------------------------------------------------

QString EngineWrapper::currentPath() const
{
    return QString::fromUtf16(mEngine->FileUtils()->CurrentPath().Ptr(),
                              mEngine->FileUtils()->CurrentPath().Length());
}

TOverwriteOptions EngineWrapper::convertOverwriteOptions(const OverwriteOptions &aOverwriteOptions)
{
    TOverwriteOptions tOverwriteOptions;
    tOverwriteOptions.iDoFileOperations = aOverwriteOptions.doFileOperations;
    tOverwriteOptions.iQueryIndex = aOverwriteOptions.queryIndex;
    tOverwriteOptions.iPostFix = TFileName(aOverwriteOptions.postFix.utf16());
    tOverwriteOptions.iOverWriteFlags = aOverwriteOptions.overWriteFlags;
    return tOverwriteOptions;
}

bool EngineWrapper::hasDrivePassword(const QModelIndex &aIndex)
{
    bool hasPassword = false;
    if (mEngine->FileUtils()->DriveEntries()->Count() > aIndex.row() && aIndex.row() >= 0)
    {
        TDriveEntry driveEntry = mEngine->FileUtils()->DriveEntries()->At(aIndex.row());
        hasPassword = driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttHasPassword;
    }
    return hasPassword;
}

bool EngineWrapper::isDriveRemovable(const QModelIndex &aIndex)
{
    bool isRemovable = false;
    if (mEngine->FileUtils()->DriveEntries()->Count() > aIndex.row() && aIndex.row() >= 0)
    {
        TDriveEntry driveEntry = mEngine->FileUtils()->DriveEntries()->At(aIndex.row());
        isRemovable = driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemovable;
    }
    return isRemovable;
}

bool EngineWrapper::isDriveLocked(const QModelIndex &aIndex)
{
    bool isRemovable = false;
    if (mEngine->FileUtils()->DriveEntries()->Count() > aIndex.row() && aIndex.row() >= 0)
    {
        TDriveEntry driveEntry = mEngine->FileUtils()->DriveEntries()->At(aIndex.row());
        isRemovable = driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLocked;
    }
    return isRemovable;
}

void EngineWrapper::GetDriveName(const QModelIndex &aIndex, QString &aDriveName)
{
    TFileName driveName;
    mEngine->FileUtils()->GetDriveName(aIndex.row(), driveName);
    aDriveName = QString::fromUtf16(driveName.Ptr(), driveName.Length());
}


void EngineWrapper::GetDriveVolumeLabel(const QModelIndex &aIndex, QString &aVolumeLabel)
{
    TFileName volumeLabel;
    mEngine->FileUtils()->GetDriveName(aIndex.row(), volumeLabel);
    aVolumeLabel = QString::fromUtf16(volumeLabel.Ptr(), volumeLabel.Length());
}

void EngineWrapper::DiskAdminSetDrivePassword(const QModelIndex &aIndex,
                                              const QString &aOldPassword,
                                              const QString &aNewPassword)
{
    TFileName oldPassword = TFileName(aOldPassword.utf16());
    TFileName newPassword = TFileName(aNewPassword.utf16());
    TRAP_IGNORE(mEngine->FileUtils()->SetDrivePasswordL(aIndex.row(), oldPassword, newPassword));
}

void EngineWrapper::DiskAdminUnlockDrive(const QModelIndex &aIndex, const QString &aOldPassword)
{
    TFileName oldPassword = TFileName(aOldPassword.utf16());
    TRAP_IGNORE(mEngine->FileUtils()->UnlockDriveL(aIndex.row(), oldPassword));
}

void EngineWrapper::DiskAdminClearDrivePassword(const QModelIndex &aIndex, const QString &aOldPassword)
{
    TFileName oldPassword = TFileName(aOldPassword.utf16());
    TRAP_IGNORE(mEngine->FileUtils()->ClearDrivePasswordL(aIndex.row(), oldPassword));
}

void EngineWrapper::DiskAdminEraseDrivePassword(const QModelIndex &aIndex)
{
    TRAP_IGNORE(mEngine->FileUtils()->EraseDrivePasswordL(aIndex.row()));
}

void EngineWrapper::DiskAdminFormatDrive(const QModelIndex &aIndex, bool aQuickFormat)
{
    TRAP_IGNORE(mEngine->FileUtils()->FormatDriveL(aIndex.row(), aQuickFormat));
}

void EngineWrapper::DiskAdminQuickFormatDrive(const QModelIndex &aIndex, bool aQuickFormat)
{
    TRAP_IGNORE(mEngine->FileUtils()->FormatDriveL(aIndex.row(), aQuickFormat));
}

void EngineWrapper::DiskAdminCheckDisk(const QModelIndex &aIndex)
{
    TRAP_IGNORE(mEngine->FileUtils()->CheckDiskL(aIndex.row()));
}

void EngineWrapper::DiskAdminScanDrive(const QModelIndex &aIndex)
{
    TRAP_IGNORE(mEngine->FileUtils()->ScanDriveL(aIndex.row()));
}

void EngineWrapper::DiskAdminSetDriveName(const QModelIndex &aIndex, const QString &aDriveName)
{
    TFileName driveName = TFileName(aDriveName.utf16());
    TRAP_IGNORE(mEngine->FileUtils()->SetDriveNameL(aIndex.row(), driveName));
}

void EngineWrapper::DiskAdminSetDriveVolumeLabel(const QModelIndex &aIndex, const QString &aVolumeLabel)
{
    TFileName volumeLabel = TFileName(aVolumeLabel.utf16());
    TRAP_IGNORE(mEngine->FileUtils()->SetDriveNameL(aIndex.row(), volumeLabel));
}

void EngineWrapper::DiskAdminEjectDrive(const QModelIndex &aIndex)
{
    TRAP_IGNORE(mEngine->FileUtils()->EjectDriveL(aIndex.row()));
}

void EngineWrapper::DiskAdminDismountDrive(const QModelIndex &aIndex)
{
    TRAP_IGNORE(mEngine->FileUtils()->DismountFileSystemL(aIndex.row()));
}

void EngineWrapper::DiskAdminEraseMBR(const QModelIndex &aIndex)
{
    TRAP_IGNORE(mEngine->FileUtils()->EraseMBRL(aIndex.row()));
}

void EngineWrapper::DiskAdminPartitionDrive(const QModelIndex &aIndex, bool aEraseMBR, int aAmountOfPartitions)
{
    TRAP_IGNORE(mEngine->FileUtils()->PartitionDriveL(aIndex.row(), aEraseMBR, aAmountOfPartitions));
}

void EngineWrapper::ToolsSetErrRd(bool aEnable)
{
    TRAP_IGNORE(mEngine->FileUtils()->SetErrRdL(aEnable));
}

bool EngineWrapper::ErrRdFileExists()
{
    return mEngine->FileUtils()->FileExists(KErrRdPath);
}

void EngineWrapper::ToolsErrorSimulateLeave(int aLeaveCode)
{
    mEngine->FileUtils()->SimulateLeaveL(aLeaveCode);
}

void EngineWrapper::ToolsErrorSimulatePanic(QString aPanicCategory, int aPanicCode)
{
    TBuf<128> panicCategory;
    panicCategory.Copy(aPanicCategory.utf16());
    mEngine->FileUtils()->SimulatePanicL(panicCategory, aPanicCode);
}

void EngineWrapper::ToolsErrorSimulateException(int aExceptionCode)
{
    mEngine->FileUtils()->SimulateExceptionL(aExceptionCode);
}

quint32 EngineWrapper::getDebugMask()
{
    return mEngine->FileUtils()->GetDebugMask();
}

void EngineWrapper::toolsSetDebugMask(quint32 aDbgMask)
{
    mEngine->FileUtils()->SetDebugMaskL(aDbgMask);
}

void EngineWrapper::toolsWriteAllFiles()
{
    mEngine->FileUtils()->WriteAllFilesL();
}

void EngineWrapper::showFileCheckSums(const QModelIndex &aIndex, TFileBrowserCmdFileChecksums checksumType)
{
    mEngine->FileUtils()->ShowFileCheckSumsL(aIndex.row(), checksumType);
}

// ---------------------------------------------------------------------------
// Functions that are called from engine
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

void EngineWrapper::ShowErrorNote(const TDesC& aDescText, TBool aNoTimeout /*= EFalse*/)
{
    QString qText = QString::fromUtf16(aDescText.Ptr(), aDescText.Length());
    Notifications::showErrorNote(qText, aNoTimeout);
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowInformationNote(const TDesC &aDescText, const TDesC &aDescTitle)
{
    QString qText = QString::fromUtf16(aDescText.Ptr(), aDescText.Length());
    QString qTitle = QString::fromUtf16(aDescTitle.Ptr(), aDescTitle.Length());
    Notifications::showInformationNote(qText, qTitle);
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowConfirmationNote(const TDesC& aDescText, TBool aNoTimeout /*= EFalse*/)
{
    QString qText = QString::fromUtf16(aDescText.Ptr(), aDescText.Length());
    Notifications::showConfirmationNote(qText, aNoTimeout);
}

void EngineWrapper::ShowProgressDialog(const TDesC& aDescText, TInt aMinimum, TInt aMaximum )
{
    const QString qText = QString::fromUtf16(aDescText.Ptr(), aDescText.Length());
    if (!mProgressDialog) {
        mProgressDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
        QObject::connect(mProgressDialog, SIGNAL(cancelled()), this, SLOT(progressDialogCancelled()));
    }

    mProgressDialog->setText(qText);
    mProgressDialog->setMinimum(aMinimum);
    mProgressDialog->setMaximum(aMaximum);
    mEngine->FileUtils()->SetAllowProcessing(true);
    mProgressDialog->show();
}

void EngineWrapper::CancelProgressDialog()
{
    if (mProgressDialog) {
        QObject::disconnect(mProgressDialog, SIGNAL(cancelled()), this, SLOT(progressDialogCancelled()));
        mProgressDialog->cancel();
        QObject::connect(mProgressDialog, SIGNAL(cancelled()), this, SLOT(progressDialogCancelled()));
    }
}

void EngineWrapper::SetProgressValue(TInt aValue)
{
    if (mProgressDialog)
        mProgressDialog->setProgressValue(aValue);
}

void EngineWrapper::progressDialogCancelled()
{
    mEngine->FileUtils()->DialogDismissedL();
}

void EngineWrapper::ShowWaitDialog(const TDesC& aDescText)
{
    const QString qText = QString::fromUtf16(aDescText.Ptr(), aDescText.Length());
    if (!mWaitDialog) {
        mWaitDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
        QObject::connect(mWaitDialog, SIGNAL(cancelled()), this, SLOT(waitDialogCancelled()));
    }

    mWaitDialog->setText(qText);
    mEngine->FileUtils()->SetAllowProcessing(true);
    //mWaitDialog->setAttribute(Qt::WA_DeleteOnClose);
    mWaitDialog->show();
}

void EngineWrapper::CancelWaitDialog()
{
    if (mWaitDialog)
        mWaitDialog->cancel();
}

void EngineWrapper::waitDialogCancelled()
{
    mEngine->FileUtils()->SetAllowProcessing(false);
}

void EngineWrapper::ProcessEvents()
{
    qApp->processEvents();
}

TBool EngineWrapper::ShowConfirmationQuery(const TDesC& aDescText)
{
    QString qText = QString::fromUtf16(aDescText.Ptr(), aDescText.Length());
    return Notifications::showConfirmationQuery(qText);
}

void EngineWrapper::NotifyModelHasChanged()
{
    emit fileSystemDataChanged();
}

// ---------------------------------------------------------------------------
