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

#ifndef VIEWMANAGER_H_
#define VIEWMANAGER_H_


#include <QObject>
#include <QVariantMap>

enum ViewIndex {
	MainView,
	ProcessView,
	ThreadView,
	ThreadDetailIndexView,
	ThreadDetailView,
	KernelObjectTypeView,
	KernelObjectView,
	KernelObjectDetailView,
	TrackingView,
	SwmtView,
	HeapDumpsView,
	ServerView,
	EComCategoryView,
	EComInterfaceView,
	EComImplementationView,
	EComImplementationDetailView,
	WindowGroupsView,
	WindowGroupsDetailView,
};

class HbMainWindow;
class HbView;
class EngineWrapper;


class ViewManager : public QObject
{
	Q_OBJECT
	
public:
	ViewManager(HbMainWindow &window, EngineWrapper &engine, QObject *parent = 0);
	
public slots:
	void showView(ViewIndex index, const QVariantMap &params);
	
	void showView(ViewIndex index);
	
	void goBack();
	
private slots:
	void viewChanged(HbView *view);
	
private:
	HbMainWindow &mWindow;
	EngineWrapper &mEngine;
};

#endif /* VIEWMANAGER_H_ */
