//
//  FiniteStateMachine.h
//  FiniteStateMachine
//
//  Created by Donald Pinckney on 12/20/15.
//  Copyright © 2015 Donald Pinckney. All rights reserved.
//

#ifndef FiniteStateMachine_h
#define FiniteStateMachine_h

#include <map>
#include <vector>
#include <functional>
#include <iostream>

typedef int FiniteStateMachineVertexID;
typedef int FiniteStateMachineEdgeID;

struct FiniteStateMachineEdge {
        FiniteStateMachineVertexID from_vertex_id;
        FiniteStateMachineVertexID to_vertex_id;
        FiniteStateMachineEdgeID edge_id;
};

template <typename VertexData>
class FiniteStateMachine {
       public:
        FiniteStateMachine(FiniteStateMachineVertexID initial_vertex)
            : current_vertex_(initial_vertex) {}

        void SetVertexData(FiniteStateMachineVertexID i, VertexData d) {
                vertex_data[i] = d;
        }

        FiniteStateMachineVertexID GetCurrentVertex() const {
                return current_vertex_;
        }

        VertexData *GetCurrentVertexData() {
                typename std::map<FiniteStateMachineVertexID,
                                  VertexData>::iterator it =
                    vertex_data.find(current_vertex_);
                if (it == vertex_data.end()) {
                        return nullptr;
                }

                return &(it->second);
        }

        void FullyConnect(
            const std::vector<FiniteStateMachineVertexID> &vertices) {
                FullyConnect(vertices, NULL);
        }

        void FullyConnect(
            const std::vector<FiniteStateMachineVertexID> &vertices,
            std::function<void(FiniteStateMachineEdge)> callback) {
                for (std::vector<FiniteStateMachineVertexID>::const_iterator
                         it = vertices.begin();
                     it != vertices.end(); it++) {
                        FiniteStateMachineVertexID vertex1 = *it;
                        for (std::vector<FiniteStateMachineVertexID>::
                                 const_iterator it2 = it + 1;
                             it2 != vertices.end(); it2++) {
                                AddEdge(vertex1, *it2, callback);
                                AddEdge(*it2, vertex1, callback);
                        }
                }
        }

        void AddEdge(FiniteStateMachineVertexID from,
                     FiniteStateMachineVertexID to) {
                AddEdge(from, to, unique_edge_id_counter_, NULL);
                unique_edge_id_counter_--;
        }

        // The edge ID must be 0 or positive.
        void AddEdge(FiniteStateMachineVertexID from,
                     FiniteStateMachineVertexID to,
                     FiniteStateMachineEdgeID edge_id) {
                AddEdge(from, to, edge_id, NULL);
        }

        void AddEdge(FiniteStateMachineVertexID from,
                     FiniteStateMachineVertexID to,
                     std::function<void(FiniteStateMachineEdge)> callback) {
                AddEdge(from, to, unique_edge_id_counter_, callback);
                unique_edge_id_counter_--;
        }

        // The edge ID must be 0 or positive.
        void AddEdge(FiniteStateMachineVertexID from,
                     FiniteStateMachineVertexID to,
                     FiniteStateMachineEdgeID edge_id,
                     std::function<void(FiniteStateMachineEdge)> callback) {
                FiniteStateMachineEdge edge;
                edge.from_vertex_id = from;
                edge.to_vertex_id = to;
                edge.edge_id = edge_id;

                adjacency_list_by_vertex_id_[from].push_back(edge);
                adjacency_list_by_edge_id_[edge_id] = edge;
                callback_functions_[edge_id] = callback;
        }

        bool EdgeWhichTransitionsToVertex(
            FiniteStateMachineVertexID destination_vertex_id,
            FiniteStateMachineEdge *maybeEdge) const {
                std::map<FiniteStateMachineVertexID,
                         std::vector<FiniteStateMachineEdge>>::const_iterator
                    it = adjacency_list_by_vertex_id_.find(current_vertex_);
                if (it == adjacency_list_by_vertex_id_.end()) {
                        return false;
                }

                const std::vector<FiniteStateMachineEdge> &edges = it->second;

                // Search for an edge that leads to our destination
                for (std::vector<FiniteStateMachineEdge>::const_iterator it =
                         edges.begin();
                     it != edges.end(); it++) {
                        if (it->to_vertex_id == destination_vertex_id) {
                                *maybeEdge = *it;
                                return true;
                        }
                }

                return false;
        }

        virtual bool TransitionToVertex(
            FiniteStateMachineVertexID destination_vertex_id) {
                // TODO(Kyle) This prevents self-edges - is it really a good
                // thing? As the system is currently, this is the only thing
                // preventing an IteratedFiniteStateMachine from resetting every
                // time. However, it seems like that could be better dealt with,
                // as there are some situations where you might want to reset
                // the current state, and those are different from staying on
                // the same state.
                if (destination_vertex_id == current_vertex_) {
                        return false;
                }

                FiniteStateMachineEdge edge;
                if (EdgeWhichTransitionsToVertex(destination_vertex_id,
                                                 &edge)) {
                        CompleteEdgeTransition(edge);
                        return true;
                }
                return false;
        }

        bool CanTransitionUsingEdge(FiniteStateMachineEdgeID edge_id) const {
                std::map<FiniteStateMachineEdgeID,
                         FiniteStateMachineEdge>::const_iterator it =
                    adjacency_list_by_edge_id_.find(edge_id);
                if (it == adjacency_list_by_edge_id_.end()) {
                        return false;
                }

                FiniteStateMachineEdge possibleEdge = it->second;
                if (possibleEdge.from_vertex_id == current_vertex_) {
                        return true;
                }
                return false;
        }

        bool TransitionUsingEdge(FiniteStateMachineEdgeID edge_id) {
                FiniteStateMachineEdge possibleEdge =
                    adjacency_list_by_edge_id_[edge_id];
                if (possibleEdge.from_vertex_id == current_vertex_) {
                        CompleteEdgeTransition(possibleEdge);
                        return true;
                }
                return false;
        }

       protected:
        FiniteStateMachineVertexID current_vertex_;
        FiniteStateMachineEdgeID unique_edge_id_counter_ = -1;
        std::map<FiniteStateMachineVertexID,
                 std::vector<FiniteStateMachineEdge>>
            adjacency_list_by_vertex_id_;
        std::map<FiniteStateMachineEdgeID, FiniteStateMachineEdge>
            adjacency_list_by_edge_id_;
        std::map<FiniteStateMachineVertexID, VertexData> vertex_data;

        std::map<FiniteStateMachineEdgeID,
                 std::function<void(FiniteStateMachineEdge)>>
            callback_functions_;

        void CompleteEdgeTransition(FiniteStateMachineEdge edge) {
                auto edge_it = callback_functions_.find(edge.edge_id);
                if (edge_it != callback_functions_.end() &&
                    edge_it->second != NULL) {
                        (edge_it->second)(edge);
                }

                current_vertex_ = edge.to_vertex_id;
        }
};

#endif /* FiniteStateMachine_h */
