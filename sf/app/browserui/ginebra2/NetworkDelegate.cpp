/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*  This file implements the NetworkDelegate class.
*/
#include <QList>
#include <QString>
#include "NetworkDelegate.h"
#include "Utilities.h"

namespace GVA {

NetworkDelegate::NetworkDelegate(NetworkImpl *networkImpl = NULL)
{
      m_networkImpl = networkImpl;

      if (m_networkImpl)
      {
          // set up handlers for system network info signals
        safe_connect(m_networkImpl, SIGNAL(networkNameChanged(const QString&)),
            this, SIGNAL(networkNameChanged(const QString&)));
        safe_connect(m_networkImpl, SIGNAL(networkSignalStrengthChanged(int)),
            this, SIGNAL(networkSignalStrengthChanged(int)));
      }

    // this will be the name javascript uses to access properties and signals
    // from this class
    setObjectName("networkDelegate");
}

NetworkDelegate::~NetworkDelegate()
{
      if (m_networkImpl)
          delete m_networkImpl;
}

//! Gets the network name for the current network mode.
QString NetworkDelegate::getNetworkName() const
{
      return m_networkImpl->getNetworkName();
}

//! Gets the network signal strength for the current network mode.
int NetworkDelegate::getNetworkSignalStrength() const
{
    return m_networkImpl->getNetworkSignalStrength();
}

} // GVA
