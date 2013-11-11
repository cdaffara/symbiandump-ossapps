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

// System includes
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <xqsettingsmanager.h>
#include <HbWidget>
#include <HbFrameItem>
#include <HbInstance>
#include <HbAction>
#include <HbMenu>
#include <HbMainWindow>
#include <HbModelIterator>
#include <HbStyleLoader>
#include <HbGroupBox>
#include <HbExtendedLocale>
#include <HbModelIterator>
#include <agendautil.h>

// User includes
#include "calendayview.h"
#include "calencommon.h"
#include "calencontext.h"
#include "calenservices.h"
#include "calendocloader.h"
#include "calendateutils.h" //useful date/time utils
#include "calendaycontentscrollarea.h"
#include "calendaycontentwidget.h"
#include "calendayhourscrollarea.h"
#include "calendayitemview.h"
#include "calendaymodelmanager.h"
#include "CalenUid.h"
#include "CalendarPrivateCRKeys.h"
#include "calenpluginlabel.h"
#include "calendaymodel.h"
#include "calendayutils.h"

//constants


/*!
 \brief Constructor
*/
CalenDayView::CalenDayView(MCalenServices &services) :
    CalenNativeView(services), mContentScrollArea(NULL), mContentWidget(NULL),
        mHourScrollArea(NULL), mVLayout(NULL), mMainContainer(NULL),
        mDocLoader(NULL), mIsLaunching(true), mSettingsManager(NULL),
        mRegionalInfoKey(XQSettingsKey::TargetCentralRepository,
            KCRUidCalendar, KCalendarShowRegionalInfo), mServices(services),
        mRegionalInfoGroupBox(NULL), mGoToTodayMenuAction(NULL), mBg(NULL),
        mGesturesAbsorber(NULL)
{
    setupMenu();

    // Create model manager
    mModelManager = new CalenDayModelManager(mServices, true, this);
    mSettingsManager = new XQSettingsManager(this);
    mSettingsManager->startMonitoring(mRegionalInfoKey);

    //setup Back functionality, launch the month view only when it is
	// the first view
    if (ECalenMonthView == mServices.getFirstView()) {
        HbAction* action = new HbAction(Hb::BackNaviAction, this);
        setNavigationAction(action);
        // Connect to the signal triggered by clicking on back button.
        connect(action, SIGNAL(triggered()), this, SLOT(onBack()));
    }

    HbStyleLoader::registerFilePath(":/calendayhourelement.css");
    HbStyleLoader::registerFilePath(":/calendayhourelement.widgetml");
    HbStyleLoader::registerFilePath(":/calendayitem.css");
    HbStyleLoader::registerFilePath(":/calendayitem.widgetml");
    HbStyleLoader::registerFilePath(":/calendayeventspane.css");
    HbStyleLoader::registerFilePath(":/calendayhourscrollarea.css");
}

/*!
 \brief Destructor
*/
CalenDayView::~CalenDayView()
{
    mSettingsManager->stopMonitoring(mRegionalInfoKey);
    
    if (mRegionalInfoGroupBox) {
        delete mRegionalInfoGroupBox;
        mRegionalInfoGroupBox = NULL;
    }
    
    if (mDocLoader) {
        delete mDocLoader;
        mDocLoader = NULL;
    }
}

/*!
 \brief Handles locale change.
 
 \param reason the reason of a change
*/
void CalenDayView::onLocaleChanged(int reason)
{
    Q_UNUSED( reason )
    if ((reason & EChangesSystemTime) || (reason & EChangesMidnightCrossover)) {
        mDate = CalenDateUtils::today();
    }
    
    if ((reason & EChangesLocale)) {
        mHourScrollArea->localeChanged();
    }
}

