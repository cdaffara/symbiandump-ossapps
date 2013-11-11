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

#include <hbcolorscheme.h>
#include <HbInstance>
#include <HbFrameDrawer>
#include <HbWidget>
#include <HbEvent>
#include <HbLabel>
#include <HbStyle>
#include <QGraphicsSceneEvent>

#include "t_contactwidgeths.h"
#include "contactwidgeths.h"


/*!
  \class TestContactWidget
  \brief test class for ContactWidgetHs.
*/

/*!
    Initialize test cases
    Called before the first testfunction is executed.
*/
void TestContactWidget::initTestCase()
{
    mContactWidget = new ContactWidgetHs();
 
    // Test application doesn't know the scene or the widget size, 
    // so the values for testing are defined here
    const int sceneWidth = 360;
    const int sceneHeight = 640;
    mSceneRect.setRect(0, 0, sceneWidth, sceneHeight);
    const int widgetWidth = 100; 
    const int widgetHeight = 130;
    mWidgetRect.setRect(0, 0, widgetWidth, widgetHeight);
            
    QVERIFY(mContactWidget);
    mTestPhase = 1;  //this is used for running different init() for test cases
    
    mCommLauncherWidget = new CommLauncherWidget();
    
    // Coordinates for the "Close-box" in the top right corner of the display
    mCloseIconX = sceneWidth - 20;
    mCloseIconY = 20;
}

/*!
    Cleanup test case
    Called after the last testfunction is executed.
*/
void TestContactWidget::cleanupTestCase()
{
    delete mContactWidget;
}

/*!
    Test setContactImage, file is not found
    Even though loading non-existing file fails,
    setContactImage returns true, because default avatar is loaded.
*/
void TestContactWidget::testSetContactImageCaseBadFile()
{
/*,,--------- tmp 3.6.
	// Generate a pic contact in case later tests using thumbnails might crash 
	  
	mContactWidget->createContactManager();	
    QVERIFY(mContactWidget->mContactManager);
    
    // -Delete all contacts
    QVERIFY(utilDeleteContacts());      
    
    QContactLocalId id = 0; 
     QString firstName = "first_av";  
     QString lastName  = "Last1";
     QString number1   = "11223344";
     QString number2   = "+3585012345";
     //QString avatar1   = "z:/private/20022F35/import/widgetregistry/2002C353/test1_avatar.jpg"; 
     QString avatar1   = "c:/data/Images/Pictures/test1_avatar.jpg";
     utilAddContact( firstName,  lastName,  number1, number2, avatar1, id); 
    
     // intentional crash
     char* p=0;
     *p=1;
,,------------------------*/
	
	const QString sImage = "bad.png";
    QVERIFY(mContactWidget->setContactImage(sImage));
}

/*!
    Test setContactImage, file is found
*/
void TestContactWidget::testSetContactImageCaseOkFile()
{
    const QString sImage = ":/background_ok.png";
    QVERIFY(mContactWidget->setContactImage(sImage));
}

/*!
    Test setContactImage, different image file formats
    2010.02.23 QImage supports:
    bmp, gif, jpg, mng, png, pbm, pgm, ppm, tiff, xbm, xpm, svg

    TODO do we need tests for these?
    mng, pbm, pgm, ppm, xbm, xpm
    
    TODO we should get some real hires pics from phone's camera and 
    test that these files are supported.
    
    Loading large image files requires several MB of memory.
    Current 10MB heapsize set in .pro file may not be enough if all camera's image resolutions must be supported.
*/
void TestContactWidget::testSetContactImageCaseFileFormats()
{
    QString folder = "c:/private/2002C355/";  // export folder used in .pro file
    QString image = folder + "background_ok.png";
    QVERIFY(mContactWidget->setContactImage(image));

    image = "qtg_large_avatar";
    QVERIFY(mContactWidget->setContactImage(image));

    image = folder + "test1_avatar.jpg";
    QVERIFY(mContactWidget->setContactImage(image));

    image = folder + "test_avatar_1600.gif";
    QVERIFY(mContactWidget->setContactImage(image));

    image = folder + "test_avatar_1600.bmp";
    QVERIFY(mContactWidget->setContactImage(image));

    // For TIFF we could test N+1 things.
    // Now just compressions: none and lzw
    image = folder + "av_1600_comprn.tif";
    QVERIFY(mContactWidget->setContactImage(image));

    image = folder + "av_1600_comprlzw.tif";
    QVERIFY(mContactWidget->setContactImage(image));
}

