#include <gtest/gtest.h>

#include <cslibs_ndt_2d/serialization/dynamic_maps/gridmap.hpp>
#include <cslibs_ndt_2d/serialization/dynamic_maps/occupancy_gridmap.hpp>
#include <cslibs_ndt_2d/serialization/static_maps/gridmap.hpp>
#include <cslibs_ndt_2d/serialization/static_maps/occupancy_gridmap.hpp>

#include <cslibs_ndt_2d/conversion/gridmap.hpp>
#include <cslibs_ndt_2d/conversion/occupancy_gridmap.hpp>

#include <cslibs_math/random/random.hpp>
#include <fstream>

const std::size_t MIN_NUM_SAMPLES = 10;
const std::size_t MAX_NUM_SAMPLES = 100;

template <std::size_t Dim>
using rng_t = typename cslibs_math::random::Uniform<double, Dim>;

void testDynamicMap(const typename cslibs_ndt_2d::dynamic_maps::Gridmap<double>::Ptr & map,
                    const typename cslibs_ndt_2d::dynamic_maps::Gridmap<double>::Ptr & map_converted)
{
    EXPECT_NE(map_converted, nullptr);

    EXPECT_NEAR(map->getResolution(),       map_converted->getResolution(),       1e-3);
    EXPECT_NEAR(map->getBundleResolution(), map_converted->getBundleResolution(), 1e-3);
    EXPECT_NEAR(map->getHeight(),           map_converted->getHeight(),           1e-3);
    EXPECT_NEAR(map->getWidth(),            map_converted->getWidth(),            1e-3);

    EXPECT_EQ(map->getMinBundleIndex()[0], map_converted->getMinBundleIndex()[0]);
    EXPECT_EQ(map->getMinBundleIndex()[1], map_converted->getMinBundleIndex()[1]);
    EXPECT_EQ(map->getMaxBundleIndex()[0], map_converted->getMaxBundleIndex()[0]);
    EXPECT_EQ(map->getMaxBundleIndex()[1], map_converted->getMaxBundleIndex()[1]);

    EXPECT_NEAR(map->getOrigin().tx(),         map_converted->getOrigin().tx(),         1e-3);
    EXPECT_NEAR(map->getOrigin().ty(),         map_converted->getOrigin().ty(),         1e-3);
    EXPECT_NEAR(map->getOrigin().yaw(),        map_converted->getOrigin().yaw(),        1e-3);
    EXPECT_NEAR(map->getInitialOrigin().tx(),  map_converted->getInitialOrigin().tx(),  1e-3);
    EXPECT_NEAR(map->getInitialOrigin().ty(),  map_converted->getInitialOrigin().ty(),  1e-3);
    EXPECT_NEAR(map->getInitialOrigin().yaw(), map_converted->getInitialOrigin().yaw(), 1e-3);
    EXPECT_NEAR(map->getMax()(0),              map_converted->getMax()(0),              1e-3);
    EXPECT_NEAR(map->getMax()(1),              map_converted->getMax()(1),              1e-3);

    using index_t = std::array<int, 2>;
    using map_t   = cslibs_ndt_2d::dynamic_maps::Gridmap<double>;
    using db_t    = typename map_t::distribution_bundle_t;
    auto check = [](const typename map_t::Ptr &m1, const typename map_t::Ptr &m2) {
        m1->traverse([&m2](const index_t& bi, const db_t& b) {
            const db_t* bb = m2->getDistributionBundle(bi);
            EXPECT_NE(bb, nullptr);

            for (std::size_t i = 0 ; i < 4 ; ++ i) {
                EXPECT_NE(b.at(i),  nullptr);
                EXPECT_NE(bb->at(i), nullptr);

                const cslibs_math::statistics::StableDistribution<double, 2, 3>/*::Ptr*/ & d/*p*/  = *(b.at(i));//->getDistribution();
                const cslibs_math::statistics::StableDistribution<double, 2, 3>/*::Ptr*/ & dd/*p*/ = *(bb->at(i));//->getDistribution();
/*                EXPECT_NE(dp, nullptr);
                EXPECT_NE(ddp, nullptr);
                const auto& d = *dp;
                const auto& dd = *ddp;*/
                EXPECT_EQ(d.getN(), dd.getN());

                for (std::size_t j = 0 ; j < 2 ; ++ j) {
                    EXPECT_NEAR(d.getMean()(j), dd.getMean()(j), 1e-3);
                    for (std::size_t k = 0 ; k < 2 ; ++ k) {
                        EXPECT_NEAR(d.getScatter()(j, k), dd.getScatter()(j, k), 1e-3);
                        EXPECT_NEAR(d.getCovariance()(j, k), dd.getCovariance()(j, k), 1e-3);
                        EXPECT_NEAR(d.getInformationMatrix()(j, k), dd.getInformationMatrix()(j, k), 1e-3);
                    }
                }
            }
        });
    };

    check(map, map_converted);
    check(map_converted, map);
}

