/**
 ** Isaac Genome Alignment Software
 ** Copyright (c) 2010-2017 Illumina, Inc.
 ** All rights reserved.
 **
 ** This software is provided under the terms and conditions of the
 ** GNU GENERAL PUBLIC LICENSE Version 3
 **
 ** You should have received a copy of the GNU GENERAL PUBLIC LICENSE Version 3
 ** along with this program. If not, see
 ** <https://github.com/illumina/licenses/>.
 **
 ** \file MatchFinderTileStats.hh
 **
 ** \brief MatchFinder statistics helper.
 **
 ** \author Roman Petrovski
 **/

#ifndef ISAAC_STATISTICS_MATCH_FINDER_TILE_STATS_H
#define ISAAC_STATISTICS_MATCH_FINDER_TILE_STATS_H

namespace isaac
{
namespace statistics{

struct MatchFinderTileStats
{
    MatchFinderTileStats() :
        uniqueMatchSeeds_(0), noMatchSeeds_(0), repeatMatchSeeds_(0), tooManyRepeatsSeeds_(0), repeatMatches_(0){}
    uint64_t uniqueMatchSeeds_;
    uint64_t noMatchSeeds_;
    uint64_t repeatMatchSeeds_;
    uint64_t tooManyRepeatsSeeds_;
    uint64_t repeatMatches_;
};

inline const MatchFinderTileStats operator +(MatchFinderTileStats left, const MatchFinderTileStats &right)
{
    left.uniqueMatchSeeds_ += right.uniqueMatchSeeds_;
    left.noMatchSeeds_ += right.noMatchSeeds_;
    left.repeatMatchSeeds_ += right.repeatMatchSeeds_;
    left.tooManyRepeatsSeeds_ += right.tooManyRepeatsSeeds_;
    left.repeatMatches_ += right.repeatMatches_;
    return left;
}

} //namespace statistics
} //namespace isaac

#endif //ISAAC_STATISTICS_MATCH_FINDER_TILE_STATS_H
