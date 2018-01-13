#ifndef CSLIBS_NDT_3D_CONVERSION_OCCUPANCY_GRIDMAP_HPP
#define CSLIBS_NDT_3D_CONVERSION_OCCUPANCY_GRIDMAP_HPP

#include <cslibs_ndt_3d/dynamic_maps/occupancy_gridmap.hpp>
#include <cslibs_ndt_3d/static_maps/occupancy_gridmap.hpp>

namespace cslibs_ndt_3d {
namespace conversion {
inline cslibs_ndt_3d::dynamic_maps::OccupancyGridmap::Ptr from(
        const cslibs_ndt_3d::static_maps::OccupancyGridmap::Ptr& src)
{
    if (!src)
        return nullptr;

    using src_map_t = cslibs_ndt_3d::static_maps::OccupancyGridmap;
    using dst_map_t = cslibs_ndt_3d::dynamic_maps::OccupancyGridmap;
    typename dst_map_t::Ptr dst(new dst_map_t(src->getOrigin(),
                                              src->getResolution()));

    using index_t = std::array<int, 3>;
    for (int idx = 0 ; idx < static_cast<int>(src->getBundleSize()[0]) ; ++ idx) {
        for (int idy = 0 ; idy < static_cast<int>(src->getBundleSize()[1]) ; ++ idy) {
            for (int idz = 0 ; idz < static_cast<int>(src->getBundleSize()[2]) ; ++ idz) {
                const index_t bi({idx, idy, idz});

                if (const typename src_map_t::distribution_bundle_t* b = src->getDistributionBundle(bi)) {
                    if (const typename dst_map_t::distribution_bundle_t* b_dst = dst->getDistributionBundle(bi)) {

                        for (std::size_t i = 0 ; i < 8 ; ++ i)
                            if (b->at(i) && (b->at(i)->numFree() > 0 || b->at(i)->numOccupied() > 0))//(b->at(i)->data().getN() > 0 && b_dst->at(i)->data().getN() == 0)
                                *(b_dst->at(i)) = *(b->at(i));
                    }
                }
            }
        }
    }

    return dst;
}

inline cslibs_ndt_3d::static_maps::OccupancyGridmap::Ptr from(
        const cslibs_ndt_3d::dynamic_maps::OccupancyGridmap::Ptr& src)
{
    if (!src)
        return nullptr;

    using index_t = std::array<int, 3>;
    const index_t min_distribution_index = src->getMinDistributionIndex();
    const index_t max_distribution_index = src->getMaxDistributionIndex();

    const std::array<std::size_t, 3> size =
    {{static_cast<std::size_t>(std::ceil((src->getMax()(0) - src->getMin()(0)) / src->getResolution())),
      static_cast<std::size_t>(std::ceil((src->getMax()(1) - src->getMin()(1)) / src->getResolution())),
      static_cast<std::size_t>(std::ceil((src->getMax()(2) - src->getMin()(2)) / src->getResolution()))}};

    auto get_bundle_index = [&size, &min_distribution_index] (const index_t & bi) {
        return index_t{{bi[0] - min_distribution_index[0],
                        bi[1] - min_distribution_index[1],
                        bi[2] - min_distribution_index[2]}};
    };

    using src_map_t = cslibs_ndt_3d::dynamic_maps::OccupancyGridmap;
    using dst_map_t = cslibs_ndt_3d::static_maps::OccupancyGridmap;
    typename dst_map_t::Ptr dst(new dst_map_t(src->getOrigin(),
                                              src->getResolution(),
                                              size));

    for (int idx = min_distribution_index[0] ; idx <= max_distribution_index[0] ; ++ idx) {
        for (int idy = min_distribution_index[1] ; idy <= max_distribution_index[1] ; ++ idy) {
            for (int idz = min_distribution_index[2] ; idz <= max_distribution_index[2] ; ++ idz) {
                const index_t bi({idx, idy, idz});

                if (const typename src_map_t::distribution_bundle_t* b = src->getDistributionBundle(bi)) {
                    const index_t bi_dst = get_bundle_index(bi);
                    if (const typename dst_map_t::distribution_bundle_t* b_dst = dst->getDistributionBundle(bi_dst)) {

                        for (std::size_t i = 0 ; i < 8 ; ++ i)
                            if (b->at(i) && (b->at(i)->numFree() > 0 || b->at(i)->numOccupied() > 0))
                                *(b_dst->at(i)) = *(b->at(i));
                    }
                }
            }
        }
    }

    return dst;
}
}
}

#endif // CSLIBS_NDT_3D_CONVERSION_OCCUPANCY_GRIDMAP_HPP