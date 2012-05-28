/*    Copyright (c) 2010-2012, Delft University of Technology
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without modification, are
 *    permitted provided that the following conditions are met:
 *      - Redistributions of source code must retain the above copyright notice, this list of
 *        conditions and the following disclaimer.
 *      - Redistributions in binary form must reproduce the above copyright notice, this list of
 *        conditions and the following disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *      - Neither the name of the Delft University of Technology nor the names of its contributors
 *        may be used to endorse or promote products derived from this software without specific
 *        prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 *    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *    OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *    Changelog
 *      YYMMDD    Author            Comment
 *      110617    F.M. Engelen      Creation of code.
 *      110822    D. Dirkx          Removed pointer to double member, minor changes.
 *      120209    D. Dirkx          Changed force models to free functions, moved implementation
 *                                  to source file.
 *      120324    K. Kumar          Added astrodynamics namespace layer; updated function input
 *                                  arguments.
 */

#include "Tudat/Astrodynamics/Aerodynamics/aerodynamicForce.h"

namespace tudat
{
namespace astrodynamics
{
namespace force_models
{

//! Compute the aerodynamic force in same reference frame as input coefficients.
Eigen::VectorXd computeAerodynamicForce( const double dynamicPressure,
                                         const double referenceArea,
                                         const Eigen::Vector3d& aerodynamicCoefficients )
{
    return dynamicPressure * referenceArea * aerodynamicCoefficients;
}

//! Compute the aerodynamic force in same reference frame as input coefficients.
Eigen::MatrixXd computeAerodynamicForce(
        const double dynamicPressure, AerodynamicCoefficientInterface& coefficientInterface )
{
    return computeAerodynamicForce( dynamicPressure,
                                    coefficientInterface.getReferenceArea( ),
                                    coefficientInterface.getCurrentForceCoefficients( ) );
}

} // namespace force_models
} // namespace astrodynamics
} // namespace tudat
