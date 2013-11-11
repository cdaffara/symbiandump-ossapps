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
* Description: Private data and helper classes used by class CntCache.
*
*/

#include <e32base.h>
#include <s32mem.h>
#include <e32std.h>

#include <xqutils.h>
#include <QEvent>
#include <QFile>
#include <QDir>
#include <hbapplication.h>
#include <hbstringutil.h>

#include <cntdb.h>
#include <cntuids.h>
#include <cntdebug.h>

#include "cntnamefetcher.h"

// constants used when fetching names from CntSrv
#define KCntSearchResultList 99
#define KCntOpenDataBase 100
_LIT(KCntServerExe, "CNTSRV.EXE");
_LIT(KCntServerName, "CNTSRV");
const TInt KAsyncMessageSlots = 6;
const TInt KCntServerMajorVersionNumber=1;
const TInt KCntServerMinorVersionNumber=1;
const TInt KCntServerBuildVersionNumber=1;
static const QEvent::Type CntAsynchOperation = QEvent::User;

// constants used for file cache
static const QString cacheFolder = "20022EF9";
static const QString cacheFilename = "contactcache.dat";

/*!
     Internal class used by CntSrvConnection to issues requests to CntSrv.
  */
class CntSrvSession : public RSessionBase
{
public:
    CntSrvSession() { mConnected = false; }
    ~CntSrvSession() { RHandleBase::Close(); }
    void executeSqlQueryL(const TDesC &sqlQuery, QList<CntNameCacheItem *> &names, CntNameOrder nameFormat, int sizeHintKB);

private:
    void connectCntSrvL();

private:
    bool mConnected;
};

CntSrvConnection::CntSrvConnection()
    : mSession(NULL),
      mIsAsynchronous(false)
{
}

CntSrvConnection::~CntSrvConnection()
{
    disconnect();

    if (mThread.isRunning()) {
        mThread.quit();
        mThread.wait();
    }

    delete mSession;

    mNames.clear();
}

void CntSrvConnection::setAsynchronous()
{
    mIsAsynchronous = true;
    mThread.start();
    moveToThread(&mThread);
}

bool CntSrvConnection::executeSqlQuery(const QString &sqlQuery, CntNameOrder nameFormat, int sizeHintKB)
{
    CNT_ENTRY

    if (!mSession) {
        mSession = new CntSrvSession();
    }

    if (mIsAsynchronous) {
        mSqlQuery = sqlQuery;
        mNameFormat = nameFormat;
        mSizeHintKB = sizeHintKB;
        HbApplication::instance()->postEvent(this, new QEvent(CntAsynchOperation));
    } else {
        mNames.clear();
        TPtrC queryPtr(sqlQuery.utf16(), sqlQuery.length());
        TRAPD(err, mSession->executeSqlQueryL(queryPtr, mNames, nameFormat, sizeHintKB));
        if (err != KErrNone) {
            qDeleteAll(mNames);
            mNames.clear();
            CNT_EXIT
            return false;
        }
    }

    CNT_EXIT
    
    return true;
}

bool CntSrvConnection::event(QEvent *event)
{
    if (event->type() == CntAsynchOperation) {
        CNT_ENTRY

        mNames.clear();
        TPtrC ptr(mSqlQuery.utf16(), mSqlQuery.length());
        TRAPD(err, mSession->executeSqlQueryL(ptr, mNames, mNameFormat, mSizeHintKB));
        if (err != KErrNone) {
            qDeleteAll(mNames);
            mNames.clear();
        }
        emit namesRead();
        qStableSort(mNames.begin(), mNames.end(), CntNameFetcher::compareNames);
        delete mSession;
        mSession = NULL;
        emit namesSorted();

        CNT_EXIT

        return true;
    }
    
    return QObject::event(event);
}

/*!
    Executes a special SQL query: the first column must be the contact id and
    the subsequent columns must be varchar fields.
    
    \param sqlQuery the SQL to execute
    \param names the list where the results will be stored
    \param nameFormat the format the names should be stored in 
    \param sizeHintKB the expected size of the buffer needed to fit the results; a too
                      small value will effectively double the fetch time, since the
                      buffer is then resized and the data refetched a second time
 */
