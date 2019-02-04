/**
 *  @file   ArborContent/include/ArborCheating/CheatingClusterCleaningAlgorithm.h
 * 
 *  @brief  Header file for the cheating cluster cleaning algorithm class.
 * 
 *  $Log: $
 */
#ifndef ARBOR_CHEATING_CLUSTER_CLEANING_ALGORITHM_H
#define ARBOR_CHEATING_CLUSTER_CLEANING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/**
 *  @brief CheatingClusterCleaningAlgorithm class
 */
class CheatingClusterCleaningAlgorithm : public pandora::Algorithm
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

    bool m_isOnlyCleanPhoton;
};

inline pandora::Algorithm *CheatingClusterCleaningAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingClusterCleaningAlgorithm();
}

} // namespace Arbor_content

#endif // #ifndef ARBOR_CHEATING_CLUSTER_CLEANING_ALGORITHM_H