/*!
    Test setContactImage, different image resolutions
*/
void TestContactWidget::testSetContactImageCaseResolutions()
{
    QString image = ":/test_avatar_640x480.jpg";
    QVERIFY(mContactWidget->setContactImage(image));

    image = ":/test_avatar_32x24.jpg";
    QVERIFY(mContactWidget->setContactImage(image));
}

/*!
    Test setContactImage, different image proportions (portrait,landscape etc)
*/
void TestContactWidget::testSetContactImageCaseProportions()
{
    QString image = ":/test_avkeys_800x600.jpg";
    QVERIFY(mContactWidget->setContactImage(image));

    image = ":/test_avkeys_600x800.jpg";
    QVERIFY(mContactWidget->setContactImage(image));
}

/*
    Test boundingRect
*/
void TestContactWidget::testBoundingRect()
{
    QRectF rect1 = mContactWidget->rect();
    QVERIFY(mContactWidget->boundingRect() == rect1);
}

/*!
    Test shape
*/
void TestContactWidget::testShape()
{
    QPainterPath path;
    path.addRect(mContactWidget->boundingRect());
    QPainterPath pathRes = mContactWidget->shape();
    QVERIFY(pathRes == path);
}

/*!
    Test communication launcher top position
*/
void TestContactWidget::testCommLauncherTopPosition()
{
    QPointF testWidgetPosition(0, 0);
    QPointF launcherPosition = mCommLauncherWidget->commLauncherPosition(testWidgetPosition, 
                               mWidgetRect, mSceneRect, mLauncherRect );
    QVERIFY(0 <= launcherPosition.y());
    QVERIFY(launcherPosition.y()<= mSceneRect.height());
}

/*!
    Test communication launcher bottom position
*/
void TestContactWidget::testCommLauncherBottomPosition()
{
    QPointF testWidgetPosition(mSceneRect.width(), mSceneRect.height());
    QPointF launcherPosition = mCommLauncherWidget->commLauncherPosition(testWidgetPosition,
                               mWidgetRect, mSceneRect, mLauncherRect );
    QVERIFY(0 <= launcherPosition.y());
    QVERIFY(launcherPosition.y()<= mSceneRect.height());   
}

/*!
    Test communication launcher middle position
*/
void TestContactWidget::testCommLauncherMiddlePosition()
{
    QPointF testWidgetPosition(mSceneRect.width()/3, mSceneRect.height()/3);
    QPointF launcherPosition = mCommLauncherWidget->commLauncherPosition(testWidgetPosition,
                               mWidgetRect, mSceneRect, mLauncherRect );
    QVERIFY(0 <= launcherPosition.y());
    QVERIFY(launcherPosition.y()<= mSceneRect.height());
} 

/*!
    Test communication launcher with negative widget position
*/
void TestContactWidget::testCommLauncherNegativePosition()
{
    QPointF testWidgetPosition(-mSceneRect.width(), -mSceneRect.height());
    QPointF launcherPosition = mCommLauncherWidget->commLauncherPosition(testWidgetPosition,
                               mWidgetRect, mSceneRect, mLauncherRect );
    QVERIFY(0 <= launcherPosition.y());
    QVERIFY(launcherPosition.y()<= mSceneRect.height());   
}

/*!
    Test communication launcher with over the scene size widget position
*/
void TestContactWidget::testCommLauncherOverSizePosition()
{
    QPointF testWidgetPosition(mSceneRect.width()*2, mSceneRect.height()*2);
    QPointF launcherPosition = mCommLauncherWidget->commLauncherPosition(testWidgetPosition,
                               mWidgetRect, mSceneRect, mLauncherRect );
    QVERIFY(0 <= launcherPosition.y());
    QVERIFY(launcherPosition.y()<= mSceneRect.height());   
}

/*
    Test changing orientation
*/
void TestContactWidget::testOrientationChange()
{
    HbMainWindow *mainWindow = new HbMainWindow();
    QVERIFY(mainWindow);
    
    mContactWidget->mLauncher->show();
    mContactWidget->orientationChange();
    QVERIFY(!mContactWidget->mLauncher->isVisible());
}