void CntSrvSession::executeSqlQueryL(const TDesC& sqlQuery, QList<CntNameCacheItem*> &names, CntNameOrder nameFormat, int sizeHintKB)
{
    int listSize = 0;

    // read the ids and names from the database
    if (!mConnected) {
        connectCntSrvL();
    }

    // allocate tmeporary buffer
    TInt bufferSize = sizeHintKB * 1024;
    CBufFlat* buffer = CBufFlat::NewL(256);
    CleanupStack::PushL(buffer);

    // try to fetch the results, if the fetch fails with
    // a positive value, it means the buffer was too small
    // in this case the buffer is resized and the results
    // are fetched again
    for (TInt tries = 0; tries < 2 && bufferSize > 0; ++tries) {
        buffer->ResizeL(bufferSize);
        TPtr8 bufferPtr = buffer->Ptr(0);
        TIpcArgs args;
        args.Set(0, &bufferPtr);
        args.Set(1, &sqlQuery);
        bufferSize = SendReceive(KCntSearchResultList, args);
        CNT_LOG_ARGS("buffer size =" << bufferSize)
        User::LeaveIfError(bufferSize);
    } 

    // store the formatted names into the list
    RBufReadStream readStream;
    TInt id;
    TBuf<256> firstName;
    TBuf<256> lastName;

    readStream.Open(*buffer);
    for (int i = 0; (id = readStream.ReadInt32L()) != 0; ++i) {
        readStream >> firstName;
        readStream >> lastName;
        CntNameCacheItem* item = new (ELeave) CntNameCacheItem(
            id,
            QString::fromUtf16(firstName.Ptr(), firstName.Length()),
            QString::fromUtf16(lastName.Ptr(), lastName.Length()),
            nameFormat);
        if (i >= listSize - 1) {
            // if the list is runnning out of space, resize it;
            // initial size is 1000 and after that it doubles
            // every time it runs out of space
            if (listSize == 0) {
                listSize = 1000;
            } else {
                listSize *= 2;
            }
            QT_TRY {
                names.reserve(listSize);
            } QT_CATCH (...) {
                // clean up and return
                CleanupStack::PopAndDestroy(buffer);
                qDeleteAll(names);
                names.clear();
                return;
            }
        }
        names.append(item);
    }

    CleanupStack::PopAndDestroy(buffer);
}

/*!
    Connect to / create a contacts server session.
 */
void CntSrvSession::connectCntSrvL()
{
    // Assume the server is already running and attempt to create a session
    // with a maximum of KAsyncMessageSlots message slots.
    TInt err = CreateSession(KCntServerName,
                             TVersion(KCntServerMajorVersionNumber, KCntServerMinorVersionNumber, KCntServerBuildVersionNumber),
                             KAsyncMessageSlots);
    
    // Server is not running
    if (err == KErrNotFound) {
        // Use the RProcess API to start the server.
        RProcess server;
        User::LeaveIfError(server.Create(KCntServerExe, KNullDesC));
        
        // Enforce server to be at system default priority EPriorityForeground
        server.SetPriority(EPriorityForeground);
        
        // Synchronize with the server.
        TRequestStatus reqStatus;
        server.Rendezvous(reqStatus);
        server.Resume();
        
        // Server will call the reciprocal static synchronization call.
        User::WaitForRequest(reqStatus);
        server.Close();
        User::LeaveIfError(reqStatus.Int());
        
        // Create the server session.
        User::LeaveIfError(CreateSession(KCntServerName,
                                         TVersion(KCntServerMajorVersionNumber, KCntServerMinorVersionNumber, KCntServerBuildVersionNumber),
                                         KAsyncMessageSlots));
    } else {
        User::LeaveIfError(err);
    }
    
    TIpcArgs args;
    args.Set(0, &KNullDesC);
    User::LeaveIfError(SendReceive(KCntOpenDataBase, args));

    mConnected = true;
}

/*!
    Creates a CntNameFetcher object.
 */
CntNameFetcher::CntNameFetcher()
    : mDbConnection(NULL),
      mAsynchDbConnection(NULL),
      mSettingsManager(NULL),
      mNameFormatSetting(NULL),
      mBufferSizeEstimate(0)
{
    CNT_ENTRY

    // get name format setting and listen to changes
    mSettingsManager = new XQSettingsManager();
    mNameFormatSetting = new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRCntSettings.iUid, KCntNameOrdering);
    mNameFormat = static_cast<CntNameOrder>(mSettingsManager->readItemValue(*mNameFormatSetting, XQSettingsManager::TypeInt).toInt());
    mSettingsManager->startMonitoring(*mNameFormatSetting, XQSettingsManager::TypeInt);
    connect(mSettingsManager, SIGNAL(valueChanged(const XQSettingsKey&, const QVariant&)), this, SLOT(setNameFormat(const XQSettingsKey&, const QVariant&)));

    // connect to contacts server
    mDbConnection = new CntSrvConnection();

    CNT_EXIT
}

/*!
    Destroys a CntNameFetcher object.
 */
CntNameFetcher::~CntNameFetcher()
{
    CNT_ENTRY

    delete mSettingsManager;
    delete mNameFormatSetting;
    delete mDbConnection;
    delete mAsynchDbConnection;

    CNT_EXIT
}

/*!
    Reads names from the file cache.

    \return true if the names were read successfully from the cache file

 */
