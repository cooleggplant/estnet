//
// Copyright (C) 2020 Computer Science VII: Robotics and Telematics - 
// Julius-Maximilians-Universitaet Wuerzburg
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef EST_ENERGYMODEL_SIMPLESOLARPANELBASE_H_
#define EST_ENERGYMODEL_SIMPLESOLARPANELBASE_H_

#include <omnetpp.h>

#include "estnet/power/contract/ISolarPanel.h"

namespace estnet {

/**
 *  Describes power production on satellite
 *  based on the solarpanel's max. output power,
 *  the system efficiencies and the sun's incidence angle.
 *
 * The orientation is modeled by the AntennaMobilty, that
 * models a fixed orientation and position offset to the
 * added to the satellite's * mobility
 */
class ESTNET_API SimpleSolarPanelBase: public ISolarPanel {
private:
    W _maxOutputPower; // max. power that the solar panel can produce

protected:
    /** @brief updates the generation and send signal to battery */
    virtual void updatePowerGeneration() override;

    /** Calculate the power generation at the moment.
     *  Therefore, a model that calculates the part of the sun-solarpanel-
     *  vector, that is pointing orthogonal to the panel
     *  Based on this, the power generation is calculated using the sun's
     *  incidence angle a, the efficiencies n and the expected maximum power
     *  generated by the solarpanel Pmax:
     *    P = n * Pmax * cos(a)
     *
     *  @return W: current power generation of solar panel in Watt */
    virtual W calculatePowerGeneration() override;

    /** @brief initialization of the module, called by omnet
     *  @param  stage: stage of initialization */
    virtual void initialize(int stage) override;

};

}  // namespace estnet

#endif /* ENERGYMODEL_SIMPLESOLARPANELBASE_H_ */
