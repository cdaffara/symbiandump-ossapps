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
* Description:  Contact Friend widget 
*
*/
#include <QDebug>
#include <QLocale>
#include <HbTranslator>
#include <QGraphicsGridLayout>
#include <hbcolorscheme.h>
#include <HbDialog>
#include <HbWidget>
#include <HbLabel>
#include <HbIconItem>
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <HbDeviceProfile>
#include <hbdevicemessagebox>
#include <hbdocumentloader.h>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>
#include <HbEvent>
#include <xqaiwdecl.h>
#include <cntservicescontact.h>
#include <hbaction.h>

#include <HbInstance>

#include "contactwidgeths.h"
#include "commlauncherwidget.h"
#include "ihswidgetpreferenceservice.h"
 
const QString translationsPath = "/resource/qt/translations/";
const QString translationsFile = "contactwidgethsplugin";

const QString defaultAvatar = "qtg_large_avatar";
const QString addContactAvatar = "qtg_small_add";
const QString normalFrameName = "qtg_fr_hsshortcut_normal";
const QString normalTextColor = "qtc_hs_list_item_title_normal";
const QString latchedFrameName = "qtg_fr_hsitems_latched";
const QString latchedTextColor = "qtc_hs_list_item_latched";
// Preferences properties
const QString widgetPrefContactId = "contactId";
// Docml file
const QString contactWidgetDocml = ":/commlauncherwidget.docml";


// TODO: THESE STRINGS ARE IN W32 SDK. THESE DEFINITIONS CAN BE REMOVED
// WHEN EVERYBODY ARE USING IT OR LATER VERSION
#ifndef XQI_CONTACTS_FETCH
#define XQI_CONTACTS_FETCH QLatin1String("com.nokia.symbian.IContactsFetch")
#endif
#ifndef XQOP_CONTACTS_FETCH_SINGLE
#define XQOP_CONTACTS_FETCH_SINGLE QLatin1String("singleFetch(QString,QString)")
#endif



/*!
  \class ContactWidgetHs
*/

/*!
    Constructor
*/
ContactWidgetHs::ContactWidgetHs(QGraphicsItem *parent, Qt::WindowFlags flags )
: HbWidget(parent, flags),
  mAvatarIconItem(0),
  mContactNameLabel(0),
  mLauncherRect(QRectF(0,0,0,70)),
  mWidgetFrameDrawer(0),
  mShareFrameDrawer(0),
  mContactLocalId(unUsedContactId),
  mAvatar(0),	
  mContactHasAvatarDetail(false),  
  mContactManager(0),
  mAppManager(0),
  mContactSelectRequest(0),
  mMainWindow(0),
  mThumbnailManager(new ThumbnailManager(this)),
  mThumbnailPixmap(QPixmap()),
  mThumbnailInProgress(false),
  mTranslator(new HbTranslator(translationsPath, translationsFile)),
  mPendingExit(false)
{
    // Localization file loading   		
	mTranslator->loadCommon();    
    
    // UI creation done in onInitialize()
    if (mainWindow()) {
        mMainWindow = mainWindow();
        connect(mMainWindow, SIGNAL(aboutToChangeOrientation()), 
                this, SLOT(orientationChange()));
        qDebug() << "mMainWindow(x,y): " << mMainWindow->width() << mMainWindow->height();
    }

    // avatar icon item has to be created without parent else the drawing goes wrong
    mAvatarIconItem = new HbIconItem();
    mCleanupHandler.add(mAvatarIconItem);
    //Create Communication launchernew
    //Has to be created without parent, otherwise timout, dismissPolicy and modality have no effect
    mLauncher = new CommLauncherWidget();
    mCleanupHandler.add(mLauncher);
    mLauncher->setTimeout(0);
    mLauncher->setDismissPolicy(HbPopup::TapAnywhere); 
    mLauncher->setBackgroundFaded(false);
    mLauncher->setModal(false);
    mLauncher->hide();
    // USE CONNECT WHEN THE LATCHED GRAPHICS IS IN THE PLATFORM
    connect(mLauncher, SIGNAL(launcherClosed()),
            this, SLOT(loadNormalLayout()));
    connect(mLauncher, SIGNAL(requestCompleted()),
    		this, SLOT(onRequestComplete()));
    mAppManager = new XQApplicationManager();    
    ASSERT(mAppManager);
    mCleanupHandler.add(mAppManager);
    mLauncher->setApplicationManager(*mAppManager);
    
    // Thumbnail manager so we can handle large size images as contact avatars    
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailMedium );
    
    connect(mThumbnailManager.data(), SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
        this, SLOT(thumbnailReady(QPixmap, void*, int, int)));
    

      
}

