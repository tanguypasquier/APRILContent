/*
 *
 * ReclusterHelper.cc source template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
 *
 * This file is part of ArborContent libraries.
 * 
 * ArborContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborHelpers/ReclusterHelper.h"

namespace arbor_content
{

float ReclusterHelper::GetTrackClusterCompatibility(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster, const pandora::TrackList &trackList,
		float energyResolutionFactor)
{
    float trackEnergySum(0.);

    for (pandora::TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
        trackEnergySum += (*trackIter)->GetEnergyAtDca();

    const float hadronicEnergyResolution(pandora.GetSettings()->GetHadronicEnergyResolution());

    if ((trackEnergySum < std::numeric_limits<float>::epsilon()) || (hadronicEnergyResolution < std::numeric_limits<float>::epsilon()))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergySum / std::sqrt(trackEnergySum));
    const float chi((pCluster->GetTrackComparisonEnergy(pandora) - trackEnergySum) / (energyResolutionFactor * sigmaE));

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ReclusterHelper::GetTrackClusterCompatibility(const pandora::Pandora &pandora, const float clusterEnergy, const float trackEnergy,
		float energyResolutionFactor)
{
    const float hadronicEnergyResolution(pandora.GetSettings()->GetHadronicEnergyResolution());

    if ((trackEnergy < std::numeric_limits<float>::epsilon()) || (hadronicEnergyResolution < std::numeric_limits<float>::epsilon()))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));
    const float chi((clusterEnergy - trackEnergy) / (energyResolutionFactor * sigmaE));

    return chi;
}


} 