/*!
    Test theme support
*/
void TestContactWidget::testThemeSupport()
{
    // Name label's text color
    QVERIFY(mContactWidget);
    // -create UI if not done yet 
    if (!mContactWidget->mContactNameLabel) {
        mContactWidget->onInitialize();
    }
    QVERIFY(mContactWidget->mContactNameLabel);
    QColor usedColor = mContactWidget->mContactNameLabel->textColor();
    QColor themeColor = HbColorScheme::color("qtc_hs_list_item_title_normal");
    QVERIFY(themeColor.isValid()); 
    QVERIFY(themeColor == usedColor);
    // TODO how to test background color?
    mContactWidget->onUninitialize();
}

void TestContactWidget::testChangeEvent()
{
	// unhandled event type
	QEvent eventInvalid = QEvent(QEvent::None);
	mContactWidget->changeEvent(&eventInvalid);
	// Theme
	QEvent eventTheme = QEvent((QEvent::Type) HbEvent::ThemeChanged);
	mContactWidget->changeEvent(&eventTheme);	
	// - text color has been taken from theme 
    QColor textColor = HbColorScheme::color("qtc_hs_list_item_title_normal");
    QVERIFY(textColor.isValid());
    QVERIFY(mContactWidget->mContactNameLabel->textColor() == textColor);
}

/*!
    Test that params used in drawing the UI can be found
*/
void TestContactWidget::testLayoutParams()
{
    qreal textHeight = 0.0;
    HbStyle s;   
    s.parameter("hb-param-text-height-tiny", textHeight);
    
    QVERIFY(textHeight>0); 
}

/*
    Test that contact name is set correctly when there is no name label
*/
void TestContactWidget::testSetNameNoLabel()
{
    mContactWidget->mContactNameLabel = 0;
    const QString name = "testContactName";
    mContactWidget->setName(name);
    QVERIFY(!mContactWidget->mContactNameLabel);
}

/*
    Test that contact name is set correctly
*/
void TestContactWidget::testSetName()
{
    mContactWidget->mContactNameLabel = new HbLabel("");
    const QString name = "testContactName";
    mContactWidget->setName(name);
    QVERIFY(mContactWidget->mContactNameLabel->plainText() == name);
}

/*
    Test that contact data is cleared correctly
*/
void TestContactWidget::testClearContactData()
{
    // Set test values to fields that are cleared
    mContactWidget->mContactLocalId = 3;
    mContactWidget->setName("testContact");
    mContactWidget->mContactHasAvatarDetail = true;
    mContactWidget->mContactImageFileName = QString("testImageFileName");
    
    mContactWidget->clearContactData();
    
    QVERIFY(mContactWidget->mContactLocalId == 0);
    QVERIFY(mContactWidget->mContactNameLabel->plainText() == "");
    QVERIFY(!mContactWidget->mContactHasAvatarDetail);
    QVERIFY(mContactWidget->mContactImageFileName == "");
}

/*
    Test that contactId returns correct string 
*/
void TestContactWidget::testContactId()
{
    const int testId = 5;
    mContactWidget->mContactLocalId = testId;
    QString testIdString; 
    testIdString.setNum((int) testId);
    QVERIFY(mContactWidget->contactId() == testIdString);
}

/*
    Test that contactImage returns correct file name string 
*/
void TestContactWidget::testContactImage()
{
    const QString testImageFileName = ":/background_ok.png";
    mContactWidget->mContactImageFileName = testImageFileName;
    
    QVERIFY(mContactWidget->contactImage() == testImageFileName);
}


/*
    Test that contactManager can be created 
*/
void TestContactWidget::testCreateContactManager()
{
	mContactWidget->createContactManager();
	
    QVERIFY(mContactWidget->mContactManager);
    QVERIFY(mContactWidget->mContactManager->error() == QContactManager::NoError);    
    qDebug() << "mContactManager->error() " << mContactWidget->mContactManager->error(); //,,
}



