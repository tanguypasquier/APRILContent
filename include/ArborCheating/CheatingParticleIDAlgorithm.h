/**
 *  @file   ArborContent/include/ArborCheating/CheatingParticleIDAlgorithm.h
 *
 *  @brief  Header file for the cheating particle id algorithm class.
 * 
 *  $Log: $
 */
#ifndef ARBOR_CHEATING_PARTICLE_ID_ALGORITHM_H
#define ARBOR_CHEATING_PARTICLE_ID_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/**
 *  @brief  CheatingParticleIDAlgorithm class
 */
class CheatingParticleIDAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    CheatingParticleIDAlgorithm();

    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    bool            m_useClusterOverTrackID;    ///< In case of PFO with tracks and clusters, take best mc particle from cluster
};

inline pandora::Algorithm *CheatingParticleIDAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingParticleIDAlgorithm();
}

} // namespace arbor_content

#endif // #ifndef ARBOR_CHEATING_PARTICLE_ID_ALGORITHM_H
