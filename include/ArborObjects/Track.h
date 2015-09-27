  /// \file Track.h
/*
 *
 * Track.h header template automatically generated by a class generator
 * Creation date : jeu. sept. 24 2015
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


#ifndef TRACK_H
#define TRACK_H

// -- pandora sdk headers
#include "Pandora/StatusCodes.h"
#include "Objects/Track.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraContentApi.h"

#include "ArborApi/ArborContentApi.h"
#include "ArborApi/ArborInputTypes.h"

namespace arbor_content
{

/** 
 * @brief Track class
 */
class Track : public pandora::Track
{
 public:
	/**
	 *  @brief Constructor with track parameters
	 */
	Track(const PandoraApi::Track::Parameters &parameters);

	/**
	 *  @brief Destructor
	 */
	virtual ~Track();

	/**
	 *  @brief Get the nearby track list
	 */
	const TrackList &GetNearbyTrackList() const;

protected:

	/**
	 *  @brief Add a nearby track
	 */
	pandora::StatusCode AddNearbyTrack(const Track *pNearbyTrack);

	/**
	 *  @brief Remove a nearby track
	 */
	pandora::StatusCode RemoveNearbyTrack(const Track *pNearbyTrack);

	/**
	 *  @brief Remove all nearby tracks
	 */
	pandora::StatusCode RemoveAllNearbyTracks();

private:

	TrackList            m_nearbyTrackList;      ///< The list of nearby tracks
}; 

} 

#endif  //  TRACK_H
