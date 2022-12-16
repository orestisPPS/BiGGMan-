//
// Created by hal9000 on 12/17/22.
//

#include "../PartialDifferentialEquations/PartialDifferentialEquation.h"
#include "../BoundaryConditions/BoundaryCondition.h"

namespace MathematicalProblem{
    class SteadyStateMathematicalProblem {
    public:
        SteadyStateMathematicalProblem(PartialDifferentialEquation *pde,
                                       map<Position,list<BoundaryConditions::BoundaryCondition*>> *bcs,
                                       list<DegreeOfFreedom*> *dof,
                                       CoordinateSystem coordinateSystem);
        ~SteadyStateMathematicalProblem();
        PartialDifferentialEquation *pde;
        map<Position,list<BoundaryConditions::BoundaryCondition*>> *boundaryConditions;
        list<DegreeOfFreedom*> *degreesOfFreedom;
        CoordinateSystem _coordinateSystem;
    };
}
