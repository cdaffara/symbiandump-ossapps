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

#ifndef HSWIDGETCOMPONENT_H
#define HSWIDGETCOMPONENT_H

#include <QObject>
#include <QTranslator>

#include "hsdomainmodel_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsWidgetComponent : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString rootPath READ rootPath)
	Q_PROPERTY(QString uri READ uri)
	
	enum State {Available, Unavailable, Uninstalling,Uninstalled };
public:
    ~HsWidgetComponent();

	QString rootPath() const;
	QString uri() const;
	bool isAvailable() const;
signals:
	void aboutToUninstall();
	void uninstalled();
	void updated();
	void unavailable();
	void available();
private:
    Q_DISABLE_COPY(HsWidgetComponent)
	HsWidgetComponent(const QString &uri, QObject *parent = 0);

	void resolveRootPathAndTranslationFilename();
    void installTranslator();
	void uninstallTranslator();
	
	void emitAboutToUninstall();
	void emitUninstalled();
	void emitUpdated();
	void emitUnavailable();
	void emitAvailable();
private:	
	QTranslator mTranslator;
	QString mUri;
	
	QString mRootPath;
	QString mDrive;
	State mState;
	QString mTranslationFilename;
	friend class HsWidgetComponentRegistry;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWIDGETCOMPONENT_H
