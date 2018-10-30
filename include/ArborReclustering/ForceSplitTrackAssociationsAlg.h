/**
 *  @file   ForceSplitTrackAssociationsAlg.h
 * 
 *  @brief  Header file for the force split track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef ARBOR_FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
#define ARBOR_FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/**
 *  @brief  ForceSplitTrackAssociationsAlg class
 */
class ForceSplitTrackAssociationsAlg : public pandora::Algorithm
{
public:
    /**
     *  @brief Default constructor
     */
    ForceSplitTrackAssociationsAlg();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::map<const pandora::Track *, const pandora::Cluster *> TrackToClusterMap;
    typedef std::map<const pandora::Track *, const pandora::Helix> TrackToHelixMap;

    unsigned int    m_minTrackAssociations;         ///< The minimum number of track associations to forcibly resolve
};

} // namespace lc_content

#endif 
