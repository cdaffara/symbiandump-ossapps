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
#ifndef LOGSDETAILSVIEW_H
#define LOGSDETAILSVIEW_H

#include "logsbaseview.h"
#include <hblistviewitem.h>

class HbListView;
class HbGroupBox;
class HbAction;
class LogsComponentRepository;
class HbAction;
class LogsAbstractViewManager;
class QGraphicsLinearLayout;


/**
 * 
 */
class LogsDetailsView : public LogsBaseView
{
    Q_OBJECT
    friend class UT_LogsDetailsView;
    
public:
    explicit LogsDetailsView( LogsComponentRepository& repository,
                              LogsAbstractViewManager& viewManager );
    ~LogsDetailsView();
    
public: // From LogsBaseView
    
    virtual void activated(bool showDialer, QVariant args);
    virtual void deactivated();
    virtual QString saveActivity(QDataStream& serializedActivity, QVariantHash& metaData);
    virtual QVariant loadActivity(
        const QString& activityId, QDataStream& serializedActivity, QVariantHash& metaData);

public slots:
    
    void callKeyPressed();
    
private slots:
    
    void initiateVoiceCall();
    void initiateVideoCall();
    void sendMessage();
    void openContact();
    void copyNumberToClipboard();
    
    //from LogsBaseView
    virtual void handleBackSoftkey();
    virtual void deleteEventAnswer(int action);
    
private: // from LogsBaseView
    
    virtual void initView();
    virtual QAbstractItemModel* model() const;
    virtual void dialpadEditorTextChanged();
    virtual void updateWidgetsSizeAndLayout();
    virtual void contactActionCompleted(bool modified);
    
private:
    
    void initListWidget();
    void updateMenu();

private:
    
    HbGroupBox* mViewName; //not owned
    HbListView* mListView; //not owned
};

class LogsDetailsViewItem : public HbListViewItem
{
    Q_OBJECT
    friend class UT_LogsDetailsView;
    
public:
    
    explicit LogsDetailsViewItem();
    virtual ~LogsDetailsViewItem();
    virtual HbAbstractViewItem *createItem();
    virtual void updateChildItems();
    
protected: // From HbAbstractViewItem
    
    /*
    * highlighting in detailsview is removed by
    * overriding the pressStateChanged method to do nothing.
    */
    virtual void pressStateChanged(bool value, bool animate);

    
private slots:

    void groupBoxClicked(bool collapsed);
    
private:
    QGraphicsLinearLayout* mLayout;
};



#endif // LOGSDETAILSVIEW_H
