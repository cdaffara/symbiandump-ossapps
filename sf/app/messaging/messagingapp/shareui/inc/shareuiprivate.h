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

#ifndef __SHARE_UI_PRIVATE_H__
#define __SHARE_UI_PRIVATE_H__

#include <QObject>
#include <QMap>
#include <QAction>
#include <QModelIndex>
#include <QStandardItemModel>
#include <HbListViewItem>
#include <xqappmgr.h>

// FORWARD DECLARATIONS
class XQAiwRequest;
class XQAiwInterfaceDescriptor;
class HbAction;
class HbDialog;
class HbListView;
class HbListViewItem;
class HbTextItem;
class HbIconItem;
class QTranslator;

#define SERVICE_INTERFACE "com.nokia.symbian.IFileShare"
#define SHARE_OP "send(QVariant)"

/**
 * Data roles for list
 */
enum ShareItemDataRole {
    ProviderNameRole = 10000,
    ProviderIconRole = 10001
};

/**
 * Share List item prototype
 */
class ShareListItem : public HbListViewItem
    {
    Q_OBJECT
    
public:
    ShareListItem(QGraphicsItem* parent=0);
    HbAbstractViewItem* createItem();
    void updateChildItems();
private:
    HbIconItem* mProviderIcon;
    HbTextItem *mProviderName;
    };

/**
 * Private implementaion of shareui.
 * implements showing sending services dialog and sending files
 */
class ShareUiPrivate : public QObject
    {
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     */
    ShareUiPrivate();
    
    /**
     * Destructor
     */
    virtual ~ShareUiPrivate();
    
    /**
     * Initiate share for the specified files.
     * @param fileList list of paths to files.eg: c:\images\sunset.jpg
     * @param embedded true if service launched is embedded in the 
     * launched application.
     * 
     * @return bool true if dialog initialization successfull. Otherwise false.
     */
    bool init(QStringList& fileList, bool embedded);
    
public slots:

    /**
     * Handles setting the arguments for triggered service.
     */
    void onTriggered(void); 
    
    /**
     * Slot for handling valid returns from the framework.
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);
    
    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);

private slots:

    /** 
     * When a list box item is pressed, this slot should be invoked. The action
     * corresponding to the clicked list item should be invoked with the stored lis
     * of files.
     * 
     * @param index QModelIndex
     * 
     */
    void itemActivated(QModelIndex index);
    
    /**
     * Reset internal data structures
     */
    void reset();
    
private:
    /**
     * Fetch the action associated with a specified interface descriptor.
     * @param interfaceDescriptor Descriptor got from application manager.
     * @return The action.
     */
    HbAction* fetchServiceAction(XQAiwInterfaceDescriptor interfaceDescriptor);    
    
    /**
     * Creates the view for the share ui dialog.
     */
    void initializeUi();
    
    /**
     * Update the shareui dialog (and associated list items) once the required
     * information has been fetched.
     * @param action HbAction* The action associated with list entry.
     * @param iconName The icon to be displayed 
     */
    void updateShareUiDialogList(HbAction* action, QString iconName);
    
    /**
     * Convert a QAction to HbAction.
     * @param action QAction*
     * @return HbAction* 
     */
    HbAction* convertAction(QAction *action);

#ifdef __SHAREUI_MIME_HANDLING__  
    /**
     * Checks if the specified content type is allowed as per the 
     * specified MIME pattern strings.
     * @param mimeType List of file MIME types
     * @param mimePattern List of file MIME patterns
     * @return true if the content is allowed.
     */
    bool isContentAllowed ( QStringList mimeType, QString mimePattern );
    
    /** Checks if the specified content type is blocked as per the 
     * specified MIME pattern strings.
     * @param mimeType List of file MIME types
     * @param mimePattern List of file MIME patterns
     * @return true if the content is blocked.
     */
    bool isContentBlocked ( QStringList mimeType, QString mimePattern );
    
    /**
     * Get file attributes
     * @param fileList List of files.
     * @param mimeTypeList (out) List of MIME types corresponding to the files.
     * @param forwardStatusList (out) List of forward status corresponding to the files.
     */
    void getFileAttributes ( QStringList& fileList, QStringList& mimeTypeList,
            QStringList& forwardStatusList );
#endif
    
    /**
     * Show notes
     */
    void showNote(QString text);

private:
    /**
     * List of files to be sent.
     */
    QList<QVariant> mFileList;
    
    /**
     * Service List
     */
    QList<XQAiwRequest*> mAiwRequestList;
    
    /**
     * ShareUi dialog
     */
    HbDialog* mSharePopup;
    
    /**
     * item model for content list view.
     */    
    QStandardItemModel* mContentItemModel;
    
    /**
     * content view
     */    
    HbListView* mContentListView;
    
    /**
     * mapping qmodelindex to the action
     */    
    QMap<QModelIndex, HbAction*> mIndexActionMap;
    
    /**
     * Application Manager.
     */
    XQApplicationManager mAppManager;
    
    /**
     * Translator member variable
     * Owned
     */ 
    QTranslator* mTranslator;
    
    /**
     * Translator member variable
     * Owned
     */
    QTranslator* mTranslator_comm;
    
    /**
     * Whether to launch the sending application as embedded or not.
     */
    bool mIsEmbedded;  
    
    };

#endif /* __SHARE_UI_PRIVATE_H__ */
