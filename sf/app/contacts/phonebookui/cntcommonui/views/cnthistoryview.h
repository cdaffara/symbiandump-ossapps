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

#ifndef CNTHISTORYVIEW_H
#define CNTHISTORYVIEW_H

#include <QObject>
#include <cntabstractview.h>
#include <qcontactid.h>

class HbListView;
class CntHistoryModel;
class HbView;
class QModelIndex;
class HbAction;
class HbDocumentLoader;
class XQAiwRequest;
class HbAbstractViewItem;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntHistoryView : public QObject, public CntAbstractView
{
    Q_OBJECT
    
public: // From CntAbstractView
    CntHistoryView();
    ~CntHistoryView();
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const{ return false; }
    HbView* view() const { return mView; }
    int viewId() const { return historyView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    
public slots:
    void updateScrollingPosition();
    void clearHistory();
    void handleClearHistory(int action);
    void itemActivated(const QModelIndex &index);
    void showPreviousView();
    void showClearHistoryMenu();
    void showRootView();
    void contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds);
    void onLongPressed (HbAbstractViewItem *item, const QPointF &coords);
    
    
    
private:
    HbDocumentLoader* docLoader();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif    
    HbListView*                 mHistoryListView; // not own
    CntHistoryModel*            mHistoryModel; // own
    HbView*                     mView; // not own
    HbDocumentLoader*           mDocumentLoader; // own
    CntAbstractViewManager*     mViewMgr; // not own
    HbAction*                   mBackKey; // not own
    QContact*                   mContact; // own
    HbAction*                   mClearHistory;  // not own
    CntViewParameters           mArgs;
    XQAiwRequest*               mRequest;    
    CntAbstractEngine*          mEngine;
};

#endif // CNTHISTORYVIEW_H

// EOF
