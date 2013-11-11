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
* Description:
*
*/
#include "logsdetailsview.h"
#include "logsdetailsmodel.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logslogger.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsdefs.h"
#include "logscontact.h"
#include "logsmodel.h"

//SYSTEM
#include <hblistview.h>
#include <hbaction.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <hblabel.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbnotificationdialog.h>
#include <hbabstractviewitem.h>
#include <QGraphicsLinearLayout>

Q_DECLARE_METATYPE(LogsDetailsModel*)


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsDetailsView::LogsDetailsView( 
    LogsComponentRepository& repository, LogsAbstractViewManager& viewManager )
    : LogsBaseView(LogsDetailsViewId, repository, viewManager),
      mViewName(0),
      mListView(0)
{
    LOGS_QDEBUG( "logs [UI] <-> LogsDetailsView::LogsDetailsView()" );
    
    //TODO: taking away due to toolbar bug. If toolbar visibility changes on view
    //activation, there will be a crash due to previous view effect is playing
    //addViewSwitchingEffects();
    
    mActivities.append( logsActivityIdViewDetails );
}
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsDetailsView::~LogsDetailsView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::~LogsDetailsView()" );
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::~LogsDetailsView()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::activated(bool showDialer, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::activated()" );
    //base class handling first
    LogsBaseView::activated(showDialer, args);
   
    LogsDetailsModel* model = qVariantValue<LogsDetailsModel*>(args);
    if ( !model ){
        LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::activated(), no model" );
        return;
    }
    
    if ( mListView && mDetailsModel && mDetailsModel != model ){
        mListView->setModel( 0 );
        delete mDetailsModel;
    }
    mDetailsModel = model;
    
    if (mViewName){
        mViewName->setHeading(mDetailsModel->headerData(0, Qt::Vertical).toString());
    }
    
    if ( mListView ){
        
        mListView->setModel( mDetailsModel, new LogsDetailsViewItem());//ownership not transferred
    }
    
    updateMenu();
    
    scrollToTopItem(mListView);
    
    LogsBaseView::activationCompleted();
    
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::activated()" );
}