bool CntNameFetcher::readNamesFromCache(QList<CntNameCacheItem*> &names)
{
    CNT_ENTRY

    bool success = true;
    quint32 itemCount;
    quint32 nameFormat;

    QFile cacheFile(XQUtils::phoneMemoryRootPath() + cacheFolder + "\\" + cacheFilename);
    if (!cacheFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&cacheFile);

    mBufferSizeEstimate = 0;
    QT_TRY {
        // read header: nr of items, name format
        in >> itemCount;
        in >> nameFormat;
        names.reserve(itemCount);

        // populate list with names
        while (itemCount-- > 0) {
            CntNameCacheItem *item = CntNameCacheItem::internalize(in, (CntNameOrder) nameFormat);
            names.append(item);
            mBufferSizeEstimate += 4 + 2 * item->name().length();
        }
    } QT_CATCH (...) {
        qDeleteAll(names);
        names.clear();
        success = false;
    }
    
    cacheFile.close();
    
    CNT_EXIT
    
    return success;
}

/*!
    Write names to the file cache.
 */
bool CntNameFetcher::writeNamesToCache(const QList<CntNameCacheItem*> &names) const
{
    CNT_ENTRY

    bool success = true;

    // create folder for cache file if it does not already exist
    QString path = XQUtils::phoneMemoryRootPath() + cacheFolder;
    if (!QDir(path).exists()) {
        QDir dir(XQUtils::phoneMemoryRootPath());
        if (!dir.mkdir(cacheFolder)) {
            CNT_EXIT_ARGS("failed to create folder: " << path)
            return false;
        }

        // have to use native Symbian code to make the dir hidden
        RFs fs;
        fs.Connect();
        TPtrC pathPtr(path.utf16(), path.length());
        if (fs.SetAtt(pathPtr, KEntryAttHidden, 0) != KErrNone) {
            fs.Close();
            return false;
        }
        fs.Close();
    }    

    // open cache file for writing
    QFile cacheFile(XQUtils::phoneMemoryRootPath() + cacheFolder + "\\" + cacheFilename);
    if (!cacheFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        CNT_EXIT_ARGS("failed to create file")
        return false;
    }
    QDataStream out(&cacheFile);

    // write the names to the cache file
    QT_TRY {
        // write header
        out << names.size();
        out << (quint32) mNameFormat;

        // write list with names
        foreach (CntNameCacheItem* name, names) {
            name->externalize(out);
        }
    } QT_CATCH (...) {
        success = false;
    }

    cacheFile.close();

    CNT_EXIT

    return success;
}

/*!
    Reads the name of one contact from the contact database synchronously.
    
    \param contactId the id of the contact
 */
CntNameCacheItem* CntNameFetcher::readOneName(QContactLocalId contactId) const
{
    CNT_ENTRY

    QString sqlQuery = QString("SELECT contact_id, first_name, last_name FROM contact WHERE (type_flags>>24)<=1 AND contact_id=%1").arg(contactId);
    mDbConnection->executeSqlQuery(sqlQuery, mNameFormat, 2);
    
    if (mDbConnection->names().size() == 0) {
        return NULL;
    }

    CNT_EXIT
    
    return mDbConnection->names().at(0);
}

/*!
    Reads the names of all contacts from the contact database asynchronously.
 */
void CntNameFetcher::readAllNamesAsynch()
{
    CNT_ENTRY
    
    if (mAsynchDbConnection != NULL) {
        // an asynch fetch is already in progress, so no need to start a new one
        return;
    }
    
    if (mBufferSizeEstimate == 0) {
        mBufferSizeEstimate = 240 * 1024;
    }
    
    CNT_LOG_ARGS("buffer size =" << mBufferSizeEstimate)
    
    mAsynchDbConnection = new CntSrvConnection();
    mAsynchDbConnection->setAsynchronous();
    connect(mAsynchDbConnection, SIGNAL(namesRead()), this, SIGNAL(databaseAccessComplete()));
    connect(mAsynchDbConnection, SIGNAL(namesSorted()), this, SLOT(sendCompletionSignal()));
    mAsynchDbConnection->executeSqlQuery("SELECT contact_id, first_name, last_name FROM contact WHERE (type_flags>>24)<=1", mNameFormat, 16 + mBufferSizeEstimate / 1024);
    
    CNT_EXIT
}

/*!
    Sorts the names quickly and in a locale aware manner.
 */
void CntNameFetcher::sortNames(QList<CntNameCacheItem *> &names) const
{
    CNT_ENTRY

    qStableSort(names.begin(), names.end(), CntNameFetcher::compareNames);

    CNT_EXIT
}

/*! 
    Compares a pair of contact names and returns true if the first
    one should be presented before the second one in a list. This
    static function is used e.g. when sorting lists of names.
 */