/*!
    Destructor
*/
ContactWidgetHs::~ContactWidgetHs()
{

}

/*!
    Slot for handle request complete
    if pending exit was set , do it.
*/
void ContactWidgetHs::onRequestComplete()
{
 if (mPendingExit)
	 emit finished();
}

/*!
    Updates the UI based on data from current contact
*/
void ContactWidgetHs::updateUiFromContact()
{
    if (!mContactManager) {
        createContactManager();	
    }
    if (mContactManager) {
        mContact = mContactManager->contact(mContactLocalId);
        getContactData();
        mLauncher->setContact(mContact);
        
        update();
    }
}

/*!
    Selects current avatar based on member variables
*/
void ContactWidgetHs::setAvatar()
{
    if (mContactLocalId > unUsedContactId) {
        if (mContactHasAvatarDetail) {
			if (mContactImageFileName == "") {
				setContactImage(mThumbnailPixmap);
			}
			else {
				setContactImage(mContactImageFileName);
			}
        } else {
            setContactImage(defaultAvatar);
        }
    } else {
        setContactImage(addContactAvatar);	
    }
}

/*!
    Called when ThumbnailManager has thumbnail ready
*/
void ContactWidgetHs::thumbnailReady(QPixmap pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    qDebug() << "ContactWidgetHs::thumbnailReady() error: " << error;
    mThumbnailInProgress=false;
    if (!error)
    {
		mThumbnailPixmap = pixmap;
		setAvatar();
		
    }
}
/*!
    Returns the name of current image file including path.
*/
QString ContactWidgetHs::contactImage() const
{
    return mContactImageFileName;
}


/*!
    Set contact avatar image used in Paint()
*/
// THIS IS THE FIRST VERSION OF THUMBNAIL HANDLING, STILL IN PROGRESS 4.6.2010
bool ContactWidgetHs::setContactImage(QPixmap& inputPixmap)
{
    bool ret = false;

    QScopedPointer<HbFrameDrawer> avatarDrawer( new HbFrameDrawer(defaultAvatar, 
					  HbFrameDrawer::NinePieces));
    //TODO: do check other way 
    // This assert must be commented because QScopedPointer not support it this way
    //ASSERT(avatarDrawer.isNull());
            
    ret=true;
    qDebug() << "setContactImage av 0, inputsize " << inputPixmap.width() << "x" << inputPixmap.height(); 
    
    qreal unit = HbDeviceProfile::current().unitValue();
    const int contactIconSize = 11 * unit;
        
    // Render frame drawer into pixmap
    QPixmap avatarPixmap(contactIconSize, contactIconSize);
    avatarPixmap.fill(QColor(0, 0, 0, 0)); //transparent background
    qDebug() << "setContactImage av 1"; //,,
  
    QPainter painter(&avatarPixmap);      
    avatarDrawer->paint(&painter, QRectF(0, 0, contactIconSize, contactIconSize));
    painter.end();
    qDebug() << "setContactImage av 2"; //,,             

    int maxSize = (inputPixmap.width() > inputPixmap.height() ? inputPixmap.width():inputPixmap.height());
    int minSize = (inputPixmap.width() < inputPixmap.height() ? inputPixmap.width():inputPixmap.height());
    int sizeFrom = maxSize - minSize;
    // We may draw the icon when thumnail processing is still in progress, 
    // so can't show the thumbnail yet.
	if (inputPixmap.width()>0) {
		if (inputPixmap.width() < inputPixmap.height())
		{
  		QPixmap pm3 = inputPixmap.copy(0,sizeFrom/2,minSize,minSize);
  		QPixmap pm2 = pm3.scaled( QSize(contactIconSize, contactIconSize), Qt::KeepAspectRatio );
  		qDebug() << "pm2 " << pm2.width() << " " << pm2.height();
  		avatarPixmap = pm2; 
		}
		else if (inputPixmap.width() > inputPixmap.height())
		{
  		QPixmap pm3 = inputPixmap.copy(sizeFrom/2,0,minSize,minSize);
  		QPixmap pm2 = pm3.scaled( QSize(contactIconSize, contactIconSize), Qt::KeepAspectRatio );
  		qDebug() << "pm2 " << pm2.width() << " " << pm2.height();
  		avatarPixmap = pm2; 
    }else
    {
		QPixmap pm2 = inputPixmap.scaled( QSize(contactIconSize, contactIconSize), Qt::IgnoreAspectRatio );
		qDebug() << "pm2 " << pm2.width() << " " << pm2.height();
		avatarPixmap = pm2; //
    }
	} 
    

    mAvatarIcon.reset(new HbIcon(QIcon(avatarPixmap))); 
  
    mAvatarIconItem->setIcon(*mAvatarIcon.data());
    mAvatarIconItem->setSize(QSize(contactIconSize, contactIconSize));
    mAvatarIconItem->setAspectRatioMode(Qt::KeepAspectRatio);
        
    // Then display the new image
    update();
    qDebug() << "setContactImage av 3"; //,,
       
    return ret;
}