/*
  Test contactsExist()
*/
void TestContactWidget::testContactsExist()
{    
	mContactWidget->createContactManager();
    QVERIFY(mContactWidget->mContactManager);
    
    qDebug() << "selfContactId " << mContactWidget->mContactManager->selfContactId(); //,,
    
    // Delete all contacts
    utilDeleteContacts();
    QVERIFY(!mContactWidget->contactsExist());

    //Create one contact
    QString secondName = "first2";  
    QString last2Name  = "Last2";
    QString number3    = "22334400";
    QString number4    = "+3585034567";
    QString avatar2   = "";    
    QContactLocalId id = 0; 
    QVERIFY(utilAddContact( secondName, last2Name, number3, number4, avatar2, id));
    QVERIFY(mContactWidget->contactsExist());
    
    qDebug() << "selfContactId " << mContactWidget->mContactManager->selfContactId(); //,,
    
/*  ,,TODO This is commented out - fails because of 2 probable bugs in Contacts 1.6.2010
     
    // Set own contact card to our one existing contact  
    qDebug() << "selfContactId " << mContactWidget->mContactManager->selfContactId()
    		 << " id " << id; //,,
    QVERIFY(mContactWidget->mContactManager->setSelfContactId(id));
    QTest::qWait(1000);
    qDebug() << "selfContactId " << mContactWidget->mContactManager->selfContactId()
    		 << " id " << id;
    // Now there should be no contacts - own contact card doesn't count
    QVERIFY(mContactWidget->contactsExist() == false);
*/    
    
    // Without existing CM 
    // -we should get same result
    delete mContactWidget->mContactManager;
    mContactWidget->mContactManager = 0;
    QVERIFY(mContactWidget->contactsExist());
}

/*
    Test getContactData()
*/
void TestContactWidget::testGetContactData()
{
	mContactWidget->createContactManager();	
    QVERIFY(mContactWidget->mContactManager);
    
	// 1. test that widget is cleared when contact is empty
    QVERIFY(mContactWidget->mContact.isEmpty());
	mContactWidget->getContactData();
	QVERIFY(mContactWidget->mContactLocalId == ContactWidgetHs::unUsedContactId);
	QVERIFY(mContactWidget->mContactNameLabel->plainText() == "");	
	
	// 2. Create a contact  
	// and test that those values are shown in the UI.	 

    // -Delete all contacts
    QVERIFY(utilDeleteContacts());

    //-Create one contact
    QString secondName = "first2";  
    QString last2Name  = "Last2";
    QString number3    = "22334400";
    QString number4    = "+3585034567";
    QString avatar2   = "";
    QContactLocalId id = 0; 
    QVERIFY(utilAddContact( secondName, last2Name, number3, number4, avatar2, id));
    QContact con1 = utilGetContact(id);
    QString name = mContactWidget->mContactManager->synthesizedDisplayLabel(con1);
    
    // -Update data from contact to UI
    mContactWidget->mContactLocalId = id;
    mContactWidget->updateUiFromContact();
    
    // Check that UI values are correct
    QVERIFY( mContactWidget->mContactNameLabel->plainText() == name );
    QVERIFY( mContactWidget->mContactLocalId == id );
    QVERIFY( mContactWidget->mContactHasAvatarDetail == false );
    QVERIFY( &mContactWidget->mContact == mContactWidget->mLauncher->mContact);
    
    // 3.
    
    // TODO since avatar handling will change from 9.2 to 10.1, 
    // testing cases where contact has avatar is not done yet. 
    /*
     QString firstName = "first1";  
     QString lastName  = "Last1";
     QString number1   = "11223344";
     QString number2   = "+3585012345";
     QString avatar1   = "z:/private/20022F35/import/widgetregistry/2002C353/test1_avatar.jpg"; 
     // Win32 path that works "c:/qt/hs_domain/bin/debug/test1_avatar.jpg";
      utilAddContact( firstName,  lastName,  number1, number2, avatar1, id); 
     */
    
    // Test the case where contactMgr doesn't exist
    delete mContactWidget->mContactManager;
    mContactWidget->mContactManager = 0;     
    delete mContactWidget;
    mContactWidget = new ContactWidgetHs();
    // -create UI if not done yet 
    if (!mContactWidget->mContactNameLabel) {
        mContactWidget->onInitialize();
    }
    qDebug() << "-new widget created, no cm";
	// -repeat the empty contact test case
    QVERIFY(mContactWidget->mContact.isEmpty());
    qDebug() << "-1";
    mContactWidget->getContactData();
    qDebug() << "-2";
	QVERIFY(mContactWidget->mContactLocalId == ContactWidgetHs::unUsedContactId);
    qDebug() << "-3";
	QVERIFY(mContactWidget->mContactNameLabel->plainText() == "");	
}

