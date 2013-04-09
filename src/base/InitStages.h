//
// Copyright (C) 2013 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#ifndef __INET_INITSTAGES_H
#define __INET_INITSTAGES_H


//TODO a MACAddress generalas es a registerInterface egy stage-ben kell legyen
// lasd Ieee80211Mac modulban a multimac parametert

enum InetInitStages
{
    STAGE_LOCAL = 0,            // for changes that don't depend on other modules
    STAGE_PHYSICAL_LAYER,       // generate MACAddresses
    STAGE_LINK_LAYER,           // register interfaces
    STAGE_NETWORK_LAYER,
    STAGE_TRANSPORT_LAYER,
    STAGE_ROUTING,
    STAGE_APPLICATION_LAYER,
    STAGE_LAST,

    NUM_STAGES
};


#endif  // __INET_INITSTAGES_H

