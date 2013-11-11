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
#ifndef LCFSEARCHRESULTSVIEW_H
#define LCFSEARCHRESULTSVIEW_H

#include <hbview.h>
#include <hbdocumentloader.h>
#include "logslogger.h"

class LcfModel;
class HbListView;
class HbTextEdit;
class HbAction;
class LcfViewLoader;
class LogsCntFinder;

const char lcfviewname[] = "logs_dialer_view";


/**
 * 
 */
class LcfSearchResultsView : public HbView
{
    Q_OBJECT
    
public:
    explicit LcfSearchResultsView();
    ~LcfSearchResultsView();
    void init( LcfViewLoader& loader );
    
public slots:
    
    void callKeyPressed();
    
private slots:

    void closeView();
    void queryReady();
    
private:
    
    void initListWidget();
    void clearSoftKey();
        
private:
    
    HbListView* mListView;//not owned
    QString mInput;
    LcfModel*  mModel;   //owned
    LogsCntFinder* mFinder;
    HbAction* mSoftKeyBackAction;
    
};


class LcfViewLoader : public HbDocumentLoader
{
public:
    
    LcfViewLoader() {}
    inline QObject *createObject(const QString& type, const QString &name)
    {
        LOGS_QDEBUG( "logs [LCFPROTO] LcfViewLoader::createObject <->" )
        LOGS_QDEBUG_2( "logs [LCFPROTO] ", name )
        
        if ( name == lcfviewname ) {
            return new LcfSearchResultsView();
        } else {
            return HbDocumentLoader::createObject( type, name );
        }
    }
};


#endif // LCFSEARCHRESULTSVIEW_H
