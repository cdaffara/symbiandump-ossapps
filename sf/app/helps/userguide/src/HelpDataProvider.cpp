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

#include <QStandardItemModel>
#include <QXmlQuery>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QDir>

#include "HelpUtils.h"
#include "HelpCommon.h"
#include "HelpProxyModel.h"
#include "HelpStandardItem.h"

#include "HelpDataProvider.h"


HelpDataProvider::HelpDataProvider()
{
	mHelpModel = new QStandardItemModel();
	mKeywordModel = new QStandardItemModel();
	mSearhResultModel = new HelpProxyModel();
	mSearhResultModel->setSourceModel(mKeywordModel);
	mAppItem = NULL;
}

HelpDataProvider::~HelpDataProvider()
{
	delete mHelpModel;
	delete mSearhResultModel;
	delete mKeywordModel;
}


///////////////////////////////////////////////////////////////////////////////////////

HelpDataProvider* gHelpDataProviderInstance = NULL;

HelpDataProvider* HelpDataProvider::instance()
{
    if(!gHelpDataProviderInstance)
    {
        gHelpDataProviderInstance = new HelpDataProvider();
    }

    return gHelpDataProviderInstance;
}

void HelpDataProvider::destroyInstance()
{
    delete gHelpDataProviderInstance;
    gHelpDataProviderInstance = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////

QAbstractItemModel* HelpDataProvider::getCategoryData()
{
    return mHelpModel;
}

QAbstractItemModel* HelpDataProvider::getSearchData(const QString& key)
{
	if(key == mLastSrhKey)
	{
		return mSearhResultModel;
	}

	mLastSrhKey = key;
    mSearhResultModel->setFilterRegExp(key);

	
    return mSearhResultModel;
}

void HelpDataProvider::setHelpContentUrl(const QString& uid, const QString& href)
{
    QUrl url = QUrl::fromLocalFile(uid+BACKSLASH+href);
    mHelpContentRoot = url.toString();
}

void HelpDataProvider::getHelpContentData(QString& content, QString& url)
{
    // url
    if(url.isEmpty())
    {
        url = mHelpContentRoot;
    }
    url.remove(URL_HEADER_LOCAL);

    // uid
    QString uid = url.section(BACKSLASH, 0, -3);
    uid.append(BACKSLASH + url.section(BACKSLASH, -2,-2));

    // href
    QString href = url.section(BACKSLASH, -1,-1);
    int anchorPos = href.indexOf(HASHMARK);
    if(anchorPos != -1)
    {
        href = href.section(HASHMARK, 0, 0);
    }

    QString path(uid);
    path.append(BACKSLASH);
    path.append(CONTENTSZIP);
    HelpUtils::loadHtmlFromZipFile(path , href, content);
}


///////////////////////////////////////////////////////////////////////////////////////

void HelpDataProvider::createHelpCategory()
{
	constructCategory();

	mHelpModel->setSortRole(Qt::DisplayRole);

	constructCategory2((HelpStandardItem*)mHelpModel->invisibleRootItem());

	if(mAppItem)
	{
		mHelpModel->appendRow(mAppItem);
		mAppItem->sortChildren(0, HelpUtils::sortOrder());
		constructCategory2(mAppItem);
	}	

	mKeywordModel->sort(0, HelpUtils::sortOrder());
}

void HelpDataProvider::constructCategory()
{
	QFileInfoList driveList = QDir::drives();
	QDir dir;
	QString lang = HelpUtils::UILocaleFromQtToSymbian();

	QString path(HelpUtils::rootPath());
	path.append(XHTMLPATH);
	path.append(lang);

	QStringList uidList;
	QStringList titleList;
	parseCategoryIndexXml(path, uidList, titleList);

	//scan other root path and construct 3rd party help
	foreach(QFileInfo fi, driveList)
	{
		path.clear();
		path.append(fi.absolutePath());
		path.append(XHTMLPATH);
		path.append(lang);
		dir.setPath(path);
		if(dir.exists())
		{
			if(QString::compare(fi.absolutePath(), HelpUtils::rootPath(), Qt::CaseInsensitive) == 0)
			{
				continue;
			}
			constructAppCategory(path, uidList);
		}
	}

	constructBuiltInCategory(path, uidList, titleList);

	mHelpModel->setSortRole(PriorityRole);
	mHelpModel->sort(0, HelpUtils::sortOrder());
}

void HelpDataProvider::constructBuiltInCategory(const QString& path, const QStringList& uidList, const QStringList& titleList)
{
	if(uidList.count() != titleList.count())
	{
		//ToDo
		return;
	}

	for(int i = 0; i < uidList.count(); i++)
	{
		if(mUpdateUidList.contains(uidList[i]))
		{
			continue;
		}

		QString uid(path);
		uid.append(BACKSLASH);
		uid.append(uidList[i]);

		constructBuiltInCategoryItem(uid, titleList[i]);
	}
}

void HelpDataProvider::constructAppCategory(const QString& path, QStringList& uidList)
{
	QDir dir(path);	
	if(!dir.exists())
	{
		return;
	}

	QStringList uidDirList = dir.entryList();
	QString pathUid;
	foreach(QString uid, uidDirList)
	{
		pathUid.clear();
		pathUid.append(path);
		pathUid.append(BACKSLASH);
		pathUid.append(uid);

		QString titleStr;
		parseAppMetaxml(pathUid, titleStr);
		if(titleStr.isEmpty())
		{
			continue;
		}

		if(uidList.contains(uid) || mUpdateUidList.contains(uid))
		{
			mUpdateUidList.append(uid);
			constructBuiltInCategoryItem(pathUid, titleStr);
			continue;
		}

		HelpStandardItem* item = NULL;
		item = new HelpStandardItem(titleStr);

		if(!item)
		{
			//no memory
			return;
		}

		item->setData(pathUid, UidRole);

		if(!mAppItem)
		{
			mAppItem = new HelpStandardItem(qtTrId(TXT_APPLICATIONS));
			if(!mAppItem)
			{
				//no memory
				return;
			}
			mAppItem->setData(APPPRIORITY, PriorityRole);
		}
		mAppItem->appendRow(item);
	}
}

void HelpDataProvider::constructBuiltInCategoryItem(const QString& uid, const QString& title)
{
	int priority;
	parseBuiltInMetaxml(uid, priority);
	
	HelpStandardItem* item = NULL;
	item = new HelpStandardItem(title);

	if(!item)
	{
		//no memory
		return;
	}
	item->setData(uid, UidRole);
	item->setData(priority, PriorityRole);
	mHelpModel->appendRow(item);
}

void HelpDataProvider::constructCategory2(HelpStandardItem* itemParent)
{
	int count = itemParent->rowCount();
	for(int i =0; i < count; i++)
	{
		HelpStandardItem* item = (HelpStandardItem*)itemParent->child(i);
		constructCategory2Item(item);
	}
}

void HelpDataProvider::constructCategory2Item(HelpStandardItem* itemParent)
{
	QStringList hrefList;
	QStringList titleList;

	QString uid = itemParent->data(UidRole).toString();
	parseCategory2IndexXml(uid, hrefList, titleList);

	if(hrefList.count() == 0 || hrefList.count() != titleList.count())
	{
		return;
	}

	for(int i = 0; i < hrefList.count(); i++)
	{
		HelpStandardItem* item = NULL;
		item = new HelpStandardItem(titleList[i]);
		if(!item)
		{
			//no memory
			return;
		}
		item->setData(hrefList[i], HrefRole);
		item->setData(uid, UidRole);
		itemParent->appendRow(item);
		constructKeywordModel(titleList[i], uid, hrefList[i]);
	}

	itemParent->sortChildren(0, HelpUtils::sortOrder());
}

void HelpDataProvider::constructKeywordModel(const QString& title, const QString& uid, const QString& href)
{
	HelpStandardItem* item = NULL;
	item = new HelpStandardItem(title);
	if(!item)
	{
		//no memory
		return;
	}
	item->setData(uid, UidRole);
	item->setData(href, HrefRole);
	mKeywordModel->appendRow(item);
}

void HelpDataProvider::parseCategoryIndexXml(const QString& path, QStringList& uidList, QStringList& titleList)
{
	QString pathIndex(path);
	pathIndex.append(BACKSLASH);
	pathIndex.append(INDEXXML);

	QFile file(pathIndex);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	//parse index xml to a stringlist, each string include id and navtitle and seperate by "specilchar"
	QXmlQuery query;
	query.bindVariable("inputdoc", &file);

	query.setQuery("doc($inputdoc)/collections/collection/xs:string(@id)");
	if(!query.isValid())
	{
		return;
	}
	if(!query.evaluateTo(&uidList))
	{
		return;
	}

	query.setQuery("doc($inputdoc)/collections/collection/xs:string(@navtitle)");
	if(!query.isValid())
	{
		return;
	}
	if(!query.evaluateTo(&titleList))
	{
		return;
	}

	QStringList featureIdLst;

	query.setQuery("doc($inputdoc)/collections/collection/number(@FeatureId)");
	if(!query.isValid())
	{
		return;
	}
	if(!query.evaluateTo(&featureIdLst))
	{
		return;
	}

	if(featureIdLst.count() != uidList.count())
	{
		return;
	}

	for(int i = featureIdLst.count()  - 1; i >= 0; i--)
	{
		int featureID = featureIdLst.at(i).toInt();
		if(!HelpUtils::suppportFeatureID(featureID))
		{
			uidList.removeAt(i);
			titleList.removeAt(i);
		}
	}
}

void HelpDataProvider::parseCategory2IndexXml(const QString& path, QStringList& hrefList, QStringList& titleList)
{
	QString pathIndex(path);
	pathIndex.append(BACKSLASH);
	pathIndex.append(INDEXXML);

	QFile file(pathIndex);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	//parse index xml to a stringlist, each string include href and navtitle and seperate by "specilchar"
	QXmlQuery query;
	query.bindVariable("inputdoc", &file);

	query.setQuery("doc($inputdoc)/topics/topicref/xs:string(@href)");	
	if(!query.isValid())
	{
		return;
	}
	if(!query.evaluateTo(&hrefList))
	{
		return;
	}	

	query.setQuery("doc($inputdoc)/topics/topicref/xs:string(@navtitle)");
	if(!query.isValid())
	{
		return;
	}
	if(!query.evaluateTo(&titleList))
	{
		return;
	}

	QStringList featureIdLst;

	query.setQuery("doc($inputdoc)/topics/topicref/number(@FeatureId)");
	if(!query.isValid())
	{
		return;
	}
	if(!query.evaluateTo(&featureIdLst))
	{
		return;
	}

	if(featureIdLst.count() != hrefList.count())
	{
		return;
	}

	for(int i = featureIdLst.count()  - 1; i >= 0; i--)
	{
		int featureID = featureIdLst.at(i).toInt();
		if(!HelpUtils::suppportFeatureID(featureID))
		{
			hrefList.removeAt(i);
			titleList.removeAt(i);
		}
	}
}

void HelpDataProvider::parseBuiltInMetaxml(const QString& path, int& priority)
{
	QString pathMetaxml(path);
	pathMetaxml.append(BACKSLASH);
	pathMetaxml.append(METAXML);

	priority = -1;

	QFile file(pathMetaxml);
	if (!file.open(QIODevice::ReadOnly))
	{
		return;
	}

	QXmlQuery query;
	QString str;
	query.bindVariable("inputdoc", &file);

	query.setQuery("doc($inputdoc)/meta/number(priority)");	
	if(query.isValid() && query.evaluateTo(&str))
	{
		priority = str.toInt();
	}
}

void HelpDataProvider::parseAppMetaxml(const QString& path, QString& title)
{
	QString pathMetaxml(path);
	pathMetaxml.append(BACKSLASH);
	pathMetaxml.append(METAXML);
	QFile file(pathMetaxml);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	//parse meta xml, get the title string
	QXmlQuery query;
	query.bindVariable("inputdoc", &file);
	query.setQuery("doc($inputdoc)/meta/string(title)");
	if(!query.isValid())
	{
		return;
	}

	if(!query.evaluateTo(&title))
	{
		return;
	}
}