/*!
 \brief Reimplemented from CalenView. Handles view (re)population
*/
void CalenDayView::doPopulation()
{
    // Triggers fading effect for heading label
    getCurrentDate();
    HbEffect::start(mHeadingLabel, "fadeOut", this, "setHeadingText");
    
    mModelManager->refreshAllModels();
    //Set date and time for hour scroll area. 
    //It's later used by hour element to display timeline
    mHourScrollArea->setDateTime(mDate);
    
    isDateValid();
    
    //set in menu go to today visible
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (mGoToTodayMenuAction and currentDateTime.date() == mDate.date()) {
        mGoToTodayMenuAction->setVisible(false);
    }
    else if(mGoToTodayMenuAction) {
        mGoToTodayMenuAction->setVisible(true);
    }
    
    // Call async. if the view is loaded first time (fix to ou1cimx1#482516)
    if (mIsLaunching) {
        mIsLaunching = false;
        QMetaObject::invokeMethod(this, "setupViewport", Qt::QueuedConnection);
    } else {
        setupViewport();
    }
    
    //Set gestures absorber
    if (!mGesturesAbsorber) {
        mGesturesAbsorber = new HbWidget(this);
        mGesturesAbsorber->setZValue(100);
        mGesturesAbsorber->grabGesture(Qt::SwipeGesture);
        mGesturesAbsorber->grabGesture(Qt::PanGesture);
        mGesturesAbsorber->setGeometry(rect());
        mGesturesAbsorber->installEventFilter(mContentScrollArea);
    }
	
    populationComplete();
}

/*!
 \brief Reimplemented from CalenView. Informs the organizer that the view's population is complete.
*/
void CalenDayView::populationComplete()
{
    CalenNativeView::populationComplete();
}

/*!
 \brief Sets up the view accroding to the 'xml'
 
 \param docLoader Pointer to document loader
*/
void CalenDayView::setupView(CalenDocLoader* docLoader)
{
    // Store document loader for further use
    mDocLoader = docLoader;

    // Get vertical layout from day view
    mVLayout = static_cast<QGraphicsLinearLayout *> (this->layout());

    // Set up day info
    mHeadingLabel = qobject_cast<HbGroupBox *> (mDocLoader->findWidget(
        CALEN_DAYVIEW_DAYINFO));
    HbEffect::add(mHeadingLabel, ":/fade_out.fxml", "fadeOut");
    HbEffect::add(mHeadingLabel, ":/fade_in.fxml", "fadeIn");
    
    // Set up main container for hour elements/content area
    mMainContainer = qobject_cast<HbWidget *> (mDocLoader->findWidget(
        CALEN_DAYVIEW_CONTENTWIDGET));
    if (Qt::Vertical == CalenDayUtils::instance()->orientation()) {
        mBg = new HbFrameItem(KCalenBackgroundColorPortrait,
            HbFrameDrawer::OnePiece, this);
    }
    else {
        mBg = new HbFrameItem(KCalenBackgroundColorLandscape,
            HbFrameDrawer::OnePiece, this);
    }
    mMainContainer->setBackgroundItem(mBg);

    // Set up hour scroll area
    mHourScrollArea
        = static_cast<CalenDayHourScrollArea *> (mDocLoader->findWidget(
            CALEN_DAYVIEW_HOURSCROLLAREA));

    // Set up content scroll area
    mContentScrollArea
        = static_cast<CalenDayContentScrollArea *> (mDocLoader->findWidget(
            CALEN_DAYVIEW_CONTENTSCROLLAREA));
    // Pass parent object to mContentWidget to install event filters on parent
    mContentWidget = new CalenDayContentWidget(mContentScrollArea);
    mContentScrollArea->setContentWidget(mContentWidget);
    
    initializeViews();
    setupSlots();
    
    // Set up regional info if variant is correct
    showRegionalInformationFadeIn();
}

//private slots

/*!
 \brief Handles 'back' functionality
*/
void CalenDayView::onBack()
{
    TRAP_IGNORE(mServices.IssueCommandL(ECalenMonthView));
}

/*!
 \brief Slot that handles first phase of day change
 
 \param direction indicates to which day view needs to be scrolled (previous or next day)
*/
void CalenDayView::dayChangeStarted(CalenScrollDirection direction)
{
    switch (direction) {
        case ECalenScrollToNext:
            mDate = mDate.addDays(1);
            break;
        case ECalenScrollToPrev:
            mDate = mDate.addDays(-1);
            break;
        default:
            break;
    }
    
    // Update the visibility of 'Go to today' option in menu
	QDateTime currentDateTime = QDateTime::currentDateTime();
    if (mGoToTodayMenuAction and currentDateTime.date() == mDate.date()) {
        mGoToTodayMenuAction->setVisible(false);
    }
    else if(mGoToTodayMenuAction) {
        mGoToTodayMenuAction->setVisible(true);
    }
    
    // Triggers fading effect for heading label.
    HbEffect::start(mHeadingLabel, "fadeOut", this, "setHeadingText");
    if (mRegionalInfoGroupBox) {
        HbEffect::start(mRegionalInfoGroupBox, "fadeOut", this,
            "showRegionalInformation");
    }
    
    mServices.Context().setFocusDate(mDate);
}

