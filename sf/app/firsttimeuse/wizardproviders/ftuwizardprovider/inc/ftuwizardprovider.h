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
* Description:  FTU wirzard provider.
*
*/


#ifndef FTU_WIZARDPROVIDER_H
#define FTU_WIZARDPROVIDER_H

#include "ftuwizardprovider_global.h"
#include "ftutest_global.h"
#include <QObject>
#include <QVector>
#include <QDir>

class FtuWizard;
class FtuWizardFactory;

FTU_TEST_CLASS(FtuWizardProviderTest)


/**
 * @ingroup group_ftuwizardprovider
 * @brief Loads and connects FTU plugins
 *
 * This service is responsible for loading FTU plugins.
 *
 * @since S60 ?S60_version.
 */
class FTUWIZARDPROVIDER_EXPORT FtuWizardProvider : public QObject
{

    Q_OBJECT

public:

    /**
     * Default constructor.
     *
     * @since S60 ?S60_version.
     */
    FtuWizardProvider(QObject* =0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
	virtual ~FtuWizardProvider();

    /**
     * Loads all FTU plugins.
     * @param plugins The list will contain all found plugins on return.      
     * @since S60 ?S60_version.
     */
    void loadFTUPlugins(QList<FtuWizardFactory*>& plugins);
    
private:

    
private slots:

private:
    
    QString mWizardManifestDir;
    
    FTU_TEST_FRIEND_CLASS(FtuWizardProviderTest)
};




#endif
