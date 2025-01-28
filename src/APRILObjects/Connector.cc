/// \file Connector.cc
/*
 *
 * Connector.cc source template automatically generated by a class generator
 * Creation date : ven. mars 20 2015
 *
 * This file is part of APRILContent libraries.
 * 
 * APRILContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * APRILContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with APRILContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "APRILObjects/Connector.h"
#include "APRILObjects/CaloHit.h"

namespace april_content
{

  Connector::Connector(const april_content::CaloHit *const pFromCaloHit, const april_content::CaloHit *const pToCaloHit, float referenceLength,
		  unsigned int creationStage) :
      m_pFromCaloHit(pFromCaloHit),
      m_pToCaloHit(pToCaloHit),
      m_referenceLength(referenceLength),
      m_creationStage(creationStage)
  {
    /* nop */
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  Connector::~Connector()
  {
    m_pFromCaloHit = NULL;
    m_pToCaloHit = NULL;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  const CaloHit *Connector::GetFrom() const
  {
    return m_pFromCaloHit;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  const CaloHit *Connector::GetTo() const
  {
    return m_pToCaloHit;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  const april_content::CaloHit *Connector::Get(ConnectorDirection direction) const
  {
    return direction == BACKWARD_DIRECTION ? this->GetFrom() : this->GetTo();
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float Connector::GetLength() const
  {
    return (this->GetFrom()->GetPositionVector() - this->GetTo()->GetPositionVector()).GetMagnitude();
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float Connector::GetTiming() const
  {
    return fabs(this->GetFrom()->GetTime() - this->GetTo()->GetTime());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float Connector::GetSmearedTiming() const
  {
    return fabs(this->GetFrom()->GetSmearedTime() - this->GetTo()->GetSmearedTime());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float Connector::GetReferenceLength() const
  {
    return m_referenceLength;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float Connector::GetNormalizedLength() const
  {
    return (this->GetLength() / m_referenceLength);
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::CartesianVector Connector::GetVector(ConnectorDirection direction) const
  {
    pandora::CartesianVector vector = GetTo()->GetPositionVector() - GetFrom()->GetPositionVector();
    return direction == FORWARD_DIRECTION ? vector : vector * -1.f;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool Connector::IsFrom(const CaloHit *const pCaloHit) const
  {
    return (pCaloHit == this->GetFrom());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  bool Connector::IsTo(const CaloHit *const pCaloHit) const
  {
    return (pCaloHit == this->GetTo());
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float Connector::GetOpeningAngle(const Connector *const pConnector) const
  {
    const pandora::CartesianVector direction(GetTo()->GetPositionVector() - GetFrom()->GetPositionVector());
    const pandora::CartesianVector otherDirection(pConnector->GetTo()->GetPositionVector() - pConnector->GetTo()->GetPositionVector());

    return direction.GetOpeningAngle(otherDirection);
  }

  unsigned int Connector::GetCreationStage() const
  {
	  return m_creationStage;
  }

} 

