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
* Description: Model manager holds models for three day views
*
*/

// System includes
#include <calenservices.h>
#include <calencontext.h>

// User includes
#include "calendaymodelmanager.h"
#include "calendaymodel.h"

/*!
 BC Model Manager constructor. Model manager use calendar context to
 populate models with proper events.
 \a services is handle to organizer services \a emptyModels if true created models
 are not populated with events \a parent
 */
CalenDayModelManager::CalenDayModelManager(
    MCalenServices &services,
    bool emptyModels,
    QObject *parent) :
    QObject(parent), mServices(services)
{
    if (emptyModels) {
        mCurrentDayTime = QDateTime();
    }
    else {
        mCurrentDayTime = mServices.Context().focusDateAndTime();
    }
    createAllModels();
}

CalenDayModelManager::~CalenDayModelManager()
{
    // not needed now
}

void CalenDayModelManager::viewsScrollingFinished(CalenScrollDirection scrollTo)
{
    if (scrollTo == ECalenScrollToNext) {
        moveForward();
    }
    else {
        moveBackward();
    }
}

/*!
 Reorganize models after move to previous day.  
 */
void CalenDayModelManager::moveBackward()
{
    mCurrentDayTime = mServices.Context().focusDateAndTime();

    CalenDayModel* tmp = mModels[NextDay];
    tmp->refreshModel(mCurrentDayTime.addDays(-1));

    mModels[NextDay] = mModels[CurrentDay];
    mModels[CurrentDay] = mModels[PreviousDay];
    mModels[PreviousDay] = tmp;
}

/*!
 Reorganize models after move to next day. 
 */
void CalenDayModelManager::moveForward()
{
    mCurrentDayTime = mServices.Context().focusDateAndTime();

    CalenDayModel* tmp = mModels[PreviousDay];
    tmp->refreshModel(mCurrentDayTime.addDays(1));

    mModels[PreviousDay] = mModels[CurrentDay];
    mModels[CurrentDay] = mModels[NextDay];
    mModels[NextDay] = tmp;
}

/*!
 Returns given model
 /a day defines model, can be (PreviousDay, CurrentDay, NextDay) only.  
 */
QAbstractItemModel &CalenDayModelManager::getModel(ModelDay day)
{
    return *(mModels[day]);
}

/*!
 Creates all models objects durring construction.  
 */
void CalenDayModelManager::createAllModels()
{

    mModels[CurrentDay] = new CalenDayModel(mCurrentDayTime, mServices, this);

    QDateTime previousDayTime;
    QDateTime nextDayTime;

    if (mCurrentDayTime.isValid()) {
        previousDayTime = mCurrentDayTime.addDays(-1);
        nextDayTime = mCurrentDayTime.addDays(1);
    }

    mModels[PreviousDay] = new CalenDayModel(previousDayTime, mServices, this);
    mModels[NextDay] = new CalenDayModel(nextDayTime, mServices, this);
}

/*!
 Refetch data for all models. Context calendar is used to fill models
 with correct events. Should be used for full (three days) repopulation.
 */
void CalenDayModelManager::refreshAllModels()
{
    mCurrentDayTime = mServices.Context().focusDateAndTime();

    mModels[PreviousDay]->refreshModel(mCurrentDayTime.addDays(-1));
    mModels[CurrentDay]->refreshModel(mCurrentDayTime);
    mModels[NextDay]->refreshModel(mCurrentDayTime.addDays(1));
}

/*!
 Refetch data given day model. Context calendar is used to fill model
 with correct events.
 /a day defines model, can be (PreviousDay, CurrentDay, NextDay) only.  
 */
void CalenDayModelManager::refreshSingleModel(CalenDayModelManager::ModelDay day)
{
    switch (day) {
        case PreviousDay: {
            mModels[PreviousDay]->refreshModel(mCurrentDayTime.addDays(-1));
        }
            break;
        case CurrentDay: {
            mModels[CurrentDay]->refreshModel(mCurrentDayTime);
        }
            break;
        case NextDay: {
            mModels[NextDay]->refreshModel(mCurrentDayTime.addDays(1));
        }
            break;
        default:
            break;
    }
}

// End of File
