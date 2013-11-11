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

#ifndef HELPDATAPROVIDER_H
#define HELPDATAPROVIDER_H

class HelpModel;
class HelpItems;
class HelpProxyModel;
class QStandardItemModel;
class HelpStandardItem;

class HelpDataProvider
{	
private:
    HelpDataProvider();
    ~HelpDataProvider();
    
public:
    static HelpDataProvider* instance();
    static void destroyInstance();

public:
	void createHelpCategory();
	QAbstractItemModel* getCategoryData();
	QAbstractItemModel* getSearchData(const QString& key=QString());
	void setHelpContentUrl(const QString& uid, const QString& href);
	void getHelpContentData(QString& content, QString& url);

private:
	//construct 1st category
	void constructCategory();
	//construct help category in rom
	void constructBuiltInCategory(const QString& path, const QStringList& uidList, const QStringList& titleList);
	//construct help category for 3rd party application
	void constructAppCategory(const QString& path, QStringList& uidList);
	void constructBuiltInCategoryItem(const QString& uid, const QString& title);

	//construct 2nd category
	void constructCategory2(HelpStandardItem* itemParent);
	void constructCategory2Item(HelpStandardItem* itemParent);

	//construct keyword list
	void constructKeywordModel(const QString& title, const QString& uid, const QString& href);

	//parse xml
	void parseCategoryIndexXml(const QString& path, QStringList& uidList, QStringList& titleList);
	void parseCategory2IndexXml(const QString& path, QStringList& hrefList, QStringList& titleList);
	void parseBuiltInMetaxml(const QString& path, int& priority);
	void parseAppMetaxml(const QString& path, QString& title);	

private:
	QStandardItemModel*		mHelpModel;         //category tree model
	QStandardItemModel*		mKeywordModel;      //keyword list model
	HelpProxyModel*			mSearhResultModel;  //search result proxy model of keyword model	
	HelpStandardItem*		mAppItem;
	QString					mHelpContentRoot;
	QString					mLastSrhKey;
	QStringList				mUpdateUidList;

};

#endif //HELPDATAPROVIDER_H
