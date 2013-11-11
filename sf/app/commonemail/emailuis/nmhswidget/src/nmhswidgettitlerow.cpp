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
 * Description: 
 *
 */

#include <qgraphicslinearlayout.h>
#include <QtGui>
#include <hbdocumentloader.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <hbcolorscheme.h>
#include <hbevent.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbtapgesture.h>
#include <hbinstantfeedback.h>
#include <hbstringutil.h>
#include "nmicons.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetconsts.h"
#include "emailtrace.h"

NmHsWidgetTitleRow::NmHsWidgetTitleRow(QGraphicsItem *parent, Qt::WindowFlags flags) :
    HbWidget(parent, flags), 
    mContainer(0),
    mMailboxIcon(0), 
    mMailboxInfo(0), 
    mUnreadCountLabel(0),
    mCollapseExpIconLabel(0), 
    mAccountName(),
    mUnreadCount(0),
    mBackgroundLayoutItem(0)
{
    NM_FUNCTION;
    grabGesture(Qt::TapGesture);
}

/*!
 Destructor
 */
NmHsWidgetTitleRow::~NmHsWidgetTitleRow()
{
    NM_FUNCTION; 
}

/*!
 \fn QPainterPath NmHsWidgetTitleRow::shape()

 Called by home screen fw to check widget boundaries, needed to draw
 outside widget boundingRect.
 /return QPainterPath path describing actual boundaries of widget 
  including child items
 */
QPainterPath NmHsWidgetTitleRow::shape() const
{
    NM_FUNCTION;
    
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    if (mContainer){
        path.addRect(mContainer->geometry());
    }
    if (mMailboxIcon){
        path.addRect(mMailboxIcon->geometry());
    }    
    return path.simplified();
}

/*!
 \fn qreal NmHsWidgetTitleRow::containerHeight()

 /return qreal defining title row container height in pixels
 */
qreal NmHsWidgetTitleRow::containerHeight()
{
    if (mContainer){
        return mContainer->preferredHeight();
    }
    return 0;
}

/*
 Setup email row ui
  Must be called after constructor.
   /return true if loading succeeded, otherwise false. False indicates that object is unusable.
 */
bool NmHsWidgetTitleRow::setupUI(HbDocumentLoader &loader)
    {
    NM_FUNCTION;

    if(!loadDocML(loader) || !setupGraphics()){
        return false;
    }
    return true;
    }

/*!
 Loads layout data and child items from docml file. Must be called after constructor.
 /return true if loading succeeded, otherwise false. False indicates that object is unusable
 */
bool NmHsWidgetTitleRow::loadDocML(HbDocumentLoader &loader)
{
    NM_FUNCTION;
    QT_TRY{
        // find container widget
        mContainer = static_cast<HbWidget*> (loader.findWidget(KNmHsWidgetTitleRowContainer));
        if (!mContainer) {
            NM_ERROR(1,"NmHsWidgetTitleRow::loadDocML Fail @ container");
            return false;
        }
    
        //child items possible to update
        mMailboxIcon = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetTitleRowMailboxIcon));
        mMailboxInfo = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetTitleRowMailboxNameLabel));
        mUnreadCountLabel = static_cast<HbLabel*> (loader.findWidget(
            KNmHsWidgetTitleRowUnreadCountLabel));
        mCollapseExpIconLabel = static_cast<HbPushButton*> (loader.findWidget(
            KNmHsWidgetTitleRowCollapseExpandIconLabel));
    
        if (!mMailboxIcon || !mMailboxInfo || !mUnreadCountLabel || !mCollapseExpIconLabel) {
            NM_ERROR(1,"NmHsWidgetTitleRow::loadDocML Fail @ icons & labels");
            return false;
        }
		
		//Expand collapse button
        connect(mCollapseExpIconLabel, SIGNAL(clicked()), this, SIGNAL(expandCollapseButtonPressed()));
		
        return true;
    }
    QT_CATCH(...){
        return false;
    }
}

/*
 Setup graphics that cannot be loaded from docml.
  /return true if loading succeeded, otherwise false. False indicates that object is unusable.
 */
bool NmHsWidgetTitleRow::setupGraphics()
{
    NM_FUNCTION;
    
    HbFrameDrawer* backgroundFrameDrawer = 0;
    QT_TRY{
        //pressed background
        backgroundFrameDrawer = new HbFrameDrawer("qtg_fr_hsitems_pressed", HbFrameDrawer::NinePieces);
        mBackgroundLayoutItem = new HbFrameItem( backgroundFrameDrawer );
        mContainer->setBackgroundItem( mBackgroundLayoutItem );
		mBackgroundLayoutItem->hide();
        
        //set fonts color
		setHighlighedFontsColor(false);
		
		//to get gestures
		setGeometry(mContainer->childrenBoundingRect());
		
        return true;
    }
    QT_CATCH(...){
        if(!mBackgroundLayoutItem && backgroundFrameDrawer){
            delete backgroundFrameDrawer;
            backgroundFrameDrawer = NULL;
        }

        return false;
    }
    
}

