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

#include <QtTest/QtTest>
#include <QObject>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntListModel;
class ModelListener;

class TestCntListModel : public QObject
{
    Q_OBJECT

private:
    void contactReady(int start, int end);
    QContact createContact(const QString& firstName, const QString& lastName);
    void addGroupMember(const QContact& group, const QContact& contact);
    void removeGroupMember(const QContact& group, const QContact& contact);

private slots:
	void initTestCase();
	void create();
	
	void data();
	void rowCount();
	
	void contact();
	void contactId();
	void indexOfContact();
	void indexOfContactId();
	void contactManager();
	void setFilter();
	void myCard();

	void rowId();
	void dataForDisplayRole();

    void handleAdded();
    void handleChanged();
    void handleRemoved();
	void handleMyCardChanged();
    void handleRelationships();
	
	void cleanupTestCase();
	
private:
    QContactManager *mManager;
    CntListModel    *mCntModel;
    ModelListener   *mModelListener;
    bool             mDataReady;

friend class ModelListener;
};

class ModelListener : public QObject
{
    Q_OBJECT

public:
    ModelListener(TestCntListModel* parent);

private slots:
    void onDataChanged(QModelIndex start, QModelIndex end);

private:
    TestCntListModel* mParent;
};
