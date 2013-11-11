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
* Description:  CalenPreviewPane implementation.
*
*/


// System includes
#include <QtGui>
#include <qfont.h>
#include <qicon.h>
#include <qpainter.h>
#include <qevent.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbfontspec.h>
#include <hbcolorscheme.h>
#include <hbtapgesture.h>
#include <hbpangesture.h>
#include <hbinstance.h>
#include <hbinstantfeedback.h>
#include <agendautil.h>
#include <agendaentry.h>

//user includes
#include "calenagendautils.h"
#include "calendateutils.h"
#include "calencontext.h"
#include "calenservices.h"
#include "calenservices.h"
#include "calencommon.h"
#include "calenpreviewpane.h"
#include "calenmonthview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenpreviewpaneTraces.h"
#endif


// Macros
#define TWO_SECONDS_TIMER 2000 // millseconds
#define SCROLLING_SPEED 10
#define MAX_PAN_DIRECTION_THRESHOLD 50
#define MIN_PAN_DIRECTION_THRESHOLD 10

static const QString EMPTYSTRING(" ");

/*!
 Constructor
 */
CalenPreviewPane::CalenPreviewPane(MCalenServices& services, 
							QGraphicsItem* parent)
	: HbScrollArea(parent),mServices(services)
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_CALENPREVIEWPANE_ENTRY );
    
	// Create the timer
	mTwoSecTimer = new QTimer(this);
	mScrollDirection = invalid;
	mIsNoEntriesAdded = true;
	mIsGestureHandled = false;
	mNoEntriesLabel = 0;
	mHtDiff = 0.0;
	mScrollDuration = 0;
	mPreviewTextColor = HbColorScheme::color("qtc_cal_day_preview_text");
	
	setAcceptDrops(true);
	setScrollDirections(Qt::Vertical);
	setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
	
	// Connect the scrollig finished signal
	connect(this, SIGNAL(scrollingEnded()), this,
				SLOT(scrollingFinished()));
	
	grabGesture(Qt::TapGesture);
	grabGesture(Qt::PanGesture);

	OstTraceFunctionExit0( CALENPREVIEWPANE_CALENPREVIEWPANE_EXIT );
}

/*!
 Destructor
 */
CalenPreviewPane::~CalenPreviewPane()
{
    OstTraceFunctionEntry0( DUP1_CALENPREVIEWPANE_CALENPREVIEWPANE_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CALENPREVIEWPANE_CALENPREVIEWPANE_EXIT );
}

/*!
 Stores the "No Entries for today" label
 */
void CalenPreviewPane::setNoEntriesLabel(HbLabel* label)
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_SETNOENTRIESLABEL_ENTRY );
    
	mNoEntriesLabel = label;
	
	if (mPreviewTextColor.isValid()) {
		mNoEntriesLabel->setTextColor(mPreviewTextColor);
	}
	OstTraceFunctionExit0( CALENPREVIEWPANE_SETNOENTRIESLABEL_EXIT );
}

/*!
 Populates the preview with proper data
 */
