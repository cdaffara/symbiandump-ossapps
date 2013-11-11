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

#ifndef CntSimEngine_H_
#define CntSimEngine_H_

#include <QObject>
#include "cntsimutility.h"

#include <cntabstractview.h>

class HbView;
class HbAction;
class QStandardItemModel;
class QStandardItem;
class HbDialog;
class CntImportViewCallback;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*
 * CntSimImportEngine this class is used to import contancts from the SIM card.
 */

class CntSimEngine : public QObject
{

Q_OBJECT

private slots:
    void stopSimImport();
    void importADNFetchResults();
    void importSDNFetchResults();
    void getSimInfoAndUpdateUI(CntSimUtility::CacheStatus& cacheStatus, int error);

public:
    CntSimEngine(CntImportViewCallback& aImportViewCallback, HbView& aView);
    ~CntSimEngine();
    
    void init(CntAbstractEngine& aAbsEngine);
    void checkEntries(QStringList& simList, QStandardItem& importSimItem);
    bool startSimImport();
    void simInfoErrorMessage(int infoError);
    bool userCancelsImport(QString& aResults);
    void setPreferredDetails(QContact& aContact );
    QContactManager *contactSymbianManager()            { return mContactSymbianManager; };


private:
    void showSimImportResults() const;
    void fetchSDNContacts();
    void timerEvent(QTimerEvent *event);    // standard QT TimerEvent callback method
    
Q_SIGNALS:
    void showNamesView();
    void closePopup();
    
private:

private:
    CntImportViewCallback& mImportViewCallback;
    HbView& mView;
    CntSimUtility *mAdnSimUtility;              // Owned
    CntSimUtility *mSdnSimUtility;              // Owned
    QContactManager *mContactSimManagerADN;
    QContactManager *mContactSimManagerSDN;
    QContactManager *mContactSymbianManager;
    QContactFetchRequest *mFetchRequestADN;     // Owned
    QContactFetchRequest *mFetchRequestSDN;     // Owned
    QList<QContact> mSaveSimContactsList;
    QList<QContact> mSaveSimContactsListSDN;
    QTimer *mTimer;
    CntAbstractEngine* mAbsEngine;
    int mTimerId;
    int mSaveCount;
    int mAdnStoreEntries;
    int mSdnStoreEntries;
    bool mAdnEntriesPresent;
    bool mFetchIsDone;
    bool mAdnStorePresent;
    bool mSdnStorePresent;
    bool mSimPresent;
    bool mSimError;
    bool mWaitingForAdnCache;
    bool mImportInProgress;
};

#endif
