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
/*
 * xqaiwgetimageclient.h
 *
 *  Created on: 2009-07-23
 *      Author: przemysl
 */

#ifndef XQAIWGETIMAGECLIENT_H_
#define XQAIWGETIMAGECLIENT_H_

#include <QObject>
#include <QVariant>
#include <QPointer>
#include <QStringList>
#include "hsutils_global.h"

class HbAction;
class HsImageGridWidget;
class HbView;

class HSUTILS_EXPORT XQAIWGetImageClient : public QObject
{
    Q_OBJECT
public:
    XQAIWGetImageClient();
    ~XQAIWGetImageClient();
    
public slots:
    void fetch();
    
private slots: 
    void imageSelected(const QString&);
    void requestCompleted(const QVariant& val);
    void imageSelectionCancelled();

signals:
    void fetchCompleted(const QString&);
    void fetchFailed(int, const QString&);

private:
    void findImages(const QString &aDir, const QStringList &aFilters, QStringList &aFoundImages);
private:
    QPointer<HsImageGridWidget> mImageGrid;//added
    HbAction *mBackAction;//added
    HbView * mCurrentView;

};

#endif /* XQAIWGETIMAGECLIENT_H_ */
