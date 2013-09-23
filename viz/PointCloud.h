#ifndef GUI_POINTCLOUD_H
#define GUI_POINTCLOUD_H

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <QObject>
#include <QDir>



/**
 * @brief This class is used for creating a visualization of a robot model and access it.
 *
 */
class PointCloud
{
public:
/**
 * @brief Constructor, does nearly nothing.
 *
 */
    PointCloud();

    /**
     * @brief
     *
     */
    ~PointCloud(){}

    /**
     * @brief Create visualization of a file
     *
     * @param path: Path to file.
     * @return osg::Node: Root node of the constructed OSG scene for the robot.
     */
    osg::ref_ptr<osg::Node> load(QString path);

    /**
     * @brief Create an empty scene.
     *
     * This function removes to an existing robot from the scene.
     *
     * @return osg::Node: Root to the empty scene.
     */
    osg::Node* loadEmptyScene();


protected:


protected:
    osg::Group* root_; /**< Root of the OSG scene containing the robot */

public:

    friend class InteractionHandler;
};

#endif
