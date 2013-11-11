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
* Description:  Wizard provider manifest parser.
*
*/


#ifndef FTU_WIZARPROVIDER_MANIFESTPARSER_H
#define FTU_WIZARPROVIDER_MANIFESTPARSER_H

#include <QObject>

class QDomElement;
/**
 * @ingroup wizardprovider
 * @brief Loads ftu plugins from an XML manifest file.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuManifestParser : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.
     * @param aParent Parent object.
     */
    FtuManifestParser(QObject* aParent = 0);
    
    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~FtuManifestParser();
    
    /**
     * Parses plugin list from manifest file.
     * @since S60 ?S60_version.
     * @param path The path where manifest file is found.
     * @return Parsed plugin paths.
     */    
    QStringList parsePluginList(const QString& path);

private:

    Q_DISABLE_COPY(FtuManifestParser)
    
    /**
     * Parses the plugin paths from a manifest file.
     * @since S60 ?S60_version.
     * @param fileName Manifest file name.
     * @return Parsed path.
     */
    QString loadFromXml(const QString& fileName);
    
    /**
     * Parses the attribute.
     * @since S60 ?S60_version.
     * @param element The element from which the attribute is parsed.
     * @param attributeName The name of the attribute to be parsed. 
     * @return Parsed attribute.
     */
    QString parseAttribute(QDomElement& element, const QString& attributeName) const;
    
    /**
     * Reads the configured plugins.
     * @since S60 ?S60_version.
     * @return List of configured manifest files.
     */    
    QStringList readManifestFilesFromConfig();

private:
    
};

#endif //FTU_WIZARPROVIDER_MANIFESTPARSER_H

