  /// \file ClusterEfficiencyAlgorithm.h
/*
 *
 * ClusterEfficiencyAlgorithm.h header template automatically generated by a class generator
 * Creation date : jeu. juin 2 2016
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


#ifndef CLUSTEREFFICIENCYALGORITHM_H
#define CLUSTEREFFICIENCYALGORITHM_H

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInternal.h"

#include "TNtupleD.h"


namespace arbor_content
{

/** 
 * @brief ClusterEfficiencyAlgorithm class
 */ 
class ClusterEfficiencyAlgorithm : public pandora::Algorithm
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
	pandora::StatusCode	Initialize();
	float               getPurity(const pandora::Cluster* cluster) const;

    bool                            m_timing;

    float                           m_timeCut;

	//TNtupleD*                       caloHitsMonitor;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterEfficiencyAlgorithm::Factory::CreateAlgorithm() const
{
	return new ClusterEfficiencyAlgorithm();
}

} 

#endif
