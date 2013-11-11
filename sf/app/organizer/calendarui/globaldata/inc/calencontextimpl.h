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
* Description:  Implementation of calendar context.
*
*/


#ifndef CALENCONTEXTIMPL_H
#define CALENCONTEXTIMPL_H

// FORWARD DECLARATIONS
class MCalenContextChangeObserver;

// INCLUDES
#include "calencontext.h"
#include "caleninstanceid.h"
#include <vwsdef.h>

#ifdef  CALENGLOBALDATA_DLL
#define CALENCONTEXTIMPL_EXPORT Q_DECL_EXPORT
#else
#define CALENCONTEXTIMPL_EXPORT Q_DECL_IMPORT
#endif

// CLASS DEFINITIONS
/**
 * The controller handles events from the rest of Calendar and delegates
 * them to the appropriate place (i.e. the action ui classes).
 */
class CalenContextImpl : public MCalenContext
    {
    public:  // Construction and destruction
        /**
         * The only reason this should be created outside of this dll is for
         * SCalenCommand, which needs a default constructor. Normal usage is
         * to use the accessor from the global data. Attempting to call
         * "setters" on any context not from the global data will panic.
         */
        CALENCONTEXTIMPL_EXPORT CalenContextImpl( MCalenContextChangeObserver* observer );
        CALENCONTEXTIMPL_EXPORT CalenContextImpl();
        CALENCONTEXTIMPL_EXPORT CalenContextImpl( const CalenContextImpl& context );
        CALENCONTEXTIMPL_EXPORT ~CalenContextImpl();

    public:  // from MCalenContext
    // Utils
        /**
         * Returns the default time for views. (Normally 8am.) This would be
         * used for example by the month view, which sets the focus time to
         * 8am on a day. Then when opening the week view, a sane time is shown.
         */
        int defaultTimeForViewsInMinnutes() const;

        /**
         * Returns the default TCalTime for views. (Normally 8am today.) This
         * would be used for example when a view is the first view loaded in
         * Calendar.
         */
        QDateTime defaultCalTimeForViewsL() const;

    // Setters
        /**
         * Sets the date and time currently focused.
         * @param aFocusTime The new focus date and time.
         */
        void setFocusDateAndTime( const QDateTime& focusDateTime);

        /**
         * Sets the date currently focused. When retrieving the focus
         * date and time after calling this function, the time component
         * will be set to the default of view. When retrieving the focus
         * time only, it will be set to -1.
         * @param aFocusDate The new focus date and time.
         */
        void setFocusDate( const QDateTime& focusDateTime );

        /**
         * Sets the id of the instance currently focused.
         * @param aInstanceId the id of the focused instance.
         */
        void setInstanceId( const TCalenInstanceId& instanceId );

        /**
         * Sets the time and instance currently focused.
         * @param aFocusTime The new focus time. If no time is focused, set
         * the Utc time of this object to be Time::NullTTime.
         * @param aInstanceId the id of the focused instance.
         */
        void setFocusDateAndTimeAndInstance( const QDateTime& focusDateTime,
                                              const TCalenInstanceId& aInstanceId);

    // Getters
        /**
         * Gets the date and time currently focused.
         * @return The currently focused date and time. When no time is
         * focused, the default time on the current date will be returned.
         */
        QDateTime focusDateAndTime() const;

        /**
         * Gets the time currently focused.
         * @return The currently focused time. When no time is
         * focused, -1 will be returned.
         */
        int focusTime() const;

        /**
         * Gets the id of the instance currently focused.
         * @return The instance id currently focused. When no instance is focused,
         * this will be TCalenInstanceId::NullInstanceIdL()
         */
        TCalenInstanceId instanceId() const;

        
public:	// Multiple Context support
		
	   /**
	    * Set multiple context ids
	    *
	    * @param aMutlipleContextIds
	    */	
		 void setMutlipleContextIds(QList<TCalenInstanceId>& mutlipleContextIds);
		
	   /**
	    * Remove multiple context id 
	    * 
	    * @param aInstanceId Instance Id for which context to be removed
	    */
		 void removeMultipleContextId(TCalenInstanceId instanceId);
		
	   /**
	    * Resets all the multiple context ids
	    * 
	    */
		 void resetMultipleContextIds(int dbId=0);
		
	   /**
	    * Getter for multiple context ids
	    *
	    * @return RArray<TCalenInstanceId>&
	    */
		 QList<TCalenInstanceId>& getMutlipleContextIds(int dbId=0);
		
	   /**
	    * Returns mutliple context's count
	    *
	    * @return 
	    */
		 int mutlipleContextIdsCount();
				
    private:  // Data
        MCalenContextChangeObserver* mObserver;
        QDateTime mFocusDate;
        int mFocusTime;
        TCalenInstanceId mInstanceId;
        int mViewId;        
        // Multiple context ids
        QList<TCalenInstanceId> mMutlipleContextIds;
        
    };

#endif // CALENCONTEXTIMPL_H

// End of file
