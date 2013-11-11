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
* Contributors:
*
* Description:
*
*/
//USER
#include "logsbaseview.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "logsabstractmodel.h"
#include "logsmodel.h"
#include "logsdetailsmodel.h"
#include "logsconfigurationparams.h"

//SYSTEM
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbmessagebox.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbeffect.h>
#include <hbabstractviewitem.h>
#include <QSignalMapper>
#include <xqservicerequest.h>
#include <hblabel.h>
#include <hblistview.h>
#include <QGraphicsLinearLayout>
#include <hbpushbutton.h>
#include <hbactivitymanager.h>
#include <hbmodeliterator.h>
#include <hbscrollbar.h>
#include <hbstringutil.h>

Q_DECLARE_METATYPE(LogsCall*)
Q_DECLARE_METATYPE(LogsMessage*)
Q_DECLARE_METATYPE(LogsContact*)
Q_DECLARE_METATYPE(LogsDetailsModel*)


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsBaseView::LogsBaseView( 
    LogsAppViewId viewId, 
    LogsComponentRepository& repository, 
    LogsAbstractViewManager& viewManager )
    : HbView(0),
      mViewId( viewId ),
      mRepository( repository ),
      mViewManager( viewManager ),
      mShowFilterMenu(0),
      mEmptyListLabel(0),
      mInitialized(false),
      mCall(0),
      mMessage(0),
      mContact(0),
      mDetailsModel(0),
      mCallTypeMapper(0),
      mOptionsMenu(0),
      mActivating(false)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::LogsBaseView()" );

    setNavigationAction(new HbAction(Hb::BackNaviAction, this));
    connect(navigationAction(), SIGNAL(triggered()), this, 
            SLOT(handleBackSoftkey()));
    
    mDialpad =  mRepository.dialpad();
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::LogsBaseView()" );
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsBaseView::~LogsBaseView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::~LogsBaseView()" );

    delete mCall;
    delete mMessage;
    delete mContact;
    delete mDetailsModel;    
    delete mCallTypeMapper;
    delete mOptionsMenu;

    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::~LogsBaseView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAppViewId LogsBaseView::viewId() const
{
    return mViewId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsBaseView::isExitAllowed()
{
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::resetView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::resetView()" );
    
    scrollToTopItem(listView());
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::resetView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::clearActivity(HbActivityManager& manager)
{
    foreach ( const QString& activity, mActivities ){
        manager.removeActivity(activity);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsBaseView::saveActivity(
    QDataStream& serializedActivity, QVariantHash& metaData)
{
    Q_UNUSED( serializedActivity );
    Q_UNUSED( metaData );
    return mActivities.isEmpty() ? QString() : mActivities.at(0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsBaseView::loadActivity(
    const QString& activityId, QDataStream& serializedActivity, QVariantHash& metaData)
{
    Q_UNUSED( activityId );
    Q_UNUSED( serializedActivity );
    Q_UNUSED( metaData );
    return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsBaseView::matchWithActivityId(const QString& activityId)
{
    return mActivities.contains(activityId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::activated(bool showDialer, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::activated()" );
    Q_UNUSED(args);
    //we have to set object tree of the repository to the current view
    mRepository.setObjectTreeToView( mViewId );

    if (!mInitialized) {
        initView();
    }
    mActivating = true;
    
    connect( mDialpad, SIGNAL( aboutToClose() ), this, 
            SLOT( dialpadClosed() ), Qt::QueuedConnection );
    connect( mDialpad, SIGNAL( aboutToOpen() ), this, 
            SLOT( dialpadOpened() ), Qt::QueuedConnection );
    connect( &mDialpad->editor(), SIGNAL( contentsChanged() ), this,
            SLOT( dialpadEditorTextChanged() ) );

    if (showDialer && !mDialpad->isOpen()) {
        openDialpad();
    }
    
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::activated()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::activationCompleted()
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::activationCompleted()" );
    mActivating = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deactivated()
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::deactivated()" );
    disconnect( mDialpad, SIGNAL( aboutToClose() ), this, 
            SLOT( dialpadClosed() ) );
    disconnect( mDialpad, SIGNAL( aboutToOpen() ), this, 
            SLOT( dialpadOpened() ) );
    disconnect( &mDialpad->editor(), SIGNAL( contentsChanged() ), this,
            SLOT( dialpadEditorTextChanged() ) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::cancelServiceRequest()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::cancelServiceRequest()" );
    if (mContact) {
        mContact->cancelServiceRequest();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::cancelServiceRequest()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::notSupported()
{
    HbMessageBox* messageBox = new HbMessageBox(QString("Not supported"));
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout( HbMessageBox::StandardTimeout );
    messageBox->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleBackSoftkey()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleBackSoftkey()" );
    mViewManager.activateView( LogsRecentViewId, false, QVariant() );
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleBackSoftkey()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::initFilterMenu()
{
    mShowFilterMenu = 
        qobject_cast<HbMenu*>( mRepository.findWidget( logsShowFilterMenuId ) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::addActionNamesToMap()
{
    mActionMap.clear();
    mActionMap.insert(XQService::LogsViewReceived, logsShowFilterReceivedMenuActionId);
    mActionMap.insert(XQService::LogsViewCalled, logsShowFilterDialledMenuActionId);
    mActionMap.insert(XQService::LogsViewMissed, logsShowFilterMissedMenuActionId);
    mActionMap.insert(XQService::LogsViewAll, logsShowFilterRecentMenuActionId);  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::initView()
{   
    Q_ASSERT_X( !mInitialized, "logs [UI] ", "view is already initialized!!" );
    mInitialized = true;
    initFilterMenu();
    addActionNamesToMap();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsBaseView::model() const
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::model()" );
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAbstractModel* LogsBaseView::logsModel() const
{
    return static_cast<LogsAbstractModel*>( model() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbListView* LogsBaseView::listView() const
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::listView()" );
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleExit()
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::handleExit()" );   
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::callKeyPressed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::callKeyPressed()" ); 
    // Call to topmost item in current list
    if ( !tryCallToDialpadNumber() && listView() && model() && model()->hasIndex(0,0) ) {
        LOGS_QDEBUG( "logs [UI]     Call to topmost item in list" ); 
        QModelIndex topIndex = model()->index(0,0);
        listView()->scrollTo( topIndex );
        listView()->setCurrentIndex( topIndex, QItemSelectionModel::Select );
        initiateCallback(topIndex); 
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::callKeyPressed()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::localeChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::localeChanged()" );
    if (logsModel()) {
        LogsConfigurationParams params;
        params.setLocaleChanged(true);
        logsModel()->updateConfiguration(params);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::localeChanged()" ); 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::showFilterMenu()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::showFilterMenu()" );
    if ( mShowFilterMenu ) {
        QRectF toolbarGeometry = toolBar()->geometry();
        QSizeF menuSize = mShowFilterMenu->size();
        LOGS_QDEBUG_2("logs [UI]    menusize:", menuSize)
        
        if ( layoutDirection() == Qt::LeftToRight ){
            QPointF pos( toolbarGeometry.bottomRight().x(),
                         toolbarGeometry.topRight().y() ); 
    
            mShowFilterMenu->setPreferredPos(pos,HbPopup::BottomRightCorner);
            LOGS_QDEBUG_2("logs [UI]    menu br pos:", pos)
        } else {
            QPointF pos( toolbarGeometry.topLeft().x(),
                         toolbarGeometry.topLeft().y() ); 
               
            mShowFilterMenu->setPreferredPos(pos,HbPopup::BottomLeftCorner);
            LOGS_QDEBUG_2("logs [UI]    menu bl pos:", pos)
        }
        mShowFilterMenu->open();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::showFilterMenu()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::openDialpad()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::openDialpad()" );
   
    updateCallButton();
    setDialpadPosition();
    mDialpad->openDialpad();
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::openDialpad()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::openContactsApp()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::openContactsApp()" );
    XQServiceRequest snd("com.nokia.services.phonebookappservices.Launch","launch()");
    XQRequestInfo info;
    info.setForeground(true);
    snd.setInfo(info);
    int retValue;
    snd.send(retValue);
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::openContactsApp()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::setDialpadPosition()
{
    HbMainWindow& window = mViewManager.mainWindow(); 
    QRectF screenRect = window.layoutRect();
    LOGS_QDEBUG_2( "logs [UI] -> LogsBaseView::setDialpadPosition(), screenRect:", 
            screenRect );
    if (window.orientation() == Qt::Horizontal) {
        // dialpad takes half of the screen width    
        mDialpad->setPos(QPointF(screenRect.width()/2,
                                 scenePos().y()));
        mDialpad->setPreferredSize(screenRect.width()/2,
                                   (screenRect.height()-scenePos().y())); 
    } else {
        // dialpad takes 55% of the screen height
        qreal screenHeight = screenRect.height();
        mDialpad->setPos(QPointF(0, screenHeight*0.45));
        mDialpad->setPreferredSize(screenRect.width(),
                                   screenHeight*0.55);        
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::setDialpadPosition()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::dialpadOpened()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::dialpadOpened()" );  
    if ( !tryMatchesViewTransition() ){
        updateWidgetsSizeAndLayout();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::dialpadOpened()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::dialpadClosed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::dialpadClosed()" );
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::dialpadClosed()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::dialpadEditorTextChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::dialpadEditorTextChanged()" );
    if ( !tryMatchesViewTransition() ) {
        updateCallButton();  
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::dialpadEditorTextChanged()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::changeFilter(HbAction* action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::changeFilter()" );
    XQService::LogsViewIndex view = mActionMap.key( action->objectName(),
            XQService::LogsViewAll );
    QVariant args(view);
    mViewManager.activateView( LogsRecentViewId, false, args );
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::changeFilter()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::addToContacts()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::addToContacts()" );
    if ( isDialpadInput() ){
        saveNumberInDialpadToContacts();
    } else if ( mContact ){
        // Use async connection to ensure that model can handle
        // contact operation completion before view
        QObject::connect(mContact, SIGNAL(saveCompleted(bool)),
                         this, SLOT(contactActionCompleted(bool)), 
                         Qt::QueuedConnection);
        saveContact();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::addToContacts()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::saveNumberInDialpadToContacts()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::saveNumberInDialpadToContacts()" );
    if (mDialpad->editor().text().length() > 0){
        delete mContact;
        mContact = 0;
        QString phoneNumber = 
            HbStringUtil::convertDigitsTo(mDialpad->editor().text(), WesternDigit);
        mContact = logsModel()->createContact(phoneNumber);
        saveContact();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::saveNumberInDialpadToContacts()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::showListItemMenu(
        HbAbstractViewItem* item, const QPointF& coords )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::showListItemMenu()" );
    HbMenu* itemContextMenu = new HbMenu();    
    itemContextMenu->setDismissPolicy(HbMenu::TapAnywhere);
    itemContextMenu->setAttribute(Qt::WA_DeleteOnClose);
    
    updateListItemData(item->modelIndex());    
    populateListItemMenu(*itemContextMenu);

    if (mDialpad->isOpen()) {
        mDialpad->closeDialpad();
    }    
    if (itemContextMenu->actions().count() > 0) {
        itemContextMenu->setPreferredPos(coords,HbPopup::TopLeftCorner);
        itemContextMenu->open();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::populateListItemMenu(HbMenu& menu)
{
    delete mCallTypeMapper;
    mCallTypeMapper = 0;

    if (mCall) {
        mCallTypeMapper = new QSignalMapper();
                
        foreach(LogsCall::CallType callType, mCall->allowedCallTypes()){
            HbAction* callAction = new HbAction;
            if (callType == LogsCall::TypeLogsVoiceCall){
                callAction->setText(hbTrId("txt_common_menu_voice_call"));
            }
            else if (callType == LogsCall::TypeLogsVideoCall){
                callAction->setText(hbTrId("txt_common_menu_video_call"));
            }
            else if (callType == LogsCall::TypeLogsVoIPCall){
                callAction->setText(hbTrId("txt_common_menu_internet_call"));
            }
            
            connect(callAction, SIGNAL(triggered()),
                    mCallTypeMapper, SLOT( map()) );
            mCallTypeMapper->setMapping(callAction, callType);
            
            // Default call type must be the first item in context menu
            if (callType != mCall->defaultCallType() ||
                menu.actions().count() == 0){
                menu.addAction(callAction);
            } else {   
                menu.insertAction(menu.actions().at(0), callAction);
            }           
        }    
        connect(mCallTypeMapper, SIGNAL(mapped(int)),
                this, SLOT( initiateCall(int)) );
    }
    
    if (mMessage) {
        HbAction* messageAction = new HbAction;
        messageAction->setText(hbTrId("txt_common_menu_send_message"));
        menu.addAction(messageAction);
        QObject::connect( messageAction, SIGNAL(triggered()), 
                          this, SLOT( createMessage() ) );
    }
    if (mContact) {
        HbAction* contactAction = new HbAction;
        if (mContact->allowedRequestType() ==
                LogsContact::TypeLogsContactOpen) {
            contactAction->setText(hbTrId("txt_dialer_ui_menu_open_contact"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              mContact, SLOT(open()) );
        } else if (mContact->allowedRequestType() ==
                LogsContact::TypeLogsContactOpenGroup) {
            contactAction->setText(hbTrId("txt_dialer_menu_open_group"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              mContact, SLOT(open()) );
        }
        else {
            contactAction->setText(hbTrId("txt_common_menu_add_to_contacts"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              this, SLOT(saveContact()) );
        
        }
        menu.addAction(contactAction);
    }    
    if (mDetailsModel) {
        HbAction* callDetailsAction = new HbAction;
        callDetailsAction->setText(hbTrId("txt_dialer_ui_menu_call_details"));
        menu.addAction(callDetailsAction);
        QObject::connect(callDetailsAction, SIGNAL(triggered()), 
                         this, SLOT(showCallDetails()));
        
        HbAction* deleteAction = new HbAction;
        deleteAction->setText(hbTrId("txt_common_menu_delete"));
        menu.addAction(deleteAction);
        QObject::connect(deleteAction, SIGNAL(triggered()), 
                         this, SLOT(deleteEvent()));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListItemData(const QModelIndex& listIndex)
{
    updateCall(listIndex);
    updateMessage(listIndex);
    updateContact(listIndex);
    updateDetailsModel(listIndex);
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateCallButton()
{
    mDialpad->setCallButtonEnabled( !mDialpad->editor().text().isEmpty() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsBaseView::tryCallToDialpadNumber( LogsCall::CallType callType )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::tryCallToDialpadNumber()" );
    bool called = false;
    if ( isDialpadInput() ){
        // Call to inputted number
        QString phoneNumber = 
            HbStringUtil::convertDigitsTo(mDialpad->editor().text(), WesternDigit);
        LogsCall::callToNumber( callType, phoneNumber );
        called = true;
    }
    LOGS_QDEBUG_2( "logs [UI] <- LogsBaseView::tryCallToDialpadNumber(), called",
                   called );
    return called;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsBaseView::tryMessageToDialpadNumber()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::tryMessageToDialpadNumber()" );
    bool messageSent = false;
    if ( isDialpadInput() ){
        // Message to inputted number
        QString phoneNumber = 
            HbStringUtil::convertDigitsTo(mDialpad->editor().text(), WesternDigit);
        LogsMessage::sendMessageToNumber( phoneNumber );
        messageSent = true;
    }
    LOGS_QDEBUG_2( "logs [UI] <- LogsBaseView::tryMessageToDialpadNumber(), sent", 
                 messageSent );
    return messageSent;
}
 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::initiateCallback(const QModelIndex& index)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::initiateCallback()" );
    updateCall(index);

    if (mCall) {
        mCall->initiateCallback();
    }
    delete mCall;
    mCall = 0;
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::initiateCallback()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::initiateCall(int callType)
{       
    if (mCall) {
        mCall->call( static_cast<LogsCall::CallType>(callType) );
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::createMessage()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::createMessage()" );
    if (mMessage) {
        mMessage->sendMessage();
    }
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::createMessage()" );   
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::saveContact()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::saveContact()" ); 
    
    if (mContact){
        HbDialog* popup = new HbDialog();
        popup->setDismissPolicy(HbDialog::NoDismiss);
        popup->setHeadingWidget(
                new HbLabel(hbTrId("txt_dial_title_add_to_contacts"), popup));
        popup->setAttribute(Qt::WA_DeleteOnClose);
        popup->setTimeout( HbPopup::NoTimeout );
        popup->addAction(
                new HbAction(hbTrId("txt_common_button_cancel"), popup));

        HbWidget* buttonWidget = new HbWidget(popup);
        QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
        
        HbPushButton* addButton = new HbPushButton(buttonWidget);
        addButton->setStretched(true);
        addButton->setText(hbTrId("txt_dial_button_save_as_a_new_contact"));
        HbIcon plusIcon("qtg_mono_plus");
        addButton->setIcon(plusIcon);
        connect(addButton, SIGNAL(clicked()), popup, SLOT(close()));
        connect(addButton, SIGNAL(clicked()), mContact, SLOT(addNew()));
        
        HbPushButton* updateButton = new HbPushButton(buttonWidget);
        updateButton->setStretched(true);
        updateButton->setText(hbTrId("txt_dial_button_update_existing_contact"));
        updateButton->setIcon(plusIcon);
        connect(updateButton, SIGNAL(clicked()), popup, SLOT(close()));
        connect(updateButton, SIGNAL(clicked()),
                mContact, SLOT(updateExisting()));
        
        layout->addItem(addButton);
        layout->addItem(updateButton);
        
        buttonWidget->setLayout(layout);
        popup->setContentWidget(buttonWidget);

        popup->open();
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::saveContact()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateCall(const QModelIndex &listIndex)
{
    delete mCall;
    mCall = 0;
    if ( model() ) {
        mCall = qVariantValue<LogsCall*>( model()->data( 
                listIndex, LogsAbstractModel::RoleCall ) );    
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateMessage(const QModelIndex &listIndex)
{
    delete mMessage;
    mMessage = 0;
    if ( model() ) {
        mMessage = qVariantValue<LogsMessage*>( model()->data( 
                listIndex, LogsAbstractModel::RoleMessage ) );
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateContact(const QModelIndex &listIndex)
{
    delete mContact;
    mContact = 0;
    if ( model() ) {
        mContact = qVariantValue<LogsContact*>( model()->data( 
                listIndex, LogsAbstractModel::RoleContact ) );    
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateDetailsModel(const QModelIndex &listIndex)
{
    delete mDetailsModel;
    mDetailsModel = 0;
    if ( model() ) {
        mDetailsModel = qVariantValue<LogsDetailsModel*>( model()->data( 
                listIndex, LogsAbstractModel::RoleDetailsModel ) );    
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::activateEmptyListIndicator(QAbstractItemModel* model)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::activateEmptyListIndicator()" );
    if ( !mEmptyListLabel ){
        mEmptyListLabel = qobject_cast<HbLabel*>(
                            mRepository.findWidget( logsLabelEmptyListId ) );
    }
    if ( model ){
         // Listen for changes in model and update empty list label accordingly
         connect( model, SIGNAL(rowsInserted(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()), 
             Qt::UniqueConnection);
         connect( model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()), 
             Qt::UniqueConnection);
         connect( model, SIGNAL(modelReset()), 
             this, SLOT(updateEmptyListWidgetsVisibility()), 
             Qt::UniqueConnection);
         // Update to reflect current situation
         updateEmptyListWidgetsVisibility();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::activateEmptyListIndicator()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deactivateEmptyListIndicator(QAbstractItemModel* model)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::deactivateEmptyListIndicator()" );
    if ( model ){
         disconnect( model, SIGNAL(rowsInserted(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         disconnect( model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         disconnect( model, SIGNAL(modelReset()), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::deactivateEmptyListIndicator()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::addViewSwitchingEffects()
{
    // Add view switching effects, these are called when 
    // HbMainWindow::setCurrentView is called, may be removed once orbit
    // has some built-in effects for view switching.
    HbEffect::add(this, ":/view_show.fxml", "show");
    HbEffect::add(this, ":/view_hide.fxml", "hide");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateEmptyListWidgetsVisibility()
{
    updateEmptyListLabelVisibility();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateEmptyListLabelVisibility()
{
    if ( mEmptyListLabel && model() ){
        LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::updateEmptyListLabelVisibility()" );   
        bool visible(model()->rowCount() == 0);
        mEmptyListLabel->setVisible(visible);
    }  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::showCallDetails()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::showCallDetails()" );
    
    if ( !mDetailsModel ){
        return;
    }
    
    QVariant arg = qVariantFromValue( mDetailsModel );
    if ( mViewManager.activateView(LogsDetailsViewId, false, arg) ){
        mDetailsModel = 0;
    }
   
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::showCallDetails()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deleteEvent()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::deleteEvent()" );
    if ( mDetailsModel ) {
        HbMessageBox::question(
                hbTrId("txt_dialer_info_call_event_will_be_removed_from"),
                this,
                SLOT(deleteEventAnswer(int)),
                HbMessageBox::Ok | HbMessageBox::Cancel,
                new HbLabel(hbTrId("txt_dialer_ui_title_delete_event")));
    }    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::deleteEvent()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deleteEventAnswer(int action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::deleteEventAnswer()" );
    if ((action == HbMessageBox::Ok) && mDetailsModel) {
        mDetailsModel->clearEvent();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::deleteEventAnswer()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::videoCallToCurrentNum()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::videoCallToCurrentNum()" );
    tryCallToDialpadNumber( LogsCall::TypeLogsVideoCall );
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::videoCallToCurrentNum()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::sendMessageToCurrentNum()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::sendMessageToCurrentNum()" );
    tryMessageToDialpadNumber();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::sendMessageToCurrentNum()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateWidgetsSizeAndLayout()
{
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::contactActionCompleted(bool modified)
{
    Q_UNUSED(modified);
}

// -----------------------------------------------------------------------------
// Sets appropriate list widget layout using listView.setLayoutName()
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListLayoutName( HbListView& listView, bool ignoreDialpad )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateListLayoutName()" );
    QString newLayout( logsListDefaultLayout  );
    Qt::Orientation orientation = mViewManager.mainWindow().orientation();    
    if (orientation == Qt::Horizontal) {
        if (mDialpad->isOpen()) {
            if (ignoreDialpad) {
                newLayout = QString( logsListDefaultLayout );
            } else {
                newLayout = QString( logsListLandscapeDialpadLayout );
            }
        } else {
            newLayout = QString( logsListLandscapeLayout );
        }
    } else {
        newLayout = QString( logsListDefaultLayout );
    }
    
    if (newLayout != listView.layoutName()) {
        LOGS_QDEBUG_2( "logs [UI]  setting new list layout name: ", newLayout );
        listView.setLayoutName( newLayout );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateListLayoutName()" );
}

// -----------------------------------------------------------------------------
// Loads appropriate section from *.docml to resize list widget
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListSize( HbListView& list )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateListSize()" );
    QString newSection( logsViewDefaultSection );
    Qt::Orientation orientation = mViewManager.mainWindow().orientation();

    if (mDialpad->isOpen()) {
        if (orientation == Qt::Horizontal) {
            newSection = QString( logsViewLandscapeDialpadSection );
        } else {
            newSection = QString( logsViewPortraitDialpadSection );
        }
    } else {
        newSection = QString( logsViewDefaultSection );
    }
    
    bool sectionChanged( mLayoutSectionName != newSection );
    mLayoutSectionName = newSection;
    LOGS_QDEBUG_2( "logs [UI]  loading new section: ", newSection );
    mRepository.loadSection( viewId(), newSection );
    
    if ( sectionChanged ){
        bool listSizeDecreased(mDialpad->isOpen());
        ensureListPositioning( list, listSizeDecreased );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateListSize()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleOrientationChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleOrientationChanged()" );
    setDialpadPosition();
    updateWidgetsSizeAndLayout();    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleOrientationChanged()");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::toggleActionAvailability( HbAction* action, bool available )
{
    if ( action ){
        action->setVisible( available );
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateContactSearchAction()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateContactSearchAction()" );
    HbAction* contactSearchAction = qobject_cast<HbAction*>( 
                      mRepository.findObject( logsRecentViewContactSearchMenuActionId ) );
    if ( contactSearchAction ) {
        if ( isContactSearchPermanentlyDisabled() ){
           contactSearchAction->setVisible(false);
        } else if ( isContactSearchEnabled() ){
           contactSearchAction->setText(hbTrId("txt_dialer_ui_opt_contact_search_off"));
           contactSearchAction->setVisible(mDialpad->isOpen());
        } else {
           contactSearchAction->setText(hbTrId("txt_dialer_ui_opt_contact_search_on"));
           contactSearchAction->setVisible(mDialpad->isOpen());
        }
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateContactSearchAction()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::isContactSearchEnabled
// -----------------------------------------------------------------------------
//
bool LogsBaseView::isContactSearchEnabled() const
{
    int currSetting = logsModel() ? 
        logsModel()->predictiveSearchStatus() : -1;
    return ( currSetting == logsContactSearchEnabled );
}

// -----------------------------------------------------------------------------
// LogsBaseView::isContactSearchPermanentlyDisabled
// -----------------------------------------------------------------------------
//
bool LogsBaseView::isContactSearchPermanentlyDisabled() const
{
    int currSetting = logsModel() ? 
        logsModel()->predictiveSearchStatus() : -1;
    return ( currSetting == logsContactSearchPermanentlyDisabled || 
        currSetting < 0 );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateDialpadCallAndMessagingActions()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateDialpadCallAndMessagingActions()" );
    HbAction* videoCallAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsCommonVideoCallMenuActionId ) );
    HbAction* sendMessageAction = qobject_cast<HbAction*>( 
            mRepository.findObject( logsCommonMessageMenuActionId ) );
    
    bool visible( isDialpadInput() );
    
    toggleActionAvailability( videoCallAction, visible );
    toggleActionAvailability( sendMessageAction, visible );
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateDialpadCallAndMessagingActions()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
bool LogsBaseView::tryMatchesViewTransition()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::tryMatchesViewTransition()" );
    bool viewChanged = false;
    if ( mDialpad->editor().text().length() > 0  && isContactSearchEnabled() ) {
        viewChanged = mViewManager.activateView( LogsMatchesViewId, true, QVariant() ); 
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::tryMatchesViewTransition()" );
    return viewChanged;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
bool LogsBaseView::isDialpadInput() const 
{
    return ( mDialpad->isOpen() && !mDialpad->editor().text().isEmpty() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::ensureListPositioning(HbListView& list, bool listSizeDecreased)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::ensureListPositioning()" );
    
    HbWidget* content = 
        qobject_cast<HbWidget*>( mRepository.findWidget( logsContentId ) );
    QList<HbAbstractViewItem *> visibleItems = list.visibleItems();
    if ( content && visibleItems.count() > 0 ){
        QRectF rect = content->contentsRect();
        LOGS_QDEBUG_2( "logs [UI]   contentsRect:", rect );
        
        // Important to force geometry as otherwise in dialpad opening case,
        // list is not scrollable if having few items (e.g. 4), find out current
        // fully visible item before geometry change so that list can be positioned
        // correctly  
        HbAbstractViewItem* firstFullyVisibleItem = visibleItems.at(0);
        qreal itemHeight = firstFullyVisibleItem->size().height();
        
        // If at least 3/4 of the item is visible, it is considered as first fully visible
        bool fullyVisible = ( firstFullyVisibleItem->mapRectToItem(
            &list, firstFullyVisibleItem->boundingRect()).y() + itemHeight / 4 >= 0 );
        if ( !fullyVisible && visibleItems.count() > 1 ){
            firstFullyVisibleItem = visibleItems.at(1);
        }
        QRectF listRect = rect;
        listRect.adjust(0, list.pos().y(), 0, 0);
        LOGS_QDEBUG_2( "logs [UI]   listRect:", listRect );
        list.setGeometry(listRect);
        
        HbModelIterator* modelIt = list.modelIterator();
        bool allItemsFitToReservedRect = true;
        if ( modelIt && itemHeight > 0 ) {
            int maxVisibleItems = rect.height() / itemHeight;
            LOGS_QDEBUG_2( "logs [UI]   max visible items:", maxVisibleItems );
            allItemsFitToReservedRect = ( modelIt->indexCount() <= maxVisibleItems );
            if ( allItemsFitToReservedRect ){
                // All items can fit the rect reserved for the list, force them to fit
                list.ensureVisible(QPointF(0,0));
            } else {
                // All items cannot fit the rect reserved, force to reserve whole
                // area so that current fully visible item is tried to be set to top
                list.scrollTo(firstFullyVisibleItem->modelIndex(), 
                              HbAbstractItemView::PositionAtTop);
               
            }
        }
        if ( listSizeDecreased && !allItemsFitToReservedRect ){
            // Make sure that user understands that list is scrollable after geometry
            // changed to smaller
            HbScrollArea::ScrollBarPolicy prevPolicy = list.verticalScrollBarPolicy();
            list.setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
            list.setVerticalScrollBarPolicy(prevPolicy);
        }
    }   
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::ensureListPositioning()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::scrollToTopItem
// -----------------------------------------------------------------------------
//
void LogsBaseView::scrollToTopItem( HbListView* list )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::scrollToTopItem()" );

    if ( list ){
        if ( model() && model()->hasIndex(0,0) ) {
            QModelIndex topIndex = model()->index(0,0);
            list->scrollTo( topIndex );
        }
        // Force also scrollbar to show itself correctly
        if ( list->verticalScrollBar() ){
            list->verticalScrollBar()->setValue(0.0);
        }
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::scrollToTopItem()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateMenuVisibility()
{
   bool visibleActionsExist = false;
   HbMenu* optionsMenu = mOptionsMenu ? mOptionsMenu : menu();
   foreach (QAction* action, optionsMenu->actions()) {
       if  (action->isVisible()) {
           visibleActionsExist = true;
       }
   }
   setMenuVisible(visibleActionsExist);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::setMenuVisible(bool visible)
{
    if (!visible && !mOptionsMenu) {
        mOptionsMenu = takeMenu();
    } else if (visible && mOptionsMenu) {
        setMenu(mOptionsMenu);
        mOptionsMenu = 0;
    }
}
