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
* Description: CalenDayView class definition.
*
*/

#ifndef CALEDAYVIEW_H
#define CALEDAYVIEW_H

// System includes
#include <QDateTime>
#include <xqsettingskey.h>

// User includes
#include "calennativeview.h"
#include "calendaycommonheaders.h"
#include "hb_calencommands.hrh"      // command ids

// Forward declarations
class QGraphicsLinearLayout;
class XQSettingsManager;
class QVariant;
class HbGroupBox;
class HbFrameItem;
class AgendaEntry;
class CalenDocLoader;
class CalenDayContentScrollArea;
class CalenDayContentWidget;
class CalenDayHourScrollArea;
class CalenDayModelManager;


// Constants


#ifdef  CALENVIEWS_DLL
#define CALENDAYVIEW_EXPORT Q_DECL_EXPORT
#else
#define CALENDAYVIEW_EXPORT Q_DECL_IMPORT
#endif

class CalenDayView : public CalenNativeView
{
    Q_OBJECT

public:
    
    /**
     * Constructor.
     * 
     * @param services Calendar services.
     */
    CALENDAYVIEW_EXPORT CalenDayView( MCalenServices &services );
    
    /**
     * Destructor.
     */
    CALENDAYVIEW_EXPORT virtual ~CalenDayView();
    
public:

//From CalenNativeView
    
    /**
     * Handles locale change.
     * 
     * @param reason Change reason.
     */    
    virtual void onLocaleChanged(int reason);
    
//From CalenView
    
    /**
     * Handles view (re)population.
     * 
     */
    CALENDAYVIEW_EXPORT virtual void doPopulation();
    
    /**
     * Informs the organizer that the view's population is complete.
     */
    virtual void populationComplete();
   
public:
    //API
    
    /**
     * Sets up the view accroding to the 'xml'
     * 
     * @param docLoader Loaded document.
     */
    CALENDAYVIEW_EXPORT void setupView(CalenDocLoader* docLoader);

private slots:

    /**
     * Handles 'back' functionality;
     */
    void onBack();
    
    /**
     * Updates selected date member and context
     */
    void dayChangeStarted(CalenScrollDirection direction);
    
    /**
     * Refreshes the model
     */
    void dayChanged(CalenScrollDirection direction);
    
    /**
     * Sets new date as heading text. Triggerd after fadeOut effect.
     */
    void setHeadingText(const HbEffect::EffectStatus &status);
    
    /**
     * Handles actions from menu
     */
    void runNewMeeting();
    void runGoToToday();
    void runChangeToAgendaView();
    void runLunarData();
    
    void showRegionalInformation(const HbEffect::EffectStatus &status);
    void showRegionalInformationFadeIn();
    void showHideRegionalInformationChanged(const XQSettingsKey& key, const QVariant& var);

public slots:
	void changeView(TCalenCommandId viewId);

private slots:
    /**
     * Scrolls content to correct position after view repopulation
     */
    void setupViewport();
    void orientationChanged(Qt::Orientation orientation);

private:
    /**
     * 
     */
    void getCurrentDate();

    /**
     * setup menu for the view
     */
    void setupMenu();
    
    /**
     * Connects view's slots.
     */
    void setupSlots();
    
    void initializeViews();
    
    void isDateValid();
    
private: // members related to view management and gesture handling
    
    CalenDayModelManager* mModelManager;
    
    CalenDayContentScrollArea* mContentScrollArea;  //!< Scroll area for content
    CalenDayContentWidget* mContentWidget;    //!< Content widget
    CalenDayHourScrollArea* mHourScrollArea;  //!< Scroll area for hour elements
    QGraphicsLinearLayout* mVLayout;    //!< Vertical layout for day view
    HbWidget* mMainContainer;//!< Container widget for hour elements and content
    
    CalenDocLoader* mDocLoader; //!< Document loader for day view
    
    bool mIsLaunching;  //!< Indicates that view is launching first time
    HbWidget *mGesturesAbsorber; //!<Widget that absorbs swipe and pan gestures
    
private:    

    XQSettingsManager* mSettingsManager;
    XQSettingsKey mRegionalInfoKey;
    MCalenServices  &mServices;
	
    /**
     * Current date/time.
     */
    QDateTime mDate;
    
    HbGroupBox *mHeadingLabel;
    
    HbGroupBox *mRegionalInfoGroupBox;
    /*!
     * \brief It keep pointer to go to Today action from menu. 
     */
    HbAction *mGoToTodayMenuAction;
    
    HbFrameItem *mBg;   //!< Background graphic for content and hour elements
};


#endif // CALEDAYVIEW_H

//End of file
