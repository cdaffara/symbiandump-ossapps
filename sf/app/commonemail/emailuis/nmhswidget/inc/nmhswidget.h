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

#ifndef NMHSWIDGET_H
#define NMHSWIDGET_H

#include <hbwidget.h>
#include "nmcommon.h"

class NmHsWidgetEmailEngine;
class NmHsWidgetTitleRow;
class QGraphicsLinearLayout;
class QTranslator;
class HbFrameDrawer;
class NmHsWidgetDateTimeObserver;
class HbLabel;
class HbDocumentLoader;
class HbListView;
class NmHsWidgetListModel;
class QModelIndex;


class NmHsWidget : public HbWidget
{
    Q_OBJECT
    
    Q_PROPERTY(QString accountId READ accountId WRITE setAccountId)
    Q_PROPERTY(QString accountIconName READ accountIconName WRITE setAccountIconName)
    Q_PROPERTY(QString widgetState READ widgetStateProperty WRITE setWidgetStateProperty)
  
public:
    NmHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidget();

    QPainterPath shape() const;
    
public slots:
    //from home screen fw
    void onInitialize();
    void onUninitialize();
    void onShow();
    void onHide();
    //engine
    void updateMailData();
    void onEngineException(const int& exc);
    //properties
    void setAccountId(const QString &text);
    QString accountId() const;
    void setAccountIconName(const QString &text);
    QString accountIconName() const;   
    void setWidgetStateProperty(QString value);
    QString widgetStateProperty();
    //user actions
    void handleExpandCollapseEvent();
    
    void openMessage(const QModelIndex& index);

signals: 
    void finished();
    void setPreferences(const QStringList &names);
    void error();
private:
    void setupLocalization();
    void setupUi();
    bool loadDocML(HbDocumentLoader &loader);
    void addNoMailsLabelToLayout();
    void removeNoMailsLabelFromLayout();
    void addEmailRowsToLayout();
    void removeEmailRowsFromLayout();
    void updateLayout(const int visibleCount);
    void toggleExpansionState();  
    void createMailRowsList();
protected:
    
private:
    //UI components
    HbWidget* mMainContainer;               //container including all UI components
    HbWidget* mEmptySpaceContainer;         //container for empty space above mWidgetContainer
    HbWidget* mWidgetContainer;             //container including title row and mContentContainer
    NmHsWidgetTitleRow* mTitleRow;          //mailbox icon, name, unread count and expand button 
    HbWidget* mContentContainer;            //container for mail rows or mNoMailsLabel
    HbLabel* mNoMailsLabel;                 //label shown in mContentContainer when no mails in inbox
    QGraphicsLinearLayout* mContentLayout;  //layout used in mContentContainer    
    HbFrameDrawer* mBackgroundFrameDrawer;  //drawer for widget background shown in mWidgetContainer
    QTranslator* mTranslator;
    
    NmHsWidgetEmailEngine* mEngine;
    NmId mAccountId;                        //mail account id widget is monitoring
    QString mAccountIconName;               //name of mailbox icon shown in titlerow
    NmHsWidgetDateTimeObserver* mDateObserver;  //observer for time/locale changes    
    bool mIsExpanded;                       //true when widget expanded, false when collapsed  
    HbListView* mListView;                  //list view for messages 
    NmHsWidgetListModel* mListModel;        //list model for mListView
    
public:    
    friend class TestNmHsWidget;     
};

#endif  // NMHSWIDGET_H
