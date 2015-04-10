  /// \file SortingHelper.h
/*
 *
 * SortingHelper.h header template automatically generated by a class generator
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


#ifndef SORTINGHELPER_H
#define SORTINGHELPER_H

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace pandora { class Cluster; }

namespace arbor_content
{

/** 
 * @brief SortingHelper class
 */ 
class SortingHelper 
{
public:
	/**
	 *  @brief  Sort clusters by inner layer, and hadronic energy for same layer
	 */
	static bool SortClustersByInnerLayer(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);
}; 

} 

#endif  //  SORTINGHELPER_H