/*!
 \brief Slot that is triggered when operation of day change is completed
 
 \param direction ndicates to which day view was scrolled (previous or next day)
*/
void CalenDayView::dayChanged(CalenScrollDirection direction)
{
    mModelManager->viewsScrollingFinished(direction);
	mHourScrollArea->setDateTime(mDate);
	mHourScrollArea->updateTimeIndicator();
	isDateValid();
}

/*!
 \brief Gets current date from context
*/
void CalenDayView::getCurrentDate()
{
    mDate = CalenNativeView::mServices.Context().focusDateAndTime();
}

/*!
 \brief Sets the menu for day view
*/
void CalenDayView::setupMenu()
{
	menu()->addAction(hbTrId("txt_calendar_opt_new_event"), this, SLOT(runNewMeeting()));
	//get pointer to this position, because need to change visibility
	mGoToTodayMenuAction = menu()->addAction(hbTrId("txt_calendar_opt_go_to_today"), this, SLOT(runGoToToday()));
	menu()->addAction(hbTrId("txt_calendar_opt_go_to_date"), this, SLOT(goToDate()));
	//TODO: Add id for this text
	//"Switch to Agenda view"
	menu()->addAction(hbTrId("txt_calendar_opt_switch_to_agenda_view"), this, SLOT(runChangeToAgendaView()));
	//TODO: Add id for this text (lunar data)
	//"Show lunar data"
	if (pluginEnabled())
	{
		menu()->addAction(hbTrId("txt_calendar_opt_show_lunar_data"), this, SLOT(runLunarData()));
	}
	menu()->addAction(hbTrId("txt_calendar_opt_settings"), this, SLOT(launchSettingsView()));
	
	// Close the menu once closeDialogs() is received
	connect(this, SIGNAL(closeDialogs()), menu(), SLOT(close()));
}

/*!
   \brief To change Day view to Agenda View
 */
void CalenDayView::runChangeToAgendaView()
{
    changeView(ECalenAgendaView);
}

/*!
   \brief Shows lunar data in popup box
 */
void CalenDayView::runLunarData()
{
	TRAP_IGNORE(mServices.IssueCommandL(ECalenRegionalPluginTapEvent));
}

/*!
   \brief This is a helper function to established connections between signals and slots
*/
void CalenDayView::setupSlots()
{
    // Connecting other view-related signals/slots
    connect(mContentScrollArea,
        SIGNAL(scrollAreaMoveStarted(CalenScrollDirection)), this,
        SLOT(dayChangeStarted(CalenScrollDirection)));

    connect(mContentScrollArea,
        SIGNAL(scrollAreaMoveFinished(CalenScrollDirection)), mContentWidget,
        SLOT(relayoutWidgets(CalenScrollDirection)));

    connect(mContentWidget, SIGNAL(
        widgetsRelayoutFinished(CalenScrollDirection)), mContentScrollArea,
        SLOT(scrollToMiddleWidget()));

    connect(mContentWidget,
        SIGNAL(widgetsRelayoutFinished(CalenScrollDirection)), this,
        SLOT(dayChanged(CalenScrollDirection)));

    connect(mContentWidget, SIGNAL(scrollPositionChanged(const QPointF &)),
        mHourScrollArea, SLOT(scrollVertically(const QPointF &)));

    connect(mHourScrollArea, SIGNAL(scrollPositionChanged(const QPointF &)),
        mContentWidget, SLOT(widgetScrolled(const QPointF &)));

    connect(CalenDayUtils::instance()->mainWindow(),
        SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(orientationChanged(Qt::Orientation)));

    connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(showHideRegionalInformationChanged(XQSettingsKey, QVariant)));
}

