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

// System includes
#include <QObject>
#include <QMap>
#include <QDateTime>

// User includes
#include "calendaycommonheaders.h"

// Forward declarations
class QAbstractItemModel;
class MCalenServices; 
class CalenDayModel;

class CalenDayModelManager : public QObject
{
    Q_OBJECT

public:
    enum ModelDay {
		PreviousDay = 0,
		CurrentDay = 1,
		NextDay = 2,
		NumberOfDays
    };

public:
    CalenDayModelManager(MCalenServices &services, bool emptyModels,
								QObject *parent = 0);
    ~CalenDayModelManager();

    QAbstractItemModel &getModel(CalenDayModelManager::ModelDay day);
    
    void refreshAllModels();
    void refreshSingleModel(CalenDayModelManager::ModelDay day);
    MCalenServices &getServices() {
        return mServices;
    }
    
public slots:
	void viewsScrollingFinished(CalenScrollDirection scrollTo);

private:
	void createAllModels();
	void moveBackward();
	void moveForward();

private:
    QMap<ModelDay, CalenDayModel*> mModels;
    MCalenServices &mServices;
    QDateTime mCurrentDayTime;
};

#endif//CALENDAYMODELMANAGER_H
