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
 ** \file SortedReferenceMetadata.hh
 **
 ** Information about the pre-processed reference data files.
 **
 ** \author Roman Petrovski
 **/

#ifndef iSAAC_REFERENCE_SORTED_REFERENCE_METADATA_HH
#define iSAAC_REFERENCE_SORTED_REFERENCE_METADATA_HH

#include <numeric>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "oligo/Kmer.hh"
#include "reference/ReferencePosition.hh"

namespace isaac
{
namespace reference
{

class SortedReferenceMetadata
{
public:
    static const unsigned OLDEST_SUPPORTED_REFERENCE_FORMAT_VERSION = 3;
    static const unsigned CURRENT_REFERENCE_FORMAT_VERSION = 9;

    struct Contig
    {
        Contig() : index_(0), decoy_(false), offset_(0), size_(0), genomicPosition_(0), totalBases_(0), acgtBases_(0){}
        Contig(const unsigned int index, const std::string &name, const bool decoy,
               const boost::filesystem::path &filePath, const uint64_t offset, const uint64_t size,
               const uint64_t genomicPosition, const uint64_t totalBases, const uint64_t acgtBases,
               const std::string &bamSqAs, const std::string &bamSqUr, const std::string &bamM5) :
                   index_(index), name_(name), decoy_(decoy), filePath_(filePath),
                   offset_(offset), size_(size),
                   genomicPosition_(genomicPosition), totalBases_(totalBases), acgtBases_(acgtBases),
                   bamSqAs_(bamSqAs), bamSqUr_(bamSqUr), bamM5_(bamM5) {}
        unsigned int index_;
        std::string name_;
        bool decoy_;
        boost::filesystem::path filePath_;
        uint64_t offset_;
        uint64_t size_;
        uint64_t genomicPosition_;
        uint64_t totalBases_;
        uint64_t acgtBases_;
        std::string bamSqAs_;
        std::string bamSqUr_;
        std::string bamM5_;

        bool operator == (const Contig &that) const
        {
            return index_ == that.index_ &&
                name_ == that.name_ &&
                decoy_ == that.decoy_ &&
                ((bamM5_.empty() && filePath_ == that.filePath_) || bamM5_ == that.bamM5_) &&
                offset_ == that.offset_ &&
                size_ == that.size_ && genomicPosition_ == that.genomicPosition_ && totalBases_ == that.totalBases_ &&
                acgtBases_ == that.acgtBases_ && bamSqAs_ == that.bamSqAs_ && bamSqUr_ == that.bamSqUr_;
        }


        friend std::ostream &operator <<(std::ostream &os, const isaac::reference::SortedReferenceMetadata::Contig &xmlContig)
        {
            return os << "SortedReferenceMetadata::Contig(" <<
                xmlContig.name_ << "," <<
                xmlContig.genomicPosition_ << "pos," <<
                xmlContig.totalBases_ << "tb," <<
                xmlContig.offset_ << "off" <<
                ")";
        }

    };
    typedef std::vector<Contig> Contigs;

    struct MaskFile
    {
        MaskFile(): maskWidth(0), mask_(0), kmers(0){}
        MaskFile(
            const boost::filesystem::path &p,
            const unsigned mw,
            const unsigned m,
            const std::size_t km) : path(p), maskWidth(mw), mask_(m), kmers(km){}
        boost::filesystem::path path;
        unsigned maskWidth;
        unsigned mask_;
        std::size_t kmers;
        template<class Archive> friend void serialize(Archive & ar, MaskFile &, const unsigned int file_version);
    };
    typedef std::vector<MaskFile> MaskFiles;
    typedef std::map<unsigned, MaskFiles> AllMaskFiles;