/*!
   \brief Initializes internal views.
   
   CalenDayView uses 3 child views which represent previous, current and next days.
   This assures that swiping to prev/next day works smoothly, because population
   of data is done earlier.
*/
void CalenDayView::initializeViews()
{
    // Create item views
    HbModelIterator *iterator(0);

    iterator = new HbModelIterator(&mModelManager->getModel(
        CalenDayModelManager::PreviousDay));
    CalenDayItemView *prevItemView = new CalenDayItemView(
        mModelManager->getServices(), iterator, 0);

    iterator = new HbModelIterator(&mModelManager->getModel(
        CalenDayModelManager::CurrentDay));
    CalenDayItemView *currItemView = new CalenDayItemView(
        mModelManager->getServices(), iterator, 0);

    iterator = new HbModelIterator(&mModelManager->getModel(
        CalenDayModelManager::NextDay));
    CalenDayItemView *nextItemView = new CalenDayItemView(
        mModelManager->getServices(), iterator, 0);

    // Connect views with widgetScrolled SLOT to support simultanous vertical scrolling
    connect(prevItemView, SIGNAL(scrollPositionChanged(const QPointF&)), mContentWidget,
        SLOT(widgetScrolled(const QPointF&)));
    connect(currItemView, SIGNAL(scrollPositionChanged(const QPointF&)), mContentWidget,
        SLOT(widgetScrolled(const QPointF&)));
    connect(nextItemView, SIGNAL(scrollPositionChanged(const QPointF&)), mContentWidget,
        SLOT(widgetScrolled(const QPointF&)));
    
    // Close menu once closeDialogs() is received
    connect(this, SIGNAL(closeDialogs()), prevItemView->contextMenu(), SLOT(close()));
    connect(this, SIGNAL(closeDialogs()), currItemView->contextMenu(), SLOT(close()));
    connect(this, SIGNAL(closeDialogs()), nextItemView->contextMenu(), SLOT(close()));

    // Install event filters to receive events necessary for gesture handling
    prevItemView->installEventFilter(mContentScrollArea);
    currItemView->installEventFilter(mContentScrollArea);
    nextItemView->installEventFilter(mContentScrollArea);

    // Add views to layout
    mContentWidget->add(prevItemView);
    mContentWidget->add(currItemView);
    mContentWidget->add(nextItemView);
}

/*!
 \brief Checks current date and if previous/next days are valid.
 
 Checks if previous/next day is in range of supported dates: 1900-01-01 - 2100-12-30
 */
void CalenDayView::isDateValid()
{
    // Fix to ou1cimx1#583805: user shouldnot be able to scroll 
    // before 01/01/1900 and after 30/12/2100
    mContentScrollArea->setDisallowedScrollDirection(ECalenScrollNoDayChange);
    if (!CalenDateUtils::isValidDay(mDate.addDays(-1))) {
        mContentScrollArea->setDisallowedScrollDirection(ECalenScrollToPrev);
    }
    else
        if (!CalenDateUtils::isValidDay(mDate.addDays(1))) {
            mContentScrollArea->setDisallowedScrollDirection(ECalenScrollToNext);
        }
}

/*!
   \brief This slot triggers new meeting creation view
*/
void CalenDayView::runNewMeeting()
{
    QDateTime dateTime(mDate);
    TRAP_IGNORE(
        dateTime.setTime(mServices.Context().defaultCalTimeForViewsL().time());
        mServices.Context().setFocusDateAndTime(dateTime);
        mServices.IssueCommandL(ECalenNewMeeting)
    );
}

/*!
   \brief This slot switches current view to today
*/
void CalenDayView::runGoToToday()
{
    mServices.Context().setFocusDateAndTime(CalenDateUtils::today());
    TRAP_IGNORE(mServices.IssueCommandL(ECalenGotoToday));
    refreshViewOnGoToDate();
}


/*!
   \brief This slot switches current view to the given by id
   
   \param viewId id of the view that needs to be displayed
*/
void CalenDayView::changeView(TCalenCommandId viewId)
{
    TRAP_IGNORE(mServices.IssueCommandL(viewId));
}

