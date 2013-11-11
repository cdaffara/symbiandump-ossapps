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
* Description:  Offers file share services.
*
*/

// SYSTEM INCLUDES
#include <QVariant>
#include <QDir>
#include <QRegExp>
#include <QFile>
#include <QStringList>
#include <QGraphicsLinearLayout>

#include <QTranslator>
#include <QLocale>
#include <hbapplication.h>
#include <HbAction>
#include <HbDialog>
#include <HbLabel>
#include <HbListView>
#include <HbListViewItem>
#include <HbNotificationDialog>
#include <HbIconItem>
#include <HbTextItem>
#include <HbColorScheme>
#include <xqserviceglobal.h>
#include <xqaiwrequest.h>
#include <xqaiwinterfacedescriptor.h>

// USER INCLUDES
#include "shareuiprivate.h"

#define LOC_TITLE_SHARING_METHODS  hbTrId("txt_share_title_sharing_methods")
#define LOC_BUTTON_CANCEL       hbTrId("txt_common_button_cancel")
#define LOC_NO_SERVICES         hbTrId("txt_shareui_no_services ")
#define LOC_SERVICE_ERROR       hbTrId("txt_shareui_service_error")
#define LOC_PROTECTED_CONTENT   hbTrId("txt_shareui_protected_content")
#define LOC_NO_FILES            hbTrId("txt_shareui_no_files")

const QString LIST_ITEM_TITLE("qtc_list_item_title_normal");

/**
 * Constructor.
 */
ShareUiPrivate::ShareUiPrivate()
    {
    mContentItemModel = 0;
    mContentListView = 0;
    mSharePopup = 0;
    
    QString locale = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
        
    mTranslator = new QTranslator();
    mTranslator_comm = new QTranslator();
    
    bool result = mTranslator->load(path + QString("share_") + locale);
    result = mTranslator_comm->load(path + QString("common_") + locale);
    qApp->installTranslator(mTranslator);
    qApp->installTranslator(mTranslator_comm);
    
    }

/**
 * Destructor.
 */
ShareUiPrivate::~ShareUiPrivate()
    {
    delete mTranslator;
    delete mTranslator_comm;
    }

/**
 * Clear the internal data structures
 */
void ShareUiPrivate::reset()
    {
    mFileList.clear();

    mIndexActionMap.clear();
    foreach(XQAiwRequest* request,mAiwRequestList)
    {
        delete request;
    }
    mAiwRequestList.clear();
    }

/**
 * Initialize and start the dialog
 * 
 * @param fileList list of tokens that would allow file manipulation.
 * @return bool error false if dialog initialization failed, true otherwise.
 * 
 */