/*!
    Set contact avatar image used in Paint()
*/
bool ContactWidgetHs::setContactImage(const QString &imageFile)
{
    bool ret = false;
    QScopedPointer<HbFrameDrawer> avatarDrawer( new HbFrameDrawer(defaultAvatar, 
					  HbFrameDrawer::NinePieces));
    //TODO: do check other way 
    //This assert must be commented because QScopedPointer not support it this way
    //ASSERT(avatarDrawer.isNull());
 
    // Try to load the image first, because setFrameGraphicsName() returns void.
    mContactImageFileName = imageFile;
    qDebug() << "setContactImage ok " << imageFile;
    if (imageFile == addContactAvatar) {
        avatarDrawer->setFrameGraphicsName(defaultAvatar);
        ret = true;
    } else {
        avatarDrawer->setFrameGraphicsName(imageFile);
        ret = true;
    }
    
    qreal unit = HbDeviceProfile::current().unitValue();
    const int contactIconSize = 11 * unit;
        
    // Render frame drawer into pixmap
    QPixmap avatarPixmap(contactIconSize, contactIconSize);
    avatarPixmap.fill(QColor(0, 0, 0, 0)); //transparent background
        
    QPainter painter(&avatarPixmap);
    
    avatarDrawer->paint(&painter, QRectF(0, 0, contactIconSize, contactIconSize));
    painter.end();
             

     
    mAvatarIcon.reset(new HbIcon(QIcon(avatarPixmap))); 
    
    // Add badge if needed
    if (imageFile == addContactAvatar) {
        HbIcon addIcon(addContactAvatar);
        QSizeF avatarSize = mAvatarIcon->size();
        addIcon.setSize(QSize(avatarSize.width()/2, avatarSize.height()/2));
        mAvatarIcon->addBadge(Qt::AlignTop | Qt::AlignRight, addIcon, 1);    
    }
    
    mAvatarIconItem->setIcon(*mAvatarIcon.data());
        
    // Then display the new image
    update();
    
 

    return ret;
}


void ContactWidgetHs::setName(const QString &sName)
{
    if (mContactNameLabel) {
        mContactNameLabel->setPlainText(sName);
    }    

    update();
}

