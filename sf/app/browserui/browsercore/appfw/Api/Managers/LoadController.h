/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef LoadController_h
#define LoadController_h

#include <QWidget>
#include <QAction>
#include <QNetworkReply>
#include "BWFGlobal.h"


namespace WRT {

    class BWF_EXPORT LoadController : public QObject
    {
        Q_OBJECT
    public:
        enum GotoBrowserMode {
            GotoModeLoading, /**< Loading Mode (i.e. page is loading) */
            GotoModeEditinLoading, /**< Editing while in  Loading Mode (i.e. editing while page is loading) */
            GotoModeEditing, /**< Editing Mode (i.e. user can or is editing the url) */
            GotoModeReloadable, /**< Reloadable Mode (i.e. the url has not changed, and can be reloaded) */
            GotoModeInvalid
        };

       /*!
        * LoadController Constructor
        * @param parent: parent QObject
        */
        LoadController();
        /*!
         * LoadController Destructor
         */
        ~LoadController();


    public:
        GotoBrowserMode mode();
        void setEditMode(bool);
        bool editMode();
        
        

    
        bool loadCanceled() { return m_canceled ;}
        void setCanceled(bool val) {m_canceled = val;}
        
        void setUrlText(QString );
        QString urlText() {return m_textBoxValue; }
        
        int progressValue() {return m_progress; }
        bool inline isPageLoading(){ return m_isPageLoading;}
        bool pointOfNoReturn();

   public slots:
        void loadStarted();
        void loadProgress( const int progress );
        void loadFinished( const bool ok );
        void urlChanged(QUrl);
        void initialLayoutCompleted();
    
 

Q_SIGNALS:
        void pageLoadStarted();
        void pageLoadFailed(); 
        void pageLoadProgress(const int);
        void pageLoadFinished(const bool ok);
        void pageUrlChanged(QString);

    private:
        GotoBrowserMode m_gotoMode;
        bool m_isPageLoading;
        bool m_canceled;
        int  m_progress;
        bool m_initialLayoutIsComplete;
        QString m_textBoxValue;
        QString m_previousTextBoxValue;


    };
}
#endif // __LOADCONTROLLER_H__
