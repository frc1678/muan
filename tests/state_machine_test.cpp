#include "../statemachine/state_machine.h"
#include "../statemachine/iterated_state_machine.h"
#include "gtest/gtest.h"

enum States : unsigned int { One, Two, Three };

TEST(StateMachineTest, Transitions) {
        auto tsm = FiniteStateMachine<int>(States::One);
        bool called = false;
        tsm.addEdge(States::One, States::Two,
                    [&](FiniteStateMachineEdge edge) { called = true; });
        tsm.transitionToVertex(States::Two);
        ASSERT_EQ(tsm.getCurrentVertex(), States::Two);
        ASSERT_TRUE(called);
}

TEST(StateMachineTest, DoesNotTakeDisabledTransitions) {
        auto tsm = FiniteStateMachine<int>(States::One);
        tsm.transitionToVertex(States::Two);
        ASSERT_EQ(tsm.getCurrentVertex(), States::One);
}

TEST(IteratedStateMachineTest, CallsUpdateFunc) {
        auto tsm = IteratedFiniteStateMachine(States::One);
        int i = 300;
        tsm.SetVertexData(States::One, [&i](FiniteStateMachineVertexID v) {
                i--;
                return States::One;
        });
        for (int j = 0; j < 300; j++) {
                tsm.update();
        }
        ASSERT_EQ(i, 0);
}

TEST(IteratedStateMachineTest, TransitionsFromUpdate) {
        auto tsm = IteratedFiniteStateMachine(States::One);
        int i = 0;
        tsm.addEdge(States::One, States::Two);
        tsm.SetVertexData(States::One, [&i](FiniteStateMachineVertexID v) {
                if (i++ < 10) {
                        return States::One;
                } else {
                        return States::Two;
                }
        });
        for (int j = 0; j < 20; j++) {
                tsm.update();
        }
        ASSERT_EQ(tsm.getCurrentVertex(), States::Two);
}

TEST(StateMachineTest, CallsInitAndDeinit) {
        auto tsm = IteratedFiniteStateMachine(States::One);
        bool did_init = false, did_deinit = false;
        tsm.addInitFunction(States::One, [&did_init]() { did_init = true; });
        tsm.addDeinitFunction(States::One,
                              [&did_deinit]() { did_deinit = true; });
        tsm.addEdge(States::One, States::One);
        tsm.addEdge(States::One, States::Two);
        tsm.transitionToVertex(States::One);
        tsm.transitionToVertex(States::Two);
        ASSERT_TRUE(did_init);
        ASSERT_TRUE(did_deinit);
}