bool ShareUiPrivate::init(QStringList& fileList, bool embedded)
    {
    reset();
    mIsEmbedded = embedded;
    // No input files
    if (fileList.count() == 0) {
        showNote(LOC_NO_FILES);
        return true;
    }

#ifdef __SHAREUI_MIME_HANDLING__   
    // Get the file attributes (MIME and forward lock)
    QStringList mimeTypeList;
    QStringList forwardStatusList;
    getFileAttributes(fileList, mimeTypeList, forwardStatusList);

    // Ignore protected files
    QStringList filteredFileList;
    QStringList filteredMimeList;
    for (int i = 0; i < forwardStatusList.count(); i++) {
        if (forwardStatusList[i].toInt()) {
            filteredFileList << fileList[i];
            filteredMimeList << mimeTypeList[i];
        }
    }
    filteredMimeList.removeDuplicates();

    // Some protected content
    if (fileList.count() != filteredFileList.count()) {
        showNote(LOC_PROTECTED_CONTENT);
        return true;
    }

    // Only protected content
    if (filteredFileList.count() == 0) {
        showNote(LOC_NO_FILES);
        return true;
    }
#endif

#ifdef __SHAREUI_MIME_HANDLING__ 
    for (int i = 0; i < filteredFileList.count(); i++) {
        mFileList.append(QDir::toNativeSeparators(filteredFileList[i]));
    }
#else
    for ( int i = 0; i < fileList.count(); i++ )
    {
        mFileList.append(QDir::toNativeSeparators(fileList[i]));
    }
#endif

    QList<XQAiwInterfaceDescriptor> serviceDescriptorList = mAppManager.list(SERVICE_INTERFACE,
        SHARE_OP);

    if (serviceDescriptorList.size() > 0) {
        initializeUi();
        //sorting the services based on service names,
        QMap<QString,XQAiwInterfaceDescriptor > serviceInterfaceMap;
        for (int i = 0; i < serviceDescriptorList.count(); i++) {
            serviceInterfaceMap.insert(serviceDescriptorList[i].serviceName(),
                serviceDescriptorList[i]);
        }
        QStringList serviceNames = serviceInterfaceMap.keys();
        serviceDescriptorList.clear();
        
        for (int i = 0; i < serviceNames.count(); i++) {
#ifdef __SHAREUI_MIME_HANDLING__          
            // Filter services based on content type
            QString allowedTypes = serviceInterfaceMap[serviceNames.at(i)].customProperty(QString(
                "allowed_mime_types"));
            QString blockedTypes = serviceInterfaceMap[serviceNames.at(i)].customProperty(QString(
                "blocked_mime_types"));

            // Check against MIME filters
            if (!isContentAllowed(filteredMimeList, allowedTypes))
                continue;

            if (isContentBlocked(filteredMimeList, blockedTypes))
                continue;
#endif            
            HbAction* action = fetchServiceAction(serviceInterfaceMap[serviceNames.at(i)]);
            QString iconName = serviceInterfaceMap[serviceNames.at(i)].customProperty(QString("aiw_action_icon"));
            if (action) {
                updateShareUiDialogList(action, iconName);
            }
        }
        serviceInterfaceMap.clear();
        if (mContentItemModel->rowCount() == 0) {
            showNote(LOC_NO_SERVICES);
            return true;
        }

        mSharePopup->show();
    }
    else {
        showNote(LOC_NO_SERVICES);
    }

    return true;
    }

/**
  * Creates the view for the share ui.
  */
void ShareUiPrivate::initializeUi()
    {
    // Dialog
    mSharePopup = new HbDialog();
    // make it delete itself on close
    mSharePopup->setAttribute( Qt::WA_DeleteOnClose, true );
    HbTextItem* heading = new HbTextItem(LOC_TITLE_SHARING_METHODS, mSharePopup);
    QColor color = HbColorScheme::color( LIST_ITEM_TITLE );
    heading->setTextColor( color );
    heading->setAlignment(Qt::AlignCenter);
    mSharePopup->setDismissPolicy(HbDialog::TapAnywhere);
    mSharePopup->setHeadingWidget(heading);
    mSharePopup->setFrameType(HbDialog::Strong);
    connect(mSharePopup, SIGNAL(aboutToClose()), this, SLOT(reset()));
        
    // Content widget
    mContentListView = new HbListView(mSharePopup);
    ShareListItem *prototype = new ShareListItem(mSharePopup);
    mContentListView->setItemPrototype(prototype);

    mContentItemModel = new QStandardItemModel(mSharePopup);
    mContentListView->setModel(mContentItemModel); 
    mContentListView->setUniformItemSizes(true);
    mSharePopup->setContentWidget(mContentListView);
    connect(mContentListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(itemActivated(QModelIndex)));  
    
    HbAction* cancelAction = new HbAction(LOC_BUTTON_CANCEL,mSharePopup);
    mSharePopup->addAction(cancelAction);
    }

/**
 * fetchServiceAction fetches the action associated with a specified interface descriptor.
 * This is used after the fetchServiceDescriptors is called, and 
 */
