  /// \file SortingHelper.cc
/*
 *
 * SortingHelper.cc source template automatically generated by a class generator
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


#include "ArborHelpers/SortingHelper.h"

#include "Objects/Cluster.h"

namespace arbor_content
{

bool SortingHelper::SortClustersByInnerLayer(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs)
{
	const unsigned int innerLayerLhs(pLhs->GetInnerPseudoLayer()), innerLayerRhs(pRhs->GetInnerPseudoLayer());

	if (innerLayerLhs != innerLayerRhs)
	    return (innerLayerLhs < innerLayerRhs);

	const unsigned int nCaloHitsLhs(pLhs->GetNCaloHits()), nCaloHitsRhs(pRhs->GetNCaloHits());

	if (nCaloHitsLhs != nCaloHitsRhs)
	    return (nCaloHitsLhs > nCaloHitsRhs);

	return (pLhs->GetHadronicEnergy() > pRhs->GetHadronicEnergy());
}

} 