/*!
 Slot for updating account name, calls updateData to update ui.
 */
void NmHsWidgetTitleRow::updateAccountName(const QString& accountName)
{
    NM_FUNCTION;
    mAccountName = accountName;
    updateData();
}

/*!
 Set account icon name
 */
void NmHsWidgetTitleRow::setAccountIcon(const QString& accountIconName)
{
    NM_FUNCTION;
    mMailboxIcon->setIcon(accountIconName);
}

/*!
 Slot for updating unread count, calls updateData to update ui.
 */
void NmHsWidgetTitleRow::updateUnreadCount(const int& unreadCount)
{
    NM_FUNCTION;
    mUnreadCount = unreadCount;
    updateData();
}


/*!
 Slot for updating expand collapse icon
 */
void NmHsWidgetTitleRow::setExpandCollapseIcon(const bool& expand)
{
    NM_FUNCTION;
    if(expand){
        HbIcon icon("qtg_mono_arrow_up");
        mCollapseExpIconLabel->setIcon(icon);
    }
    else{
        HbIcon icon("qtg_mono_arrow_down");
        mCollapseExpIconLabel->setIcon(icon);
    }
}

/*!
 Update the data displayed in the UI
 */
void NmHsWidgetTitleRow::updateData()
{
    NM_FUNCTION;
    mMailboxInfo->setPlainText(mAccountName);
    //If unread count is -1, hide the unread count label completely.
    //This indicates that there are no mails at all (or the initial sync is not done)
    if (mUnreadCount >= 0) {
        QString unreadCount(HbStringUtil::convertDigits(hbTrId("txt_mail_widget_list_l1").arg(mUnreadCount)));
        mUnreadCountLabel->setPlainText(unreadCount);
        QFontMetrics fm(mUnreadCountLabel->font());
        qreal textWidth = fm.width(unreadCount);
        mUnreadCountLabel->setMaximumWidth(textWidth);
    }
    else {
        mUnreadCountLabel->setMaximumWidth(0);
    }
}

/*!
    sets fonts color.
    /param bool pressed indicates if row is pressed down or not
*/
void NmHsWidgetTitleRow::setHighlighedFontsColor( bool pressed )
    {
    NM_FUNCTION;
    QColor newFontColor;
    
    if(pressed){
        newFontColor = HbColorScheme::color("qtc_hs_list_item_pressed");
    }
    else{
        newFontColor = HbColorScheme::color("qtc_hs_list_item_title_normal");
    }
 
    mMailboxInfo->setTextColor(newFontColor);
    mUnreadCountLabel->setTextColor(newFontColor);
    }

/*!
    change background pressed state
    /param bool show if true then shown, false hide
*/
void NmHsWidgetTitleRow::showHighlight( bool show )
    {
    NM_FUNCTION;;
    
    if(show){
        mBackgroundLayoutItem->show();
    }
    else{
        mBackgroundLayoutItem->hide();
    }
    }

/*
 * NmHsWidgetTitleRow::gestureEvent(QGestureEvent *event)
 */
void NmHsWidgetTitleRow::gestureEvent(QGestureEvent *event)
{
    NM_FUNCTION;
    
    if(!event){
        return;
    }    
    HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture));
    if(!gesture){
        return;
    }
    //to avoid opening email account mistakenly  when tabbing expand/collapse button
    //we dont handle events that are on the top, down or right side of the button
    QPointF posFromScene = mapFromScene(event->mapToGraphicsScene(gesture->position()));
    bool inArea = false;
    
    if(this->layoutDirection() == Qt::LeftToRight &&  
       posFromScene.x() < mUnreadCountLabel->geometry().right()) {
        inArea = true;
    }
    else if(this->layoutDirection() == Qt::RightToLeft && 
            posFromScene.x() > mUnreadCountLabel->geometry().right()) {
        inArea = true;
    }
    
    if (inArea) {    
        switch (gesture->state()) {
            case Qt::GestureStarted:
                setHighlighedFontsColor(true);
                showHighlight(true);
            break;
            case Qt::GestureCanceled:
                setHighlighedFontsColor(false);
                showHighlight(false);
            break;
            case Qt::GestureFinished:
                setHighlighedFontsColor(false);
                showHighlight(false);
                if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                    HbInstantFeedback::play(HbFeedback::BasicItem);
                    emit mailboxLaunchTriggered();
                }
            break;
        default: 
            break;
        }
    }
}

/*
 * NmHsWidgetTitleRow::event()
 */
bool NmHsWidgetTitleRow::event( QEvent *event )
{
    NM_FUNCTION;
    QEvent::Type eventType = event->type();
    if( eventType == HbEvent::ThemeChanged ){
        setHighlighedFontsColor(false);
        return true;
    }
    return HbWidget::event(event);
}
