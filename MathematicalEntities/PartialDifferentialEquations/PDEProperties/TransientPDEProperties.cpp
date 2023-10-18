//
// Created by hal9000 on 10/15/23.
//

#include "TransientPDEProperties.h"

namespace MathematicalEntities {
    MathematicalEntities::TransientPDEProperties::TransientPDEProperties(unsigned short physicalSpaceDimensions,
                                                                         MathematicalEntities::FieldType fieldType)
            : SecondOrderLinearPDEProperties(physicalSpaceDimensions, fieldType) {

    }

    void MathematicalEntities::TransientPDEProperties::setIsotropicTemporalProperties(double secondOrderCoefficient,
                                                                                      double firstOrderCoefficient) {
        if (_fieldType == FieldType::ScalarField) {
            _scalarFieldGlobalTemporalProperties.secondOrderCoefficient = make_unique<double>(secondOrderCoefficient);
            _scalarFieldGlobalTemporalProperties.firstOrderCoefficient = make_unique<double>(firstOrderCoefficient);
        }
        else if (_fieldType == FieldType::VectorField) {
            auto coefficient1 = make_unique<NumericalVector<double>>(_dimensions, firstOrderCoefficient);
            auto coefficient2 = make_unique<NumericalVector<double>>(_dimensions, secondOrderCoefficient);
            _vectorFieldGlobalTemporalProperties.firstOrderCoefficients = std::move(coefficient1);
            _vectorFieldGlobalTemporalProperties.secondOrderCoefficients = std::move(coefficient2);
        }
    }

    void MathematicalEntities::TransientPDEProperties::setAnisotropicTemporalProperties(
            TemporalScalarFieldPDECoefficients scalarFieldProperties, unsigned int *nodeId) {
        switch (_fieldType) {
            case FieldType::ScalarField:
                if (_locallyAnisotropicScalarFieldTemporalProperties != nullptr && nodeId != nullptr) {
                    _locallyAnisotropicScalarFieldTemporalProperties->at(nodeId) = std::move(scalarFieldProperties);
                }
                else
                    throw invalid_argument("TransientPDEProperties::setAnisotropicTemporalProperties:"
                                           " Node ID not found. If new nodes need to be assigned use setLocallyAnisotropicTemporalProperties.");
                break;
            case FieldType::VectorField:
                throw invalid_argument("TransientPDEProperties::setAnisotropicTemporalProperties:"
                                       " Cannot set anisotropic temporal properties for a vector field.");
                break;
        }
    }
    
    void MathematicalEntities::TransientPDEProperties::setAnisotropicTemporalProperties(
            TemporalVectorFieldPDECoefficients vectorFieldProperties, unsigned int *nodeId) {
        switch (_fieldType) {
            case FieldType::ScalarField:
                throw invalid_argument("TransientPDEProperties::setAnisotropicTemporalProperties:"
                                       " Cannot set anisotropic temporal properties for a scalar field.");
                break;
            case FieldType::VectorField:
                if (_locallyAnisotropicVectorFieldTemporalProperties != nullptr && nodeId != nullptr) {
                    _locallyAnisotropicVectorFieldTemporalProperties->at(nodeId) = std::move(vectorFieldProperties);
                }
                else
                    throw invalid_argument("TransientPDEProperties::setAnisotropicTemporalProperties:"
                                           " Node ID not found. If new nodes need to be assigned use setLocallyAnisotropicTemporalProperties.");
                break;
        }
    }

    void MathematicalEntities::TransientPDEProperties::setLocallyAnisotropicTemporalProperties(
            unique_ptr<map<unsigned int*, TemporalScalarFieldPDECoefficients>> scalarFieldProperties) {
        if (_fieldType == ScalarField) {
            _locallyAnisotropicScalarFieldTemporalProperties = std::move(scalarFieldProperties);
        }
        else
            throw invalid_argument("TransientPDEProperties::setLocallyAnisotropicTemporalProperties:"
                                   " Cannot set locally anisotropic temporal properties for a vector field.");

    }     
    
    void MathematicalEntities::TransientPDEProperties::setLocallyAnisotropicTemporalProperties(
            unique_ptr<map<unsigned int*, TemporalVectorFieldPDECoefficients>> vectorFieldProperties) {
        if (_fieldType == VectorField) {
            _locallyAnisotropicVectorFieldTemporalProperties = std::move(vectorFieldProperties);
        }
        else
            throw invalid_argument("TransientPDEProperties::setLocallyAnisotropicTemporalProperties:"
                                   " Cannot set locally anisotropic temporal properties for a scalar field.");
    }

    double TransientPDEProperties::getTemporalCoefficient(unsigned int derivativeOrder, Direction direction) {
        if (_locallyAnisotropicScalarFieldTemporalProperties != nullptr ||
            _locallyAnisotropicVectorFieldTemporalProperties != nullptr)
            throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                   " Cannot get global temporal properties when locally anisotropic properties are set.");
        switch (_fieldType) {
            case ScalarField:
                switch (derivativeOrder) {
                    case 0:
                        return *_scalarFieldGlobalTemporalProperties.secondOrderCoefficient;
                    case 1:
                        return *_scalarFieldGlobalTemporalProperties.firstOrderCoefficient;
                    default:
                        throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                               " Invalid derivative order.");
                }
            case VectorField:
                auto i = spatialDirectionToUnsigned[direction];
                switch (derivativeOrder) {
                    case 0:
                        return (*_vectorFieldGlobalTemporalProperties.secondOrderCoefficients)[i];
                    case 1:
                        return (*_vectorFieldGlobalTemporalProperties.firstOrderCoefficients)[i];
                    default:
                        throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                               " Invalid derivative order.");
                }
        }
    }

    double TransientPDEProperties::getTemporalCoefficient(unsigned int derivativeOrder, unsigned int *nodeId,
                                                          Direction direction) {
       
        if (_fieldType == ScalarField) {
            if (_locallyAnisotropicScalarFieldTemporalProperties == nullptr)
                throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                       " No temporal properties set.");
            else {
                switch (derivativeOrder) {
                    case 0:
                        return *_locallyAnisotropicScalarFieldTemporalProperties->at(
                                nodeId).secondOrderCoefficient;
                    case 1:
                        return *_locallyAnisotropicScalarFieldTemporalProperties->at(nodeId).firstOrderCoefficient;
                    default:
                        throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                               " Invalid derivative order.");
                }
            }
        } else if (_fieldType == VectorField) {
            if (_locallyAnisotropicVectorFieldTemporalProperties == nullptr)
                throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                       " No temporal properties set.");
            else {
                auto i = spatialDirectionToUnsigned[direction];
                switch (derivativeOrder) {
                    case 0:
                        return (*_locallyAnisotropicVectorFieldTemporalProperties->at(
                                nodeId).secondOrderCoefficients)[i];
                    case 1:
                        return (*_locallyAnisotropicVectorFieldTemporalProperties->at(
                                nodeId).firstOrderCoefficients)[i];
                    default:
                        throw invalid_argument("TransientPDEProperties::getTemporalCoefficient:"
                                               " Invalid derivative order.");
                }
            }
        }
    }
} // SecondOrderLinearPDEProperties