void testStaticMap(const typename cslibs_ndt_2d::static_maps::Gridmap<double>::Ptr & map,
                   const typename cslibs_ndt_2d::static_maps::Gridmap<double>::Ptr & map_converted)
{
    EXPECT_NE(map_converted, nullptr);

    EXPECT_NEAR(map->getResolution(),       map_converted->getResolution(),       1e-3);
    EXPECT_NEAR(map->getBundleResolution(), map_converted->getBundleResolution(), 1e-3);

    EXPECT_EQ(map->getSize()[0],       map_converted->getSize()[0]);
    EXPECT_EQ(map->getSize()[1],       map_converted->getSize()[1]);
    EXPECT_EQ(map->getBundleSize()[0], map_converted->getBundleSize()[0]);
    EXPECT_EQ(map->getBundleSize()[1], map_converted->getBundleSize()[1]);

    EXPECT_NEAR(map->getHeight(),       map_converted->getHeight(),       1e-3);
    EXPECT_NEAR(map->getWidth(),        map_converted->getWidth(),        1e-3);

    EXPECT_NEAR(map->getOrigin().tx(),  map_converted->getOrigin().tx(),  1e-3);
    EXPECT_NEAR(map->getOrigin().ty(),  map_converted->getOrigin().ty(),  1e-3);
    EXPECT_NEAR(map->getOrigin().yaw(), map_converted->getOrigin().yaw(), 1e-3);

    using index_t = std::array<int, 2>;
    using map_t   = cslibs_ndt_2d::static_maps::Gridmap<double>;
    using db_t    = typename map_t::distribution_bundle_t;
    auto check = [](const typename map_t::Ptr &m1, const typename map_t::Ptr &m2) {
        m1->traverse([&m2](const index_t& bi, const db_t& b) {
            const db_t* bb = m2->getDistributionBundle(bi);
            EXPECT_NE(bb, nullptr);

            for (std::size_t i = 0 ; i < 4 ; ++ i) {
                EXPECT_NE(b.at(i),  nullptr);
                EXPECT_NE(bb->at(i), nullptr);

                const cslibs_math::statistics::StableDistribution<double, 2, 3>/*::Ptr*/ & d/*p*/  = *(b.at(i));//->getDistribution();
                const cslibs_math::statistics::StableDistribution<double, 2, 3>/*::Ptr*/ & dd/*p*/ = *(bb->at(i));//->getDistribution();
/*                EXPECT_NE(dp, nullptr);
                EXPECT_NE(ddp, nullptr);
                const auto& d = *dp;
                const auto& dd = *ddp;*/
                EXPECT_EQ(d.getN(), dd.getN());

                for (std::size_t j = 0 ; j < 2 ; ++ j) {
                    EXPECT_NEAR(d.getMean()(j), dd.getMean()(j), 1e-3);
                    for (std::size_t k = 0 ; k < 2 ; ++ k) {
                        EXPECT_NEAR(d.getScatter()(j, k), dd.getScatter()(j, k), 1e-3);
                        EXPECT_NEAR(d.getCovariance()(j, k), dd.getCovariance()(j, k), 1e-3);
                        EXPECT_NEAR(d.getInformationMatrix()(j, k), dd.getInformationMatrix()(j, k), 1e-3);
                    }
                }
            }
        });
    };

    check(map, map_converted);
    check(map_converted, map);
}

