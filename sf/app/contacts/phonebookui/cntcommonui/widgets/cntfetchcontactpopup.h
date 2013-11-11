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

#ifndef CNTFETCHCONTACTPOPUP_H_
#define CNTFETCHCONTACTPOPUP_H_

#include <qtcontacts.h>
#include <hbabstractitemview.h>
#include <QSet>

class HbDialog;
class HbListView;
class HbLabel;
class HbAction;
class HbSearchPanel;
class HbStaticVkbHost;

class QGraphicsLinearLayout;
class QItemSelectionModel;

class CntListModel;
class CntFetchMarkAll;
class CntDocumentLoader;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

/*!
  CntFetchContactPopup is simple to use popup for fetching (selecting) number
  of contacts.
  
  \code
  \endcode
  
  Note that by default, CntFetchContactPopup deletes itself when closing the popup.
*/
class CntFetchContactPopup : public QObject
{
    Q_OBJECT
    
public:
    ~CntFetchContactPopup();
    
    static CntFetchContactPopup* createMultiSelectionPopup(  QString aTitle, QString aAction, QContactManager& aContactMgr );
    static CntFetchContactPopup* createSingleSelectionPopup( QString aTitle, QContactManager& aContactMgr );

    void setSelectedContacts( QSet<QContactLocalId> aIds );
    void showPopup();
    
signals:
    void fetchReady( QSet<QContactLocalId> aIds );
    void fetchCancelled();

private slots:
    void setFilter( const QString& aFilter );
    void contactSelected( const QModelIndex& aIndex );
    void contactsSelected( const QItemSelection & selected, const QItemSelection & deselected );
    void selectAll( int aState );
    void dialogDismissed( HbAction* aAction );
    void loadLayout( Qt::Orientation aOrientation );
    void orientationChanged( Qt::Orientation aOrientation );
    void handleKeypadOpen();
    void handleKeypadClosed();
    void closePopup();
    void disconnectSignalsAndDelete();
    
private:
    CntFetchContactPopup( QContactManager& aManager );
    void constructPopupDialog( QString aTitle, QString aAction, HbAbstractItemView::SelectionMode aMode );
    
private:
    QContactManager& mManager;
    QList<QContactLocalId> mIdList;
    
    HbDialog* mPopup; // own
    HbListView* mListView; // own
    HbLabel* mEmptyView; // own
    HbLabel* mHeading; // own
    HbAction* mPrimaryAction; // own
    HbSearchPanel* mSearch; //own
    CntListModel* mModel; // own
    HbStaticVkbHost* mVirtualKeyboard; // own
    CntFetchMarkAll* mMarkAll; // own
    QItemSelectionModel* mSelectionModel; // own
    CntDocumentLoader* mDoc; // own
    QString mTitle;
    QList<QContactLocalId> mIds;
};

#endif /* CNTFETCHCONTACTPOPUP_H_ */