/*
    Test updateUiFromContact()
*/
void TestContactWidget::testUpdateUiFromContact()
{
	mContactWidget->createContactManager();
    QVERIFY(mContactWidget->mContactManager);
    
    // Delete all contacts
    QVERIFY(utilDeleteContacts());

    //Create one contact
    QString secondName = "first2";  
    QString last2Name  = "Last2";
    QString number3    = "22334400";
    QString number4    = "+3585034567";
    QString avatar2   = "";
    QContactLocalId id = 0; 
    QVERIFY(utilAddContact( secondName, last2Name, number3, number4, avatar2, id));
    QContact con1 = utilGetContact(id);
    QString name = mContactWidget->mContactManager->synthesizedDisplayLabel(con1);
    
    // Update data from contact to UI
    mContactWidget->mContactLocalId = id;
    mContactWidget->updateUiFromContact();
    
    // Check that UI values are correct
    QVERIFY( mContactWidget->mContactNameLabel->plainText() == name );
    QVERIFY( mContactWidget->mContactLocalId == id );
    QVERIFY( mContactWidget->mContactHasAvatarDetail == false );
    QVERIFY( &mContactWidget->mContact == mContactWidget->mLauncher->mContact);
    
    // Since updateUiFromContact() calls getContactData(), it should be enough to test just one
    // kind of contact here. Detailed contact tests are done in testGetContactData().
}

/*
  Test onContactsChanged.  
*/
void TestContactWidget::testOnContactsChanged()
{
	mContactWidget->createContactManager();
    QVERIFY(mContactWidget->mContactManager);
    
    // Delete all contacts
    utilDeleteContacts();
    QVERIFY(!mContactWidget->contactsExist());

    //Create one contact
    QString secondName = "first2";  
    QString last2Name  = "Last2";
    QString number3    = "22334400";
    QString number4    = "+3585034567";
    QString avatar2   = "";    
    QContactLocalId id = 0; 
    QVERIFY(utilAddContact( secondName, last2Name, number3, number4, avatar2, id));
    QContact con1 = utilGetContact(id);
    QString name = mContactWidget->mContactManager->synthesizedDisplayLabel(con1);
    
    // Update data from contact to UI
    mContactWidget->mContactLocalId = id;
    mContactWidget->updateUiFromContact();
	QVERIFY( mContactWidget->mContactNameLabel->plainText() == name );
    
	// Change contact's name
    QString newFirst = "updatedName";
    QContactName cname = con1.detail(QContactName::DefinitionName);
    cname.setValue(QString(QLatin1String(QContactName::FieldFirstName)), newFirst);
    QVERIFY(con1.saveDetail(&cname));
    QVERIFY( mContactWidget->mContactManager->saveContact(&con1));
    name = mContactWidget->mContactManager->synthesizedDisplayLabel(con1);
    // Changed contact's data should be updated to UI, 
    // onContactsChanged() calls updateUiFromContact().
    
    // Wait for UI to update
	QTest::qWait(3 * 1000);  // 3 sec
	QVERIFY( mContactWidget->mContactNameLabel->plainText() == name );
	// TODO once 10.1 avatar handling is working, this could also test that changing avatar works 
}

/*
  Test setContactId
*/
void TestContactWidget::testSetContactId()
{
	// Create a contact whose id we will set to widget
	mContactWidget->createContactManager();
    QVERIFY(mContactWidget->mContactManager);
    
    // Delete all contacts
    utilDeleteContacts();
    QVERIFY(!mContactWidget->contactsExist());

    // Create one contact
    QString secondName = "first2";  
    QString last2Name  = "Last2";
    QString number3    = "22334400";
    QString number4    = "+3585034567";
    QString avatar2   = "";    
    QContactLocalId id = 0; 
    QVERIFY(utilAddContact( secondName, last2Name, number3, number4, avatar2, id));
    
	QString ids = QString::number(id);
	mContactWidget->mContactLocalId = 0;
	mContactWidget->setContactId(ids);
	QVERIFY(mContactWidget->mContactLocalId == id);
}

