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
    : public FiniteStateMachine<std::function<FiniteStateMachineVertexID(FiniteStateMachineVertexID)> > {
 public:
  IteratedFiniteStateMachine(FiniteStateMachineVertexID initialVertex) : FiniteStateMachine(initialVertex), initial_vertex_(initialVertex) {}

  void update() {
    if (!started_) transitionToVertex(initial_vertex_);
    started_ = true;
    std::function<FiniteStateMachineVertexID(FiniteStateMachineVertexID)> *currentUpdateFunc = getCurrentVertexData();
    if (currentUpdateFunc == NULL) {
      // TODO: Do something with the case of no given update function.
      return;
    }

    FiniteStateMachineVertexID newState = (*currentUpdateFunc)(getCurrentVertex());
    transitionToVertex(newState);
  }
 private:
  bool started_ = false;
  FiniteStateMachineVertexID initial_vertex_;
};

#endif /* IteratedFiniteStateMachine_h */