/*!
    Creates the UI for the widget
*/
void ContactWidgetHs::createUI()
{
    
    qreal unit = HbDeviceProfile::current().unitValue();
    // Currently not possible to create a hbframedrawer using docml, so the size is fixed here. 
    const int contactIconSize = 11 * unit;
    const int widgetMargin =   0.5 * unit;
    const int textRow = 2;
    const int rows = 4; // 4 using 2 margins
    
    setContentsMargins(0, 0, 0, 0);

    QGraphicsGridLayout *layout = new QGraphicsGridLayout(this);

    mWidgetFrameDrawer = new HbFrameDrawer(normalFrameName, 
                                           HbFrameDrawer::NinePieces);
    ASSERT(mWidgetFrameDrawer);
    
    mShareFrameDrawer = new HbFrameDrawer(normalFrameName, 
                                          HbFrameDrawer::NinePieces);
    ASSERT(mShareFrameDrawer);
    
    
    
    // Widget frame
    qreal corner = 1.5 * unit; // from shortcut widget layout spec: 1.5un    
    mWidgetFrameDrawer->setBorderWidths(corner, corner); 
    HbFrameItem *widgetFrame = new HbFrameItem(mWidgetFrameDrawer); //drawer ownership transfered to item
    layout->addItem(widgetFrame, 0, 0, rows, 3);
          
    // Avatar frame    
    HbFrameItem *shareFrame = new HbFrameItem(mShareFrameDrawer); //drawer ownership transfered to item
    layout->addItem(shareFrame, 1, 1, 1, 1);
    
    // Contact icon  
    layout->addItem(mAvatarIconItem, 1, 1, 1, 1);
    
    // Contact name
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    qreal textHeight = 0.0; 
    style()->parameter("hb-param-text-height-tiny", textHeight);	
    if (textHeight > 0) {
        fontSpec.setTextHeight(textHeight);
    }

    if (mContactLocalId == unUsedContactId) {
        mContactNameLabel = new HbLabel("");
    } else {
		QString name = getContactDisplayName(mContact);
        mContactNameLabel = new HbLabel(name);
    }            
    
    mContactNameLabel->setAlignment(Qt::AlignHCenter);
    mContactNameLabel->setFontSpec(fontSpec);
    // color from theme
    QColor textColor = HbColorScheme::color(normalTextColor);
    if (textColor.isValid()) {
        mContactNameLabel->setTextColor(textColor);
    } else {
        qDebug() << "ContactWidgetHs::createUI()  textColor is NOT Valid!! using default color";
    }
    mContactNameLabel->setZValue(1);
    layout->addItem(mContactNameLabel, textRow, 1, 1, 1);
    
    layout->setColumnFixedWidth(0, widgetMargin);
    layout->setColumnFixedWidth(1, contactIconSize); 
    layout->setColumnFixedWidth(2, widgetMargin);
    layout->setRowFixedHeight(0, widgetMargin);
    layout->setRowFixedHeight(1, contactIconSize);
    layout->setRowFixedHeight(textRow,   fontSpec.textHeight());    
    layout->setRowFixedHeight(textRow+1, widgetMargin);
    
    layout->setHorizontalSpacing(0.5);
    layout->setVerticalSpacing(1.5);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // Widget size
    qreal sizeX = widgetMargin * 2; // margins from layout spec
    sizeX += contactIconSize;
    sizeX += 1;                     // 2*0.5 horizontal spacing
    qreal sizeY = widgetMargin * 2; // margins from layout spec
    sizeY += contactIconSize;
    sizeY += fontSpec.textHeight();
    sizeY += 5;                     // 3*0.5 vertical spacing, 5 because 4.5 broke snaping
    
    
    qDebug() << "widget sizex, sizey, unit, rows " << sizeX << sizeY << unit << rows;
    
    // Without preferredSize() moving widgets to another page doesn't work
    setPreferredSize( sizeX, sizeY ); 
    resize(preferredSize());
}

QString ContactWidgetHs::getContactDisplayName(QContact& contact)
{
    QString name = mContactManager->synthesizedContactDisplayLabel(contact);
    if (name.isEmpty())
    {
        name = hbTrId("txt_friend_widget_contact_unnamed");
    }
    return name;
}

/*!
    Implemented to receive mouse press events.
    Otherwise these events will propagate to any topmost item beneath this item.
*/
void ContactWidgetHs::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

