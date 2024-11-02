#include "besee_utils.h"
#include "datatypes/track.h"
#include "utils/controls_constants.h"
#include "utils/logging/signal_loggers.h"

SimController::SimController(double sampleRate_Hz)
{
    this->sampleRate_Hz = sampleRate_Hz;
    this->lastExecutedTime = 0;
}

SimCaccController::SimCaccController(BeseeCore *env, double setSpeed, double sampleRate_Hz)
{
    this->env = env;
    this->setSpeed = setSpeed;
    this->sampleRate_Hz = sampleRate_Hz;
    this->lastExecutedTime = 0;
}

void SimCaccController::init()
{
    this->controller.initCalibrations(
        "default", PROJECT_BASE + "cav_autera_software/calibrations/cacc_params.yaml",
        PROJECT_BASE + "cav_autera_software/calibrations/UnboundedMaxTorqueLookUp.csv",
        PROJECT_BASE + "cav_autera_software/calibrations/logging.conf");
}

void SimCaccController::step(double ts)
{

    // early return if controller should not be run for this timestep
    if (ts < (this->lastExecutedTime + (1 / this->sampleRate_Hz))) {
        return;
    }
    this->lastExecutedTime = ts;

    // get vehicle speed
    double egoSpeed_mps = this->env->ego.getVx_mps();

    // default values
    bool leadExists = false;
    Track *leadTrack = new Track();

    // get lead vehicle information in ego frame (if it exists)
    auto actors = this->env->getActorStatesInEgoFrame();
    auto leadVehicleIterator = actors.find("lead");

    // if lead exists, set lead speed and distance parameters
    if (leadVehicleIterator != actors.end()) {

        // get lead properties and calculate relative state from ego vehicle
        auto lead = this->env->getActor("lead");
        auto relLeadState = leadVehicleIterator->second; // x, vx, y, vy, heading

        // construct full state vector and dummy covariance matrix
        Eigen::VectorXd fullState(9);
        fullState << relLeadState[0], relLeadState[2], 0, relLeadState[1], relLeadState[3], 0, 0, 0,
            0;
        Eigen::MatrixXd covar = Eigen::MatrixXd::Zero(9, 9);

        // construct dummy new detection and track representing lead info
        Detection *det = new Detection(fullState, covar, lead->length_m, lead->width_m, 0, VEHICLE);

        LOG_SIGNAL("rel_lead_x", ts, relLeadState[0])

        leadTrack = new Track(*det, 0, 0, false, SOURCE_UNDEFINED);
        leadExists = true;
    }

    // step controller and get actuation commands
    std::tuple commands = this->controller.controllerStep(this->setSpeed, egoSpeed_mps, leadExists,
                                                          *leadTrack, false);

    // set actuation commands to vehicle
    this->env->setCavEnable(true);
    this->env->setCavLongitudinalCommands(std::get<0>(commands), std::get<1>(commands));

    // free memory from temp lead track
    delete leadTrack;
}