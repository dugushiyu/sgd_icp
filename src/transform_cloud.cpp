/*
 Copyright (c) 2019
 
 Fahira Afzal Maken and Lionel Ott,
 The University of Sydney, Australia.
 
 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */




#include <boost/program_options.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include <pcl/common/transforms.h>


namespace po = boost::program_options;

using Point_t = pcl::PointXYZ;
using Cloud_t = pcl::PointCloud<Point_t>;


int main(int argc, char * argv[])
{
    // +------------------------------------------------------------------------
    // | Parse command line arguments
    // +------------------------------------------------------------------------

    po::options_description desc("Options");
    desc.add_options()
        ("help", "Show the help message")
        ("input", po::value<std::string>(), "Input point cloud")
        ("output", po::value<std::string>(), "Output point cloud")
        ("x", po::value<float>(), "Translation along X")
        ("y", po::value<float>(), "Translation along Y")
        ("z", po::value<float>(), "Translation along Z")
        ("roll", po::value<float>(), "Rotation around roll axis")
        ("pitch", po::value<float>(), "Rotation around pitch axis")
        ("yaw", po::value<float>(), "Rotation around yaw axis")
    ;

    // Configure positional arguments
    po::positional_options_description pos_args;
    pos_args.add("input", 1);
    pos_args.add("output", 1);
    pos_args.add("x", 1);
    pos_args.add("y", 1);
    pos_args.add("z", 1);
    pos_args.add("roll", 1);
    pos_args.add("pitch", 1);
    pos_args.add("yaw", 1);

    po::variables_map vm;
    po::store(
            po::command_line_parser(argc, argv)
                .options(desc)
                .positional(pos_args)
                .run(),
            vm
    );
    po::notify(vm);

    if(vm.count("help") || !vm.count("input") || !vm.count("output") ||
       !vm.count("x") || !vm.count("y") || !vm.count("z") ||
       !vm.count("roll") || !vm.count("pitch") || !vm.count("yaw")
    )
    {
        std::cout << desc << std::endl;
        return 1;
    }


    // +------------------------------------------------------------------------
    // | Transforming of the input cloud
    // +------------------------------------------------------------------------
    Cloud_t::Ptr input(new Cloud_t);
    pcl::io::loadPCDFile<Point_t>(vm["input"].as<std::string>(), *input);

    Eigen::Quaternionf rotation(Eigen::Matrix3f(
                    Eigen::AngleAxisf(vm["yaw"].as<float>(), Eigen::Vector3f::UnitZ()) *
                    Eigen::AngleAxisf(vm["pitch"].as<float>(), Eigen::Vector3f::UnitY()) *
                    Eigen::AngleAxisf(vm["roll"].as<float>(), Eigen::Vector3f::UnitX())
    ));
    Eigen::Vector3f offset(
            vm["x"].as<float>(),
            vm["y"].as<float>(),
            vm["z"].as<float>()
    );

    Cloud_t::Ptr output(new Cloud_t);
    pcl::transformPointCloud<Point_t>(*input, *output, offset, rotation);

    pcl::io::savePCDFile(vm["output"].as<std::string>(), *output);
}
