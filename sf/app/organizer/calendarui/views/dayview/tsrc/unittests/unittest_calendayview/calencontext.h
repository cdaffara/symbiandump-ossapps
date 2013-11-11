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

#ifndef CALENCONTEXT_H
#define CALENCONTEXT_H
#include <QDateTime>

// FORWARD DECLARATIONS
class TCalenInstanceId;
class QDateTime;

// INCLUDES

// CLASS DEFINITIONS
/**
 * The controller handles events from the rest of Calendar and delegates
 * them to the appropriate place (i.e. the action ui classes).
 */
class MCalenContext
{
public:
	QDateTime focusDateAndTime()
		{
		return mFocusedDate;
		}

	void setFocusDate(const QDateTime& focusDateTime)
		{
	    mFocusedDate = focusDateTime;;
		}

	void setFocusDateAndTime(const QDateTime& focusDateTime)
		{
		mFocusedDate = focusDateTime;
		}

	QDateTime defaultCalTimeForViewsL()
		{
		return QDateTime(QDate(2000, 10, 10));
		}
    
	void setFocusDateAndTimeAndInstance( const QDateTime& focusDateTime,
                                         const TCalenInstanceId& aInstanceId )
    {
        Q_UNUSED(aInstanceId);
        mFocusedDate = focusDateTime;
    }

private:
    QDateTime mFocusedDate;
	
};



#endif // CALENCONTEXT_H
// End of file