void testDynamicOccMap(const typename cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>::Ptr & map,
                       const typename cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>::Ptr & map_converted)
{
    EXPECT_NE(map_converted, nullptr);

    EXPECT_NEAR(map->getResolution(),       map_converted->getResolution(),       1e-3);
    EXPECT_NEAR(map->getBundleResolution(), map_converted->getBundleResolution(), 1e-3);
    EXPECT_NEAR(map->getHeight(),           map_converted->getHeight(),           1e-3);
    EXPECT_NEAR(map->getWidth(),            map_converted->getWidth(),            1e-3);

    EXPECT_EQ(map->getMinBundleIndex()[0], map_converted->getMinBundleIndex()[0]);
    EXPECT_EQ(map->getMinBundleIndex()[1], map_converted->getMinBundleIndex()[1]);
    EXPECT_EQ(map->getMaxBundleIndex()[0], map_converted->getMaxBundleIndex()[0]);
    EXPECT_EQ(map->getMaxBundleIndex()[1], map_converted->getMaxBundleIndex()[1]);

    EXPECT_NEAR(map->getOrigin().tx(),         map_converted->getOrigin().tx(),         1e-3);
    EXPECT_NEAR(map->getOrigin().ty(),         map_converted->getOrigin().ty(),         1e-3);
    EXPECT_NEAR(map->getOrigin().yaw(),        map_converted->getOrigin().yaw(),        1e-3);
    EXPECT_NEAR(map->getInitialOrigin().tx(),  map_converted->getInitialOrigin().tx(),  1e-3);
    EXPECT_NEAR(map->getInitialOrigin().ty(),  map_converted->getInitialOrigin().ty(),  1e-3);
    EXPECT_NEAR(map->getInitialOrigin().yaw(), map_converted->getInitialOrigin().yaw(), 1e-3);
    EXPECT_NEAR(map->getMax()(0),              map_converted->getMax()(0),              1e-3);
    EXPECT_NEAR(map->getMax()(1),              map_converted->getMax()(1),              1e-3);

    using index_t = std::array<int, 2>;
    using map_t   = cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>;
    using db_t    = typename map_t::distribution_bundle_t;
    auto check = [](const typename map_t::Ptr &m1, const typename map_t::Ptr &m2) {
        m1->traverse([&m2](const index_t& bi, const db_t& b) {
            const db_t* bb = m2->getDistributionBundle(bi);
            EXPECT_NE(bb, nullptr);

            for (std::size_t i = 0 ; i < 4 ; ++ i) {
                EXPECT_NE(b.at(i),  nullptr);
                EXPECT_NE(bb->at(i), nullptr);

                EXPECT_EQ(b.at(i)->numFree(), bb->at(i)->numFree());
                EXPECT_EQ(b.at(i)->numOccupied(), bb->at(i)->numOccupied());

                const std::shared_ptr<cslibs_math::statistics::StableDistribution<double, 2, 3>> d  = b.at(i)->getDistribution();
                const std::shared_ptr<cslibs_math::statistics::StableDistribution<double, 2, 3>> dd = bb->at(i)->getDistribution();
                if (d) {
                    EXPECT_NE(dd, nullptr);
                    EXPECT_EQ(d->getN(), dd->getN());

                    for (std::size_t j = 0 ; j < 2 ; ++ j) {
                        EXPECT_NEAR(d->getMean()(j), dd->getMean()(j), 1e-3);
                        for (std::size_t k = 0 ; k < 2 ; ++ k) {
                            EXPECT_NEAR(d->getScatter()(j, k), dd->getScatter()(j, k), 1e-3);
                            EXPECT_NEAR(d->getCovariance()(j, k), dd->getCovariance()(j, k), 1e-3);
                            EXPECT_NEAR(d->getInformationMatrix()(j, k), dd->getInformationMatrix()(j, k), 1e-3);
                        }
                    }
                } else
                    EXPECT_EQ(dd, nullptr);
            }
        });
    };

    check(map, map_converted);
    check(map_converted, map);
}

