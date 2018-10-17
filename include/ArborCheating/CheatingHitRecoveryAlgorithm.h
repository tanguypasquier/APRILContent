/**
 *  @file   ArborContent/include/ArborCheating/CheatingHitRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the cheating cluster cleaning algorithm class.
 * 
 *  $Log: $
 */
#ifndef ARBOR_CHEATING_HIT_RECOVERY_ALGORITHM_H
#define ARBOR_CHEATING_HIT_RECOVERY_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/**
 *  @brief CheatingHitRecoveryAlgorithm class
 */
class CheatingHitRecoveryAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
};

inline pandora::Algorithm *CheatingHitRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingHitRecoveryAlgorithm();
}

} // namespace Arbor_content

#endif 
