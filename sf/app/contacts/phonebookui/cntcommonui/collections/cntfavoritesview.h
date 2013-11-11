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

#ifndef CNTFAVORITESVIEW_H
#define CNTFAVORITESVIEW_H

#include <QObject>
#include <hbdocumentloader.h>
#include <cntabstractview.h>
#include <QSet>

class HbAction;
class HbView;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntFavoritesView : public QObject, public CntAbstractView
{
    friend class TestCntFavoritesView;
    Q_OBJECT
    
public:
    CntFavoritesView();
    ~CntFavoritesView();
    
public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return collectionFavoritesView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    
private slots:
    void showPreviousView();
    void openSelectionPopup();
    void handleMemberSelection( QSet<QContactLocalId> aIds );
    void setOrientation(Qt::Orientation orientation);

private:
    QContactManager* getContactManager();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QContact* mContact; // own
    HbView* mView; // own
    HbAction* mSoftkey;
    CntAbstractViewManager* mViewManager;
    HbDocumentLoader mDocumentLoader;
    CntAbstractEngine* mEngine;
};

#endif // CNTFAVORITESVIEW_H

// EOF