    struct AnnotationFile
    {
        enum Type
        {
            Unknown,
            KUniqueness,  //number of consecutive matches required to have small number of distance-K neighbors and 0 repeats
            KRepeatness,  //number of consecutive matches required to have no neighbors
        };
        AnnotationFile(): type_(Unknown), k_(0){}
        AnnotationFile(
            const Type type,
            const boost::filesystem::path &p,
            const unsigned k) : type_(type), path_(p), k_(k){}
        Type type_;
        boost::filesystem::path path_;
        unsigned k_;
        template<class Archive> friend void serialize(Archive & ar, AnnotationFile &, const unsigned int file_version);
        friend std::ostream& operator <<(std::ostream &os, const AnnotationFile& annotationFile)
        {
            return os << "AnnotationFile(" <<
                annotationFile.k_ << "," << annotationFile.path_ <<
                ")";
        }
    };
    typedef std::vector<AnnotationFile> AnnotationFiles;

private:
    AllMaskFiles maskFiles_;
    AnnotationFiles annotationFiles_;
    Contigs contigs_;
    unsigned formatVersion_;

public:
    SortedReferenceMetadata() :
        formatVersion_(CURRENT_REFERENCE_FORMAT_VERSION)
    {
    }

    void makeAbsolutePaths(const boost::filesystem::path &basePath);

    void putContig(const Contig &contig);

    void putContig(const uint64_t genomicOffset,
                   const std::string& name,
                   const boost::filesystem::path &sequencePath,
                   const uint64_t byteOffset,
                   const uint64_t byteSize,
                   const uint64_t totalBases,
                   const uint64_t acgtBases,
                   const unsigned index,
                   const std::string &bamSqAs,
                   const std::string &bamSqUr,
                   const std::string &bamM5);
    void addMaskFile(
        const unsigned seedLength,
        const unsigned int maskWidth,
        const unsigned mask, const boost::filesystem::path &filePath,
        const std::size_t kmers);

    /**
     ** Precondition: the contigs in the current instance are sequentially
     ** indexed from 0 and there are no duplicates.
     **/
    const Contigs &getContigs() const {return contigs_;}
    Contigs &getContigs() {return contigs_;}

    std::size_t getContigsCount() const {return contigs_.size();}

    /**
     ** \brief Return the number of contigs for which includeContig() returns true
     **/
    template <typename IncludeContigF>
    std::size_t getFilteredContigsCount(
        const IncludeContigF &includeContig) const
    {
        return std::count_if(contigs_.begin(), contigs_.end(),
                             boost::bind(includeContig, boost::bind(&Contig::index_, _1)));
    }

    /**
     ** \brief Return a list of contig where each contig is at the corresponding karyotype index.
     **        Only the contigs for which includeContig() returns true are included
     **
     ** Precondition: the contigs in the current instance are sequentially
     ** indexed from 0 and there are no duplicates.
     **/
    template <typename IncludeContigF>
    Contigs getFilteredContigs(
        IncludeContigF &includeContig) const
    {
        Contigs ret;
        ret.reserve(contigs_.size());
        std::remove_copy_if(contigs_.begin(), contigs_.end(), std::back_inserter(ret),
                            !boost::bind(includeContig, boost::bind(&Contig::index_, _1)));
        return ret;
    }


    /**
     ** \return Total number of kmers in all mask files
     **/
    uint64_t getTotalKmers(const unsigned seedLength) const;

    bool supportsSeedLength(const unsigned seedLength) const
    {
        return maskFiles_.end() != maskFiles_.find(seedLength);
    }
    const MaskFiles &getMaskFileList(const unsigned seedLength) const {return maskFiles_.at(seedLength);}
    MaskFiles &getMaskFileList(const unsigned seedLength) {return maskFiles_[seedLength];}

    bool hasKUniquenessAnnotation() const {return hasAnnotation(AnnotationFile::KUniqueness);}
    const AnnotationFile& getKUniquenessAnnotation() const {return getAnnotation(AnnotationFile::KUniqueness);}
    void setKUniquenessAnnotation(const boost::filesystem::path &path, const unsigned k)
        {setAnnotation(AnnotationFile::KUniqueness, path, k);}

    bool hasKRepeatnessAnnotation() const {return hasAnnotation(AnnotationFile::KRepeatness);}
    const AnnotationFile& getKRepeatnessAnnotation() const {return getAnnotation(AnnotationFile::KRepeatness);}
    void setKRepeatnessAnnotation(const boost::filesystem::path &path, const unsigned k)
        {setAnnotation(AnnotationFile::KRepeatness, path, k);}

    void clearAnnotations() {annotationFiles_.clear();}

    void clearMasks() {maskFiles_.clear();}

    void merge(SortedReferenceMetadata &that);

