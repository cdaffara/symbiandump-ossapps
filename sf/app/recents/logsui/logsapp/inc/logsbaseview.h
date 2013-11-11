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
#ifndef LOGSBASEVIEW_H
#define LOGSBASEVIEW_H

#include <hbview.h>
#include <xqaiwdecl.h>
#include "logsdefs.h"
#include "logscall.h"

class LogsComponentRepository;
class LogsAbstractViewManager;
class Dialpad;
class LogsCall;
class LogsMessage;
class LogsContact;
class LogsDetailsModel;
class HbAbstractViewItem;
class HbMenu;
class QSignalMapper;
class HbLabel;
class HbListView;
class LogsAbstractModel;
class QDataStream;
class HbActivityManager;

/**
 * 
 */
class LogsBaseView : public HbView
{
    Q_OBJECT
    friend class UT_LogsBaseView;
    friend class UT_LogsViewManager;
    
public:
    
    virtual ~LogsBaseView();

public:
    enum LogsViewMap{
        ViewAll,
        ViewCalled,
        ViewReceived,
        ViewMissed
    };
    
    LogsAppViewId viewId() const;
    virtual void activated(bool showDialer, QVariant args);
    virtual void deactivated();
    virtual bool isExitAllowed();
    virtual void resetView(); 
    virtual void clearActivity(HbActivityManager& manager);
    virtual QString saveActivity(QDataStream& serializedActivity, QVariantHash& metaData);
    virtual QVariant loadActivity(
        const QString& activityId, QDataStream& serializedActivity, QVariantHash& metaData);
    virtual bool matchWithActivityId(const QString& activityId);
    virtual void cancelServiceRequest();

public slots:
    
    virtual void handleExit();
    virtual void callKeyPressed();
    void localeChanged();
    
signals:

    void exitAllowed();

protected slots:
    
    virtual void handleBackSoftkey();
    virtual void dialpadOpened();
    virtual void dialpadClosed();
    virtual void dialpadEditorTextChanged();
    virtual void updateEmptyListWidgetsVisibility();
    virtual void updateWidgetsSizeAndLayout();
    virtual void contactActionCompleted(bool modified);
    virtual void updateEmptyListLabelVisibility();
    
    //slots bellow are used in *.docml
    void showFilterMenu();
    void openDialpad();
    void openContactsApp();
    void notSupported();
    void changeFilter(HbAction* action);
    void addToContacts();
    void saveNumberInDialpadToContacts();
    
    //other slots
    void showListItemMenu(HbAbstractViewItem* item, const QPointF& coords);    
    void initiateCallback(const QModelIndex &listIndex);
    void initiateCall(int callType);
    void createMessage();
    void saveContact();
    void showCallDetails();
    
    void deleteEvent();
    virtual void deleteEventAnswer(int action);
    
    void videoCallToCurrentNum();
    void sendMessageToCurrentNum();

    void handleOrientationChanged();
    
protected:
  
    explicit LogsBaseView( LogsAppViewId viewId, 
                           LogsComponentRepository& repository, 
                           LogsAbstractViewManager& viewManager );
        
    void setDialpadPosition();
    void initFilterMenu();
    void addActionNamesToMap();

    void updateCall(const QModelIndex& listIndex);
    void updateMessage(const QModelIndex& listIndex);
    void updateContact(const QModelIndex& listIndex);
    void updateDetailsModel(const QModelIndex &listIndex);
    
    /**
     * Sets appropriate list widget layout using listView.setLayoutName()
     * @param ignoreDialpad if true, dialpad visiblilty won't affect layout
     */
    void updateListLayoutName( HbListView& list, bool ignoreDialpad = false );
    /**
     * Loads appropriate section from *.docml to resize list widget
     */
    void updateListSize( HbListView& list );
    
    /**
     * Derived class should call this once it has completed activation.
     */
    virtual void activationCompleted();
  
protected:
    
    /**
     * Initialize view widgets from the docml file. Must be called once on first 
     * view activation.
     */
    virtual void initView();
    
    virtual QAbstractItemModel* model() const;
    
    virtual LogsAbstractModel* logsModel() const;
    
    virtual HbListView* listView() const;
    
    virtual void populateListItemMenu(HbMenu& menu);
    
    /**
     * Updates data specific to the list item, e.g. call/message/contact
     */
    virtual void updateListItemData(const QModelIndex& listIndex);
    
    /*
     * Update call button state.
     */
    virtual void updateCallButton();
    
    /**
     * Make call if dialpad is opened and contains inputted number.
     * @return true if call was made
     */
    bool tryCallToDialpadNumber(
        LogsCall::CallType callType = LogsCall::TypeLogsVoiceCall);
    bool tryMessageToDialpadNumber();
	
    void activateEmptyListIndicator(QAbstractItemModel* model);
    void deactivateEmptyListIndicator(QAbstractItemModel* model);
    
    void addViewSwitchingEffects();
    void toggleActionAvailability( HbAction* action, bool available );

    void updateContactSearchAction();
    bool isContactSearchEnabled() const;
    bool isContactSearchPermanentlyDisabled() const;
    void updateDialpadCallAndMessagingActions();
    bool tryMatchesViewTransition();
    bool isDialpadInput() const;
    void ensureListPositioning( HbListView& list, bool listSizeDecreased );
    void scrollToTopItem( HbListView* list );
    
    void updateMenuVisibility();
    void setMenuVisible(bool visible);
    
protected:
    
    LogsAppViewId mViewId;
    LogsComponentRepository& mRepository;
    LogsAbstractViewManager& mViewManager;
    
    HbMenu* mShowFilterMenu; //not owned
    Dialpad* mDialpad; //not owned
    HbLabel* mEmptyListLabel; // not owned
    
    QMap<XQService::LogsViewIndex, QString>   mActionMap;
    
    bool mInitialized;
    
    LogsCall* mCall;       //owned
    LogsMessage* mMessage; //owned
    LogsContact* mContact; //owned
    LogsDetailsModel* mDetailsModel; //owned
    
    QSignalMapper* mCallTypeMapper;
    QString mLayoutSectionName;
    
    QStringList mActivities;
    HbMenu* mOptionsMenu;
    bool mActivating;
};




#endif // LOGSBASEVIEW_H