/*
    Test that contact selection can be started
   // This cannot be tested automatically, because blocks UI while waiting for user to select contact
*/
/*
void TestContactWidget::testLaunchSingleContactSelecting()
{

	QVERIFY(mContactWidget->launchSingleContactSelecting());

	// This should probably first wait some seconds, then get pointer to top most window and
	// THEN simulate a mouseClick.
	QTest::qWait(5 * 1000);  // 5 sec
	QPoint windowPos = QPoint( mCloseIconX, mCloseIconY );
	HbMainWindow* window = new HbMainWindow();
	QWidget* widget = window->viewport();
    QTest::mouseClick(widget,Qt::LeftButton,0,windowPos);
}
*/ 

/*
    Test onShow, onHide
*/
void TestContactWidget::testOnShowHide()
{
	mContactWidget->onShow();
	mContactWidget->onHide();	
	// The methods do nothing, but need to exist. Otherwise adding to HS fails.
	// So we call them to increase coverage data.
    QVERIFY(1);
    // TODO remove mouseReleaseEvent if not used/needed 
    mContactWidget->mouseReleaseEvent(0);
}

/*
    Test onContactSelectCompleted
*/
void TestContactWidget::testOnContactSelectCompleted()
{
	// Bad parameter
	mContactWidget->mContactLocalId = ContactWidgetHs::unUsedContactId +1;
	QVariant value;	
	mContactWidget->onContactSelectCompleted(value);
	QVERIFY(mContactWidget->mContactLocalId == ContactWidgetHs::unUsedContactId);
	// Valid parameter: no contactId = user closed UI without selecting contact.
	// -widget's contactId doesn't change
	CntServicesContactList contactList;	
	QVariant noId;
	noId.setValue(contactList);
	mContactWidget->onContactSelectCompleted(noId);
	QVERIFY(mContactWidget->mContactLocalId == ContactWidgetHs::unUsedContactId);
	// Valid parameter: contactId
    // -Delete all contacts
    utilDeleteContacts();
    QVERIFY(!mContactWidget->contactsExist());

    // -Create one contact
    QString secondName = "first2";  
    QString last2Name  = "Last2";
    QString number3    = "22334400";
    QString number4    = "+3585034567";
    QString avatar2   = "";    
    QContactLocalId id = 0; 
    QVERIFY(utilAddContact( secondName, last2Name, number3, number4, avatar2, id));		
	
	CntServicesContact contactPar;
	contactPar.mContactId = id;
	contactList.append(contactPar);
	QVariant varId;
	varId.setValue(contactList);
	mContactWidget->onContactSelectCompleted(varId);
	QVERIFY(mContactWidget->mContactLocalId == id);		
}

/*
    Test mousePressEvent
*/
void TestContactWidget::testMousePressEvent()
{
	// Not handled
	QGraphicsSceneMouseEvent *event0 = 0;
	mContactWidget->mousePressEvent(event0);
	// Not handled	
	QGraphicsSceneMouseEvent* eventN = new QGraphicsSceneMouseEvent(QEvent::None);
	mContactWidget->mousePressEvent(eventN);
	delete eventN;
	
	HbMainWindow *mainWindow = new HbMainWindow();
	QVERIFY(mainWindow);
	// show comms.launcher	
	QVERIFY(mContactWidget->mLauncher);
	QVERIFY(!mContactWidget->mLauncher->isVisible());
	mContactWidget->mContactLocalId = 5;
	QGraphicsSceneMouseEvent eventM(QEvent::GraphicsSceneMousePress);
	mContactWidget->mousePressEvent(&eventM);
	QVERIFY(mContactWidget->mLauncher->isVisible());
}

/*
    Test creating contact when main window exists
*/
void TestContactWidget::testCreateContactMainWindowExists()
{
    HbMainWindow *mainWindow = new HbMainWindow();
    QVERIFY(mainWindow);
    
    ContactWidgetHs *contact = new ContactWidgetHs();
    QVERIFY(contact->mMainWindow);
    QVERIFY(contact->mLauncher);
    QVERIFY(contact->mAppManager);
    
    delete contact;
}

