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
#ifndef CPPHONELOCALISATION_H_
#define CPPHONELOCALISATION_H_

#include <QObject>
#include <QList>
#include "cptelephonyutilsdefs.h"

// Forward declarations 
class QTranslator; 

class TELEPHONYUTILS_EXPORT PhoneLocalisation : public QObject
    {
    Q_OBJECT
    
public: 
    
    enum TranslationFileId {
        TranslationFileCommon, 
        TranslationFileTelephoneCp
    }; 
        
public:
    
    PhoneLocalisation(QObject *parent = NULL);
    ~PhoneLocalisation();

    bool installTranslator(TranslationFileId translationFileId);
    void removeTranslators();

private:     
    
    QList<QTranslator *> m_translators;
    
    };


#endif /* CPPHONELOCALISATION_H_ */
