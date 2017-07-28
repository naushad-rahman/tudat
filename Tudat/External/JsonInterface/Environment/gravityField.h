/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_JSONINTERFACE_GRAVITYFIELD_H
#define TUDAT_JSONINTERFACE_GRAVITYFIELD_H

#include <Tudat/SimulationSetup/EnvironmentSetup/createGravityField.h>

#include "Tudat/External/JsonInterface/Support/valueAccess.h"
#include "Tudat/External/JsonInterface/Support/valueConversions.h"

namespace tudat
{

namespace simulation_setup
{

//! Map of `GravityFieldType`s supported by `json_interface`.
static std::map< std::string, GravityFieldType > gravityFieldTypes =
{
    { "central",           central },
    { "centralSpice",      central_spice },
    { "sphericalHarmonic", spherical_harmonic }
};

//! Convert `GravityFieldType` to `json`.
void to_json( json& jsonObject, const GravityFieldType& gravityFieldType );

//! Convert `json` to `GravityFieldType`.
void from_json( const json& jsonObject, GravityFieldType& gravityFieldType );

//! Create a `json` object from a shared pointer to a `GravityFieldSettings` object.
void to_json( json& jsonObject, const boost::shared_ptr< GravityFieldSettings >& gravityFieldSettings );

//! Create a shared pointer to a `GravityFieldSettings` object from a `json` object.
void from_json( const json& jsonObject, boost::shared_ptr< GravityFieldSettings >& gravityFieldSettings );

} // namespace simulation_setup

} // namespace tudat

#endif // TUDAT_JSONINTERFACE_GRAVITYFIELD_H
