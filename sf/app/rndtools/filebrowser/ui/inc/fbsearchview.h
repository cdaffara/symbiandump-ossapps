/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FBSEARCHVIEW_H
#define FBSEARCHVIEW_H

#include <HbView>

#include <QDateTime>

class QStringList;

class EngineWrapper;
class HbDataForm;
class HbDataFormModelItem;
class HbProgressDialog;

/**
  * Settings class that is used for 
  */
class SearchAttributes
{
public:
    QString   mSearchDir;
    QString   mWildCards;
    QString   mTextInFile;
    int       mMinSize;
    int       mMaxSize;
    QDate     mMinDate;
    QDate     mMaxDate;
    bool      mRecurse;
    bool      mDefaultWildCard;
};

/**
 * search results class.
 */
class SearchResults
{
public:
    int mNumberOfFoundFiles;
    QStringList *mFoundFilesList;
};

class FbSearchView : public HbView
{
    Q_OBJECT

public:
    explicit FbSearchView(EngineWrapper &engineWrapper);
    ~FbSearchView();
    void open(const QString &path);

signals:
    void finished(bool ok);

private slots:
    void accept();
    void reject();

private:
    void initDataForm();
    void createToolbar();
    void loadAttributes();
    void readFormItems();

    void startFileSearch();
    void fileSearchResults();

private:
    /* EngineWrapper */
    EngineWrapper &mEngineWrapper;  

    /* Search attributes */
    SearchAttributes mAttributes;    
    /* Search results */
    SearchResults mResults;

    HbDataForm *mForm;
    HbDataFormModelItem *mSearchFileNameItem;
    HbDataFormModelItem *mWildCardItem;
    HbDataFormModelItem *mHasString;
    HbDataFormModelItem *mRecurse;
    HbDataFormModelItem *mMinSize;
    HbDataFormModelItem *mMaxSize;
    HbDataFormModelItem *mMinDate;
    HbDataFormModelItem *mMaxDate;
    QString mPath;
    HbProgressDialog *mProgressDialog;
};

#endif // FBSEARCHVIEW_H
