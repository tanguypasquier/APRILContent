/**
 *  @file   ArborContent/include/ArborCheating/CheatingTrackToClusterMatching.h
 * 
 *  @brief  Header file for the cheating track to cluster matching algorithm class.
 * 
 *  $Log: $
 */
#ifndef ARBOR_CHEATING_TRACK_TO_CLUSTER_MATCHING_ALGORITHM_H
#define ARBOR_CHEATING_TRACK_TO_CLUSTER_MATCHING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/**
 *  @brief CheatingTrackToClusterMatching class
 */
class CheatingTrackToClusterMatching : public pandora::Algorithm
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

	bool IsParent(const pandora::MCParticle* parent, const pandora::MCParticle* daughter);

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    bool m_shouldMergeTrackClusters;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingTrackToClusterMatching::Factory::CreateAlgorithm() const
{
    return new CheatingTrackToClusterMatching();
}

} // namespace arbor_content

#endif // #ifndef ARBOR_CHEATING_TRACK_TO_CLUSTER_MATCHING_ALGORITHM_H
