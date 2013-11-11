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
 * Description: Model manager holds models for three day views
 *
 */
#ifndef CALENDAYMODELMANAGER_H
#define CALENDAYMODELMANAGER_H

#include <QObject>
#include <QMap>
#include <QDateTime>

#include "calendaycommonheaders.h"
#include <QAbstractListModel>
class QAbstractItemModel;
class MCalenServices;
class CalenDayModel;

/*!
 Test class TestAbstractListModel
 */
class TestAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    TestAbstractListModel() : QAbstractListModel() {
    }
    
    ~TestAbstractListModel() {
    }
    
    int rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return 0;
    }
    
    QVariant data(const QModelIndex &index, int role) const {
        Q_UNUSED(index);
        Q_UNUSED(role);
        return QVariant(0);
    }
};

class CalenDayModelManager : public QObject
	{
Q_OBJECT

public:
	enum ModelDay
		{
		PreviousDay = 0, CurrentDay = 1, NextDay = 2, NumberOfDays
		};

public:
	CalenDayModelManager(MCalenServices &services, bool emptyModels, QObject *parent = 0) :
		mServices(services)
		{
		Q_UNUSED(emptyModels);
		Q_UNUSED(parent);
		}
	~CalenDayModelManager()
		{
		}

	QAbstractItemModel &getModel(CalenDayModelManager::ModelDay day)
		{
		Q_UNUSED(day);
		model = new TestAbstractListModel();
		return *model;
		}

	void refreshAllModels()
		{
		}
	void refreshSingleModel(CalenDayModelManager::ModelDay day)
		{
		Q_UNUSED(day);
		}
	MCalenServices &getServices()
		{
		return mServices;
		}

public slots:
	void viewsScrollingFinished(CalenScrollDirection scrollTo)
		{
		Q_UNUSED(scrollTo);
		}

private:
	void createAllModels()
		{
		}
	void moveBackward()
		{
		}
	void moveForward()
		{
		}

private:
	QMap<ModelDay, CalenDayModel*> mModels;
	MCalenServices &mServices;
	QDateTime mCurrentDayTime;
	TestAbstractListModel* model;
	};

#endif//CALENDAYMODELMANAGER_H
