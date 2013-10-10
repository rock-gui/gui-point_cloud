#include "PointCloudVisualization.hpp"
#include <iostream>
#include <osgUtil/Optimizer>
#include <osg/PolygonMode>
#include <osg/Point>
#include <base/Logging.hpp>

namespace vizkit3d
{

PointCloudVisualization::PointCloudVisualization()
{
    reduction_ = 1.0;
    cloud_ = new osg::Vec3Array();
    point_size_ = 1.f;

    color_index_ = 0;
    colors_.push_back(osg::Vec4d(0.8,0.8,0.8,1));
    colors_.push_back(osg::Vec4d(1,1,1,1));
    colors_.push_back(osg::Vec4d(0.2,0.2,0.2,1));
    colors_.push_back(osg::Vec4d(0,0,0,1));
    colors_.push_back(osg::Vec4d(1,0,0,1));
    colors_.push_back(osg::Vec4d(0,1,0,1));
    colors_.push_back(osg::Vec4d(0,0,1,1));
    colors_.push_back(osg::Vec4d(0,1,1,1));
    colors_.push_back(osg::Vec4d(1,0,1,1));
    colors_.push_back(osg::Vec4d(1,1,0,1));

    color_ = colors_[color_index_];
}


void analysePrimSet(osg::PrimitiveSet*prset, const osg::Vec3Array *verts) {
    unsigned int ic;
    unsigned int i2;
    unsigned int nprim=0;
    osg::notify(osg::WARN) << "Prim set type "<< prset->getMode() << std::endl;
    for (ic=0; ic<prset->getNumIndices(); ic++) { // NB the vertices are held in the drawable -
        osg::notify(osg::WARN) << "vertex "<< ic << " is index "<<prset->index(ic) << " at " <<
                                  (* verts)[prset->index(ic)].x() << "," <<
                                  (* verts)[prset->index(ic)].y() << "," <<
                                  (* verts)[prset->index(ic)].z() << std::endl;
    }
    // you might want to handle each type of primset differently: such as:
    switch (prset->getMode()) {
    case osg::PrimitiveSet::TRIANGLES: // get vertices of triangle
        osg::notify(osg::WARN) << "Triangles "<< nprim << " is index "<<prset->index(ic) << std::endl;
        for (i2=0; i2<prset->getNumIndices()-2; i2+=3) {
        }
        break;
    case osg::PrimitiveSet::TRIANGLE_STRIP: // look up how tristrips are coded
        break;
        // etc for all the primitive types you expect. EG quads, quadstrips lines line loops....
    }
}

void PointCloudVisualization::setModelFile(QString modelFile)
{
    cloud_->clear();

    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(std::string(modelFile.toAscii().data()));
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    osg::Geode* geode = loadedModel.get()->asGeode();
    for (unsigned int i=0; i<geode->getNumDrawables(); i++) {
        osg::Drawable *drawable=geode->getDrawable(i);
        osg::Geometry *geom=dynamic_cast<osg::Geometry *> (drawable);
        for (unsigned int ipr=0; ipr<geom->getNumPrimitiveSets(); ipr++) {
            //osg::PrimitiveSet* prset=geom->getPrimitiveSet(ipr);
            osg::notify(osg::WARN) << "Primitive Set "<< ipr << std::endl;

            const osg::Vec3Array* vertices = dynamic_cast<const osg::Vec3Array*>(geom->getVertexArray());
            cloud_->insert(cloud_->end(), vertices->begin(), vertices->end());
        }
    }

    modelFile_ = modelFile;

    emit childrenChanged();
    setDirty();
}

QString PointCloudVisualization::modelFile() const{
    return modelFile_;
}

osg::ref_ptr< osg::Node > PointCloudVisualization::createMainNode()
{
    osg::Group* group = new osg::Group;

    return group;
}

void PointCloudVisualization::setCloudColor(int color)
{
    this->color_index_ = color%colors_.size();
    this->color_ = colors_[color_index_];
    setDirty();
}

int PointCloudVisualization::cloudColor()
{
    return color_index_;
}

float PointCloudVisualization::pointSize()
{
    return point_size_;
}

void PointCloudVisualization::setPointSize(float point_size)
{
    point_size_ = point_size;
}

void PointCloudVisualization::setReduction(double reduction)
{
    if(reduction > 1. || reduction <= 0.){
        LOG_ERROR("Reduction factor must be larger than 0.0 and smaller or equal to 1.0. Setting to 1.0");
        reduction = 1.0;
    }
    reduction_ = reduction;
    setDirty();
}

void PointCloudVisualization::updateDataIntern ( const base::samples::Pointcloud& data )
{
    cloud_->clear();
    int step = data.points.size()/(data.points.size()*reduction_);
    for(uint i=0; i<data.points.size(); i=i+step){
        cloud_->push_back(to_osg(data.points[i]));
    }
}

void PointCloudVisualization::updateMainNode( osg::Node* node )
{
    if(node->asGroup()->getNumChildren() > 0){
        node->asGroup()->removeChildren(0, 1);
    }

    osg::Geode* geode = new osg::Geode;

    osg::Geometry* osg_points_ = new osg::Geometry;
    osg_points_->setVertexArray( cloud_ );
    osg_points_->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,cloud_->size()));


    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color_);

    osg_points_->setColorArray(colors);
    osg_points_->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);


    osg::StateSet* stateset = node->getOrCreateStateSet();
    //stateset->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
    stateset->setAttribute( new osg::Point( point_size_ ), osg::StateAttribute::ON);
    ///////////////////////////////////////////////////////////////////
    // vertex shader using just Vec4 coefficients
    char vertexShaderSource[] =
            "void main(void) \n"
            "{ \n"
            "\n"
            "    gl_FrontColor = gl_Color;\n"
            "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}\n";



    osg::Program* program = new osg::Program;
    stateset->setAttribute(program);

    osg::Shader* vertex_shader = new osg::Shader(osg::Shader::VERTEX, vertexShaderSource);
    program->addShader(vertex_shader);
    //std::cout << cloud_->size() << std::endl;
    geode->addDrawable(osg_points_);
    node->asGroup()->addChild(geode);
}
//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(PointCloudVisualization)
}

