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


#include "LoadController.h"
#include "webpagecontroller.h"
#include "wrtbrowsercontainer.h"
#include <QDebug>

#define ENOUGHCONTENT 60

namespace WRT {

LoadController::LoadController ( ) :
    m_gotoMode(GotoModeEditing), 
    m_isPageLoading(false), 
    m_canceled(false),
    m_progress(0),
  m_initialLayoutIsComplete(false)
{

}

LoadController::~LoadController()
{

}

void LoadController::setEditMode(bool editing) 
{
   
    //qDebug() << " LoadController::setEditMode" << editing << m_gotoMode;
    if (editing) {
        if (m_gotoMode ==  GotoModeLoading ) {
            m_gotoMode = GotoModeEditinLoading;
        }
        else if ( m_gotoMode == GotoModeReloadable ){
            m_gotoMode = GotoModeEditing;
        }
       
    }
    else {
        if (m_gotoMode ==  GotoModeEditinLoading ) {
            m_gotoMode = GotoModeLoading;
        }
        else if (m_gotoMode ==  GotoModeEditing){
            m_gotoMode = GotoModeReloadable;
        }

    }
    qDebug() << "Leave  LoadController::setEditMode  " << m_gotoMode;
}


bool LoadController::editMode() 
{
    bool editing=false;
    if (m_gotoMode == GotoModeEditinLoading || m_gotoMode == GotoModeEditing) {
        editing = true;
    }
    return editing;
}

LoadController::GotoBrowserMode LoadController::mode()
{
    return m_gotoMode;
}

void LoadController::loadStarted()
{
    m_progress = 0;
    m_canceled = 0;
    m_isPageLoading = true;
  m_initialLayoutIsComplete = false;

    m_gotoMode = GotoModeLoading;

    //qDebug() << __PRETTY_FUNCTION__;
    emit pageLoadStarted();
}

void LoadController::loadFinished(bool ok)
{
    //qDebug() << __PRETTY_FUNCTION__  << ok << m_gotoMode ;
    m_isPageLoading = false;

    m_progress = 100;
   
    if (m_gotoMode ==  GotoModeEditinLoading ) {
        m_gotoMode = GotoModeEditing;
    }
    else {
        m_gotoMode = GotoModeReloadable;

    }

    // FIXME it is a temp fix for the url change issued with cached pages
    if (ok) {
        WebPageController * pageController = WebPageController::getSingleton();
        if (pageController->currentPage()->loadController() == this)
            m_previousTextBoxValue = m_textBoxValue;
            m_textBoxValue = pageController->currentDocUrl();
    }
// TODO: Change to editing mode if load failed
/*
    // if page succeed, set the text and goto reloading mode, else load mode
    if(ok)
    {
        m_gotoMode = GotoModeReloadable;

    }
    else
    {
        m_gotoMode = GotoModeEditing;
    }
*/
    emit pageLoadFinished(ok);
    if (!ok)
        emit pageLoadFailed();
    //qDebug() << __PRETTY_FUNCTION__  << m_gotoMode ;
}

void LoadController::loadProgress(int progress)
{
    m_progress = progress;

    //qDebug() << __PRETTY_FUNCTION__  << progress ;
    emit pageLoadProgress(progress);
}

void LoadController::urlChanged(QUrl url) 
{
//    qDebug() << __PRETTY_FUNCTION__  << url;
    // Save the url 
    m_textBoxValue = url.toString();
    emit pageUrlChanged(m_textBoxValue);
}
  
void LoadController::setUrlText(QString str)
{
//    qDebug() << __PRETTY_FUNCTION__  << str;
    m_textBoxValue = str; 
}

void LoadController:: initialLayoutCompleted()
{
   m_initialLayoutIsComplete = true;
}

bool LoadController:: pointOfNoReturn()
{
   bool noreturn(false);
   if( m_initialLayoutIsComplete ) {
      noreturn = true; 
   }
   else {
   		m_textBoxValue	= m_previousTextBoxValue;
   }
   return noreturn;
}

} // namespace WRT
