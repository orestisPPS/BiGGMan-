//
// Created by hal9000 on 2/18/23.
//

#ifndef UNTITLED_DOFINITIALIZER_H
#define UNTITLED_DOFINITIALIZER_H

#include "../../MathematicalProblem/SteadyStateMathematicalProblem.h"
#include "../../DegreesOfFreedom/DegreeOfFreedom.h"
#include "../../Discretization/Mesh/Mesh.h"

namespace NumericalAnalysis {

    class DOFInitializer {
    public:
        DOFInitializer(Mesh *mesh, DomainBoundaryConditions *domainBoundaryConditions, struct Field_DOFType* degreesOfFreedom);
        list<DegreeOfFreedom*> *freeDegreesOfFreedom;
        list<DegreeOfFreedom*> *boundedDegreesOfFreedom;
        list<tuple<DegreeOfFreedom*, double>> *fluxDegreesOfFreedom;
        list<DegreeOfFreedom*> *totalDegreesOfFreedom;
        
    private:
        void initiateInternalNodeDOFs(Mesh *mesh, Field_DOFType* degreesOfFreedom) const;
        void initiateBoundaryNodeDOF(Mesh *mesh, Field_DOFType *problemDOFTypes,
                                     DomainBoundaryConditions *domainBoundaryConditions) const;
        void removeDuplicatesAndDelete(Mesh *mesh) const;
        void assignDOFIDs() const;

        //TODO: Implement initial conditions. Check if there is meaning in domain  initial conditions as a mathematical object.
        void applyInitialConditions(list<DegreeOfFreedom*>);
    };

} // DOFInitializer

#endif //UNTITLED_DOFINITIALIZER_H
