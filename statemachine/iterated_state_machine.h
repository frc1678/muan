//
//  IteratedFiniteStateMachine.h
//  FiniteStateMachine
//
//  Created by Donald Pinckney on 12/22/15.
//  Copyright © 2015 Donald Pinckney. All rights reserved.
//

#ifndef IteratedFiniteStateMachine_h
#define IteratedFiniteStateMachine_h

#include "state_machine.h"
#include <functional>

// Because this isn't actually templated, could easily move functions to cpp.

class IteratedFiniteStateMachine
    : public FiniteStateMachine<std::function<FiniteStateMachineVertexID(
          FiniteStateMachineVertexID)>> {
       public:
        IteratedFiniteStateMachine(FiniteStateMachineVertexID initialVertex)
            : FiniteStateMachine(initialVertex),
              initial_vertex_(initialVertex) {}

        void Start() {
                current_vertex_ = initial_vertex_;
                auto init_it = init_functions_.find(current_vertex_);
                if (init_it != init_functions_.end()) {
                        (init_it->second)();
                }
        }

        void Update() {
                if (!started_) TransitionToVertex(initial_vertex_);
                started_ = true;
                std::function<FiniteStateMachineVertexID(
                    FiniteStateMachineVertexID)> *currentUpdateFunc =
                    GetCurrentVertexData();

                if (currentUpdateFunc == nullptr) {
                        // TODO: Do something with the case of no given Update
                        // function.
                        return;
                }

                FiniteStateMachineVertexID newState =
                    (*currentUpdateFunc)(GetCurrentVertex());

                auto deinit_it = deinit_functions_.find(GetCurrentVertex());

                if (deinit_it != deinit_functions_.end()) {
                        (deinit_it->second)();
                }

                auto init_it = init_functions_.find(newState);
                if (init_it != init_functions_.end()) {
                        (init_it->second)();
                }

                TransitionToVertex(newState);
        }

        void AddInitFunction(FiniteStateMachineVertexID id,
                             std::function<void()> init) {
                init_functions_[id] = init;
        }

        void AddDeinitFunction(FiniteStateMachineVertexID id,
                               std::function<void()> deinit) {
                deinit_functions_[id] = deinit;
        }

       private:
        bool started_ = false;
        FiniteStateMachineVertexID initial_vertex_;
        std::map<FiniteStateMachineVertexID, std::function<void()>>
            init_functions_, deinit_functions_;
};

#endif /* IteratedFiniteStateMachine_h */
