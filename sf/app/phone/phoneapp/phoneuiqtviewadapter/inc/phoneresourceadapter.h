/*!
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
* Description:  Converts Symbian resources PhoneAction objects.
*
*/

#ifndef PHONERESOURCEADAPTER_H
#define PHONERESOURCEADAPTER_H

#include <QObject>
#include <QMap>
#include "phoneaction.h"

#ifdef BUILD_PHONEUIQTVIEWADAPTER
#define PHONEURESOURCEADAPTER_EXPORT Q_DECL_EXPORT
#else
#define PHONEURESOURCEADAPTER_EXPORT Q_DECL_IMPORT
#endif

class PhoneUIQtButtonsController;
class HbAction;

class PHONEURESOURCEADAPTER_EXPORT PhoneResourceAdapter : public QObject
{
    Q_OBJECT
    
public:

    /*!
        \fn Instance (QObject *parent)
        
        This method is a Singleton Design Pattern's way to get
        instance of this class.
    */
    static PhoneResourceAdapter* Instance (QObject *parent = 0);
        
    /*!
        Destructor
     */
    virtual ~PhoneResourceAdapter ();
    
    /*!
        \fn PhoneUIQtButtonsController* buttonsController ()
        
        This method returns the pointer to buttons controller object
        
        Returns pointer to buttons controller.
    */
    PhoneUIQtButtonsController* buttonsController ();
    
    /*!
        \fn QMap<PhoneAction::ActionType, PhoneAction *> PhoneResourceAdapter::convert (int symbianResourceId)
        
        This method converts given Symbian resource id to mapping of translated
        texts, actions etc.
        
        Returns map of actions.
    */
    QMap<PhoneAction::ActionType, PhoneAction *> convert (int symbianResourceId, ...);
 
    /*!
        \fn QString PhoneResourceAdapter::convert (int symbianResourceId)
        
        This method converts given Symbian resource id to QString.
        
        Returns QString.
    */
    QString convertToString(int symbianResourceId, 
            const QString &causeCode = QString("")) const;

    /*!
        \fn QString PhoneResourceAdapter::convert (int symbianResourceId, QString text )
        
        Converts given Symbian resource id to QString and formats given text to
        resource converted text.
        
        Returns QString.
    */   
    QString convertToStringWithParam(int symbianResourceId, const QString &text) const;
    
    /*!
        \fn int PhoneResourceAdapter::defaultToolbarResourceId ()
        
        Returns default toolbar resource id.
    */    
    int defaultToolbarResourceId() const;
    
    /*!
        \fn QString PhoneResourceAdapter::convertCommandToString()
        
        Converts given Symbian command id to QString.
        
        Returns QString.
    */ 
    QString convertCommandToString(int symbianCommandId) const;

    /*!
        \fn QString PhoneResourceAdapter::convertToHbActions()
        
        Converts given Symbian command id to HbAction list.
        
        Returns HbAction list.
    */ 
    QList<HbAction*> convertToHbActions(int symbianResourceId) const;

    /*!
        \fn QString PhoneResourceAdapter::convertToToolBarCommandList()
        
        Converts given Symbian command id to tool bar command list.
        
        Returns command list.
    */
    QList<PhoneAction::ToolBarItem> convertToToolBarCommandList(
            int symbianResourceId ) const;
    
protected:    

    /*!
        Constructor
     */
    PhoneResourceAdapter (QObject *parent = 0);

private:

    PhoneUIQtButtonsController* m_buttonsCtrl; // Own
    
};

#endif // PHONERESOURCEADAPTER_H
