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

#ifndef CNTGROUPACTIONSVIEW_H
#define CNTGROUPACTIONSVIEW_H

#include <hbdocumentloader.h>

#include <cntabstractview.h>

class HbView;
class HbIcon;
class HbAction;
class HbListView;
class HbAbstractViewItem;
class QModelIndex;
class QStandardItemModel;
class CntActionLauncher;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactAction;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupActionsView : public QObject, public CntAbstractView
{
    Q_OBJECT

public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return groupActionsView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    
public:
    CntGroupActionsView();
    ~CntGroupActionsView();
    
#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif    
    
    void showPreviousView();
    void editGroup();
    void listItemSelected(const QModelIndex &index);
    void executeAction(QContact&, QContactDetail, QString);
    void actionExecuted(CntActionLauncher* aAction);
    void actionCancelled();
    void executeConferenceCallAction(QContact& aContact, const QContactDetail& aDetail, const QString& aAction);
    void onLongPressed (HbAbstractViewItem *item, const QPointF &coords);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif      
    void populatelist(QString label,HbIcon icon, QString secondaryText, QString action);
    bool sendKeyPressed();
   
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    
    QContact*                   mGroupContact;
    HbAction*                   mEditGrpDetailAction; 
    QStandardItemModel*         mModel; // own
    CntAbstractViewManager*     mViewManager;
    HbDocumentLoader            mDocumentLoader;
    HbView*                     mView; // own

    HbAction*                   mSoftkey; // owned by view
    HbListView*                 mListView; // owned by layout
    int                         mPopupCount;
    QStringList                 mEmailActionParams;
    QVariantMap                 mMessageActionParams;
    bool                        mIsExecutingAction;
    
    CntAbstractEngine*          mEngine;
};

#endif // CNTGROUPACTIONSVIEW_H

// EOF
