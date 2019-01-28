/**
 *  @file   ArborContent/include/ArborCheating/NearbyHitRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the cheating cluster cleaning algorithm class.
 * 
 *  $Log: $
 */
#ifndef NEARBY_HIT_RECOVERY_ALGORITHM_H
#define NEARBY_HIT_RECOVERY_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{
typedef std::map<const pandora::MCParticle* const, pandora::CaloHitList> MCPCaloHitListMap;
typedef std::map<const pandora::MCParticle* const, pandora::ClusterList> MCPClusterListMap;
typedef std::map<const pandora::Cluster* const,    pandora::CaloHitList> ClusterCaloHitListMap;

/**
 *  @brief NearbyHitRecoveryAlgorithm class
 */
class NearbyHitRecoveryAlgorithm : public pandora::Algorithm
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

    pandora::StatusCode MakeClusterHitsAssociation(ClusterCaloHitListMap& clusterCaloHitListMap);
    pandora::StatusCode AddHitToCluster(ClusterCaloHitListMap& clusterCaloHitListMap);

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

	unsigned int m_nNeighborHits;

	float m_maxHitsDistance;

    pandora::AlgorithmTool *m_pAlgorithmTool;
};

inline pandora::Algorithm *NearbyHitRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new NearbyHitRecoveryAlgorithm();
}

} // namespace Arbor_content

#endif 