void CalenPreviewPane::populateLabel(QDateTime date)
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_POPULATELABEL_ENTRY );
    
	mScrollDirection = up;
	
	// Scroll contents to zero position before we display to the user
	// if it was scrolling already or timer was running
	mNumOfScrolls = 0;
	mScrollDuration = 0;
	stopScrolling();
	
	mDate = date;
	
	// Get Instances for the day
	GetInstanceListL();
	
	// Get the content of the scroll area
	QGraphicsWidget* content = contentWidget();
	// Adjust the layout
	QGraphicsLinearLayout* layout = static_cast<QGraphicsLinearLayout *>
															(content->layout());
	int instanceCount = mInstanceArray.count();
	HbFontSpec font(HbFontSpec::Secondary);
	if (mIsNoEntriesAdded) {
		if (!instanceCount) {
			if (mPreviewTextColor.isValid()) {
				mNoEntriesLabel->setTextColor(mPreviewTextColor);
			}
		    mNoEntriesLabel->setVisible(true);
		    OstTraceFunctionExit0( CALENPREVIEWPANE_POPULATELABEL_EXIT );
		    return;
		} else {
			// Remove the no entries label
			mNoEntriesLabel->setVisible(false);
		}
	}
	int count = layout->count();
	int labelCount = mLabelList.count();
	for (int i = 0; i < labelCount; i++) {
		mLabelList.at(i)->clear();
	}
	if (instanceCount) {
		mIsNoEntriesAdded = false;
		// Parse the instances and add them to the layout
		for (int i = 0; i <instanceCount; i++) {
			HbLabel* label;
			if(!count) {
				label = new HbLabel(this);
				
				// Set the required font
				label->setFontSpec(font);
				
				// Set the elide mode to right
				label->setElideMode(Qt::ElideRight);
				// Add the label to the list
				mLabelList.append(label);
				layout->addItem(label);
			} else {
				// Reuse the same label
				label = mLabelList.at(i);
				count--;
			}
			
			if (mPreviewTextColor.isValid()) {
				label->setTextColor(mPreviewTextColor);
			}
			QString summary = mInstanceArray[i].summary();
			if(!summary.length()) {
				// No summary display "Unnamed"
				summary.append(hbTrId("txt_calendar_preview_unnamed"));
			}
			// Check the entry type, based on the type display time field in
			// preview pane.
			QString start;
			if(mInstanceArray[i].type() != AgendaEntry::TypeTodo && 
					mInstanceArray[i].type() != AgendaEntry::TypeAnniversary && 
					!CalenAgendaUtils::isAlldayEvent(mInstanceArray[i])) {
				QDateTime startTime = mInstanceArray[i].startTime();
				// Check if event starts in past
				checkStartTimeOfEvent(startTime);
				HbExtendedLocale systemLocale =HbExtendedLocale::system();
				start = systemLocale.format(startTime.time(), 
				                                    r_qtn_time_usual_with_zero);
				start.append(EMPTYSTRING);	
			}
			// Append summary to start time
			QString text = start.append(summary);
			label->setPlainText(text);
			layout->setStretchFactor(label,0);
		}
		
		// Cleanup the remaining labels
		if (count) {
			int layoutCount = layout->count();
			int offset = layoutCount - count;
			int labelCount = mLabelList.count();
			for (int i = 0; i < count; i++) {
				QGraphicsLayoutItem* item = layout->itemAt(offset);
				layout->removeAt(offset);
				int index = --labelCount;
				mLabelList.removeAt(index);
				if (item) {
					delete item;
				}
			}
		}
	} else { // Delete all the items in the layout
		// TODO: Need to see if we can clear all the items at one shot
		// remove the items if any
		for (int i = 0; i < count; i++) {
			QGraphicsLayoutItem* item = layout->itemAt(0);
			layout->removeAt(0);
			// Clear the label text
			mLabelList.at(i)->clear();
			if (item) {
				delete item;
			}
		}
		// Clear the list
		mLabelList.clear();
		
		if (mPreviewTextColor.isValid()) {
			mNoEntriesLabel->setTextColor(mPreviewTextColor);
		}
		// Add the no entries text to the preview pane
		mNoEntriesLabel->setVisible(true);
		mIsNoEntriesAdded = true;
	}
	layout->activate();
	
	OstTraceFunctionExit0( DUP1_CALENPREVIEWPANE_POPULATELABEL_EXIT );
}

/*!
 Fetches the instance for a given day
 */
void CalenPreviewPane::GetInstanceListL()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_GETINSTANCELISTL_ENTRY );
    
	mInstanceArray.clear();

	// Find Meetings, Remainders, Anniversaries and Day Notes
	QDateTime dayStart( CalenDateUtils::beginningOfDay( mDate ) );

	// Make a request to get all the entries for the mDate
	AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(
										AgendaUtil::IncludeAnniversaries |
										AgendaUtil::IncludeAppointments |
											AgendaUtil::IncludeEvents |
											AgendaUtil::IncludeIncompletedTodos | 
										AgendaUtil::IncludeReminders);
	mInstanceArray = mServices.agendaInterface()->
									createEntryIdListForDay(dayStart, filter);
	
	OstTraceFunctionExit0( CALENPREVIEWPANE_GETINSTANCELISTL_EXIT );
}

/*!
 Returns the date which this preview pane is representing
 */
QDateTime CalenPreviewPane::Date()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_DATE_ENTRY );
    
	OstTraceFunctionExit0( CALENPREVIEWPANE_DATE_EXIT );
	return mDate;
}

/*!
 Starts the auto scroll on the preview pane
 */
void CalenPreviewPane::startAutoScroll()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_STARTAUTOSCROLL_ENTRY );
    
	if (mIsNoEntriesAdded) {
		scrollContentsTo(QPointF(0.0,0.0));
		OstTraceFunctionExit0( CALENPREVIEWPANE_STARTAUTOSCROLL_EXIT );
		return;
	}
	
	// Stop the timer
	mTwoSecTimer->stop();
	
	// Set the proper scroll direction
	mScrollDirection = up;
	
	// Start the 2 seconds timer
	mTwoSecTimer->setSingleShot(true);
	connect(mTwoSecTimer, SIGNAL(timeout()), this, SLOT(onTwoSecondsTimeout()));
	mTwoSecTimer->start(TWO_SECONDS_TIMER);
	
	OstTraceFunctionExit0( DUP1_CALENPREVIEWPANE_STARTAUTOSCROLL_EXIT );
}