    bool singleFileReference() const;

    template<class Archive> friend void serialize(Archive & ar, SortedReferenceMetadata &, const unsigned int file_version);
    template<class Archive> friend void serialize(Archive & ar, const SortedReferenceMetadata &, const unsigned int file_version);

private:
    bool hasAnnotation(const AnnotationFile::Type type) const {return annotationFiles_.end() !=
        std::find_if(annotationFiles_.begin(), annotationFiles_.end(), boost::bind(&AnnotationFile::type_, _1) == type);}

    const AnnotationFile& getAnnotation(const AnnotationFile::Type type) const
        {ISAAC_ASSERT_MSG(hasAnnotation(type), "Annotation type " << type << " requested for reference that does not have one");
        return *std::find_if(annotationFiles_.begin(), annotationFiles_.end(), boost::bind(&AnnotationFile::type_, _1) == type);}
    AnnotationFile& getAnnotation(const AnnotationFile::Type type)
        {ISAAC_ASSERT_MSG(hasAnnotation(type), "Annotation type " << type << " requested for reference that does not have one");
        return *std::find_if(annotationFiles_.begin(), annotationFiles_.end(), boost::bind(&AnnotationFile::type_, _1) == type);}

    void setAnnotation(const AnnotationFile::Type type, const boost::filesystem::path &path, const unsigned k)
    {
        if (!hasAnnotation(type))
        {
            annotationFiles_.push_back(AnnotationFile(type, path, k));
        }
        else
        {
            getAnnotation(type) = AnnotationFile(type, path, k);
        }
    }

};

typedef std::vector<SortedReferenceMetadata> SortedReferenceMetadataList;

inline std::size_t genomeLength(const SortedReferenceMetadata::Contigs &contigList)
{
    return std::accumulate(
        contigList.begin(), contigList.end(),
        std::size_t(0), boost::bind<std::size_t>(std::plus<std::size_t>(), _1, boost::bind(&SortedReferenceMetadata::Contig::totalBases_, _2)));
}


inline uint64_t getLongestGenomeLength(const isaac::reference::SortedReferenceMetadataList &sortedReferenceMetadataList)
{
    std::size_t longestGenome = 0;
    BOOST_FOREACH(const isaac::reference::SortedReferenceMetadata &sortedReferenceMetadata, sortedReferenceMetadataList)
    {
        const std::vector<isaac::reference::SortedReferenceMetadata::Contig> xmlContigs = sortedReferenceMetadata.getContigs();
        const std::size_t genomeLength = reference::genomeLength(xmlContigs);
        longestGenome = std::max(longestGenome, genomeLength);
    }
    return longestGenome;
}

/**
 * \brief Translate from genomic offset to reference position. Not particularly fast as it uses binary search to
 *        locate the relevant contig.
 * \param       genomicOffset 0-based offset from the first base of the first contig in the contigList
 * \param       contigList genomicPosition_ ordered list of contigs to search
 * \return      the genomic position given the offset from the first reference base
 */
inline ReferencePosition genomicOffsetToPosition(uint64_t genomicOffset, const SortedReferenceMetadata::Contigs &contigList)
{
    const SortedReferenceMetadata::Contigs::const_iterator ub = std::upper_bound(
        contigList.begin(), contigList.end(), genomicOffset,
        [](uint64_t genomicOffset, const SortedReferenceMetadata::Contig &contig){return genomicOffset < contig.genomicPosition_;});
    ISAAC_ASSERT_MSG(contigList.begin() != ub, "upper_bound returns first element of 0-based list. Empty?:" << contigList.size());
    const SortedReferenceMetadata::Contig &contig = *(ub - 1);
    if (genomicOffset - contig.genomicPosition_ < contig.totalBases_)
    {
        return ReferencePosition(contig.index_, genomicOffset - contig.genomicPosition_);
    }
//    ISAAC_THREAD_CERR << "genomicOffset:" << genomicOffset << " " << contigList.back().name_ << " " << contigList.back().genomicPosition_ << std::endl;
    return ReferencePosition(ReferencePosition::NoMatch);
}


} // namespace reference
} // namespace isaac

#endif // #ifndef iSAAC_REFERENCE_SORTED_REFERENCE_METADATA_HH
