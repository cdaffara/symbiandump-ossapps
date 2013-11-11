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

#ifndef CNTABSTRACTLISTVIEW_H_
#define CNTABSTRACTLISTVIEW_H_

#include "cntnamesview.h"
#include "cntactionlauncher.h"
#include "cntactionmenubuilder.h"

#include <hbaction.h>
#include <hbabstractviewitem.h>
#include <QObject>

#include <qcontact.h>
#include <cntlistmodel.h>

class HbView;
class HbListView;
class HbLabel;
class HbDocumentLoader;
class HbSearchPanel;
class HbShrinkingVkbHost;
class CntExtensionManager;
class CntFetchContacts;
class HbMenu;

class CntNamesViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntNamesView)
    
public:
    CntNamesViewPrivate();
    virtual ~CntNamesViewPrivate();

public slots:    
    void showFinder();
    void hideFinder();
    void setFilter(const QString &filterString);
    
    void handleExtensionAction();
    
    void createNewContact();
    void deleteContact( QContact& aContact );
    void deleteMultipleContacts();
    
    void handleDeleteMultipleContacts( QSet<QContactLocalId> aIds );

    void showPreviousView();
    void showCollectionView();
    void showContactView( QContact& aContact );
    void showContactView( const QModelIndex& );
    void showContactEditorView( QContact& aContact );
    void showContextMenu(HbAbstractViewItem* aItem, QPointF aPoint);
    void showSettings();
    
    void executeAction( QContact& aContact, QContactDetail aDetail, QString aAction );
    void actionExecuted( CntActionLauncher* aAction );
    void handleDeleteContact( int aAction );
    void importSim();
    
    void handleContactAddition(const QList<QContactLocalId> & aAddedList);
    void handleContactRemoval(const QList<QContactLocalId> & aRemovedList);
    void handleSelfContactIdChange(const QContactLocalId & aOldId, const QContactLocalId & aNewId);
    
private slots:
    void switchOrientation();
    void handleImportContacts( HbAction *aAction );
    
public:
    bool isFinderVisible();
    void activate( const CntViewParameters aArgs );
    void deactivate();
    void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
private:
    void changeDeleteActionStatus();
    void focusLineEdit();
    void setScrollPosition(int focusedContact);
    
public:
    CntNamesView *q_ptr;
    
public:  // lazy initializations
    HbListView *list();
    HbDocumentLoader *document();
    
private:
    CntAbstractViewManager*     mViewManager;
    CntListModel*               mListModel;
    HbView*                     mView;
    HbListView*                 mListView;
    HbLabel*                    mEmptyList;
    HbSearchPanel*              mSearchPanel;
    HbDocumentLoader*           mLoader;
    HbShrinkingVkbHost*         mVirtualKeyboard;
    HbAction*                   mSoftkey;
    HbAction*                   mNamesAction;
    CntActionMenuBuilder*       mMenuBuilder;
    HbAction*                   mImportSim;
    HbAction*                   mNewContact;
    QContactLocalId             mHandledContactId;
    HbAction*                   mMultipleDeleter;
    QActionGroup*               mActionGroup;
	HbMenu*                     mMenu;
	bool                        mFilterChanged;
	CntAbstractEngine*          mEngine;
	static bool                 mIsFirstTimeUse; // FTU flag
};

#endif /* CNTABSTRACTLISTVIEW_H_ */
