/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  Language plugin view class
 *
 */
#ifndef	CPLANGUAGEVIEW_H
#define	CPLANGUAGEVIEW_H

#include <QList>
#include <QStringList>
#include <cpbasesettingview.h>
#include <hbinputlanguage.h>
#include "cplanguagepluginutil.h"
#include "extendedlocaleutil.h"
#include "cplanguageplugin_global.h"

class QStringList;
class QPluginLoader;
class HbDataFormModel;
class HbDataFormModelItem;
class HbInputLanguage;
class QModelIndex;

class CpLanguageView : public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit CpLanguageView(QGraphicsItem *parent = 0);
    virtual ~CpLanguageView();
    
private slots:   
    //handle combobox index changes 
    void onRegionChanged(int index);    
    void onPrimaryInputLanguageChanged(int index);
    void onSecondaryInputLanguageChanged(int index);
    
    //observe input language changed event
    void onPrimaryInputLanguageChanged(const HbInputLanguage &inputLanguage);
    void onSecondaryInputLanguageChanged(const HbInputLanguage &inputLanguage);
    
    //launch regional setting view
 	void launchRegionalSettingsView();    
 	
 	//handle data form page changed event
 	void onDataFormItemActivated(const QModelIndex &modelIndex);
protected:
 	//From CpBaseSettingView
 	virtual void close();
private:
 	void fillDataFormComboBoxItem(HbDataFormModelItem *comboBoxItem,
 	    const QStringList &items, int currentIndex = -1);
    void updateSecondaryInputLanguageItem();
    bool promptRestart();
 	int restartDevice(); 
 	LanguageRegionMapping *languageRegionMapping(const QString &language);
private:
 	QList<LanguageRegionMapping> mLanguageRegionMapping;
 	
 	LanguageRegionMapping mPreviousSetting;
 	LanguageRegionMapping mCurrentSetting;
 	
 	HbDataFormModel *mModel;
 	
 	QList<HbDataFormModelItem*> mLanguagePages;
 	HbDataFormModelItem *mCurrentLanguagePage;
 	
 	//Current items
    HbDataFormModelItem *mCurrentRegionItem;
    HbDataFormModelItem *mCurrentPrimaryInputLanguageItem;
    HbDataFormModelItem *mCurrentSecondaryInputLanguageItem;
    
    QList<HbInputLanguage> mPrimaryInputLanguages; 
    QList<HbInputLanguage> mSecondaryInputLanguages;
    QStringList mSupportedLanguages;
    QStringList mSupportedRegions;
    
	QPluginLoader *mClockPluginLoader;	
	
	bool mChineseVariant;
	
    LANGUAGEPLUGIN_TEST_FRIEND_CLASS(TestLanguagePlugin)
};
#endif //CPLANGUAGEVIEW_H
