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
* Description:   ?Description
*
*/

#ifndef GLXFETCHER_H
#define GLXFETCHER_H

#include <hbmainwindow.h>
#include <xqserviceprovider.h>
#include <QStringList>
#include <xqsharablefile.h>
#include <QModelIndex>
#include <QAbstractItemModel>

//FORWARD CLASS DECLARATION
class GlxGetImageService;
class GlxImageViewerService;
class GlxStateManager;
class CGlxImageViewerManager;
class GlxGetImageServiceNSDI;
class GlxGetImageServiceDSDI;
class GlxBrowseImageService;

class GlxExternalUtility;
/**
 * This is qthigway service handler class for all services provide by photos.
 * 
 */
class GlxAiwServiceHandler: public HbMainWindow
    {
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxAiwServiceHandler();

    /**
     * Destructor.
     */
    ~GlxAiwServiceHandler();

    /**
     * launch image fetcher view
     * @param viewTitle title for image fetcher
     */
	void launchFetcher(QString viewTitle,int fetcherFilterType = 0);

	/**
     * launch image viewer 
     * @param viewTitle title for image fetcher
     */
	void launchImageViewer(QString viewTitle);
	
	void launchImageBrowser(QString viewTitle);
	
public slots:  
    /**
     * This slot is called when image to be returned to fetcher 
     * client is selected
     * @param index index of the selected image
     * @param model model for the current view where image is selected 
     */
    void itemSelected(const QModelIndex &  index,QAbstractItemModel & model);

    /**
     * This slot is called when service client is closed
     * 
     */
    void handleClientDisconnect();
    
private:
    /// state manager for the services
	GlxStateManager *mStateMgr;
	/// image fetcher service provider
    GlxGetImageService* mFetcherService;
    /// image fetcher service provider with new service name and depricated interface name
    GlxGetImageServiceNSDI* mNSDIService;
    /// image fetcher service provider with depricated name and depricated inaterface name
    GlxGetImageServiceDSDI* mDSDIService;
    GlxImageViewerService* mImageViewerService;
	GlxBrowseImageService* mImageBrowserService;
    GlxExternalUtility *mUtil;
    };

/**
 *  GlxGetImageService : Image fetcher service provider
 * 
 */	
class GlxGetImageService : public XQServiceProvider
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxGetImageService( GlxAiwServiceHandler *parent = 0 );

    /**
     * Destructor.
     */
    ~GlxGetImageService();

    /**
     * check if service is active
     */
    bool isActive();

    /**
     * called to complete fetch service and return to client
     * @param fileList list of Uri to be returned to client 
     */
    void complete( QStringList filesList);
    
public slots:
    /**
     * slot for qthighway to notify provider about request
     */
    void fetch();
    
    /**
     * slot for service provider to notify client about error
     */
    void fetchFailed( int errorCode );
    
private:
    /**
     * called to complete qthighway service
     * @param fileList list of Uri to be returned to client
     */
    void doComplete( QStringList filesList);
    
private:
    /// current service request id
    int mImageRequestIndex;
    /// service handler for all photos services
    GlxAiwServiceHandler* mServiceApp;
};

/**
 * Class Description
 *  GlxGetImageServiceDSDI
 *  Service provide for new service and depricated interface
 */ 
class GlxGetImageServiceNSDI : public XQServiceProvider
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxGetImageServiceNSDI( GlxAiwServiceHandler *parent = 0 );
    /**
     * Destructor.
     */
    ~GlxGetImageServiceNSDI();

    /**
     * check if service is active
     */
    bool isActive();

    /**
     * called to complete fetch service and return to client
     * @param fileList list of Uri to be returned to client 
     */
    void complete( QStringList filesList);
    
public slots:
    /**
     * slot for qthighway to notify provider about request
     */
    void fetch( QVariantMap filter , QVariant flag );

    /**
     * slot for qthighway to notify provider about request
     */
    void fetch();
    
public slots:
    /**
     * slot for service provider to notify client about error
     */
    void fetchFailed( int errorCode );
    
private:
    /**
     * called to complete fetch service and return to client
     * @param fileList list of Uri to be returned to client 
     */
    void doComplete( QStringList filesList);
    
private:
    /// current service request id
    int mImageRequestIndex;
    /// service handler for all photos services
    GlxAiwServiceHandler* mServiceApp;
};

/**
 *  GlxGetImageServiceDSDI
 *  Service provide for depricated service and depricated interface
 */ 
class GlxGetImageServiceDSDI : public XQServiceProvider
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxGetImageServiceDSDI( GlxAiwServiceHandler *parent = 0 );
    /**
     * Destructor.
     */
    ~GlxGetImageServiceDSDI();

    /**
     * check if service is active
     */
    bool isActive();

    /**
     * called to complete fetch service and return to client
     * @param fileList list of Uri to be returned to client 
     */
    void complete( QStringList filesList);
    
public slots:
    /**
     * slot for qthighway to notify provider about request
     */
    void fetch( QVariantMap filter, QVariant flag);
    
public slots:
    /**
     * slot for service provider to notify client about error
     */
    void fetchFailed( int errorCode );
    
private:
    /**
     * called to complete fetch service and return to client
     * @param fileList list of Uri to be returned to client 
     */
    void doComplete( QStringList filesList);
    
private:
    /// current service request id
    int mImageRequestIndex;
    /// service handler for all photos services
    GlxAiwServiceHandler* mServiceApp;
};

/**
 * Image viewer service provider 
 */
class GlxImageViewerService : public XQServiceProvider
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxImageViewerService( GlxAiwServiceHandler *parent = 0 );

    /**
     * Destructor.
     */
    ~GlxImageViewerService();

    /**
     * compete request 
     * @param ok true id success else false
     */
    void complete(bool ok);

    /**
     * check if request is aSync
     * @return true if async else false
     */
    bool asyncRequest() {return mAsyncRequest;}

public slots:
    /**
     * slot for qthighway to notify provider about request
     */
    bool view(QString file);

    /**
     * slot for qthighway to notify provider about request
     */
    bool view(XQSharableFile file);

private:
    /// service handler for all photos services
    GlxAiwServiceHandler* mServiceApp;
    /// current request id
    int mAsyncReqId;
    bool mRetValue;
    bool mAsyncRequest;
    /// image viewer manager instance to get info regarding file
    CGlxImageViewerManager* mImageViewerInstance;
};

/**
 *  GlxGetImageService : Image fetcher service provider
 * 
 */	
class GlxBrowseImageService : public XQServiceProvider
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxBrowseImageService( GlxAiwServiceHandler *parent = 0 );

    /**
     * Destructor.
     */
    ~GlxBrowseImageService();

    /**
     * called to complete fetch service and return to client
     * @param fileList list of Uri to be returned to client 
     */
    void complete( bool ok);
    bool asyncRequest() {return mAsyncRequest;}
public slots:
    /**
     * slot for qthighway to notify provider about request
     */
    void browse();
    
    
private:
    /// current service request id
    int mAsyncReqId;
	bool mAsyncRequest;
    /// service handler for all photos services
    GlxAiwServiceHandler* mServiceApp;
};
    
#endif //GLXFETCHER_H
