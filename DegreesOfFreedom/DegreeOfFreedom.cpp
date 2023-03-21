//
// Created by hal9000 on 11/28/22.
//

#include "DegreeOfFreedom.h"


namespace DegreesOfFreedom{

    DegreeOfFreedom::DegreeOfFreedom(DOFType* dofType, unsigned* parentNode, bool isConstrained) :
            _dofType(dofType), parentNode(parentNode) ,_value(numeric_limits<double>::quiet_NaN()) {
        if (isConstrained)
            id = new DegreeOfFreedomID(ConstraintType::Fixed);
        else
            id = new DegreeOfFreedomID(ConstraintType::Free);
            
    }

    DegreeOfFreedom::DegreeOfFreedom(DOFType* dofType, double value, unsigned* parentNode, bool isConstrained) :
            _dofType(dofType), _value(value) , parentNode(parentNode) {
        if (isConstrained)
            id = new DegreeOfFreedomID(ConstraintType::Fixed);
        else
            throw invalid_argument("A DOF with a prescribed value must be constrained");
    }
    
    DegreeOfFreedom::~DegreeOfFreedom() {
        delete id;
        //delete parentNode;
        //delete _dofType;
        id = nullptr;
        parentNode = nullptr;
        _dofType = nullptr;
    }

    bool DegreeOfFreedom::operator==(const DegreeOfFreedom &dof) {
        return _dofType == dof._dofType &&
               *parentNode == *dof.parentNode;
    }

    bool DegreeOfFreedom::operator!=(const DegreeOfFreedom &dof) {
        return !(*this == dof);
    }

    DOFType const &DegreeOfFreedom::type() {
        return *(_dofType);
    }


    double DegreeOfFreedom::value() const {
        return _value;
    }

    void DegreeOfFreedom::setValue(double value) {
        _value = value;
    }
    //Prints in the CLI the DOF ID value, DOF type  and constraint type
    void DegreeOfFreedom::Print() {
        std::cout
                << "DOF ID: " << *id->value << " DOF Type: " << _dofType << " Constraint Type: " << id->constraintType()
                << std::endl;
    }
}