/*!
   \brief Sets heading text according to date from model and locale.
          It's connected to modelReset signal.
   
   \param status Parameter required in order to call this slot autmatically
                 when an effect is complete.
*/
void CalenDayView::setHeadingText(const HbEffect::EffectStatus &status)
{   
    Q_UNUSED(status)

    // Format the date as per the device locale settings
    HbExtendedLocale systemLocale = HbExtendedLocale::system();

    // Get localised name of the day from locale
    QString dayString = systemLocale.dayName(mDate.date().dayOfWeek());
    // Append a single space
    dayString.append(" ");
    // Set the heading

    // Append the date which is formatted as per the locale   
    mHeadingLabel->setHeading(QString("%1 %2").arg(dayString).arg(
        systemLocale.format(mDate.date(), r_qtn_date_usual_with_zero)));

    HbEffect::start(mHeadingLabel, "fadeIn");
}

/*!
   \brief Displays regional information
   
   \param status Parameter required in order to call this slot autmatically
                 when an effect is complete.
*/
void CalenDayView::showRegionalInformation(const HbEffect::EffectStatus &status)
{
	Q_UNUSED(status);
	showRegionalInformationFadeIn();
}

/*!
   \brief Runs effect on lunar data label and change text according to date change.
*/
void CalenDayView::showRegionalInformationFadeIn()
{
	showHideRegionalInformationChanged(mRegionalInfoKey, 3);
	if (mRegionalInfoGroupBox) {
	    HbEffect::start(mRegionalInfoGroupBox, "fadeIn");
	}
}

/*!
   \brief To Show and hide regional plugin label depends upon settings.
*/
void CalenDayView::showHideRegionalInformationChanged(
    const XQSettingsKey& key,
    const QVariant&)
{
    if ((key.key() == mRegionalInfoKey.key()) && pluginEnabled()) {
        int showRegionalInfo =
            mSettingsManager->readItemValue(mRegionalInfoKey).toUInt();
        if (showRegionalInfo) {
            if (!mRegionalInfoGroupBox) {
                mRegionalInfoGroupBox = new HbGroupBox();
                CalenPluginLabel *regionalInfo = new CalenPluginLabel(
                    mServices, this);
                regionalInfo->setFontSpec(HbFontSpec(HbFontSpec::Primary));
                mRegionalInfoGroupBox->setContentWidget(regionalInfo);
                mVLayout->insertItem(1, mRegionalInfoGroupBox);
            }
            QString *pluginString = pluginText();
            HbLabel *pluginInfoLabel = qobject_cast<HbLabel *> (
                mRegionalInfoGroupBox->contentWidget());
            pluginInfoLabel->setPlainText(*pluginString);
        }
        else {
            if (mRegionalInfoGroupBox) {
                mVLayout->removeItem(mRegionalInfoGroupBox);
                delete mRegionalInfoGroupBox;
                mRegionalInfoGroupBox = NULL;
            }
        }
    }
}

/*!
   \brief Scrolls view according to current day and events.
*/
void CalenDayView::setupViewport()
{
    QDateTime currentTime = QDateTime::currentDateTime();

    // If we have event in current day and hour, scroll to this event
    if (mDate.date() == currentTime.date()){

        QDateTime midnight = currentTime;
        midnight.setTime(QTime(23, 59));

        //Filter flags (only timed events)
        AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(AgendaUtil::IncludeAppointments);
        QList<AgendaEntry> list;
        // Fetch the instance list from the agenda interface
        list = mServices.agendaInterface()->fetchEntriesInRange(currentTime, midnight, filter);

        if(!list.isEmpty()){
            int hourToScrollTo(list.first().startTime().time().hour());
            mHourScrollArea->scrollToHour(hourToScrollTo);           
        }
        else{
            mHourScrollArea->scrollToHour(currentTime.time().hour());
        }
    }
    else {
        //Scroll view to 7am
        mHourScrollArea->scrollToHour(7);
    }
}

/*!
 \brief Slot which is called whenever the orientation of the device changes.
 
 Changes the backgroung graphic element when orientation chandes.
 
 \param orientation Current device orientation
 */
void CalenDayView::orientationChanged(Qt::Orientation orientation)
{
    if (Qt::Vertical == orientation) {
        mBg = new HbFrameItem(KCalenBackgroundColorPortrait,
            HbFrameDrawer::OnePiece, this);
    }
    else {
        mBg = new HbFrameItem(KCalenBackgroundColorLandscape,
            HbFrameDrawer::OnePiece, this);
    }
    mMainContainer->setBackgroundItem(mBg);
}

//End of File
