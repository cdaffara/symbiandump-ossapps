/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the base class of all Calendar views.
 *
*/


#ifndef CALENNATIVEVIEW_H
#define CALENNATIVEVIEW_H

#ifdef  CALENVIEWS_DLL
#define CALENNATIVEVIEW_EXPORT Q_DECL_EXPORT
#else
#define CALENNATIVEVIEW_EXPORT Q_DECL_IMPORT
#endif

//  System includes
#include <QObject>
#include <QPointer>
#include <hblabel.h>
#include <hblistwidget.h>
#include <hbabstractviewitem.h>

// User includes
#include "calennotificationhandler.h"   // MCalenNotificationHandler
#include "calenview.h"

const int WORKAROUND_TO_LIMIT_MAX_SPEED=8000;

class MCalenServices;
class HbDateTimePicker;
class AfActivityStorage;

class  CalenNativeView : public CalenView,
                        public MCalenNotificationHandler
    {
    Q_OBJECT
    
	public:  // Constructors and destructor
        /**
         * Destructor.
         */
	    virtual ~CalenNativeView();
	    virtual void populationComplete();
		TBool pluginEnabled();
		virtual void refreshViewOnGoToDate();
		QString *pluginText();
	    /**
	     * captureScreenshot captures the current screenshot
	     */
		CALENNATIVEVIEW_EXPORT void captureScreenshot(bool captureScreenShot = false);
		
	protected:  // New functions
	    CalenNativeView( MCalenServices& services );
	    
	    void HandleNotification( const TCalenNotification notification );
	    virtual void onLocaleChanged(int reason)=0;
	    virtual void onContextChanged() {};
	    virtual bool checkIfWeCanSwipe(QDateTime& date, bool rightGesture);
	    /**
	     * removeActivity  removes the current view from the  activity
	     * 
	     */
	    bool removeActivity();
	    
	signals:
			void closeDialogs();

	protected slots:
	
        void goToDate();
        void goToSelectedDate();
        void deleteBeforeDate();
        void deleteAllEntries();
        void launchSettingsView();
	    virtual void changeOrientation(Qt::Orientation orientation);
	    /**
	     * saveActivity saves the current view as an activity
	     * 
	     */
	    void saveActivity();
     private:
	void refreshDatePicker();
	
    protected:
        AfActivityStorage *mActivityStorage;
        MCalenServices	&mServices; // not owned.
        int             mActivityId; // Recent Activity ID, currently it holdes wither of ECalenMonthView or ECalenAgendaView
        QVariantHash    mScreenShotMetadata; // Screenshot
        bool mEntriesInDataBase;
    private:
        QPointer<HbDateTimePicker> mDatePicker;
        bool                mIsCapturedScreenShotValid; // to check if the captured screenshot is valid
	};

#endif  // CALENNATIVEVIEW_H

// End of file