/*!
 Slot to handle two seconds timer time out
 Starts the scrolling animation in required direction
 */
void CalenPreviewPane::onTwoSecondsTimeout()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_ONTWOSECONDSTIMEOUT_ENTRY );
    
	mTwoSecTimer->stop();
	disconnect(mTwoSecTimer, SIGNAL(timeout()), 
										   this, SLOT(onTwoSecondsTimeout()));
	
	// Calculate the timer and the height difference of pane and its content
	if (!mScrollDuration) {
        qreal contentHeight = contentWidget()->size().height();
        qreal paneHeight = size().height();
        mHtDiff = contentHeight - paneHeight;
        if (mHtDiff > 0) { // content is more than widget height, we need to scroll
            mScrollDuration = mHtDiff / SCROLLING_SPEED;
        }
	}
	
    // Start the scrolling in the proper direction
	if (mScrollDirection == up && mScrollDuration) {
		// Start scrolling upwards
		mScrollDirection = down;
		mNumOfScrolls++;
		QPointF targetPos(0.0, -mHtDiff);
        scrollContentsTo(-targetPos, mScrollDuration * 1000);
	} else if (mScrollDirection == down && mScrollDuration) {
		mScrollDirection = up;
		mNumOfScrolls++;
		// Start scrolling downwards
		QPointF targetPos(0.0, 0.0);
        scrollContentsTo(targetPos, mScrollDuration * 1000);
	}
	
	OstTraceFunctionExit0( CALENPREVIEWPANE_ONTWOSECONDSTIMEOUT_EXIT );
}

/*!
 Slot to handle scrolling finished
 Restarts the two seconds timer
 */
void CalenPreviewPane::scrollingFinished()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_SCROLLINGFINISHED_ENTRY );
    
	// If we are here because of calling scrollContentsTo()
	if (!mNumOfScrolls) {
		OstTraceFunctionExit0( CALENPREVIEWPANE_SCROLLINGFINISHED_EXIT );
		return;
	}
	// Now start the two seconds timer again
	mTwoSecTimer->setSingleShot(true);
	connect(mTwoSecTimer, SIGNAL(timeout()), this, SLOT(onTwoSecondsTimeout()));
	mTwoSecTimer->start(TWO_SECONDS_TIMER);
	
	OstTraceFunctionExit0( DUP1_CALENPREVIEWPANE_SCROLLINGFINISHED_EXIT );
}

/*!
    Function to listen for all gestures
 */
void CalenPreviewPane::gestureEvent(QGestureEvent *event)
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_GESTUREEVENT_ENTRY );
    
    if(HbPanGesture *gesture = qobject_cast<HbPanGesture *>(event->gesture(Qt::PanGesture))) {
        if (gesture->state() == Qt::GestureUpdated) {
            // Check if effect is not yet completed, ignore the current gesture if it is
            if (mIsGestureHandled) {
                OstTraceFunctionExit0( CALENPREVIEWPANE_GESTUREEVENT_EXIT );
                return;
            }
            // TODO: This work around till framework provides an api
            // to know the direciton of the pan, until then we need
            // calculate the direction explicitly
            // Get to know the direction of the gesture
            QPointF delta = gesture->sceneOffset();
            // Check the horizontal and vertical offsets
            qreal horizontalDiff = delta.x();
            qreal verticalDiff = delta.y();
            if (abs(verticalDiff) > MAX_PAN_DIRECTION_THRESHOLD) {
                // Now see if x coord diff has crossed threshold
                if (horizontalDiff > MAX_PAN_DIRECTION_THRESHOLD) {
                    // right gesture
                    mView->handlePreviewPaneGesture(true);
                    event->accept(Qt::PanGesture);
                } else if (horizontalDiff < -MAX_PAN_DIRECTION_THRESHOLD){
                    // left gesture
                    mView->handlePreviewPaneGesture(false);
                    event->accept(Qt::PanGesture);
                } else {
                    event->accept(Qt::PanGesture);
                    // Since user tried to tap and ended up giving low pan
                    // hence, handle it as tap
                    tapTriggered();
                    OstTraceFunctionExit0( DUP1_CALENPREVIEWPANE_GESTUREEVENT_EXIT );
                    return;
                }
            } else if (abs(verticalDiff) < MAX_PAN_DIRECTION_THRESHOLD) {
               if (horizontalDiff > MIN_PAN_DIRECTION_THRESHOLD) {
                   // right gesture
                   mView->handlePreviewPaneGesture(true);
                   event->accept(Qt::PanGesture);
               } else if (horizontalDiff < -MIN_PAN_DIRECTION_THRESHOLD){
                   // left gesture
                   mView->handlePreviewPaneGesture(false);
                   event->accept(Qt::PanGesture);
               }else {
                   event->accept(Qt::PanGesture);
                   // Since user tried to tap and ended up giving low pan
                   // hence, handle it as tap
                   tapTriggered();
                   OstTraceFunctionExit0( DUP2_CALENPREVIEWPANE_GESTUREEVENT_EXIT );
                   return;
               }
            }
        }
    } else if(QTapGesture *tapGesture = qobject_cast<QTapGesture *>(event->gesture(Qt::TapGesture))) {
        if (tapGesture && tapGesture->state() == Qt::GestureFinished) {
            event->accept(Qt::TapGesture);
            // Handle the tap
            tapTriggered();
        }
    }  else if(QTapAndHoldGesture *tapGesture = qobject_cast<QTapAndHoldGesture *>(event->gesture(Qt::TapAndHoldGesture))) {
        if (tapGesture && tapGesture->state() == Qt::GestureFinished) { 
            event->accept(Qt::TapAndHoldGesture);
            // Handle the tap
            tapTriggered();
        }
    }
    
    OstTraceFunctionExit0( DUP3_CALENPREVIEWPANE_GESTUREEVENT_EXIT );
}

