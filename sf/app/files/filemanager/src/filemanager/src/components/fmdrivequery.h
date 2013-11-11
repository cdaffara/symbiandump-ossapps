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
 * 
 * Description:
 *     The header file of backup drive query of of file manager
 */
#ifndef FMDRIVEQUERY_H
#define FMDRIVEQUERY_H

#include "fmcommon.h"
#include "fmdialog.h"
#include "fmdrivemodel.h"

#include <QString>
#include <QMap>
#include <QStringList>
#include <QGraphicsItem>

class HbListView;
class QGraphicsWidget;

// CLASS DECLARATION
/**
 *  This class is used for drive select query dialog
 *
 */
class FmDriveQuery : public FmDialog, public FmDriveListProvider
{
    Q_OBJECT
public:
	FmDriveQuery( QGraphicsItem *parent = 0 );
	~FmDriveQuery();
	QString selectedDrive() const;
	
	// from FmDriveListProvider
	virtual void getDriveList( QStringList &driveList );
	
public slots:
	void activated(const QModelIndex &index); 
    void refreshModel( QString path );
private:
	QGraphicsWidget *mContentWidget;   /// Content widget of popup dialog
    HbListView      *mListView;        /// list view
    FmDriveModel    *mDriveModel;      /// drive model
    
    QString mSelectedDrive;
};

#endif //FMDRIVEQUERY_H
