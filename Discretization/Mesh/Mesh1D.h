//
// Created by hal9000 on 3/11/23.
//

#ifndef UNTITLED_MESH1D_H
#define UNTITLED_MESH1D_H

#include "Mesh.h"
#include "GhostPseudoMesh/GhostPseudoMesh.h"

namespace Discretization {

    class Mesh1D : public Mesh {
    public:
        Mesh1D(Array<Node *> *nodes);
        
        ~Mesh1D();

        unsigned dimensions() override;
        
        SpaceEntityType space() override;
                
        Node* node(unsigned i) override;
        
        Node* node(unsigned i, unsigned j) override;
        
        Node* node(unsigned i, unsigned j, unsigned k) override;

        map<vector<double>, Node*>* createParametricCoordToNodesMap() override;
        
        void printMesh() override;
        
    protected:
        
        map<Position, vector<Node*>*> *addDBoundaryNodesToMap() override;
        
        vector<Node*>* addInternalNodesToVector() override;
        
        vector<Node*>* addTotalNodesToVector() override;

        // Calculates the metrics of all the nodes based on the given coordinate system.
        // If coordinateSystem is Template then the metrics are calculated based on the template coordinate system before
        // the final coordinate system is calculated.
        // If coordinateSystem is Natural then the metrics are calculated based on the final calculated coordinate system.
        void calculateMeshMetrics(CoordinateType coordinateSystem) override;
        
        Metrics* calculateNodeMetrics(Node* node, CoordinateType coordinateSystem) override;
        
        GhostPseudoMesh* createGhostPseudoMesh(unsigned ghostLayerDepth) override;
        
        

    };

} // Discretization

#endif //UNTITLED_MESH1D_H