/*!
 Handles the tap and launches the agendaview
 */
void CalenPreviewPane::tapTriggered()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_TAPTRIGGERED_ENTRY );
    
    HbInstantFeedback::play(HbFeedback::Basic);
    // Preview pane tapped
    // Stop the scrolling first
    stopScrolling();
    // Issue command to launch agenda view
    mServices.IssueCommandL(ECalenAgendaView);
    mView->disconnectAboutToQuitEvent();
    
    OstTraceFunctionExit0( CALENPREVIEWPANE_TAPTRIGGERED_EXIT );
}

/*!
 Set monthview pointer
 */
void CalenPreviewPane::setView(CalenMonthView* view)
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_SETVIEW_ENTRY );
    
	mView = view;
	
	OstTraceFunctionExit0( CALENPREVIEWPANE_SETVIEW_EXIT );
}

/*!
 Stops the auto scrolling
 */
void CalenPreviewPane::stopScrolling()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_STOPSCROLLING_ENTRY );
    
    mTwoSecTimer->stop();
	if (isScrolling()) {
        mNumOfScrolls = 0; // required so that timer will not get started again in 
        // scrollingFinished() slot
        // Stop the scrolling by sending the foucs out event
        QFocusEvent* focusEvent = new QFocusEvent(QFocusEvent::FocusOut, Qt::MouseFocusReason);
        focusOutEvent(focusEvent); 
        
        // Set the contents position to zero
		scrollContentsTo(QPointF(0.0,0.0));
	}
	
	OstTraceFunctionExit0( CALENPREVIEWPANE_STOPSCROLLING_EXIT );
}

/*!
 Checks if the start time of the event falls on the date for which preview
 pane is being shown. If start time is in past, then time will be 12:00AM
 */
void CalenPreviewPane::checkStartTimeOfEvent(QDateTime &dateTime)
{
    // If event start time is in past
    if (dateTime.date() < mDate.date()) {
        // Set the time to 12:00AM of mDate
        dateTime.setDate(mDate.date());
        QTime time(0,0,0,0); // 0 means 12:00 AM
        dateTime.setTime(time);
    }
}

/*!
 Sets the effect beign handled flag to true
 */
void CalenPreviewPane::effectStarted()
{
    mIsGestureHandled = true;
}

/*!
 Resets the effect beign handled flag to true
 */
void CalenPreviewPane::effectFinished()
{
    mIsGestureHandled = false;
}

/*!
 Slot to handle the change in theme
 */
void CalenPreviewPane::handleThemeChange()
{
    OstTraceFunctionEntry0( CALENPREVIEWPANE_HANDLETHEMECHANGE_ENTRY );
    
    mPreviewTextColor = HbColorScheme::color("qtc_cal_day_preview_text");
    
    OstTraceFunctionExit0( CALENPREVIEWPANE_HANDLETHEMECHANGE_EXIT );
}

// End of file  --Don't remove this.
