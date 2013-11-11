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
* Description:  Test class for contactwidgeths
*
*/

#ifndef T_CONTACTWIDGETHS_H
#define T_CONTACTWIDGETHS_H

#include <QtTest/QtTest>
#include <QContactid.h>
#include <QContact.h>

//forward declaration
class ContactWidgetHs;
class CommLauncherWidget;

/**
* TestContactWidget
*/
class TestContactWidget : public QObject
{
    Q_OBJECT

private slots:
    /**
    * t_contactwidgeths.cpp
    */
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    //
    void testSetContactImageCaseBadFile();
    void testSetContactImageCaseOkFile();
    void testSetContactImageCaseFileFormats();
    void testSetContactImageCaseResolutions();
    void testSetContactImageCaseProportions();
    void testBoundingRect();
    void testShape();
    
    void testCommLauncherTopPosition();
    void testCommLauncherBottomPosition();
    void testCommLauncherMiddlePosition();
    void testCommLauncherNegativePosition();
    void testCommLauncherOverSizePosition();
    
    void testOrientationChange();
    void testThemeSupport();    
    void testLayoutParams();
    void testChangeEvent();
    void testSetNameNoLabel();
    void testSetName();
    void testClearContactData();
    void testContactId();
    void testContactImage();

    void testCreateContactManager();
    void testContactsExist();
    void testGetContactData();
    void testUpdateUiFromContact();
    void testOnContactsChanged();
    void testSetContactId();
    
    void testOnContactSelectCompleted();    
    void testOnShowHide();
    void testMousePressEvent();
    void testCreateContactMainWindowExists();
    void testCreateLauncherMainWindowExists();
    
    void testLoadNormalLayout();
    void testLoadLayout();
   
    void setTestPhase();
    
    //void testLaunchSingleContactSelecting();
    // TODO missing cases:
    // -does automatic test for paint() make sense?
    // -mousePressEvent() is just a dummy now    
    
    /**
    * t_commlauncherwidget.cpp
    */
    void testCreateCallButton();
    void testCreateSendMsgButton();
    void testCreateEmailButton();
    void testCreatePhonebookButton();
    void testCreateInvalidButton();
    void testTransitionEffects();
    void testCreateUiWithNumber();
    void testCreateUiWithoutNumber();
    void testCommLauncherWidth();
    void testPopupAboutToClose();
    // Opening phonebook is not tested here because it would require user actions 
    // to close the phonebook.
    
    //,,1.6.2010 this is here just for creating test contact with avatar
    void testLast();
    
private:
    
    // utility methods used by test cases
    bool utilDeleteContacts();
    bool utilAddContact( QString& namef,  QString& namel,
                         QString& phone1, QString& phone2, QString& avatarFile,
                         QtMobility::QContactLocalId& mOutContactLocalId );
    QtMobility::QContact utilGetContact(QtMobility::QContactLocalId id);
    
private:
    ContactWidgetHs *mContactWidget;
    CommLauncherWidget *mCommLauncherWidget;
    QRectF mSceneRect;
    QRectF mWidgetRect;
    QRectF mLauncherRect;
    // used for running different init() for different test cases
    int mTestPhase;
    // Coordinates for the "Close-box" in the top right corner of the display
    int mCloseIconX ;
    int mCloseIconY;

};

#endif //T_CONTACTWIDGETHS_H
