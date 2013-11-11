/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar context, info on what's currently focused
*
*/


//debug
#include "calendarui_debug.h"
#include "calencontextimpl.h"
#include "calencontextchangeobserver.h"
#include "calendateutils.h"
//#include <lbsposition.h>

const int KCalenDefaultTimeForViews = 480;  // 480 minutes == 8 hours == 8 am.

// ----------------------------------------------------------------------------
// CalenContextImpl::CalenContextImpl
// Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CalenContextImpl::CalenContextImpl( MCalenContextChangeObserver* observer )
: mObserver( observer ),
  mFocusTime( -1 ),
  mInstanceId( TCalenInstanceId::nullInstanceId() )
    {
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::CalenContextImpl
// Default Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CalenContextImpl::CalenContextImpl()
: mInstanceId( TCalenInstanceId::nullInstanceId() )
    {
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::CalenContextImpl
// Copy Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CalenContextImpl::CalenContextImpl( const CalenContextImpl& aContext )
    {
    // When copying, set the observer to NULL because the only context that
    // should be set is accessed from the global data.
    mObserver = NULL;
    mFocusTime = aContext.mFocusTime;
    mInstanceId = aContext.mInstanceId;
    mViewId = aContext.mViewId;
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::~CalenContextImpl
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CalenContextImpl::~CalenContextImpl()
	{
	if(mMutlipleContextIds.count())
		{
		mMutlipleContextIds.clear();
		}
	}

// ----------------------------------------------------------------------------
// CalenContextImpl::DefaultTimeForViews
// If a view knows what day they should be looking at, but doesn't know what
// time (for example the month view only deals in days), they should set the
// time to this value.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
int CalenContextImpl::defaultTimeForViewsInMinnutes() const
    {
    return KCalenDefaultTimeForViews;
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::DefaultCalTimeForViewsL
// If a view has no preference as to what time/day to be focused on (e.g. when
// a view is the first view loaded), they should set focus to this TCalTime.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
QDateTime CalenContextImpl::defaultCalTimeForViewsL() const
    {
    QDateTime ret = CalenDateUtils::today();
    QTime currentTime = ret.time();
    currentTime.setHMS(7, 0, 0, 0);
    ret.setTime(currentTime);

    return ret;
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::SetFocusDateAndTimeL
// Sets the focus time
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenContextImpl::setFocusDateAndTime( const QDateTime& focusDateTime)
    {

    mFocusDate = focusDateTime;
    mFocusTime = focusDateTime.time().minute() + 1;
    
    if( mFocusTime < 0 )
        mFocusTime = 0;
    mInstanceId = TCalenInstanceId::nullInstanceId();

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    if(mObserver)
        mObserver->ContextChanged();
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::SetFocusDateL
// Sets the focus date
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenContextImpl::setFocusDate( const QDateTime& focusDateTime)
    {
    mFocusDate = focusDateTime;
    mFocusTime = -1;
    mInstanceId = TCalenInstanceId::nullInstanceId();

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    if(mObserver)
        mObserver->ContextChanged();
    }

// ----------------------------------------------------------------------------
// CalenContextImpl::SetInstanceIdL
// Sets the instance id
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenContextImpl::setInstanceId( const TCalenInstanceId& aInstanceId )
    {

    // Set the null date and time
    mFocusDate = QDateTime();
    mFocusTime = -1;
    mInstanceId = aInstanceId;

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    if(mObserver)
        mObserver->ContextChanged();
    }

// -----------------------------------------------------------------------------
// CalenContextImpl::SetFocusDateAndTimeAndInstanceL
// Sets the currently focused time and instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenContextImpl::setFocusDateAndTimeAndInstance( const QDateTime& focusDateTime,
                                                              const TCalenInstanceId& instanceId )
    {
    mFocusDate = focusDateTime;
    mFocusTime = focusDateTime.time().minute() + 1;
    if( mFocusTime < 0 )
		mFocusTime = 0;
    mInstanceId = instanceId;

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    if(mObserver)
        mObserver->ContextChanged();
    }

// -----------------------------------------------------------------------------
// CalenContextImpl::FocusDateAndTimeL
// Returns the focus time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
QDateTime CalenContextImpl::focusDateAndTime() const
    {
    QDateTime ret;

    if( mFocusDate.isValid() )
        {
        ret = mFocusDate;
        }
    else
        {
        ret = mInstanceId.mInstanceTime;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CalenContextImpl::FocusTime
// Returns the focus time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
int CalenContextImpl::focusTime() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return mFocusTime;
    }

// -----------------------------------------------------------------------------
// CalenContextImpl::InstanceId
// Returns the instance id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalenInstanceId CalenContextImpl::instanceId() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return mInstanceId;
    }

// -----------------------------------------------------------------------------
// CalenContextImpl::SetMutlipleContextIds
// Set multiple context ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenContextImpl::setMutlipleContextIds(
									QList<TCalenInstanceId>& mutlipleContextIds)
    {
    
    mMutlipleContextIds.clear();
    mMutlipleContextIds = mutlipleContextIds;
  
	}

// -----------------------------------------------------------------------------
// CalenContextImpl::RemoveMultipleContextId
// Remove multiple context id 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	  
void CalenContextImpl::removeMultipleContextId(TCalenInstanceId instanceId)
	{
		
	for(int index = 0;index < mMutlipleContextIds.count();index++)
		{
		if(instanceId == mMutlipleContextIds[index])
			{
			mMutlipleContextIds.removeAt(index);
			}
		}
	
    }
		
// -----------------------------------------------------------------------------
// CalenContextImpl::ResetMultipleContextIds
// Resets all the multiple context ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CalenContextImpl::resetMultipleContextIds(TInt /*aDbId*/)
	{
		
	if(mMutlipleContextIds.count())
		{
		mMutlipleContextIds.clear();
		}
	
    }
		
// -----------------------------------------------------------------------------
// CalenContextImpl::GetMutlipleContextIds
// Getter for multiple context ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
QList<TCalenInstanceId>& CalenContextImpl::getMutlipleContextIds(TInt /*aDbId*/)
	{	
	return mMutlipleContextIds;
	}
		
// -----------------------------------------------------------------------------
// CalenContextImpl::MutlipleContextIdsCount
// Returns mutliple context's count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
int CalenContextImpl::mutlipleContextIdsCount()
	{	
	return mMutlipleContextIds.count();
	}
// End of file
