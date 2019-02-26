/*
 *
 * ArborCluster.cc source template automatically generated by a class generator
 * Creation date : lun. mars 30 2015
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


#include "ArborObjects/Cluster.h"

namespace arbor_content
{
  ArborCluster::ArborCluster(const PandoraContentApi::Cluster::Parameters &parameters) :
      pandora::Cluster(parameters)
  {
  }

  ArborCluster::~ArborCluster()
  {
  }

  void ArborCluster::SetMotherCluster(ArborCluster* cluster)
  {
	  m_motherCluster = cluster;
  }
} 