/*!
    \fn void ContactWidgetHs::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)

    Widget start is triggered from release \a event.
    \sa 
*/
void ContactWidgetHs::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	qDebug() << "mouseReleaseEvent event->type() = " << (int)event->type();

	//Q_UNUSED(event);
    if (event && event->type() == QEvent::GraphicsSceneMouseRelease) {
        // If the widget doesn't have contact yet and
        // there are contacts, select one.    
        if (mContactLocalId == unUsedContactId) {
            if (contactsExist()) {				    
                launchSingleContactSelecting();                  
            } else {
                // Otherwise ask if user wants to open phonebook
                // tmp variable used for title, otherwise parent param is ignored in mb
                QString title = hbTrId("txt_friend_widget_info_you_can_not_use_this_widge");
                HbDeviceMessageBox mb( title, HbMessageBox::MessageTypeQuestion, this);
                mb.setAction(new QAction(hbTrId("txt_common_button_open"),   &mb), 
                    HbDeviceMessageBox::AcceptButtonRole);
                mb.setAction(new QAction(hbTrId("txt_common_button_cancel"), &mb), 
                    HbDeviceMessageBox::RejectButtonRole);
                mb.setIconVisible(false);
                if (mb.exec() == mb.action(HbDeviceMessageBox::AcceptButtonRole)) {				        
                    mLauncher->openPhonebookCreateNew();
                }
            }
        } else if (!mLauncher->isVisible()) {
            // Change the frame layout
            loadLayout(latchedFrameName, latchedTextColor);

            // create and show CommunicationsLauncher
            createLauncherWithPosition();
            mLauncher->show();

            mLauncherRect = mLauncher->boundingRect();
            qDebug() << "after exec rect() " << mLauncherRect;
        }
    }
}


// Store contact data to member variables and to UI
void ContactWidgetHs::getContactData()
{
    if (!mContactManager) { 
        createContactManager();
    }
    if (mContact.isEmpty()) {
        qDebug() << "Empty contact with id " << mContactLocalId << ".  UI cleared, id=0";  
        clearContactData();
    } else {
    
        // Id
        mContactLocalId = mContact.localId();
        qDebug() << "contact id " << mContactLocalId;   
        // Name
        QString name = getContactDisplayName(mContact);
        setName( name );
        // Avatar
        QString avatarFile;
        QList<QContactAvatar> details = mContact.details<QContactAvatar>();
        for (int i=0; i<details.count(); i++)
        {
            if (details.at(i).imageUrl().isValid())
            {			
            	    					
				mAvatar.reset( new QContactAvatar(details.at(i)));
				//mCleanupHandler.add(mAvatar);
				avatarFile = mAvatar->imageUrl().toString();  //,,remove this once tn is working

				mThumbnailInProgress = true;
				qDebug() << "calling mThumbnailManager->getThumbnail";
				mThumbnailManager->getThumbnail(mAvatar->imageUrl().toString());
                break;                
            }
        }        
        qDebug() << "ContactWidgetHs::getContactData() av.file" << avatarFile;
        if (avatarFile.length() > 0) {
            mContactHasAvatarDetail = true;
            mContactImageFileName = ""; 
            //,,mContactImageFileName = avatarFile;
        }
        else {
            mContactHasAvatarDetail = false;
        } 
    }
    if (!mThumbnailInProgress) {
		setAvatar();
    }
}

void ContactWidgetHs::clearContactData()
{
    mContactLocalId = unUsedContactId;
    setName("");
    mContactHasAvatarDetail = false;
    mContactImageFileName = "";
}

/*!
    Create contact manager
*/
void ContactWidgetHs::createContactManager()
{
    QString sBackend = "memory"; // not localised
#ifdef Q_OS_SYMBIAN
    sBackend = "symbian";
#endif
    if (mContactManager) {
        delete mContactManager;		
    }    
    qDebug() << "createContactManager() backend " << sBackend;
    mContactManager = new QContactManager(sBackend);
    mCleanupHandler.add(mContactManager);
    if (mContactManager->error()) {
        qDebug() << "cm can't connect to backend " << sBackend;
    } else { // connect contact change observation
        connect(mContactManager, SIGNAL(contactsChanged(QList<QContactLocalId>)),
            this, SLOT(onContactsChanged(QList<QContactLocalId>)));
        connect(mContactManager, SIGNAL(contactsRemoved(QList<QContactLocalId>)),
            this, SLOT(onContactsRemoved(QList<QContactLocalId>)));
        connect(mContactManager,
        		SIGNAL(selfContactIdChanged(const QContactLocalId &,
        				const QContactLocalId &)),
        		this,
        		SLOT(onSelfContactIdChanged(const QContactLocalId &,
        				const QContactLocalId &)));
    }
}