void testStaticOccMap(const typename cslibs_ndt_2d::static_maps::OccupancyGridmap<double>::Ptr & map,
                      const typename cslibs_ndt_2d::static_maps::OccupancyGridmap<double>::Ptr & map_converted)
{
    EXPECT_NE(map_converted, nullptr);

    EXPECT_NEAR(map->getResolution(),       map_converted->getResolution(),       1e-3);
    EXPECT_NEAR(map->getBundleResolution(), map_converted->getBundleResolution(), 1e-3);

    EXPECT_EQ(map->getSize()[0],       map_converted->getSize()[0]);
    EXPECT_EQ(map->getSize()[1],       map_converted->getSize()[1]);
    EXPECT_EQ(map->getBundleSize()[0], map_converted->getBundleSize()[0]);
    EXPECT_EQ(map->getBundleSize()[1], map_converted->getBundleSize()[1]);

    EXPECT_NEAR(map->getHeight(),       map_converted->getHeight(),       1e-3);
    EXPECT_NEAR(map->getWidth(),        map_converted->getWidth(),        1e-3);

    EXPECT_NEAR(map->getOrigin().tx(),  map_converted->getOrigin().tx(),  1e-3);
    EXPECT_NEAR(map->getOrigin().ty(),  map_converted->getOrigin().ty(),  1e-3);
    EXPECT_NEAR(map->getOrigin().yaw(), map_converted->getOrigin().yaw(), 1e-3);

    using index_t = std::array<int, 2>;
    using map_t   = cslibs_ndt_2d::static_maps::OccupancyGridmap<double>;
    using db_t    = typename map_t::distribution_bundle_t;
    auto check = [](const typename map_t::Ptr &m1, const typename map_t::Ptr &m2) {
        m1->traverse([&m2](const index_t& bi, const db_t& b) {
            const db_t* bb = m2->getDistributionBundle(bi);
            EXPECT_NE(bb, nullptr);

            for (std::size_t i = 0 ; i < 4 ; ++ i) {
                EXPECT_NE(b.at(i),  nullptr);
                EXPECT_NE(bb->at(i), nullptr);

                EXPECT_EQ(b.at(i)->numFree(), bb->at(i)->numFree());
                EXPECT_EQ(b.at(i)->numOccupied(), bb->at(i)->numOccupied());

                const std::shared_ptr<cslibs_math::statistics::StableDistribution<double, 2, 3>> d  = b.at(i)->getDistribution();
                const std::shared_ptr<cslibs_math::statistics::StableDistribution<double, 2, 3>> dd = bb->at(i)->getDistribution();
                if (d) {
                    EXPECT_NE(dd, nullptr);
                    EXPECT_EQ(d->getN(), dd->getN());

                    for (std::size_t j = 0 ; j < 2 ; ++ j) {
                        EXPECT_NEAR(d->getMean()(j), dd->getMean()(j), 1e-3);
                        for (std::size_t k = 0 ; k < 2 ; ++ k) {
                            EXPECT_NEAR(d->getScatter()(j, k), dd->getScatter()(j, k), 1e-3);
                            EXPECT_NEAR(d->getCovariance()(j, k), dd->getCovariance()(j, k), 1e-3);
                            EXPECT_NEAR(d->getInformationMatrix()(j, k), dd->getInformationMatrix()(j, k), 1e-3);
                        }
                    }
                } else
                    EXPECT_EQ(dd, nullptr);
            }
        });
    };

    check(map, map_converted);
    check(map_converted, map);
}