/*
    Test creating contact when main window exists
*/
void TestContactWidget::testCreateLauncherMainWindowExists()
{
    HbMainWindow *mainWindow = new HbMainWindow();
    QVERIFY(mainWindow);
    mContactWidget->mMainWindow = mainWindow;
    
    mContactWidget->createLauncherWithPosition();
    QVERIFY(mContactWidget->mLauncher->mButtonCount != 0);
}

/*
    Test loading normal layout
*/
void TestContactWidget::testLoadNormalLayout()
{
    mContactWidget->mWidgetFrameDrawer->setFrameGraphicsName("");
    mContactWidget->loadNormalLayout();
    const QString normalFrameName = "qtg_fr_hsshortcut_normal";
    const QString normalTextColor = "qtc_hs_list_item_title_normal";
    QColor color = HbColorScheme::color(normalTextColor);
    QVERIFY(mContactWidget->mWidgetFrameDrawer->frameGraphicsName() == normalFrameName);    
    QVERIFY(mContactWidget->mShareFrameDrawer->frameGraphicsName() == normalFrameName);
    QVERIFY(mContactWidget->mContactNameLabel->textColor() == color);
}

/*
    Test loading latched layout
*/
void TestContactWidget::testLoadLayout()
{
    mContactWidget->mWidgetFrameDrawer->setFrameGraphicsName("");
    const QString frameName = "qtg_fr_hsshortcut_normal";
    const QString textColor = "qtc_hs_list_item_title_normal";
    QColor color = HbColorScheme::color(textColor);
    mContactWidget->loadLayout(frameName, textColor);
    QVERIFY(mContactWidget->mWidgetFrameDrawer->frameGraphicsName() == frameName);    
    QVERIFY(mContactWidget->mShareFrameDrawer->frameGraphicsName() == frameName);
    QVERIFY(mContactWidget->mContactNameLabel->textColor() == color);
}

//---------------------------------------------------------------------------------------
// Utility methods used by test cases
//---------------------------------------------------------------------------------------

bool TestContactWidget::utilDeleteContacts()
{
	bool ret=true;
	qDebug() << "utilDeleteContacts";
	// Assumes contact manager has been created	
    ASSERT(mContactWidget->mContactManager);
    
    QList<QContactLocalId> contactIds = mContactWidget->mContactManager->contactIds();
    int i;
    for(i=0; i<contactIds.count(); i++) {
		QContactLocalId id = contactIds.at(i);
		ret = mContactWidget->mContactManager->removeContact( id );  
        if (!ret) {
			qDebug() << "-ERROR cannot delete contact "  << id;  
			break;
        }
        qDebug() << "deleted contact "  << id;  
	}
    qDebug() << "After delete: contacts " << mContactWidget->mContactManager->contactIds().count()
    		 << " self.id " << mContactWidget->mContactManager->selfContactId();
	return ret;
}


bool TestContactWidget::utilAddContact(
                     QString& namef,  QString& namel,
                     QString& phone1, QString& phone2, QString& avatarFile,
                     QtMobility::QContactLocalId& mOutContactLocalId )
{
    ASSERT(mContactWidget->mContactManager);
    QContactManager* cm = mContactWidget->mContactManager;
    QContact contact;
    
    // Set contact's name
    QContactName name;
    name.setFirstName(namef);
    name.setLastName(namel);
    name.setCustomLabel( namef + " " + namel );
    contact.saveDetail(&name);

    // Add a phone number
    QContactPhoneNumber number;
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber( phone1 );
    contact.saveDetail(&number);
    contact.setPreferredDetail("DialAction", number);

    // Add a second phone number
    QContactPhoneNumber number2;
    number2.setContexts(QContactDetail::ContextWork);
    number2.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    number2.setNumber( phone2 );
    contact.saveDetail(&number2);

    // Avatar if filename given
    if (avatarFile.length() >0) {
        QContactAvatar* avatar = new QContactAvatar();
        avatar->setImageUrl( avatarFile );        
        contact.saveDetail(avatar);
    }

    // Save the contact
    bool res = cm->saveContact(&contact);
    mOutContactLocalId = contact.localId();
    QTest::qWait(1000);

    return res;
}

QContact TestContactWidget::utilGetContact(QContactLocalId id)
{
    ASSERT(mContactWidget->mContactManager);
    
    // Get contact based on id
    return mContactWidget->mContactManager->contact( id );
}

QTEST_MAIN(TestContactWidget)
