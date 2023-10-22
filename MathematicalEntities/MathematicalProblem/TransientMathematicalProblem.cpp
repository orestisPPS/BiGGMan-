//
// Created by hal9000 on 12/17/22.
//

#include "TransientMathematicalProblem.h"
#include "../PartialDifferentialEquations/TransientPartialDifferentialEquation.h"

#include <utility>

namespace MathematicalEntities {
    
    TransientMathematicalProblem::TransientMathematicalProblem(
            shared_ptr<TransientPartialDifferentialEquation> pde, shared_ptr<DomainBoundaryConditions> bcs,
            shared_ptr<InitialConditions> initialConditions, struct Field_DOFType *degreesOfFreedom) :
            SteadyStateMathematicalProblem(pde, std::move(bcs), degreesOfFreedom),
            transientPDE(std::move(pde)), initialConditions(std::move(initialConditions)) {
        
    }


}// MathematicalEntities