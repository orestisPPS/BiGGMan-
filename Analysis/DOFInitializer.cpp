//
// Created by hal9000 on 2/18/23.
//

#include "DOFInitializer.h"

namespace Analysis {
    
    DOFInitializer::DOFInitializer(Mesh* mesh,DomainBoundaryConditions* domainBoundaryConditions, Field_DOFType* degreesOfFreedom) {
        freeDegreesOfFreedom = new list<DegreeOfFreedom*>();
        boundedDegreesOfFreedom = new list<DegreeOfFreedom*>();
        fluxDegreesOfFreedom = new list<DegreeOfFreedom*>();
        totalDegreesOfFreedom = new list<DegreeOfFreedom*>();
        initiateInternalNodeDOFs(mesh, degreesOfFreedom);
        initiateBoundaryNodeBoundedDOF(mesh, degreesOfFreedom, domainBoundaryConditions);
        initiateBoundaryNodeFluxDOF(mesh, degreesOfFreedom, domainBoundaryConditions);
        
    }
    
    void DOFInitializer::initiateInternalNodeDOFs(Mesh *mesh, Field_DOFType *degreesOfFreedom) const {
        //March through the mesh nodes
        for (auto & internalNode : *mesh->internalNodes){
            for (auto & DOFType : *degreesOfFreedom->DegreesOfFreedom){
                auto dof = new DegreeOfFreedom(DOFType, internalNode->id.global, false);
                freeDegreesOfFreedom->push_back(dof);
                totalDegreesOfFreedom->push_back(dof);
            }            
        }
    }
    
    void DOFInitializer::initiateBoundaryNodeFluxDOF(Mesh *mesh, Field_DOFType *problemDOFTypes,
                                                     DomainBoundaryConditions *domainBoundaryConditions) const {
        //March through the boundaryNodes map
        for (auto & boundaryNode : *mesh->boundaryNodes){
            auto position = boundaryNode.first;
            auto nodesAtPosition = boundaryNode.second;
            auto neumannBCAtPosition = domainBoundaryConditions->
                    GetBoundaryConditions(position, BoundaryConditionType::Neumann);
            //March through the nodes at the Position  that is the key of the boundaryNodes map
            for (auto & node : *nodesAtPosition){
                
                auto dofValue = -1.0;
                //If the length of the list of Neumann BCs at the position is 1,
                // the same value of flux is applid to all nodes of the boundary
                if (problemDOFTypes->DegreesOfFreedom->size() == 1 &&neumannBCAtPosition->size() == 1){
                    dofValue = neumannBCAtPosition->front()->scalarValueAt(
                            node->coordinates.positionVectorPtr());
                    auto dof = new DegreeOfFreedom(problemDOFTypes->DegreesOfFreedom->front(), dofValue,
                                                   node->id.global, true);
                    fluxDegreesOfFreedom->push_back(dof);
                    freeDegreesOfFreedom->push_back(dof);
                    totalDegreesOfFreedom->push_back(dof);
                }
                else if (problemDOFTypes->DegreesOfFreedom->size() > 1 &&neumannBCAtPosition->size() > 1){
                    auto vectorValue = neumannBCAtPosition->front()->vectorValueAt(
                            node->coordinates.positionVectorPtr());
                    auto dofType = problemDOFTypes->DegreesOfFreedom->front();
                    for (int i = 0; i < problemDOFTypes->DegreesOfFreedom->size(); i++){
                            dofValue = vectorValue[i];
                            dofType = problemDOFTypes->DegreesOfFreedom->at(i);
                    }
                    auto dof = new DegreeOfFreedom(dofType, dofValue,node->id.global, true);
                    fluxDegreesOfFreedom->push_back(dof);
                    freeDegreesOfFreedom->push_back(dof);
                    totalDegreesOfFreedom->push_back(dof);
                    }
                else{
                    throw std::invalid_argument("The number of DOF types and the number of Neumann BCs at the boundary are not equal.");
                }
            }
        }
    }
    
    void DOFInitializer::initiateBoundaryNodeBoundedDOF(Mesh *mesh, Field_DOFType *degreesOfFreedom,
                                                        DomainBoundaryConditions *domainBoundaryConditions) const {
        //March through the boundaryNodes map
        for (auto & boundaryNode : *mesh->boundaryNodes){
            auto position = boundaryNode.first;
            auto nodesAtPosition = boundaryNode.second;
            auto dirichletBCAtPosition = domainBoundaryConditions->
                    GetBoundaryConditions(position, BoundaryConditionType::Dirichlet);
            //March through the nodes at the Position  that is the key of the boundaryNodes map
            for (auto & node : *nodesAtPosition){
                auto dofValue = -1.0;
                //If the length of the list of Dirichlet BCs at the position is 1,
                // the same value of flux is applid to all nodes of the boundary
                if (degreesOfFreedom->DegreesOfFreedom->size() == 1 &&dirichletBCAtPosition->size() == 1){
                    dofValue = dirichletBCAtPosition->front()->scalarValueAt(
                            node->coordinates.positionVectorPtr());
                    auto dof = new DegreeOfFreedom(degreesOfFreedom->DegreesOfFreedom->front(), dofValue,
                                                   node->id.global, true);
                    boundedDegreesOfFreedom->push_back(dof);
                    totalDegreesOfFreedom->push_back(dof);
                }
                else if (degreesOfFreedom->DegreesOfFreedom->size() > 1 &&dirichletBCAtPosition->size() > 1){
                    auto vectorValue = dirichletBCAtPosition->front()->vectorValueAt(
                            node->coordinates.positionVectorPtr());
                    auto dofType = degreesOfFreedom->DegreesOfFreedom->front();
                    for (int i = 0; i < degreesOfFreedom->DegreesOfFreedom->size(); i++){
                        dofValue = vectorValue[i];
                        dofType = degreesOfFreedom->DegreesOfFreedom->at(i);
                    }
                    auto dof = new DegreeOfFreedom(dofType, dofValue,node->id.global, true);
                    boundedDegreesOfFreedom->push_back(dof);
                    totalDegreesOfFreedom->push_back(dof);
                }
                else{
                    throw std::invalid_argument("The number of DOF types and the number of Dirichlet BCs at the boundary are not equal.");
                }
            }
        }
    }
}