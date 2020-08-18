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

#ifndef __ESTNET_TARGETTRACKING_H_
#define __ESTNET_TARGETTRACKING_H_

#include "estnet/node/tracking/contract/INodeTracking.h"
#include "estnet/attitudecontroller/AttitudeTarget.h"

namespace estnet {

/**
 * TODO - Generated class
 */
class ESTNET_API TargetTracking: public INodeTracking {
public:
    inet::Quaternion getNewOrientation();

    void setTarget(AttitudeTarget target);

protected:
    /** @brief initialization */
    virtual void initialize(int stage) override;
    /** @brief number of init stages */
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }
    AttitudeTarget _target;
    static omnetpp::simsignal_t targetStats;

};

} //namespace

#endif