// -----------------------------------------------------------------------------
// LogsDetailsView::deactivated
// -----------------------------------------------------------------------------
//
void LogsDetailsView::deactivated()
{
    //base class handling first
    LogsBaseView::deactivated();
    if ( mListView ){
        mListView->setModel( 0 );
    }
    delete mDetailsModel;
    mDetailsModel = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsDetailsView::saveActivity(
    QDataStream& serializedActivity, QVariantHash& metaData)
{
    if ( mDetailsModel ){
        mDetailsModel->getLogsEvent().serialize(serializedActivity);
    }
    return LogsBaseView::saveActivity(serializedActivity, metaData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsView::loadActivity(
    const QString& activityId, QDataStream& serializedActivity, QVariantHash& metaData)
{
    Q_UNUSED( activityId );
    Q_UNUSED( metaData );
    LogsEvent event(serializedActivity);
    LogsDetailsModel* details = mRepository.model()->logsDetailsModel(event);
    return qVariantFromValue( details );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::callKeyPressed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::callKeyPressed()" );
    
    if ( !tryCallToDialpadNumber() && mCall ){
        mCall->initiateCallback();
    }

    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::callKeyPressed()" );
}

// -----------------------------------------------------------------------------
// LogsDetailsView::initiateVoiceCall
// -----------------------------------------------------------------------------
//
void LogsDetailsView::initiateVoiceCall()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::initiateVoiceCall()" );
    if ( !tryCallToDialpadNumber() && mCall ){
        mCall->call(LogsCall::TypeLogsVoiceCall);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::initiateVoiceCall()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::initiateVideoCall()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::initiateVideoCall()" );
    if ( !tryCallToDialpadNumber(LogsCall::TypeLogsVideoCall) && mCall ){
        mCall->call(LogsCall::TypeLogsVideoCall);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::initiateVideoCall()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::sendMessage()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::sendMessage()" );
    if ( !tryMessageToDialpadNumber() && mMessage ){
        mMessage->sendMessage();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::sendMessage()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::openContact()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::openContact()" );
    if ( mContact ){
        // Use async connection to ensure that model can handle
        // contact operation completion before view
        QObject::connect(mContact, SIGNAL(openCompleted(bool)),
                         this, SLOT(contactActionCompleted(bool)),
                         Qt::QueuedConnection);
        mContact->open();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::openContact()" );
}

// -----------------------------------------------------------------------------
// LogsDetailsView::copyNumberToClipboard()
// -----------------------------------------------------------------------------
//
void LogsDetailsView::copyNumberToClipboard()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::copyNumberToClipboard()" );
    if ( isDialpadInput() ){
        mDialpad->editor().setSelection(0, mDialpad->editor().text().length());
        mDialpad->editor().copy();
        mDialpad->editor().setSelection(0, 0);
    } else if ( mDetailsModel ) {
        mDetailsModel->getNumberToClipboard();   
    }
    QString infoMessage;
    infoMessage.append(hbTrId("txt_dialer_dpopinfo_number_copied"));
    HbNotificationDialog::launchDialog(infoMessage);
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::copyNumberToClipboard()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::contactActionCompleted(bool modified)
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::contactActionCompleted()" );
    
    if (modified){
        updateMenu();
        if (mViewName){
            mViewName->setHeading(
                    mDetailsModel->headerData(0, Qt::Vertical).toString());
        }
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::contactActionCompleted()" );
}

// -----------------------------------------------------------------------------
// LogsDetailsView::handleBackSoftkey
// -----------------------------------------------------------------------------
//
void LogsDetailsView::handleBackSoftkey()
{
    mViewManager.activatePreviousView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsView::deleteEventAnswer(int action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::deleteEventAnswer()" );
    LogsBaseView::deleteEventAnswer(action);
    if (action == HbMessageBox::Ok) {
        handleBackSoftkey(); 
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::deleteEventAnswer()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::initView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::initView()" );
    //base class handling first
    LogsBaseView::initView();
        
    mViewName = qobject_cast<HbGroupBox*>(
                        mRepository.findWidget( logsGroupboxDetailViewNameId ) );  
    initListWidget();

    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::initView()" );   
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsDetailsView::model() const
{
    return mDetailsModel;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsDetailsView::initListWidget()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::initListWidget()" );
    mListView = qobject_cast<HbListView*> 
                    ( mRepository.findWidget( logsDetailsListWidgetId ) );
    Q_ASSERT_X(mListView != 0, "logs [UI] ", "couldn't find list widget !!");

    mListView->setItemRecycling(true);
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::initListWidget() " );
}

// -----------------------------------------------------------------------------
// LogsDetailsView::updateMenu
// -----------------------------------------------------------------------------
//
void LogsDetailsView::updateMenu()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::updateMenu()" );
    if ( !mDetailsModel ) {
        return;
    }
    
    updateCall( mDetailsModel->index(0) );
    updateMessage( mDetailsModel->index(0) );
    updateContact( mDetailsModel->index(0) );
    
    HbAction* voiceCallAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsDetailsViewVoiceCallMenuActionId ) );
    HbAction* videoCallAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsCommonVideoCallMenuActionId ) );
    HbAction* internetCallAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsDetailsViewInternetCallMenuActionId ) );
    HbAction* messageAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsCommonMessageMenuActionId ) );
    HbAction* addToContactsAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsCommonAddToContactsMenuActionId ) );
    HbAction* openContactAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsDetailsOpenContactMenuActionId ) );
    HbAction* copyNumberAction = qobject_cast<HbAction*>( 
                mRepository.findObject( logsDetailsCopyNumberMenuActionId ) );
    
    
    
    bool voiceCallAvailable(false);
    bool videoCallAvailable(false);
    bool internetCallAvailable(false); // Currently always disabled
    if (mCall) {
        foreach( LogsCall::CallType callType, mCall->allowedCallTypes() ){
            if ( callType == LogsCall::TypeLogsVoiceCall ){
                voiceCallAvailable = true;
            } else if ( callType == LogsCall::TypeLogsVideoCall ) {
                videoCallAvailable = true;
            }
        }
    }
    
    bool contactCanBeAdded(false);
    bool contactCanBeOpened(false);
    if ( isDialpadInput() ){
        // Contact addition will be done using input field number
        contactCanBeAdded = true;
        // No need for voice call option in options menu as voice dialling
        // to dialpad num is possible from green button
        voiceCallAvailable = false;
    } else if (mContact){
        if (mContact->allowedRequestType() == LogsContact::TypeLogsContactSave){
            contactCanBeAdded = true;
        }
        if (mContact->allowedRequestType() == LogsContact::TypeLogsContactOpen){
            contactCanBeOpened = true;
        }
    }
    bool copyNumberAllowed = !mDetailsModel->getLogsEvent().isRemotePartyPrivate();
    
    toggleActionAvailability(voiceCallAction, voiceCallAvailable);
    toggleActionAvailability(videoCallAction, videoCallAvailable);
    toggleActionAvailability(internetCallAction, internetCallAvailable);
    toggleActionAvailability(messageAction, mMessage);
    toggleActionAvailability(addToContactsAction, contactCanBeAdded);
    toggleActionAvailability(openContactAction, contactCanBeOpened);
    toggleActionAvailability(copyNumberAction, copyNumberAllowed);
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::updateMenu()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsView::dialpadEditorTextChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::dialpadEditorTextChanged()" );
    if ( !tryMatchesViewTransition() ) {
        updateMenu();
        updateCallButton();  
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::dialpadEditorTextChanged()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsView::updateWidgetsSizeAndLayout()
{
    LOGS_QDEBUG( "logs [UI] -> LogsDetailsView::updateWidgetsSizeAndLayout()" );
    if ( mListView ) {
        updateMenu();
        updateListLayoutName(*mListView, true);
        updateListSize(*mListView);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsView::updateWidgetsSizeAndLayout()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsDetailsViewItem::LogsDetailsViewItem()
    : HbListViewItem(0), mLayout(0)    
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsDetailsViewItem::~LogsDetailsViewItem( )
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsViewItem::pressStateChanged(bool value, bool animate)
{
    Q_UNUSED(value);
    Q_UNUSED(animate);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbAbstractViewItem *LogsDetailsViewItem::createItem()
{
    return new LogsDetailsViewItem(*this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsViewItem::updateChildItems()
{  
    HbListViewItem::updateChildItems();

    QVariant customData = modelIndex().data(LogsDetailsModel::RoleDuplicatesSeparator);
    if (customData.isValid() && customData.toBool()) {
        LOGS_QDEBUG( "logs [UI] -> LogsDetailsViewItem::updateChildItems(), groupbox" ); 
        HbGroupBox* groupBox = new HbGroupBox(this);
        groupBox->setHeading(modelIndex().data(Qt::DisplayRole).toString());
        //groupBox->setCollapsable(true);

        HbWidget* content = new HbWidget();
        content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        content->setMaximumHeight(0);
        groupBox->setContentWidget(content);
        groupBox->setCollapsed(true);
          
        connect(groupBox, SIGNAL(toggled(bool)), this, SLOT(groupBoxClicked(bool)));

        if (!mLayout) {
            mLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
            mLayout->setContentsMargins(0,0,0,0);        
        }

        mLayout->addItem(layout());
        mLayout->addItem(groupBox);
        mLayout->setItemSpacing(0,0);
        setLayout(mLayout);             
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsViewItem::groupBoxClicked(bool collapsed)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsDetailsViewItem::groupBoxClicked(): ", collapsed );    
    QAbstractItemModel* model = const_cast <QAbstractItemModel*>(modelIndex().model());
    model->setData(modelIndex(), QVariant(collapsed));
    LOGS_QDEBUG( "logs [UI] <- LogsDetailsViewItem::groupBoxClicked()" );
}
