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

#ifndef VIDEOCOLLECTIONUILOADERDEF_H
#define VIDEOCOLLECTIONUILOADERDEF_H

// Constants
static const char* DOCML_VIDEOCOLLECTIONVIEW_FILE         = ":/layout/collectionview.docml";
static const char* DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST = "listsSection";
static const char* DOCML_NAME_VIEW                        = "view";

// Videocollection View
static const char* DOCML_NAME_VC_HEADINGBANNER            = "vc:mBanner";
static const char* DOCML_NAME_VC_COLLECTIONWIDGET         = "vc:mCollectionWidget";
static const char* DOCML_NAME_VC_COLLECTIONCONTENTWIDGET  = "vc:mCollectionContentWidget";
static const char* DOCML_NAME_VC_VIDEOLISTWIDGET          = "vc:mListWidget";

// Videocollection Options Menu
static const char* DOCML_NAME_OPTIONS_MENU                = "vc:mOptionsMenu";
static const char* DOCML_NAME_SORT_MENU                   = "vc:mSortBy";

static const char* DOCML_NAME_SORT_BY_DATE                = "vc:mDate";
static const char* DOCML_NAME_SORT_BY_NAME                = "vc:mName";
static const char* DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS     = "vc:mNumberOfItems";
static const char* DOCML_NAME_SORT_BY_SIZE                = "vc:mSize";

static const char* DOCML_NAME_ADD_TO_COLLECTION           = "vc:mAddtoCollection";
static const char* DOCML_NAME_CREATE_COLLECTION           = "vc:mCreateNewCollection";
static const char* DOCML_NAME_DELETE_MULTIPLE             = "vc:mDeleteMultiple";

// Videocollection no content label
static const char* DOCML_NAME_NO_CONTENT_LABEL             = "vc:mNoContentLabel";

// video multiselection dialog
static const char* DOCML_VIDEOSELECTIONDIALOG_FILE        = ":/layout/videolistselectiondialog.docml";
static const char* DOCML_NAME_DIALOG                      = "mMultiSelectionDialog";
static const char* DOCML_NAME_DLG_HEADINGLBL              = "mHeadingLabel";
static const char* DOCML_NAME_CHECK_CONTAINER             = "mCheckBoxContainer";
static const char* DOCML_NAME_MARKALL                     = "mCheckMarkAll";
static const char* DOCML_NAME_LBL_MARKALL                 = "mCheckBoxText";
static const char* DOCML_NAME_LBL_SELECTION               = "mSelectionCount";
static const char* DOCML_NAME_LIST_CONTAINER              = "mListContainer";

// async loading timeout
static const int ASYNC_FIND_TIMEOUT                       = 50; // ms

/** VideoCollectionUiLoader parameter class */ 
class VideoCollectionUiLoaderParam
{
public:
    enum // Loading phases
    {
        LoadPhasePrimary,
        LoadPhaseSecondary
    };
    
public:
    VideoCollectionUiLoaderParam(const QString& name,
        const char *docml,
        bool isWidget,
        int loadPhase):
        mName(name),
        mIsWidget(isWidget),
        mReceiver(0),
        mDocml(docml),
        mSection(0),
        mMember(0),
        mPhase(loadPhase)
    {
        // members already set
    }
    
    VideoCollectionUiLoaderParam(const QString& name,
        const char *docml,
        const char *section,
        bool isWidget,
        int loadPhase):
        mName(name),
        mIsWidget(isWidget),
        mReceiver(0),
        mDocml(docml),
        mSection(section),
        mMember(0),
        mPhase(loadPhase)
    {
        // members already set
    }
    
    VideoCollectionUiLoaderParam(const QString& name,
        const char *docml,
        const char *section,
        bool isWidget,
        int loadPhase,
        QObject *receiver,
        const char *member):
        mName(name),
        mIsWidget(isWidget),
        mReceiver(receiver),
        mDocml(docml),
        mSection(section),
        mMember(member),
        mPhase(loadPhase)
    {
        // members already set
    }
    
    bool operator==(const VideoCollectionUiLoaderParam &param) const
    {
        return mName == param.mName && mDocml == param.mDocml && mSection == param.mSection;
    }
    
    bool operator==(const QString &name) const
    {
        return mName == name;
    }
    
public:
    QString mName;
    bool mIsWidget;
    QObject *mReceiver;
    const char *mDocml;
    const char *mSection;
    const char *mMember;
    int mPhase;
};

#endif//VIDEOCOLLECTIONUILOADERDEF_H
