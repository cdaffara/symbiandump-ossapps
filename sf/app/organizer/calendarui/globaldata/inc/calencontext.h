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

#include "caleninstanceid.h"

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
    public:  // New functions
    // Utils
        /**
         * Returns the default time for views. (Normally 8am.) This would be
         * used for example by the month view, which sets the focus time to
         * 8am on a day. Then when opening the week view, a sane time is shown.
         */
        virtual int defaultTimeForViewsInMinnutes() const = 0;

        /**
         * Returns the default TCalTime for views. (Normally 8am today.) This
         * would be used for example when a view is the first view loaded in
         * Calendar.
         */
        virtual QDateTime defaultCalTimeForViewsL() const = 0;

    // Setters
        /**
         * Sets the date and time currently focused.
         * @param aFocusTime The new focus date and time.
         */
        virtual void setFocusDateAndTime( const QDateTime& focusDateTime) = 0;

        /**
         * Sets the date currently focused. When retrieving the focus
         * date and time after calling this function, the time component
         * will be set to the default of view. When retrieving the focus
         * time only, it will be set to -1.
         * @param aFocusDate The new focus date and time.
         */
        virtual void setFocusDate( const QDateTime& focusDateTime) = 0;

        /**
         * Sets the id of the instance currently focused.
         * @param aInstanceId the id of the focused instance.
         */
        virtual void setInstanceId( const TCalenInstanceId& instanceId ) = 0;

        /**
         * Sets the time and instance currently focused.
         * @param aFocusTime The new focus time. If no time is focused, set
         * the Utc time of this object to be Time::NullTTime.
         * @param aInstanceId the id of the focused instance.
         */
        virtual void setFocusDateAndTimeAndInstance( const QDateTime& focusDateTime,
                                                      const TCalenInstanceId& aInstanceId ) = 0;

    // Getters
        /**
         * Gets the date and time currently focused.
         * @return The currently focused date and time. When no time is
         * focused, the default time on the current date will be returned.
         */
        virtual QDateTime focusDateAndTime() const = 0;
	

        /**
         * Gets the time currently focused.
         * @return The currently focused time. When no time is
         * focused, -1 will be returned.
         */
        virtual int focusTime() const = 0;

        /**
         * Gets the id of the instance currently focused.
         * @return The instance id currently focused. When no instance is focused,
         * this will be TCalenInstanceId::NullInstanceIdL()
         */
        virtual TCalenInstanceId instanceId() const = 0;

 public: // For Mutliple Context Support    
 
     /**
	    * Set multiple context ids
	    *
	    * @param aMutlipleContextIds
	    */	
		 virtual void setMutlipleContextIds( QList<TCalenInstanceId>& mutlipleContextIds)  = 0;
		
	   /**
	    * Remove multiple context id 
	    * 
	    * @param aInstanceId Instance Id for which context to be removed
	    */
		 virtual void removeMultipleContextId(TCalenInstanceId instanceId) = 0;
		
	   /**
	    * Resets all the multiple context ids
	    * 
	    */
		 virtual void resetMultipleContextIds(int dbId = 0) = 0;
		
	   /**
	    * Getter for multiple context ids
	    *
	    * @return RArray<TCalenInstanceId>&
	    */
		 virtual QList<TCalenInstanceId>& getMutlipleContextIds(int dbId = 0) = 0;
		
	   /**
	    * Returns mutliple context's count
	    *
	    * @return 
	    */
		virtual int mutlipleContextIdsCount() = 0;
		
    };

#endif // CALENCONTEXT_H

// End of file
