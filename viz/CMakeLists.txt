rock_vizkit_plugin(point_cloud-viz
    PointCloud.cpp PointCloudVisualization.cpp PointCloudVisualization.hpp
    DEPS_PKGCONFIG base-types base-viz
    MOC PointCloud.h PointCloudVisualization.hpp
    HEADERS PointCloud.h PointCloudVisualization.hpp)
