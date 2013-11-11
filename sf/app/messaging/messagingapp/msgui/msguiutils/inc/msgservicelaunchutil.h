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
 * Description:This class provides duration of media file by parsing it
 *
 */

#ifndef MSGSERVICELAUNCHUTIL_H_
#define MSGSERVICELAUNCHUTIL_H_

#ifdef BUILD_MSGUI_UTILS_DLL
#define MSGUI_UTILS_DLL_EXPORT Q_DECL_EXPORT
#else
#define MSGUI_UTILS_DLL_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>

/**
 * This class provides duration of media file by parsing it
 * 
 */
class MSGUI_UTILS_DLL_EXPORT MsgServiceLaunchUtil : public QObject
{
    Q_OBJECT
    
public:   
    /**
     * Constructor
     */
    MsgServiceLaunchUtil(QObject* parent=0);
    
    /**
     * Destructor
     */
    ~MsgServiceLaunchUtil();
    
    /**
     * Utility function to launch different content viewers.
     * @param mimeType Mime type of the content.
     * @param filePath Absolute file path of content.
     */
    void launchContentViewer(
            const QString &mimeType, 
            const QString &filePath);    
    
    /**
     * Utility function to launch different content viewers.
     * @param mimeType Mime type of the content.
     * @param filePath Absolute file path of content.
     */
    void launchContentViewer(int messageId); 

private slots:

    /**
     * Service launch complete.
     */
    void handleOk(const QVariant& result);

    /**
     * Service launch errors.
     */
    void handleError(int errorCode, const QString& errorMessage);
    
private:
    
    /**
     * This launches any sharable content using XQSharable
     * @param filePath reference to sharable file path
    */
    void launchViaSharableFile(const QString &filePath);   
    
};    

#endif /* MSGSERVICELAUNCHUTIL_H_ */

//eof