HbAction* ShareUiPrivate::fetchServiceAction
(XQAiwInterfaceDescriptor interfaceDescriptor)
    {   
    XQAiwRequest* request = mAppManager.create(interfaceDescriptor, 
            SHARE_OP, mIsEmbedded);
    mAiwRequestList.append(request);
    
    HbAction* action = convertAction(request->createAction());
    if ( action )
        {
        connect(request, SIGNAL(triggered()), this, SLOT(onTriggered()));
        connect(request, SIGNAL(requestOk(const QVariant&)), this, 
                SLOT(handleOk(const QVariant&)));
        connect(request, SIGNAL(requestError(int,const QString&)), this, 
                SLOT(handleError(int,const QString&)));
        }
    return action;
    }


/**
 * Update the SendUi Dialog (and associated list items) once the required
 * information has been fetched.
 */
void ShareUiPrivate::updateShareUiDialogList(HbAction* action, QString iconName)
    {
    QStandardItem*  item = new QStandardItem();
    item->setData(action->text(), ProviderNameRole);
    item->setData(iconName, ProviderIconRole);
    mContentItemModel->appendRow(item);
    QModelIndex index = item->index();
    mIndexActionMap[index] = action;
    }

/**
 * Slot for handling send service selection on the UI.
 */
void ShareUiPrivate::onTriggered(void)
    {
    
    XQAiwRequest* request = 0;
    request = qobject_cast<XQAiwRequest*>(sender());
    if(request)
        {
        QList<QVariant> args;
        QVariant fileList(mFileList);
        args << fileList;
        request->setEmbedded(mIsEmbedded);
        request->setArguments(args);
        }
    }

/**
 * Slot for handling valid returns from the framework.
 */
void ShareUiPrivate::handleOk(const QVariant& result)
{
    Q_UNUSED(result)
         
    XQAiwRequest* request = 0;
    request = qobject_cast<XQAiwRequest*>(sender());
    if(request)
    {
       disconnect(request, 
               SIGNAL(requestError(int,const QString&)), 
               this, 
               SLOT(handleError(int,const QString&)));
    }
}

/**
 * Slot for handling errors from the framework.
 */
void ShareUiPrivate::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorCode)
    Q_UNUSED(errorMessage)
    
    showNote(LOC_SERVICE_ERROR);
    }

/** 
 * When a list box item is pressed, this slot should be invoked. The action
 * corresponding to the clicked list item should be invoked with the stored lis
 * of files.
 * 
 * @param index QModelIndex
 */

void ShareUiPrivate::itemActivated(QModelIndex index)
    {
    HbAction* action = mIndexActionMap[index];
    
    if ( action )
        {
        action->setEnabled(true);
        action->activate(HbAction::Trigger);
        } 
    
    mSharePopup->close();
    }


/**
 * Convert a qaction to hbaction.
 * 
 * @param action QAction*
 * @return HbAction* 
 */
HbAction* ShareUiPrivate::convertAction(QAction *action)
    {
    HbAction *actionHb = 0;
    if (action) 
        {
        actionHb = new HbAction(HbIcon(action->icon()), action->text());
        actionHb->setVisible(action->isVisible());
        actionHb->setCheckable(action->isCheckable());
        actionHb->setEnabled(action->isEnabled());

        connect(actionHb, SIGNAL(triggered()), action, SIGNAL(triggered()));
        connect(actionHb, SIGNAL(changed()), action, SIGNAL(changed()));
        connect(actionHb, SIGNAL(hovered()), action, SIGNAL(hovered()));
        connect(actionHb, SIGNAL(toggled(bool)), action, SIGNAL(toggled(bool)));
        
        action->setParent(actionHb); 
        }
    
    return actionHb;
    }

#ifdef __SHAREUI_MIME_HANDLING__  
/**
 * Checks if the specified content type is allowed as per the 
 * specified MIME pattern strings
 */
bool ShareUiPrivate::isContentAllowed( QStringList mimeType, 
        QString mimePattern )
    {
    if ( mimePattern.trimmed().length() ==  0 )
        return true;
    
    QStringList patterns = mimePattern.split(",");
    
    for ( QStringList::Iterator it = mimeType.begin(); 
            it != mimeType.end(); ++it ) 
        {    
        bool match = false;
        for ( QStringList::Iterator it1 = patterns.begin(); 
                it1 != patterns.end(); ++it1 ) 
            {               
            QString pattern = (*it1).trimmed();
            QRegExp rx(pattern, Qt::CaseInsensitive, QRegExp::Wildcard); 
            // Not case sensitive, wild cards allowed
            if ( rx.exactMatch( *it ) )
                {
                match = true;
                break;
                }
            }
        
        if ( ! match )
            {
            return false;           
            }
        }
    
    return true;
    }