cslibs_ndt_2d::dynamic_maps::Gridmap<double>::Ptr generateDynamicMap()
{
    using map_t = cslibs_ndt_2d::dynamic_maps::Gridmap<double>;
    rng_t<1> rng_coord(-100.0, 100.0);

    // fill map
    cslibs_math_2d::Transform2d origin(rng_coord.get(), rng_coord.get(), rng_t<1>(-M_PI, M_PI).get());
    const double resolution = rng_t<1>(1.0, 5.0).get();
    typename map_t::Ptr map(new map_t(origin, resolution));
    const int num_samples = static_cast<int>(rng_t<1>(MIN_NUM_SAMPLES, MAX_NUM_SAMPLES).get());

    cslibs_math_2d::Pointcloud2<double>::Ptr cloud(new cslibs_math_2d::Pointcloud2<double>());
    for (int i = 0 ; i < num_samples ; ++ i) {
        const cslibs_math_2d::Point2d p(rng_coord.get(), rng_coord.get());
        cloud->insert(p);
    }
    map->insert(cloud);
    return map;
}

cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>::Ptr generateDynamicOccMap()
{
    using map_t = cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>;
    rng_t<1> rng_coord(-10.0, 10.0);

    // fill map
    cslibs_math_2d::Transform2d origin(rng_coord.get(), rng_coord.get(), rng_t<1>(-M_PI, M_PI).get());
    const double resolution = rng_t<1>(1.0, 5.0).get();
    typename map_t::Ptr map(new map_t(origin, resolution));
    const int num_samples = static_cast<int>(rng_t<1>(MIN_NUM_SAMPLES, MAX_NUM_SAMPLES).get());

    cslibs_math_2d::Pointcloud2<double>::Ptr cloud(new cslibs_math_2d::Pointcloud2<double>());
    cslibs_math_2d::Transform2d start(rng_coord.get(), rng_coord.get(), rng_t<1>(-M_PI, M_PI).get());
    for (int i = 0 ; i < num_samples ; ++ i) {
        const cslibs_math_2d::Point2d p(rng_coord.get(), rng_coord.get());
        cloud->insert(p);
    }
    map->insert(cloud, start);
    return map;
}

TEST(Test_cslibs_ndt_2d, testDynamicGridmapConversion)
{
    using map_t = cslibs_ndt_2d::dynamic_maps::Gridmap<double>;
    const typename map_t::Ptr map = generateDynamicMap();

    // conversion
    const typename map_t::Ptr & map_double_converted =
            cslibs_ndt_2d::conversion::from<double>(cslibs_ndt_2d::conversion::from<double>(map));

    EXPECT_NE(map_double_converted, nullptr);
    testDynamicMap(map, map_double_converted);
}

TEST(Test_cslibs_ndt_2d, testDynamicOccupancyGridmapConversion)
{
    using map_t = cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>;
    const typename map_t::Ptr map = generateDynamicOccMap();

    // conversion
    const typename map_t::Ptr & map_double_converted =
            cslibs_ndt_2d::conversion::from<double>(cslibs_ndt_2d::conversion::from<double>(map));

    EXPECT_NE(map_double_converted, nullptr);
    testDynamicOccMap(map, map_double_converted);
}

TEST(Test_cslibs_ndt_2d, testStaticGridmapConversion)
{
    using tmp_map_t = cslibs_ndt_2d::dynamic_maps::Gridmap<double>;
    const typename tmp_map_t::Ptr tmp_map = generateDynamicMap();

    using map_t = cslibs_ndt_2d::static_maps::Gridmap<double>;
    const typename map_t::Ptr map = cslibs_ndt_2d::conversion::from<double>(tmp_map);

    // conversion
    const typename map_t::Ptr & map_double_converted =
            cslibs_ndt_2d::conversion::from<double>(cslibs_ndt_2d::conversion::from<double>(map));

    EXPECT_NE(map_double_converted, nullptr);
    testStaticMap(map, map_double_converted);
}

