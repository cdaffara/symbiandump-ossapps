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
 * Description:
 */

#ifndef MMSTESTBED_H
#define MMSTESTBED_H

#ifdef BUILD_MMSTESTBED_DLL
#define MMSTESTBED_EXPORT Q_DECL_EXPORT
#else
#define MMSTESTBED_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QString>

#include <msvids.h>  //for consts like KMsvNullIndexEntryIdValue
#include <MSVAPI.H>
#include <MTCLREG.H>
#include <mmsclient.h>
//#include <mmsservercommon.h>

class CTestUiTimer;
class CMsvSession;
class CMmsClientMtm;
class CMmsSettings;
class CMmsHeaders;
class CLogEvent;
class CLogViewEvent;
class CLogFilter;
class CLogClient;

class MMSTESTBED_EXPORT MmsTestBed : public QObject, public MMsvSessionObserver
    {
    Q_OBJECT

public:
    /**
     *
     */
    MmsTestBed();
    ~MmsTestBed();
    void setConnectionLocal(bool value);
    void fromOutboxToMmsc();
    void fromMmscToInbox();
    /**
     * cleanup MMS service
     */
    void cleanupAndCreateNewService();
    void deleteNotifications();
    void restoreFactorySettings();
    void setFetchingState( TMmsReceivingMode aState );
    void sendFromFile();
    void sendOneByOne();
    void sendNotifications();
    void copyDrafts();
    void garbageCollection(TUint32 aReason = 0x00000001); //0x00000001 == KMmsReasonBoot
    void messageVariation();
    void cancelSendScheduling();
    void scheduledSend(TMsvId aBoxId, TInt aDelay = 5);
    void deleteSendSchedule();
    void cancelFetchScheduling();
    void fetchForced(TInt aDelay = 5);
    void deleteFetchSchedule();
    void doAFetchCycle();
    void createEntry(TMsvEntry& aNewEntry, CMsvEntry& aClientEntry);
    void cleanOutbox();
    void cleanInbox();
    void cleanSent();
    void cleanDrafts();
    void cleanAll();
    void reply();
    void replyToAll();
    void forward();
    void sendReadReport(); // send a read report
    int fromOutboxToMmscWithMemoryFailure(); // returns memory failure count
    int fromMmscToInboxWithMemoryFailure(); // returns memory failure count
    void sendViaClient();
    void scheduledFetch(TInt aDelay = 5);
    void fetchForcedImmediate();
    void getEventType();
    void addEventType();
    void deleteEventType();
    void cleanLog();
    void setOnline(bool value);
    void generateDeliveryReport( CMmsHeaders* aMmsHeaders );
    void sendDeliveryReport();

signals:
    void entryCreatedInDraft(long int id);
    void entryMovedToOutbox(long int id);
    void entryMovedToSent(long int id);
    void entryCreatedInInbox(long int id);

protected:
    /**
     * From MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
            TAny* aArg2, TAny* aArg3);

private:
    void findDefaultL();
    void createMmsService();
    void cleanup();
    void deleteEntry(TMsvId aEntryId, CMsvEntry& aClientEntry);
    void testFile(TFileName& aFilePath, TInt aCommand = 0, TMsvId aBox = KMsvGlobalOutBoxIndexEntryId);
    void addMMBoxDescriptions();
    void encodeMessageFromDrafts();
    TMsvId findMMSFolder();
    bool checkLogClient();
    int getLogEntries();

private:
    //data
    CTestUiTimer* iTimer;
    CMsvOperationActiveSchedulerWait* iWait;
    CMsvSession* iSession;
    CClientMtmRegistry* iClientMtmRegistry;
    CMmsClientMtm* iMmsClient;
    CMmsSettings* iSettings;
    CMmsHeaders* iMmsHeaders;
    TMsvId iServiceId;
    TMsvId iDefaultServiceId;
    TMsvSessionEvent iEvent;
    RFs iFs;
    TFileName iFilename;
    TFileName iCurrentFile;
    TFileName iCurrentPath;
    TParse iParse;     // parse buffer as member to save stack space
    CBufFlat* iEncodeBuffer;
    CMsvEntrySelection* iMsvEntrySelection;
    CLogEvent* iLogEvent;
    CLogClient* iLogClient;
    CLogViewEvent* iLogView;
    CLogFilter* iLogFilter;
    };

#endif // MMSTESTBED_H
