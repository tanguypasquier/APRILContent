  /// \file Branch.h
/*
 *
 * Branch.h header template automatically generated by a class generator
 * Creation date : lun. mai 4 2015
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


#ifndef BRANCH_H
#define BRANCH_H

#include "Pandora/PandoraInternal.h"

#include "ArborApi/ArborInputTypes.h"

namespace arbor_content
{

/** 
 *  @brief  Branch class
 */ 
class Branch 
{
public:
	typedef std::vector<const CaloHit *> List;
	typedef List::value_type              value_type;
	typedef List::iterator                iterator;
	typedef List::const_iterator          const_iterator;
	typedef List::reverse_iterator        reverse_iterator;
	typedef List::const_reverse_iterator  const_reverse_iterator;

	/**
	 *  @brief  Default constructor
	 */
	Branch();

	/**
	 *  @brief  Constructor with iterators on a branch
	 *
	 *  @param  first the starting iterator of the branch
	 *  @param  last the ending iterator of the branch
	 */
	Branch(const_iterator first, const_iterator last);

	/**
	 *  @brief  Copy constructor
	 */
	Branch(const Branch &branch);

	/**
	 *  @brief  Destructor
	 */
	virtual ~Branch();

	/**
	 *  @brief  Build-up a branch from the calo hit by looking a UNIQUE backward connection.
	 *          Stops when no connection is found.
	 *
	 *  @param  pCaloHit the calo hit to start with
	 */
	pandora::StatusCode BuildFromCaloHit(const CaloHit *const pCaloHit);

	/**
	 *  @brief  Returns an begin iterator on branch
	 */
	iterator begin();

	/**
	 *  @brief  Returns an begin iterator on branch
	 */
	const_iterator begin() const;

	/**
	 *  @brief  Returns an end iterator on branch
	 */
	iterator end();

	/**
	 *  @brief  Returns an end iterator on branch
	 */
	const_iterator end() const;

	/**
	 *  @brief  Returns a begin reverse iterator on branch
	 */
	reverse_iterator rbegin();

	/**
	 *  @brief  Returns a begin reverse iterator on branch
	 */
	const_reverse_iterator rbegin() const;

	/**
	 *  @brief  Returns an end reverse iterator on branch
	 */
	reverse_iterator rend();

	/**
	 *  @brief  Returns an end reverse iterator on branch
	 */
	const_reverse_iterator rend() const;

	/**
	 *  @brief  Clear the branch content
	 */
	void clear();

	/**
	 *  @brief  Returns the size of the branch (n calo hits)
	 */
	size_t size() const;

	/**
	 *  @brief  Add a calo hit to the branch (outermost calo hit).
	 *          A check is performed to test whether the calo hit is connected with the last calo hit
	 *
	 *  @param  pCaloHit the calo hit to add
	 */
	pandora::StatusCode push_back(const CaloHit *const pCaloHit);

	/**
	 *  @brief  Add a calo hit to the branch (innermost calo hit).
	 *          A check is performed to test whether the calo hit is connected with the first calo hit
	 *
	 *  @param  pCaloHit the calo hit to add
	 */
	pandora::StatusCode push_front(const CaloHit *const pCaloHit);

	/**
	 *  @brief  Cut the branch and keep only a part of the branch including the calo hit of the iterator
	 *
	 *  @param  position an iterator pointing on the calo hit from/to which the branch has to be cut
	 *  @param  keepForwardPart whether to keep the forward part of the branch
	 */
	pandora::StatusCode Cut(iterator position, bool keepForwardPart = true);

	/**
	 *  @brief  Whether the branch is still valid in terms of connected calo hits.
	 *          Returns true if the structure still follow a branch structure and false if the branch structure
	 *          is broken (some connections removed)
	 */
	bool IsValid() const;

	/**
	 *  @brief  Assignment operator
	 */
	Branch &operator =(const Branch &branch);

private:

	std::vector<const CaloHit *>         m_caloHitList;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::Branch()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::~Branch()
{
	clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::iterator Branch::begin()
{
	return m_caloHitList.begin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::const_iterator Branch::begin() const
{
	return m_caloHitList.begin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::iterator Branch::end()
{
	return m_caloHitList.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::const_iterator Branch::end() const
{
	return m_caloHitList.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::reverse_iterator Branch::rbegin()
{
	return m_caloHitList.rbegin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::const_reverse_iterator Branch::rbegin() const
{
	return m_caloHitList.rbegin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::reverse_iterator Branch::rend()
{
	return m_caloHitList.rend();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Branch::const_reverse_iterator Branch::rend() const
{
	return m_caloHitList.rend();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Branch::clear()
{
	return m_caloHitList.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline size_t Branch::size() const
{
	return m_caloHitList.size();
}

} 

#endif  //  BRANCH_H
