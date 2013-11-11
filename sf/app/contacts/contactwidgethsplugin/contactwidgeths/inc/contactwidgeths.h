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
* Description: Contact Friend widget  
*
*/

#ifndef CONTACTWIDGETHS_H
#define CONTACTWIDGETHS_H

#include <hbwidget.h>
#include <QString>
#include <qtcontacts.h>

#include <cntservicescontact.h>
#include <XQServiceUtil.h>
#include <xqappmgr.h>
#include <thumbnailmanager_qt.h>
#include <QObjectCleanupHandler>
#include <QScopedPointer>
#include "contactwidgetplugin_global.h"
#include "commlauncherwidget.h"

class HbIconItem;
class HbLabel;
class HbFrameDrawer;
class HbMainWindow;
class HbDocumentLoader;
class HbTranslator;


QTM_USE_NAMESPACE

/**
* ContactWidgetHs
*/
class ContactWidgetHs : public HbWidget
{
    Q_OBJECT

    /**
     * Background image filename property.
     */
    Q_PROPERTY(QString contactImage READ contactImage WRITE setContactImage )
    /**
     * Contact id property. 0 = no contact
     */
    Q_PROPERTY(QString contactId READ contactId WRITE setContactId )

    enum contactId { 
    	unUsedContactId = 0 // No Pbk contact should use this    
    };
    
public:
    ContactWidgetHs(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~ContactWidgetHs();
    
    QString contactImage() const;
    QString contactId() const;

public slots:
    void onInitialize();
    void onUninitialize();
    void onShow();
    void onHide();

signals:
    void setPreferences(const QStringList &names);    
    void finished();    
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *aEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void changeEvent(QEvent *event);

private slots:
    void orientationChange();
    void onContactSelectCompleted(const QVariant &value);
    void onContactsChanged( const QList<QContactLocalId> &contactIds );
    void onContactsRemoved( const QList<QContactLocalId> &contactIds );
    void thumbnailReady(QPixmap, void*, int, int);
    void loadNormalLayout();
    void onRequestComplete();
    void onSelfContactIdChanged(const QContactLocalId &theOldId,
            const QContactLocalId &theNewId);
    
private:
    void createUI();
    void getContactData();
    bool setContactImage(const QString &bgImage);
    bool setContactImage(QPixmap& avPixmap);
    void setContactId(const QString &Id);    
    void setName( const QString &sName );
    QString getContactDisplayName(QContact& contact);
    void updateUiFromContact();
    void setAvatar();
    void clearContactData();
    // needed in all Contacts methods
    void createContactManager();
    // Returns whether there are contacts in db that can be selected to this widget
    bool contactsExist();
    // Returns whether contact selecting request was sent succesfully
    bool launchSingleContactSelecting();
    void createLauncherWithPosition();
    void loadLayout(const QString frameName, const QString textColor);
    void finishWidget();

private:
    HbIconItem *mAvatarIconItem;
    HbLabel *mContactNameLabel;
    QRectF mLauncherRect;
    CommLauncherWidget *mLauncher;
    HbFrameDrawer *mWidgetFrameDrawer;
    HbFrameDrawer *mShareFrameDrawer;
    QScopedPointer<HbIcon> mAvatarIcon;
   
    // Contact
    QContact mContact;
    QContactLocalId mContactLocalId;
    QString mContactImageFileName;
    // need to use scopepointer as QObjectCleanupHandler is not accepting
    // it as a parameter
    QScopedPointer<QContactAvatar> mAvatar;
    bool mContactHasAvatarDetail;
    // Contact manager
    QContactManager *mContactManager;
    XQApplicationManager* mAppManager;
    XQAiwRequest *mContactSelectRequest;

    HbMainWindow *mMainWindow;
    QScopedPointer<ThumbnailManager> mThumbnailManager;
    QPixmap mThumbnailPixmap;
    // When true, thumbnail generating is in progress
    bool mThumbnailInProgress; 
    QScopedPointer<HbTranslator> mTranslator;
    
    QObjectCleanupHandler mCleanupHandler;
    
    bool mPendingExit;
    
    CONTACTWIDGET_TEST_FRIEND_CLASS(TestContactWidget)

};

#endif //ContactWIDGETHS_H