bool CntNameFetcher::compareNames(const CntNameCacheItem* a, const CntNameCacheItem* b)
{
    QString aName = a->name();
    QString bName = b->name();

    if (aName.isEmpty()) {
        return false;
    } else if (bName.isEmpty()) {
        return true;
    }

    return (HbStringUtil::compareC(aName, bName) < 0);
}

/*!
    Notifies clients that the name format has changed. This function is called by the framework
    if the name format settings is changed, see the constructor.
 */
void CntNameFetcher::setNameFormat(const XQSettingsKey &/*key*/, const QVariant &value)
{
    CNT_ENTRY

    bool ok = false;
    CntNameOrder newNameFormat = static_cast<CntNameOrder>(value.toInt(&ok));
    if (ok && newNameFormat != mNameFormat) {
        mNameFormat = newNameFormat;
        emit nameFormatChanged(mNameFormat);
    }

    CNT_EXIT
}

/*!
    Emits the results of a completed asynch database operation.
 */
void CntNameFetcher::sendCompletionSignal()
{
    CNT_ENTRY

    emit namesAvailable(mAsynchDbConnection->names());

    delete mAsynchDbConnection;
    mAsynchDbConnection = NULL;

    CNT_EXIT
}

/*!
    Creates a CntNameCacheItem object.
 */
CntNameCacheItem::CntNameCacheItem(QContactLocalId id, const QString& firstName, const QString& lastName, CntNameOrder nameFormat)
{
    mContactId = id;
    setFormattedName(firstName, lastName, nameFormat);
}

/*!
    Destroys a CntNameCacheItem object.
 */
CntNameCacheItem::~CntNameCacheItem()
{
}

/*!
    Changes the format used to present the name.
 */
void CntNameCacheItem::setNameFormat(CntNameOrder newFormat)
{
    QString firstName = mName.mid(mFirstNamePosition&0xffff, mFirstNamePosition>>16);
    QString lastName = mName.mid(mLastNamePosition&0xffff, mLastNamePosition>>16);
    setFormattedName(firstName, lastName, newFormat);
}

/*!
    Copies the contents of the other cache item to this one.
 */
void CntNameCacheItem::operator=(const CntNameCacheItem &other)
{
    mContactId = other.mContactId;
    mFirstNamePosition = other.mFirstNamePosition;
    mLastNamePosition = other.mLastNamePosition;
    mName = other.mName;
}

/*!
    Externalizes a CntNameCacheItem object.
 */
void CntNameCacheItem::externalize(QDataStream &stream)
{
    stream << mContactId;
    stream << mFirstNamePosition;
    stream << mLastNamePosition;
    stream << mName;
}

/*!
    Internalizes a CntNameCacheItem object.
 */
CntNameCacheItem* CntNameCacheItem::internalize(QDataStream &stream, CntNameOrder nameFormat)
{
    quint32 id;
    quint32 firstNamePosition;
    quint32 lastNamePosition;
    QString name;
    
    stream >> id;
    stream >> firstNamePosition;
    stream >> lastNamePosition;
    stream >> name;
    
    QString firstName = name.mid(firstNamePosition&0xffff, firstNamePosition>>16);
    QString lastName = name.mid(lastNamePosition&0xffff, lastNamePosition>>16);

    return new CntNameCacheItem(id, firstName, lastName, nameFormat);
}

/*!
    Sets the formatted name and positions of the first name and last name,
    according to the name format in the parameter.
 */
void CntNameCacheItem::setFormattedName(const QString& firstName, const QString& lastName, CntNameOrder nameFormat)
{
    int firstNameLength = firstName.length();
    int lastNameLength = lastName.length();

    if (lastNameLength == 0) {
        mName = firstName;
        mFirstNamePosition = firstNameLength << 16;
        mLastNamePosition = 0;
    } else if (firstNameLength == 0) {
        mName = lastName;
        mFirstNamePosition = 0;
        mLastNamePosition = lastNameLength << 16;
    } else {
        if (nameFormat == CntOrderLastFirst) {
            mName = lastName + " " + firstName;
            mFirstNamePosition = (firstNameLength << 16) | (lastNameLength + 1);
            mLastNamePosition = (lastNameLength << 16);
        } else if (nameFormat == CntOrderLastCommaFirst) {
            mName = lastName + ", " + firstName;
            mFirstNamePosition = (firstNameLength << 16) | (lastNameLength + 2);
            mLastNamePosition = (lastNameLength << 16);
        } else {
            mName = firstName + " " + lastName;
            mFirstNamePosition = (firstNameLength << 16);
            mLastNamePosition = (lastNameLength << 16) | (firstNameLength + 1);
        }
    }
}

QString CntNameCacheItem::firstName() const
{
    return mName.mid(mFirstNamePosition&0xffff, mFirstNamePosition>>16);
}

QString CntNameCacheItem::lastName() const
{
    return mName.mid(mLastNamePosition&0xffff, mLastNamePosition>>16);
}
