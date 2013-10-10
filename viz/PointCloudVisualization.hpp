#ifndef GUI_POINTCLOUDVISUALIZATION_H
#define GUI_POINTCLOUDVISUALIZATION_H

#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <base/samples/Pointcloud.hpp>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>

#include <Qt/qobject.h>

namespace vizkit3d
{

inline osg::Vec3d to_osg(base::Point point){
    return osg::Vec3d(point.x(), point.y(), point.z());
}

class PointCloudVisualization: public Vizkit3DPlugin<base::samples::Pointcloud>
{
    Q_OBJECT
    Q_PROPERTY(QString modelFile READ modelFile WRITE setModelFile)
    Q_PROPERTY(int cloudColor READ cloudColor WRITE setCloudColor)
    Q_PROPERTY(int pointSize READ pointSize WRITE setPointSize)
    //Q_PROPERTY(double reduction READ reduction WRITE setReduction)

public:
    PointCloudVisualization();

    void setModelFile(QString modelFile);
    QString modelFile() const;

    Q_INVOKABLE void updateData(const base::samples::Pointcloud &cloud)
    {Vizkit3DPlugin<base::samples::Pointcloud>::updateData(cloud);};

    Q_INVOKABLE void setCloudColor(int  color);
    Q_INVOKABLE int cloudColor();
    Q_INVOKABLE float pointSize();
    Q_INVOKABLE void setPointSize(float point_size);
    Q_INVOKABLE void setReduction(double reduction);
    Q_INVOKABLE double reduction() const;

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
    virtual void updateMainNode( osg::Node* node );
    void updateDataIntern ( const base::samples::Pointcloud& data );
    osg::Vec4d color_;
    int color_index_;
    std::vector<osg::Vec4d> colors_;
    float point_size_;
    double reduction_;
    osg::Vec3Array* cloud_;
    QString modelFile_;
};

}
#endif // GUI_POINTCLOUDVISUALIZATION_H
