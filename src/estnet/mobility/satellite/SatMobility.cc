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

#include "SatMobility.h"

#include <memory.h>

#include "common/EulerAngleHelpers.h"

namespace estnet {

Define_Module(SatMobility)

SatMobility::SatMobility() :
        _iAttitudePropagator(nullptr), _iPositionPropagator(nullptr), _doAutoUpdate(
                false), _selfUpdateIV_s(0.0) {
    this->_jdGlobal = GlobalJulianDate::getInstancePtr();

}

void SatMobility::initialize(int stage) {
    // get parameters

    _doAutoUpdate = par("enableSelfTrigger");
    _selfUpdateIV_s = par("selfTriggerTimeIv");
    extUpdtSignalNamePrefix = par("extUpdtSignalNamePrefix").stringValue();

    if (stage == inet::INITSTAGE_LOCAL) {

        // get attitude and position propagators

        // get the attitude propagator
        _iAttitudePropagator = nullptr;
        // enumerate over all submodules and try to find the first suitable attitude
        // module
        for (omnetpp::cModule::SubmoduleIterator iter(this); !iter.end();
                ++iter) {
            omnetpp::cModule *subM = *iter;
            try {
                _iAttitudePropagator = dynamic_cast<IAttitudePropagator*>(subM);
            } catch (const omnetpp::cRuntimeError &e) {
                std::cout << subM->getFullName() << " : "
                        << e.getFormattedMessage() << std::endl;
                _iAttitudePropagator = nullptr;
            }
            if (_iAttitudePropagator != nullptr) {
                // We have found a valid attitude propagator
                break;
            }
        }
        //_iAttitudePropagator =
        // getModuleFromPar<IMobility>(par("attitudePropagatorModule"),
        // getContainingNode(this));
        // throw error, if no valid attitude module was found!
        if (_iAttitudePropagator == nullptr) {
            throw omnetpp::cRuntimeError(
                    "No suitable attitude propagator module found for "
                            "SatMobility Module %s !\n",
                    this->getFullPath().c_str());
        }

        // get the position propagator
        _iPositionPropagator = nullptr;
        // enumerate over all submodules and try to find the first suitable position
        // module
        for (omnetpp::cModule::SubmoduleIterator iter(this); !iter.end();
                ++iter) {
            omnetpp::cModule *subM = *iter;
            try {
                _iPositionPropagator = dynamic_cast<IPositionPropagator*>(subM);
            } catch (const omnetpp::cRuntimeError &e) {
                _iPositionPropagator = nullptr;
            }
            if (_iPositionPropagator != nullptr) {
                // We have found a valid position propagator
                break;
            }
        }
        // throw error, if no valid position module was found!
        if (_iPositionPropagator == nullptr) {
            throw omnetpp::cRuntimeError(
                    "No suitable position propagator module "
                            "found for SatMobility Module %s !\n",
                    this->getFullPath().c_str());
        }
    }
    IPropagatorBase *pBase = nullptr;
    pBase = dynamic_cast<IPropagatorBase*>(_iAttitudePropagator);
    if (pBase != nullptr) {
        pBase->initializePropagator();
    }
    pBase = dynamic_cast<IPropagatorBase*>(_iPositionPropagator);
    if (pBase != nullptr) {
        pBase->initializePropagator();
    }

    std::stringstream ss(this->par("initialRotationSpeed").stdstringValue());
    std::string segment;
    std::vector<std::string> seglist;
    while (std::getline(ss, segment, ',')) {
        seglist.push_back(segment);
    }
    if (seglist.size() == 3) {
        _iAttitudePropagator->setStateFromQuaternion(
                getCurrentAngularPosition(),
                cQuaternion(0,
                        inet::Coord(std::stod(seglist[0]),
                                std::stod(seglist[1]), std::stod(seglist[2]))),
                getCurrentAngularAcceleration(), _jdGlobal->currentSimTime());
    } else {
        throw omnetpp::cRuntimeError(
                "The initialRoationSpeed parameter should have the format \"x.x,y.y,z.z\".");
    }
}

int SatMobility::getOrbitalPeriod() {
    int orbitalPeriod = 0;
    IPositionPropagator *pBase =
            dynamic_cast<IPositionPropagator*>(_iPositionPropagator);
    if (pBase != nullptr) {
        orbitalPeriod = pBase->getOrbitalPeriod();
    }
    return orbitalPeriod;
}

double SatMobility::getOrbitalRadius() const {
    int orbitalRadius = 0;
    IPositionPropagator *pBase =
            dynamic_cast<IPositionPropagator*>(_iPositionPropagator);
    if (pBase != nullptr) {
        orbitalRadius = pBase->getOrbitalRadius(_jdGlobal->currentSimTime());
    }
    return orbitalRadius;
}

void SatMobility::externalPositionUpdate(
        tPropStatePosition_Ptr const &newPosition) {
    IPropagatorBase *pBase =
            dynamic_cast<IPropagatorBase*>(_iPositionPropagator);
    if (pBase != nullptr) {
        pBase->setState(std::static_pointer_cast<PropState>(newPosition));
    }
}

void SatMobility::externalPositionUpdateECI(cEci const &newPosition,
        const cJulian &positionTimeStamp) {
    _iPositionPropagator->setStateFromECI(newPosition, positionTimeStamp);
}

void SatMobility::externalAttitudeUpdate(
        tPropStateAttitude_Ptr const &newAttitude) {
    IPropagatorBase *pBase =
            dynamic_cast<IPropagatorBase*>(_iAttitudePropagator);
    if (pBase != nullptr) {
        pBase->setState(std::static_pointer_cast<PropState>(newAttitude));
    }
}

void SatMobility::externalAttitudeUpdateEuler(cEulerAngles const &newAttitude,
cEulerAngles const &newAngularVel, cEulerAngles const &newAngularAcc,
        const cJulian &attitudeTimeStamp) {
    _iAttitudePropagator->setStateFromEuler(newAttitude, newAngularVel,
            newAngularAcc, attitudeTimeStamp);
}

void SatMobility::externalAttitudeUpdateQuaternion(
cQuaternion const &newAttitude,
cQuaternion const &newAngularVel, cQuaternion const &newAngularAcc,
        const cJulian &attitudeTimeStamp) {
    _iAttitudePropagator->setStateFromQuaternion(newAttitude, newAngularVel,
            newAngularAcc, attitudeTimeStamp);
}

double SatMobility::getMaxSpeed() const {
    // TODO: Max speed calculation...
    return 0.0;
}

inet::Coord SatMobility::getCurrentPositionWithoutSignal() {
    cEci acEci;
    _iPositionPropagator->getECIAtTime(_jdGlobal->currentSimTime(), acEci);
    // we're not emitting a position changed signal here
    // as this method is there to be called by our
    // subscribers without causing am infinite signal
    // recursion

    return acEci.getPos();
}

inet::Coord SatMobility::getCurrentPosition() {

    inet::Coord rtn = this->getCurrentPositionWithoutSignal();
    // position (potentially) changed, we're emitting
    // a signal to let our listeners know
    emit(mobilityStateChangedSignal, this);
    return rtn;
}

inet::Coord SatMobility::getPositionAtTime(double time) {
    cEci acEci;
    _iPositionPropagator->getECIAtTime(_jdGlobal->simTime2JulianDate(time),
            acEci);
    // we shouldn't emit a position change signal here
    // because OsgNode's drawOrbit calls this with future times!
    return acEci.getPos();
}

inet::Coord SatMobility::getCurrentVelocity() {
    cEci acEci;
    _iPositionPropagator->getECIAtTime(_jdGlobal->currentSimTime(), acEci);
    return acEci.getVel();
}

inet::Coord SatMobility::getVelocityAtTime(double time) {
    cEci acEci;
    _iPositionPropagator->getECIAtTime(_jdGlobal->simTime2JulianDate(time),
            acEci);
    // we shouldn't emit a position change signal here
    // because OsgNode's drawOrbit calls this with future times!
    return acEci.getVel();
}

inet::Coord SatMobility::getCurrentAcceleration() {
    // TODO
    return inet::Coord();
}

inet::Quaternion SatMobility::getCurrentAngularPosition() {
    int64_t time = getSimulation()->getSimTime().inUnit(omnetpp::SIMTIME_S);

    // check if we got it cached
    std::map<int64_t, cQuaternion>::const_iterator anglesIt =
            this->_cachedAngularPositions.find(time);
    if (anglesIt != this->_cachedAngularPositions.end()) {
        return anglesIt->second;
    }

    // grab the latest attitude from the propagator
    cQuaternion quaternion;
    _iAttitudePropagator->getQuaternionAttitudeAtTime(
            _jdGlobal->currentSimTime(), quaternion);

    this->_cachedAngularPositions.emplace(time, quaternion);
    return quaternion;
}

inet::Quaternion SatMobility::getAngularPositionAtTime(double time) {
    cQuaternion quaternion;
    _iAttitudePropagator->getQuaternionAttitudeAtTime(
            _jdGlobal->simTime2JulianDate(time), quaternion);
    return quaternion;
}

inet::Quaternion SatMobility::getCurrentAngularVelocity() {
    cQuaternion quaternion;
    _iAttitudePropagator->getQuaternionAngularVelAtTime(
            _jdGlobal->currentSimTime(), quaternion);
    return quaternion;
}

inet::Quaternion SatMobility::getAngularVelocityAtTime(double time) {
    cQuaternion quaternion;
    _iAttitudePropagator->getQuaternionAngularVelAtTime(
            _jdGlobal->simTime2JulianDate(time), quaternion);
    return quaternion;
}

inet::Quaternion SatMobility::getCurrentAngularAcceleration() {
    cQuaternion quaternion;
    _iAttitudePropagator->getQuaternionAngularAccAtTime(
            _jdGlobal->currentSimTime(), quaternion);
    return quaternion;
}

inet::Coord SatMobility::getConstraintAreaMax() const {
    // TODO: return true constraint area max
    return inet::Coord();
}

inet::Coord SatMobility::getConstraintAreaMin() const {
    // TODO: return true constraint area min
    return inet::Coord();
}

}  // namespace estnet
