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
 ** \file AlignerBase.hh
 **
 ** \brief Helper functionality for various read aligners
 ** 
 ** \author Roman Petrovski
 **/

#ifndef iSAAC_ALIGNMENT_FRAGMENT_BUILDER_ALIGNER_BASE_HH
#define iSAAC_ALIGNMENT_FRAGMENT_BUILDER_ALIGNER_BASE_HH

#include "alignment/Cigar.hh"
#include "alignment/Cluster.hh"
#include "alignment/FragmentMetadata.hh"
#include "alignment/templateBuilder/FragmentSequencingAdapterClipper.hh"
#include "reference/Contig.hh"

namespace isaac
{
namespace alignment
{
namespace templateBuilder
{

/**
 ** \brief Utility component creating and scoring all Fragment instances from a
 ** list Seed Matches for a single Cluster (each Read independently).
 **/
class AlignerBase: public boost::noncopyable
{
public:
    AlignerBase(
        const bool collectMismatchCycles,
        const AlignmentCfg &alignmentCfg);

    static void clipReference(
        const int64_t referenceSize,
        int64_t &fragmentPosition,
        std::vector<char>::const_iterator &sequenceBegin,
        std::vector<char>::const_iterator &sequenceEnd);

protected:
    const bool collectMismatchCycles_;
    const AlignmentCfg &alignmentCfg_;

    unsigned updateFragmentCigar(
        const flowcell::ReadMetadata &readMetadata,
        const reference::ContigList &contigList,
        FragmentMetadata &fragmentMetadata,
        const bool reverse,
        const unsigned contigId,
        const int64_t strandPosition,
        const Cigar &cigarBuffer,
        const unsigned cigarOffset) const;

    /**
     * \brief Sets the sequence iterators according to the masking information stored in the read.
     *        Adjusts fragment.position to point at the first non-clipped base.
     *
     */
    static void clipReadMasking(
        const alignment::Read &read,
        FragmentMetadata &fragment,
        std::vector<char>::const_iterator &sequenceBegin,
        std::vector<char>::const_iterator &sequenceEnd);
};

} // namespace templateBuilder
} // namespace alignment
} // namespace isaac

#endif // #ifndef iSAAC_ALIGNMENT_FRAGMENT_BUILDER_ALIGNER_BASE_HH
