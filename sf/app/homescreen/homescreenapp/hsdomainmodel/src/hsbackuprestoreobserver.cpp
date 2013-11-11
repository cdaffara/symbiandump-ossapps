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
* Description:  Home screen Backup/Restore observer.
*
*/


#include "hsdatabase.h"
#include "hsbackuprestoreobserver.h"
#ifdef Q_OS_SYMBIAN
#include <connect/sbdefs.h>
#include <e32property.h>
#include <XQSettingsManager>
#include "hsbackuprestoreactivecallback.h"
#include "hsbackuprestoreobserver_p.h"

/*!
    \class HsBackupRestoreObserver
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!
    Constructs a new observer with the given \a parent object.
*/
HsBackupRestoreObserverPrivate::HsBackupRestoreObserverPrivate(HsBackupRestoreObserver *observer,
                                                               QObject *parent):
    QObject(parent),
    mQ(observer),
    mCallBack(NULL),
    mActiveBackupClient(NULL),
    mLastType(conn::EBURNormal)
{
    XQPublishAndSubscribeSettingsKey backupRestoreKey(KUidSystemCategory.iUid, KUidBackupRestoreKey);
    XQSettingsManager *sm = new XQSettingsManager(this);
    
    connect(sm,SIGNAL(valueChanged(XQSettingsKey, const QVariant &)), this,
			          SLOT(handleKeyChange(XQSettingsKey, const QVariant &)));
    sm->startMonitoring(backupRestoreKey);
}

/*!
    Destructor.
*/
HsBackupRestoreObserverPrivate::~HsBackupRestoreObserverPrivate()
{
    delete mCallBack;
    delete mActiveBackupClient;
}

/*!
    Informs active backup client, when Home screen is ready for backup/restore.
*/
void HsBackupRestoreObserverPrivate::readyForBUR()
{
	mActiveBackupClient->ConfirmReadyForBURL( KErrNone );
}

/*!
    \fn HsBackupRestoreObserverPrivate::handleKeyChange()

    Handles the key change. Creates active backup client and 
    informs client about backup/restore state change.
*/

void HsBackupRestoreObserverPrivate::handleKeyChange(XQSettingsKey key, const QVariant &value)
{    
    conn::TBURPartType type = static_cast< conn::TBURPartType >
        ( value.toInt() & conn::KBURPartTypeMask );
		
	// Test if the device is going into backup or restore mode, and we are
    // required to participate.
    if ( key.key() == KUidBackupRestoreKey && 
         (( type == conn::EBURBackupFull || type == conn::EBURRestoreFull ) ||
         ( type == conn::EBURBackupPartial || type == conn::EBURRestorePartial )) )
    {
        if ( !mCallBack )
        {
            TRAPD(err, mCallBack = CHsBURActiveCallback::NewL(mQ));
            if ( err )
            {
                qDebug() << "HsBackupRestoreObserverPrivate::handleKeyChange - BUR active callback creation failed with error " << err;
            }
        }

        if ( !mActiveBackupClient )
        {
            TRAPD(err, mActiveBackupClient = conn::CActiveBackupClient::NewL( mCallBack ));
            if ( err )
            {
                qDebug() << "HsBackupRestoreObserverPrivate::handleKeyChange - Active backup client creation failed with error " << err;
            }

            if ( ( type == conn::EBURBackupPartial || 
                   type == conn::EBURRestorePartial ) &&
                 !mActiveBackupClient->DoesPartialBURAffectMeL() )
            {
                delete mCallBack;
                mCallBack = NULL;
                delete mActiveBackupClient;
                mActiveBackupClient = NULL;
                return;
            }           
            mQ->backupRestoreStarted();            
        }        
    }
    else
    {
        if ( key.key() == KUidBackupRestoreKey && type == conn::EBURNormal )
        {
            if ( (mLastType == conn::EBURBackupFull || mLastType == conn::EBURBackupPartial) ||
            	 (mLastType == conn::EBURRestorePartial || mLastType == conn::EBURRestoreFull) )
            {                
                mQ->backupRestoreCompleted();
            }         
            // delete once back to normal.
            delete mCallBack;
            mCallBack = NULL;
            delete mActiveBackupClient;
            mActiveBackupClient = NULL;            
        }
    }

    mLastType = type;
}
#endif //Q_OS_SYMBIAN

/*!
    Singleton. 
*/
HsBackupRestoreObserver *HsBackupRestoreObserver::instance()
{
    if (!mInstance) {
        mInstance = new HsBackupRestoreObserver();
    }
    return mInstance;
}

/*!
    Constructs a new observer with the given \a parent object.
*/
HsBackupRestoreObserver::HsBackupRestoreObserver(QObject *parent)
 : QObject(parent),
   mBUROngoing(false)
{
#ifdef Q_OS_SYMBIAN
    mD = new HsBackupRestoreObserverPrivate(this);
#endif //Q_OS_SYMBIAN
}

/*!
    Destructor.
*/
HsBackupRestoreObserver::~HsBackupRestoreObserver()
{
#ifdef Q_OS_SYMBIAN
    delete mD;
#endif //Q_OS_SYMBIAN
}

/*!
    Called when backup/restore is started.
*/
void HsBackupRestoreObserver::backupRestoreStarted()
{
	mBUROngoing = true;
	emit event_backupRestoreStarted();
}

/*!
    Called when backup/restore is completed.
*/
void HsBackupRestoreObserver::backupRestoreCompleted()
{
	mBUROngoing = false;
	emit event_backupRestoreCompleted();
}

/*!
    Called when Home screen is ready for backup/restore.
*/
void HsBackupRestoreObserver::readyForBUR()
{
#ifdef Q_OS_SYMBIAN
    mD->readyForBUR();
#endif //Q_OS_SYMBIAN
}

/*!
    Returns true if bacup/restore is ongoing.
*/
bool HsBackupRestoreObserver::checkBUR()
{
	return mBUROngoing;
}
/*!
    Points to the observer instance.
*/
HsBackupRestoreObserver *HsBackupRestoreObserver::mInstance(0);

// End of File
