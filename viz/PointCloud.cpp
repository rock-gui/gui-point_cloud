#include "PointCloud.h"
#include "fstream"
#include "sstream"
#include "osg/Point"
#include <osgUtil/Optimizer>
//#include <resource_retriever/retriever.h>
//#include "ros/ros.h"
#include <base/Logging.hpp>
#include <QFileInfo>
#include <QString>

#include <fstream>
#include <streambuf>
#include <base/samples/Pointcloud.hpp>

PointCloud::PointCloud(){
    //Root is the entry point to the scene graph
    osg::Group* root = new osg::Group();
    root_ = root;
}

osg::Node* PointCloud::loadEmptyScene(){
    root_->removeChildren(0, root_->getNumChildren());
    return root_;
}


osg::ref_ptr<osg::Node> PointCloud::load(QString path){

    root_ = loadEmptyScene()->asGroup();

    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(std::string(path.toAscii().data()));
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    osg::StateSet* stateset = loadedModel->getOrCreateStateSet();
    ///////////////////////////////////////////////////////////////////
    // vertex shader using just Vec4 coefficients
    char vertexShaderSource[] =
            "void main(void) \n"
            "{ \n"
            "\n"
            "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}\n";



    osg::Program* program = new osg::Program;
    stateset->setAttribute(program);

    osg::Shader* vertex_shader = new osg::Shader(osg::Shader::VERTEX, vertexShaderSource);
    program->addShader(vertex_shader);

    return loadedModel;
}

