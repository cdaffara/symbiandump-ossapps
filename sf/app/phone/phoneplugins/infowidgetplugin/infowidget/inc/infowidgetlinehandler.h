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

#ifndef INFOWIDGETLINEHANDLER_H_
#define INFOWIDGETLINEHANDLER_H_

#include <QObject>

// Class declaration
class InfoWidgetLineHandler: public QObject 
    {
    Q_OBJECT
    
public:    
    InfoWidgetLineHandler(QObject *parent = NULL);
    ~InfoWidgetLineHandler();    

    int activeLine() const; 
    
signals: 
    void handleMessage(int message);
    void handleError(int operation, int errorCode);
    
private: 
    int m_activeLine;
    
    }; 

#endif /* INFOWIDGETLINEHANDLER_H_ */


