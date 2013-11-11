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

#ifndef FBFOLDERSELECTORWRAPPER_H_
#define FBFOLDERSELECTORWRAPPER_H_

#include "fbdriveentry.h"
#include "fbfileentry.h"
#include "FBFolderSelector.h"

#include <QString>
#include <QStringList>
#include <QModelIndexList>

class CEngine;
class SearchAttributes;
class SearchResults;
class FileBrowserView;
class HbProgressDialog;

class QModelIndex;
class CFileBrowserFolderSelector;
class EngineWrapper;


/**
 * class that is used for communicating between Symbian and Qt code.
 */
class FbFolderSelectorWrapper : public QObject, public MFolderSelectorUI
{
    Q_OBJECT
public:
    FbFolderSelectorWrapper();
    ~FbFolderSelectorWrapper();
    
    bool init();

    QString currentPath() const;
    bool isDriveListViewActive() const;
    int itemCount() const;
    FbDriveEntry getDriveEntry(const int aIndex) const;
    FbFileEntry getFileEntry(const int aIndex) const;

    void moveDownToDirectory(int);
    void moveUpOneLevel();

signals:
    void FolderSelectionChanged();

public:  // From MFolderSelectorUI
    virtual void InformFolderSelectionChanged();

private:
    CFileBrowserFolderSelector *mDestinationFolderSelector;
};

#endif // FBFOLDERSELECTORWRAPPER_H_