/*!
    Initializes the widget.
*/
void ContactWidgetHs::onInitialize()
{
    qDebug() << "ContactWidgetHs::onInitialize()";    
    createUI();
    setAvatar();
    
    // Update the UI
    update();
}

/*!
    Uninitializes the widget.
*/
void ContactWidgetHs::onUninitialize()
{
    qDebug() << "ContactWidgetHs::onUninitialize()";
    hide();
}

/*!
    \fn void ContactWidgetHs::onShow()

    Called when the widget is shown.
    This method is needed. Otherwise the widget cannot be added to HS.    
*/
void ContactWidgetHs::onShow()
{
    qDebug() << "ContactWidgetHs::onShow()";
}


/*!
    \fn void ContactWidgetHs::onHide()

    Called when the widget is hidden.
    This method is needed. Otherwise the widget cannot be added to HS.
*/
void ContactWidgetHs::onHide()
{
    qDebug() << "ContactWidgetHs::onHide()";
    // close the launcher if it's open
    if (mLauncher->isVisible()) {
        mLauncher->close();
    }
}


/*!
    Orientation changed slot
*/
void ContactWidgetHs::orientationChange()
{    
    // Close the launcher if it's visible
    if (mLauncher->isVisible()) {
        mLauncher->hide();
    }
    loadNormalLayout();
}    

/*!
    Checks if there are contacts to select.
 */
bool ContactWidgetHs::contactsExist()
{
    if (!mContactManager) { 
        createContactManager();
    }
    bool ret=false;
    // 
    if (mContactManager) {
        QList<QContactLocalId> contactIds = mContactManager->contactIds();
        qDebug() << "contact count " << contactIds.count();
        if (contactIds.count() > 0) {
        qDebug() << "first " << contactIds.first();
        int i;
        for(i=0; i<contactIds.count(); i++) {
			qDebug() << "contactIds i " << i << " id " << contactIds.at(i);
        }                        
            if (contactIds.first() != mContactManager->selfContactId() ||
                contactIds.count() > 1) {
                ret=true;
            }
        }
    }
    
    return ret;
}

/*!
    This launches SINGLE contact selecting.
*/
bool ContactWidgetHs::launchSingleContactSelecting()
{
	bool result = false;
	   
    if (mContactSelectRequest) {
        delete mContactSelectRequest;
        mContactSelectRequest = 0;
    }
    qDebug() << "- launchSingleContactSelecting() starts";  //,,
    mContactLocalId = unUsedContactId;
    
    mContactSelectRequest = mAppManager->create(XQI_CONTACTS_FETCH,
                                                XQOP_CONTACTS_FETCH_SINGLE,
                                                false);
    mCleanupHandler.add(mContactSelectRequest);
    connect(mContactSelectRequest, SIGNAL(requestOk(QVariant)),
            this, SLOT(onContactSelectCompleted(QVariant)));  
    QList<QVariant> args;
    args << hbTrId("txt_friend_widget_title_select_contact");
    args << KCntActionAll;
    mContactSelectRequest->setArguments(args);
    
    qDebug() << "---- setArgs done ---------------------"; //,,28.5.          
    
    result = mContactSelectRequest->send();
    if (!result) {
        qDebug() << "Sending XQServiceRequest failed";
    }  
    
    qDebug() << "- launchSingleContactSelecting() done"; //,,
   
    return result;
}

/*!
   This slot gets selected contact's id from value parameter.
*/
void ContactWidgetHs::onContactSelectCompleted(const QVariant &value)
{
    qDebug() << "- onContactSelectCompleted() start";

    CntServicesContactList returnValue;
    if(value.canConvert<CntServicesContactList>()) {
        returnValue = qVariantValue<CntServicesContactList>(value);

        if (returnValue.count() == 0) {
            // User didn't select any contact
			mContactLocalId = ContactWidgetHs::unUsedContactId;
            qDebug() << "No contact selected";
        } else {
            for (int i = 0; i < returnValue.count(); ++i) {
                QString recipientName = returnValue[i].mDisplayName;
                qDebug() << recipientName << " id from param " << returnValue[i].mContactId;
                mContactLocalId = returnValue[i].mContactId;
            }
        }
    } else {
        qDebug() << "Can't convert CntServicesContactList! no contact selected";
        mContactLocalId = ContactWidgetHs::unUsedContactId;
    }
    // Save localId preference
    emit setPreferences( QStringList() << widgetPrefContactId );
    updateUiFromContact();

    qDebug() << "- onContactSelectCompleted() end, mContactLocalId " << mContactLocalId; //,,
}