/**
 * Checks if the specified content type is allowed as per the 
 * specified MIME pattern strings
 */
bool ShareUiPrivate::isContentBlocked( QStringList mimeType, 
        QString mimePattern )
    {
    if ( mimePattern.trimmed().length() ==  0 )
        return false;
    
    QStringList patterns = mimePattern.split(",");
    
    for ( QStringList::Iterator it = mimeType.begin(); 
            it != mimeType.end(); ++it ) 
        {   
        bool match = false;
        for ( QStringList::Iterator it1 = patterns.begin(); 
                it1 != patterns.end(); ++it1 ) 
            {                
            QString pattern = (*it1).trimmed();
            QRegExp rx(pattern, Qt::CaseInsensitive, QRegExp::Wildcard); 
            // Not case sensitive, wild cards allowed
            if ( rx.exactMatch( *it ) )
                {
                match = true;
                break;
                }
            }
        
        if ( match )
            {
            return true;           
            }
        }
    
    return false;
    }
 
/**
 * Get the file MIME types and forward status
 */
void ShareUiPrivate::getFileAttributes
(QStringList& fileList, QStringList& mimeTypeList, QStringList& forwardStatusList)
    {
    // Attributes to read
    QList<int> attrNames;
    attrNames.append(XQApplicationManager::MimeType);
    attrNames.append(XQApplicationManager::IsForwardable);
    
    for ( QStringList::Iterator it = fileList.begin(); 
                  it != fileList.end(); ++it ) 
        {      
        QFile file(*it);
        QVariantList attrValues;
        bool ok = mAppManager.getDrmAttributes(file, attrNames, attrValues);
        if ( !ok )
            {
            mimeTypeList << QString();
            forwardStatusList << QString("1"); // Forward ok.
            continue;
            }
        
        int i = 0;
        foreach (QVariant v, attrValues)
            {
            if ( attrNames[i] == XQApplicationManager::MimeType )
                mimeTypeList << v.toString();
            if ( attrNames[i] == XQApplicationManager::IsForwardable )
                forwardStatusList << v.toString();
            i++;
            }
        }
    }
#endif

/**
 * Show notes
 */
void ShareUiPrivate::showNote(QString text)
    {
    HbNotificationDialog* dlg = new HbNotificationDialog();
    dlg->setFocusPolicy(Qt::NoFocus);
    dlg->setAttribute(Qt::WA_DeleteOnClose, true);
    dlg->setTitle(text);
    dlg->show();
    }

/**
 * List item prototype constructor.
 */
ShareListItem::ShareListItem(QGraphicsItem* parent) :
        HbListViewItem(parent),
        mProviderName(NULL),
        mProviderIcon(NULL)
    {
    }
    
/**
 * @see HbListViewItem
 */
HbAbstractViewItem* ShareListItem::createItem()
    {
    return new ShareListItem(*this);
    }

/**
 * @see HbListViewItem
 */
void ShareListItem::updateChildItems()
    {
    QGraphicsLinearLayout *layout = 
            new QGraphicsLinearLayout(Qt::Horizontal, this);

    if (!mProviderIcon)
        {
        QVariant iconName = modelIndex().data(ProviderIconRole);     
        mProviderIcon = new HbIconItem(HbIcon(iconName.toString()), this);
        }
    
    if (!mProviderName)
        {
        QVariant name = modelIndex().data(ProviderNameRole);     
        mProviderName = new HbTextItem(name.toString(), this);
        }
    
    layout->addItem(mProviderIcon);
    layout->addItem(mProviderName);
    
    HbListViewItem::updateChildItems();
    }

// End of file

