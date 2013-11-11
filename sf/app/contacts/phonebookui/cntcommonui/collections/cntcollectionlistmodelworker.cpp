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
*
*/

#include "cntcollectionlistmodelworker.h"
#include "cntfavourite.h"
#include "cntdebug.h"

#include <hbglobal.h>
#include <hbapplication.h>

#include <cntuids.h>
#include <xqsettingskey.h>

// CONSTANTS
const int CntNamesLengthLimit = 30;

/*!
    Constructor
*/
CntCollectionListModelWorker::CntCollectionListModelWorker(QString unnamed, QString noFavs, QString noMembers, int sortOrder) :
    mStarted(false),
    mProcessingJobs(false),
    mStopped(false),
    mManager(NULL),
    mUnnamed(unnamed),
    mNoFavorites(noFavs),
    mNoMembers(noMembers),
    mSortOrder(sortOrder)
{
    CNT_ENTRY
    
    moveToThread(this);
    
    CNT_EXIT
}

/*!
    Destructor
    
    Clears any remaining jobs and waits until the last one gets completed.
*/
CntCollectionListModelWorker::~CntCollectionListModelWorker()
{
    CNT_ENTRY
    
    mMutex.lock();
    
    mJobs.clear();
    
    mMutex.unlock();

    exit();
    
    // Ugly as hell, but needed for UTs or otherwise they'll freeze
#ifndef PBK_UNIT_TEST
    wait();
#else
    wait(5000);
#endif

    CNT_EXIT
}

/*!
    Re-implemented from QThread, start the event loop.
*/
void CntCollectionListModelWorker::run()
{
    CNT_ENTRY

    exec();
    
    delete mManager;
    mManager = NULL;
    
    CNT_EXIT
}

/*!
    Re-implemented from QThread to capture QEvent::User type events
    in order to start the job processing loop.

    \param event the event sent to this object
    \return bool
*/
bool CntCollectionListModelWorker::event(QEvent *event)
{
    if (event->type() == QEvent::User)
    {
        processJobs();
        return true;
    }

    return QThread::event(event);
}

/*!
    Schedule an asynch job for the given contact id.

    \param id the QContactLocalId of the group
*/
void CntCollectionListModelWorker::scheduleJob(int id)
{
    CNT_ENTRY
    
    mMutex.lock();

    if (!mStarted) {
        // starting the event loop; minimum priority is used as this thread
        // should interfere as little as possible with more time-critical tasks,
        // like updating the UI during scrolling
        start(QThread::IdlePriority);
        mStarted = true;
    }

    if (!mProcessingJobs) {
        // new job => start processing jobs
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(QEvent::User));
    }

    mJobs.append(id);
    
    mMutex.unlock();
    
    CNT_EXIT
}

/*!
    Job processing loop. Loops forever until there are no more jobs left.
    If jobs exist, takes the first one from the list and processes it.
*/
void CntCollectionListModelWorker::processJobs()
{
    CNT_ENTRY
    
    forever
    {
        mMutex.lock();
        int jobCount = mJobs.count();

        if (jobCount == 0)
        {
            mProcessingJobs = false;
            mMutex.unlock();
            break;
        }
        else
        {
            // get next job
            int id = mJobs.takeFirst();

            mMutex.unlock();
            fetchInformation(id);
        }

        HbApplication::processEvents();
    }
    
    CNT_EXIT
}

/*!
    Fetch the second row text (member names or placeholder text if no members exist)
    and the amount of contacts in the group. Emits the result as a signal for
    CntCollectionListModel to handle.

    \param id the QContactLocalId of the group in the given row
*/
void CntCollectionListModelWorker::fetchInformation(int id)
{
    CNT_ENTRY
    
    if (!mManager)
    {
        mManager = new QContactManager("symbian");
    }
    
    QContact contact = mManager->contact(id);

    int favoriteGroupId = CntFavourite::favouriteGroupId( mManager );

    QString secondLineText;

    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationship::First);
    rFilter.setRelatedContactId(contact.id());

    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldFirstName);
    sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldLastName);
    sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;

    if (mSortOrder == CntOrderFirstLast)
    {
        sortOrders.append(sortOrderFirstName);
        sortOrders.append(sortOrderLastName);
    } 
    else
    {
        sortOrders.append(sortOrderLastName);
        sortOrders.append(sortOrderFirstName);
    }

    // group members and their count
    QList<QContactLocalId> groupMemberIds = mManager->contactIds(rFilter, sortOrders);

    if (!groupMemberIds.isEmpty())
    {
        QStringList nameList;
        for(int i = 0;i < groupMemberIds.count();i++)
        {
            QContactFetchHint nameOnlyFetchHint;
            /*QStringList details;
            details << QContactDisplayLabel::DefinitionName;
            nameOnlyFetchHint.setDetailDefinitionsHint(details);*/
            nameOnlyFetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);

            QContact contact = mManager->contact(groupMemberIds.at(i), nameOnlyFetchHint);
            QString memberName = contact.displayLabel();
            if (memberName.isEmpty())
            {
                memberName = mUnnamed;
            }

            // Incase the format is LastnameCommaFirstname, remove the commas
            memberName = memberName.replace(", ", " ");
            nameList << memberName;
            if (nameList.join(", ").length() > CntNamesLengthLimit)
            {
                break;
            }
        }
        QString names = nameList.join(", ").trimmed();

        secondLineText = names;
    }
    else
    {
        if (id == favoriteGroupId)
        {
            secondLineText = mNoFavorites;
        }
        else
        {
            secondLineText = mNoMembers;
        }
    }

    emit fetchDone(id, secondLineText, groupMemberIds);
    
    CNT_EXIT
}