/*!
    Set contact id
*/
void ContactWidgetHs::setContactId(const QString &Id)
{
    qDebug() << "ContactWidgetHs::setContactId() " << Id;
    mContactLocalId = Id.toInt();
    updateUiFromContact();
}

/*!
    Returns contact id
*/
QString ContactWidgetHs::contactId() const
{
    QString sId;
    sId.setNum((int) mContactLocalId);
    qDebug() << "ContactWidgetHs::contactId() " << sId;
    return sId;
}

/*!
    Slot gets called when contact details changed
*/
void ContactWidgetHs::onContactsChanged( const QList<QContactLocalId> &contactIds )
{
    qDebug() << "ContactWidgetHs::onContactsChanged";
    int i;
    for(i=0; i<contactIds.count(); i++) {
        if (contactIds.at(i) == mContactLocalId) {
            qDebug() << "-updating widget from contact " << mContactLocalId;
            updateUiFromContact();
        }
    }
}

/*!
    Slot gets called when contact removed
*/
void ContactWidgetHs::onContactsRemoved( const QList<QContactLocalId> &contactIds )
{
    qDebug() << "ContactWidgetHs::onContactsRemoved, removed count " << contactIds.count();
    int i;
    for(i=0; i<contactIds.count(); i++) {
        if (contactIds.at(i) == mContactLocalId) {
            qDebug() << "-deleting widget with removed contact " << mContactLocalId;
            finishWidget();
            break;
        }
    }
}

/*!
    Implemented to receive theme change event.
*/
void ContactWidgetHs::changeEvent(QEvent *event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        // get the text color from theme and set it to name label
        QColor textColor = HbColorScheme::color(normalTextColor);
        if (textColor.isValid()) {
            mContactNameLabel->setTextColor(textColor);
        } else {
            qDebug() << "ContactWidgetHs::changeEvent()  textColor is NOT Valid!! using default color";
        }
    }
}

/*!
    Creates communication launcher UI if needed and 
    sets the correct position for launcher
*/
void ContactWidgetHs::createLauncherWithPosition() 
{
    QRectF boundingRect = this->sceneBoundingRect(); // Widget size
    QRectF sceneRect(0, 0, 0, 0);
    if (mMainWindow) {
        sceneRect = mMainWindow->sceneRect();
    }
    
    mLauncher->createUI();    
    QPointF pos = mLauncher->commLauncherPosition(scenePos(), boundingRect, sceneRect, mLauncherRect);
    mLauncher->setPreferredPos(pos);
    mLauncher->selectAppearEffect(scenePos(), pos);
}

/*!
    Updates the layout with normal frame and text color
*/
void ContactWidgetHs::loadNormalLayout()
{
    loadLayout(normalFrameName, normalTextColor);
}

/*!
    Updates the layout with given frame and text color
*/
void ContactWidgetHs::loadLayout(const QString frameName, const QString textColor)
{
    mWidgetFrameDrawer->setFrameGraphicsName(frameName);
//    mWidgetFrameDrawer->themeChanged();
    
    mShareFrameDrawer->setFrameGraphicsName(frameName);
//    mShareFrameDrawer->themeChanged();
    
    QColor color = HbColorScheme::color(textColor);
    mContactNameLabel->setTextColor(color);

    update();
}

void ContactWidgetHs::onSelfContactIdChanged(const QContactLocalId &theOldId,
        const QContactLocalId &theNewId) {
    if (0 != theNewId && mContactLocalId == theNewId) {
        qDebug() << "-deleting widget after selfcontact change"
                 << mContactLocalId;
        finishWidget();
    }
}

void ContactWidgetHs::finishWidget() {
    mAvatarIconItem->deleteLater();
    mContactNameLabel->deleteLater();
    mContactLocalId = unUsedContactId;
    mContactHasAvatarDetail = false;
    
    if (!mLauncher->isPendingRequest()) {
        emit finished();
    } else {
    	mPendingExit = true;
    }
}
Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
