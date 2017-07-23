#define BOOST_TEST_MAIN

#include <string>

#include <boost/make_shared.hpp>
#include <boost/test/unit_test.hpp>

#include "Tudat/Mathematics/BasicMathematics/linearAlgebra.h"
#include "Tudat/Astrodynamics/BasicAstrodynamics/physicalConstants.h"
#include "Tudat/Astrodynamics/BasicAstrodynamics/orbitalElementConversions.h"

#include "Tudat/External/SpiceInterface/spiceInterface.h"
#include "Tudat/Mathematics/NumericalIntegrators/rungeKuttaCoefficients.h"
#include "Tudat/Astrodynamics/BasicAstrodynamics/accelerationModel.h"
#include "Tudat/Astrodynamics/BasicAstrodynamics/keplerPropagator.h"
#include "Tudat/SimulationSetup/tudatSimulationHeader.h"
#include "Tudat/SimulationSetup/PropagationSetup/dynamicsSimulator.h"

namespace tudat
{

namespace unit_tests
{

////Using declarations.

using namespace tudat;
using namespace interpolators;
using namespace numerical_integrators;
using namespace spice_interface;
using namespace ephemerides;
using namespace simulation_setup;
using namespace basic_astrodynamics;
using namespace orbital_element_conversions;
using namespace tudat::propagators;

BOOST_AUTO_TEST_SUITE( test_multi_arc_dynamics )

BOOST_AUTO_TEST_CASE( testKeplerMultiArcDynamics )
{
    //Load spice kernels.
    std::string kernelsPath = input_output::getSpiceKernelPath( );
    spice_interface::loadSpiceKernelInTudat( kernelsPath + "pck00009.tpc");
    spice_interface::loadSpiceKernelInTudat( kernelsPath + "de-403-masses.tpc");
    spice_interface::loadSpiceKernelInTudat( kernelsPath + "de421.bsp");
    spice_interface::loadSpiceKernelInTudat( kernelsPath + "naif0009.tls");


    for( unsigned testCase = 0; testCase < 2; testCase++ )
    {
        std::vector< std::string > bodyNames;
        bodyNames.push_back( "Earth" );
        bodyNames.push_back( "Moon" );

        // Specify initial time
        double initialEphemerisTime = 1.0E7;
        double finalEphemerisTime = 2.0E7;
        double maximumTimeStep = 3600.0;
        double buffer = 5.0 * maximumTimeStep;

        // Create bodies needed in simulation
        std::map< std::string, boost::shared_ptr< BodySettings > > bodySettings =
                getDefaultBodySettings( bodyNames, initialEphemerisTime - buffer, finalEphemerisTime + buffer );
        boost::dynamic_pointer_cast< InterpolatedSpiceEphemerisSettings >( bodySettings[ "Moon" ]->ephemerisSettings )->
                resetFrameOrigin( "Earth" );
        bodySettings[ "Moon" ]->ephemerisSettings->resetMakeMultiArcEphemeris( true );
        bodySettings[ "Earth" ]->ephemerisSettings = boost::make_shared< ConstantEphemerisSettings >(
                    Eigen::Vector6d::Zero( ) );

        NamedBodyMap bodyMap = createBodies( bodySettings );

        // Set accelerations between bodies that are to be taken into account.
        SelectedAccelerationMap accelerationMap;
        std::map< std::string, std::vector< boost::shared_ptr< AccelerationSettings > > > accelerationsOfMoon;
        accelerationsOfMoon[ "Earth" ].push_back( boost::make_shared< AccelerationSettings >( central_gravity ) );
        accelerationMap[ "Moon" ] = accelerationsOfMoon;

        std::vector< std::string > bodiesToIntegrate, centralBodies;
        bodiesToIntegrate.push_back( "Moon" );
        centralBodies.push_back( "SSB" );

        std::vector< double > integrationArcStarts, integrationArcEnds;

        double integrationStartTime = initialEphemerisTime + 1.0E4;
        double integrationEndTime = finalEphemerisTime - 1.0E4;
        double arcDuration = 1.0E6;
        double arcOverlap = 1.0E4;

        double currentStartTime = integrationStartTime;
        double currentEndTime = integrationStartTime + arcDuration;

        do
        {
            integrationArcStarts.push_back( currentStartTime );
            integrationArcEnds.push_back( currentEndTime );

            currentStartTime = currentEndTime - arcOverlap;
            currentEndTime = currentStartTime + arcDuration;
        }
        while( currentEndTime < integrationEndTime );

        unsigned int numberOfIntegrationArcs = integrationArcStarts.size( );

        std::vector< Eigen::VectorXd > systemInitialStates;
        std::vector< Eigen::Vector6d > initialKeplerElements;

        systemInitialStates.resize( numberOfIntegrationArcs );
        initialKeplerElements.resize( numberOfIntegrationArcs );

        double earthGravitationalParameter =  bodyMap.at( "Earth" )->getGravityFieldModel( )->getGravitationalParameter( );
        for(  unsigned int j = 0; j < numberOfIntegrationArcs; j++ )
        {
            systemInitialStates[ j ]  = spice_interface::getBodyCartesianStateAtEpoch(
                        bodiesToIntegrate[ 0 ], "Earth", "ECLIPJ2000", "NONE", integrationArcStarts[ j ] );
            initialKeplerElements[ j ] = (
                        orbital_element_conversions::convertCartesianToKeplerianElements(
                            Eigen::Vector6d( systemInitialStates[ j ] ),
                            earthGravitationalParameter ) );
        }

        AccelerationMap accelerationModelMap = createAccelerationModelsMap(
                    bodyMap, accelerationMap, bodiesToIntegrate, centralBodies );

        std::vector< boost::shared_ptr< SingleArcPropagatorSettings< double > > > arcPropagationSettingsList;
        for( unsigned int i = 0; i < numberOfIntegrationArcs; i++ )
        {
            arcPropagationSettingsList.push_back(
                        boost::make_shared< TranslationalStatePropagatorSettings< double > >
                        ( centralBodies, accelerationModelMap, bodiesToIntegrate,
                          systemInitialStates.at( i ), integrationArcEnds.at( i ) ) );
        }

        if( testCase == 0 )
        {
            boost::shared_ptr< IntegratorSettings< > > integratorSettings =
                    boost::make_shared< IntegratorSettings< > >
                    ( rungeKutta4, initialEphemerisTime, 120.0 );
            MultiArcDynamicsSimulator< > dynamicsSimulator(
                        bodyMap, integratorSettings, boost::make_shared< MultiArcPropagatorSettings< double > >(
                            arcPropagationSettingsList ), integrationArcStarts );
        }
        else if( testCase == 1 )
        {
            std::vector< boost::shared_ptr< IntegratorSettings< > > > integratorSettingsList;
            for( unsigned int i = 0; i < numberOfIntegrationArcs; i++ )
            {
                integratorSettingsList.push_back( boost::make_shared< IntegratorSettings< > >
                                                  ( rungeKutta4, integrationArcStarts.at( i ), 120.0 ) );
            }
            MultiArcDynamicsSimulator< > dynamicsSimulator(
                        bodyMap, integratorSettingsList, boost::make_shared< MultiArcPropagatorSettings< double > >(
                            arcPropagationSettingsList ) );
        }


        boost::shared_ptr< Ephemeris > moonEphemeris = bodyMap.at( "Moon" )->getEphemeris( );

        double testStartTime, testEndTime;
        double testTimeStep = 10000.0;
        double timeBuffer = 1000.0;

        Eigen::Vector6d stateDifference;

        for( unsigned int i = 0; i < numberOfIntegrationArcs ; i++ )
        {
            if( i == 0 )
            {
                testStartTime = integrationArcStarts.at( i ) + timeBuffer;
            }
            else
            {
                testStartTime = integrationArcEnds.at( i - 1 ) + timeBuffer;
            }

            if( i == numberOfIntegrationArcs - 1 )
            {
                testEndTime = integrationArcEnds.at( i ) - timeBuffer;
            }
            else
            {
                testEndTime = integrationArcStarts.at( i + 1 ) - timeBuffer;
            }

            double currentTestTime = testStartTime;
            while( currentTestTime < testEndTime )
            {
                stateDifference = ( moonEphemeris->getCartesianState( currentTestTime ) ) -
                        ( orbital_element_conversions::convertKeplerianToCartesianElements(
                              propagateKeplerOrbit( initialKeplerElements.at( i ), currentTestTime - integrationArcStarts.at( i ),
                                                    earthGravitationalParameter ), earthGravitationalParameter ) );
                for( int i = 0; i < 3; i++ )
                {
                    BOOST_CHECK_SMALL( stateDifference( i ), 1.0E-4 );
                    BOOST_CHECK_SMALL( stateDifference( i + 3 ), 1.0E-10 );

                }
                currentTestTime += testTimeStep;
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END( )

}

}