TEST(Test_cslibs_ndt_2d, testStaticOccupancyGridmapConversion)
{
    using tmp_map_t = cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>;
    const typename tmp_map_t::Ptr tmp_map = generateDynamicOccMap();

    using map_t = cslibs_ndt_2d::static_maps::OccupancyGridmap<double>;
    const typename map_t::Ptr map = cslibs_ndt_2d::conversion::from<double>(tmp_map);

    // conversion
    const typename map_t::Ptr & map_double_converted =
            cslibs_ndt_2d::conversion::from<double>(cslibs_ndt_2d::conversion::from<double>(map));

    EXPECT_NE(map_double_converted, nullptr);
    testStaticOccMap(map, map_double_converted);
}

TEST(Test_cslibs_ndt_2d, testDynamicGridmapFileBinarySerialization)
{
    using map_t = cslibs_ndt_2d::dynamic_maps::Gridmap<double>;
    const typename map_t::Ptr map = generateDynamicMap();

    // to file
    cslibs_ndt_2d::dynamic_maps::saveBinary<double>(map, "/tmp/dynamic_map_binary_2d");

    // from file
    typename map_t::Ptr map_from_file;
    const bool success = cslibs_ndt_2d::dynamic_maps::loadBinary<double>("/tmp/dynamic_map_binary_2d", map_from_file);

    // tests
    EXPECT_TRUE(success);
    testDynamicMap(map, map_from_file);
}

TEST(Test_cslibs_ndt_2d, testDynamicOccupancyGridmapFileBinarySerialization)
{
    using map_t = cslibs_ndt_2d::dynamic_maps::OccupancyGridmap<double>;
    const typename map_t::Ptr map = generateDynamicOccMap();

    // to file
    cslibs_ndt_2d::dynamic_maps::saveBinary<double>(map, "/tmp/dynamic_occ_map_binary_2d");

    // from file
    typename map_t::Ptr map_from_file;
    const bool success = cslibs_ndt_2d::dynamic_maps::loadBinary<double>("/tmp/dynamic_occ_map_binary_2d", map_from_file);

    // tests
    EXPECT_TRUE(success);
    testDynamicOccMap(map, map_from_file);
}

TEST(Test_cslibs_ndt_2d, testStaticGridmapFileBinarySerialization)
{
    using map_t = cslibs_ndt_2d::static_maps::Gridmap<double>;
    const typename map_t::Ptr map = cslibs_ndt_2d::conversion::from<double>(generateDynamicMap());

    // to file
    cslibs_ndt_2d::static_maps::saveBinary<double>(map, "/tmp/static_map_binary_2d");

    // from file
    typename map_t::Ptr map_from_file;
    const bool success = cslibs_ndt_2d::static_maps::loadBinary<double>("/tmp/static_map_binary_2d", map_from_file);

    // tests
    EXPECT_TRUE(success);
    testStaticMap(map, map_from_file);
}

TEST(Test_cslibs_ndt_2d, testStaticOccupancyGridmapFileBinarySerialization)
{
    using map_t = cslibs_ndt_2d::static_maps::OccupancyGridmap<double>;
    const typename map_t::Ptr map = cslibs_ndt_2d::conversion::from<double>(generateDynamicOccMap());

    // to file
    cslibs_ndt_2d::static_maps::saveBinary<double>(map, "/tmp/static_occ_map_binary_2d");

    // from file
    typename map_t::Ptr map_from_file;
    const bool success = cslibs_ndt_2d::static_maps::loadBinary<double>("/tmp/static_occ_map_binary_2d", map_from_file);

    // tests
    EXPECT_TRUE(success);
    testStaticOccMap(map, map_from_file);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
