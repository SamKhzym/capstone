#include "besee_utils.h"
#include "gtest/gtest.h"
#include <vector>
#include <memory>

class BeseeStraightRoadTest : public ::testing::Test {
protected:
    BeseeStraightRoadTest() {

        // initialize ego vehicle
        EgoVehicle ego;

        // initialize straight road of 1000m
        vector<LaneSegment> laneSegments({
            StraightLaneSegment(1000)
        });

        // initialize lane definition with 4 lanes, 4m wide
        LaneDefinition lanes(
            laneSegments,
            4,
            4
        );

        // initialize four actors, one for each lane
        vector<shared_ptr<PathFollowingActor>> actors({
            make_shared<PathFollowingActor>("actor1", lanes.getLaneWaypoints(0)),
            make_shared<PathFollowingActor>("actor2", lanes.getLaneWaypoints(1)),
            make_shared<PathFollowingActor>("actor3", lanes.getLaneWaypoints(2)),
            make_shared<PathFollowingActor>("actor4", lanes.getLaneWaypoints(3))
        });

        besee = new BeseeCore(ego, actors, lanes);

    }

    // Declare member variable for besee core object
    BeseeCore* besee;
};

TEST_F(BeseeStraightRoadTest, RelActorStatesStatic) {

    // set all actors to be 50m along path
    for (int i = 0; i < besee->actors.size(); i++) { 
        besee->actors.at(i)->distAlongPath = 50; 
        besee->actors.at(i)->update(0);
    }

    auto relStates = besee->getActorStatesInEgoFrame();
    
    // assert the relative position of each actor is [50, 4i] compared to ego vehicle
    // assert relative speeds are 0
    // assert relative headings are 0
    for (int i = 0; i < besee->actors.size(); i++) {
        auto actorState = relStates.at(besee->actors.at(i)->name);
        EXPECT_NEAR(actorState(0), 50, 0.01);
        EXPECT_NEAR(actorState(1), 4*i, 0.01);
        EXPECT_NEAR(actorState(2), 0, 0.01);
        EXPECT_NEAR(actorState(3), 0, 0.01);
        EXPECT_NEAR(actorState(4), 0, 0.01);
    }

}

TEST_F(BeseeStraightRoadTest, RelActorStatesMoving) {

    // set all actors to be 50m along path at 10i m/s
    for (int i = 0; i < besee->actors.size(); i++) { 
        besee->actors.at(i)->distAlongPath = 50; 
        besee->actors.at(i)->speed_mps = i*10; 
        besee->actors.at(i)->update(0);
    }

    auto relStates = besee->getActorStatesInEgoFrame();
    
    // assert the relative position of each actor is [50, 4i] compared to ego vehicle
    // assert relative speeds are 10i
    // assert relative headings are 0
    for (int i = 0; i < besee->actors.size(); i++) {
        auto actorState = relStates.at(besee->actors.at(i)->name);
        EXPECT_NEAR(actorState(0), 50, 0.01);
        EXPECT_NEAR(actorState(1), 4*i, 0.01);
        EXPECT_NEAR(actorState(2), 10*i, 0.01);
        EXPECT_NEAR(actorState(3), 0, 0.01);
        EXPECT_NEAR(actorState(4), 0, 0.01);
    }

}

TEST_F(BeseeStraightRoadTest, RelActorStatesStaticAngled) {

    // set all actors to be 50m along path
    for (int i = 0; i < besee->actors.size(); i++) { 
        besee->actors.at(i)->distAlongPath = 50; 
        besee->actors.at(i)->speed_mps = 0; 
        besee->actors.at(i)->update(0);
    }

    besee->ego.state(0,2) = M_PI/2; // set ego's heading to 90deg angle (counterclockwise)

    auto relStates = besee->getActorStatesInEgoFrame();
    
    // assert the relative position of each actor is [4i, -50] compared to ego vehicle (vehicles are now to the right)
    // assert relative speeds are 0
    // assert relative headings are -90deg
    for (int i = 0; i < besee->actors.size(); i++) {
        auto actorState = relStates.at(besee->actors.at(i)->name);
        EXPECT_NEAR(actorState(0), 4*i, 0.01);
        EXPECT_NEAR(actorState(1), -50, 0.01);
        EXPECT_NEAR(actorState(2), 0, 0.01);
        EXPECT_NEAR(actorState(3), 0, 0.01);
        EXPECT_NEAR(actorState(4), -M_PI/2, 0.01);
    }

}

TEST_F(BeseeStraightRoadTest, RelActorStatesMovingAngled) {

    // set all actors to be 50m along path with speed 10i
    for (int i = 0; i < besee->actors.size(); i++) { 
        besee->actors.at(i)->distAlongPath = 50; 
        besee->actors.at(i)->speed_mps = 10*i; 
        besee->actors.at(i)->update(0);
    }

    besee->ego.state(0,2) = M_PI/2; // set ego's heading to 90deg angle (counterclockwise)

    auto relStates = besee->getActorStatesInEgoFrame();
    
    // assert the relative position of each actor is [4i, -50] compared to ego vehicle (vehicles are now to the right)
    // assert relative speeds are [0, -10i]
    // assert relative headings are -90deg
    for (int i = 0; i < besee->actors.size(); i++) {
        auto actorState = relStates.at(besee->actors.at(i)->name);
        EXPECT_NEAR(actorState(0), 4*i, 0.01);
        EXPECT_NEAR(actorState(1), -50, 0.01);
        EXPECT_NEAR(actorState(2), 0, 0.01);
        EXPECT_NEAR(actorState(3), -10*i, 0.01);
        EXPECT_NEAR(actorState(4), -M_PI/2, 0.01);
    }

}

TEST_F(BeseeStraightRoadTest, RelActorStatesMovingOppositeDirections) {

    // set all actors to be 50m along path with speed 10i
    for (int i = 0; i < besee->actors.size(); i++) { 
        besee->actors.at(i)->distAlongPath = 50; 
        besee->actors.at(i)->speed_mps = 10*i; 
        besee->actors.at(i)->update(0);
    }

    besee->ego.state(0,2) = M_PI; // set ego's heading to 180deg angle (counterclockwise)

    auto relStates = besee->getActorStatesInEgoFrame();
    
    // assert the relative position of each actor is [-50, -4i] compared to ego vehicle (vehicles are now to the right)
    // assert relative speeds are [-10i, 0]
    // assert relative headings are -180deg
    for (int i = 0; i < besee->actors.size(); i++) {
        auto actorState = relStates.at(besee->actors.at(i)->name);
        EXPECT_NEAR(actorState(0), -50, 0.01);
        EXPECT_NEAR(actorState(1), -4*i, 0.01);
        EXPECT_NEAR(actorState(2), -10*i, 0.01);
        EXPECT_NEAR(actorState(3), 0, 0.01);
        EXPECT_NEAR(actorState(4), -M_PI, 0.01);
    }

}