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
* Description:  Represents a page in the framework.
*
*/

#ifndef HSIMAGEGRIDWIDGET_H
#define HSIMAGEGRIDWIDGET_H

#include <HbView>
#include "hsutils_global.h"

class HbGridView;
class QStandardItemModel;
class QModelIndex;

class HSUTILS_EXPORT HsImageGridWidget : public HbView
{
	Q_OBJECT

public:

    HsImageGridWidget(const QStringList &imageFiles, QGraphicsItem *parent = 0);
	~HsImageGridWidget();

    void setContent(const QStringList &imageFiles);
	
signals:
    void imageSelected(const QString &selectedImage);

private slots:
	void gridItemActivated(const QModelIndex &index);

private:	
	HbGridView *mGridView;
	QStandardItemModel *mGridModel;
};

#endif // HSIMAGEGRIDWIDGET_H
