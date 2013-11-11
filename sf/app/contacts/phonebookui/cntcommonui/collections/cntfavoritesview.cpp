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

#include "cntfavoritesview.h"
#include "cntfetchcontactpopup.h"
#include "cntglobal.h"
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>
#include <QSet>

const char *CNT_FAVORITE_UI_XML = ":/xml/contacts_favorite.docml";

CntFavoritesView::CntFavoritesView() :
    mContact(NULL),
    mView(NULL),
    mSoftkey(NULL),
    mViewManager(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_FAVORITE_UI_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("favoritesView")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_favorite.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
}
/*!
Destructor
*/
CntFavoritesView::~CntFavoritesView()
{
    mView->deleteLater();
    
    delete mContact;
    mContact = NULL;
}

void CntFavoritesView::activate( const CntViewParameters aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    mContact = new QContact(aArgs.value(ESelectedGroupContact).value<QContact>());
    mViewManager = &mEngine->viewManager();

    HbPushButton *addButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_add")));
    connect(addButton, SIGNAL(clicked()), this, SLOT(openSelectionPopup()));
    connect(addButton, SIGNAL(longPress(QPointF)), this, SLOT(openSelectionPopup()));
    
    // If no contacts are present, then disable the button 
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QLatin1String(QContactType::TypeContact));
   
    QList<QContactLocalId> contactIds = getContactManager()->contactIds(filter);   
    if (contactIds.isEmpty())
    {
        addButton->setEnabled(false); 
    }
    
}

void CntFavoritesView::deactivate()
{
}

void CntFavoritesView::openSelectionPopup()
{
    CntFetchContactPopup* popup = CntFetchContactPopup::createMultiSelectionPopup(
            hbTrId("txt_phob_title_favorite_contacts"), 
            hbTrId("txt_common_button_save"),
            *getContactManager());
    connect(popup, SIGNAL(fetchReady(QSet<QContactLocalId>)), 
            this, SLOT(handleMemberSelection(QSet<QContactLocalId>)));
    QSet<QContactLocalId> ids;
    popup->setSelectedContacts(ids);
    popup->showPopup();
}

void CntFavoritesView::handleMemberSelection( QSet<QContactLocalId> aIds )
{
    if ( aIds.isEmpty() )
    {
        showPreviousView();
    }
    else
    {
        QList<QContactRelationship> memberships;
        foreach (QContactLocalId id, aIds) {
            QContactId contactId;
            contactId.setLocalId(id);
            QContactRelationship membership;
            membership.setRelationshipType(QContactRelationship::HasMember);
            membership.setFirst(mContact->id());
            membership.setSecond(contactId);
            memberships.append(membership);
        }
    
        if (!memberships.isEmpty()) {
            getContactManager()->saveRelationships(&memberships, NULL);
        }
    
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, favoritesMemberView);
        QVariant var;
        var.setValue(*mContact);
        viewParameters.insert(ESelectedGroupContact, var);
        mViewManager->changeView(viewParameters);
    }
}

void CntFavoritesView::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocumentLoader.load(CNT_FAVORITE_UI_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        mDocumentLoader.load(CNT_FAVORITE_UI_XML, "landscape");
    }
}

void CntFavoritesView::showPreviousView()
{
    CntViewParameters args;
    mViewManager->back(args);
}

QContactManager* CntFavoritesView::getContactManager()
{
    if (!mViewManager) {
        return NULL;
    }

    return &mEngine->contactManager(SYMBIAN_BACKEND);
}

// end of file
