#include "Project.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <fstream>

#include "lib_json/json/json.h"


Project::Project(const QString &path) {

    if (path.length() > 0) {
        m_projectFilePath = path;
        if (QFile::exists(path)) {
            loadFromFile();
        }
        else {
            initEmptyProject();
            bool ok = save();
            Q_ASSERT(ok);
        }
    }
    else {
        Q_ASSERT(false);
    }

}

Project::~Project() {}

void Project::initEmptyProject() {


}

void Project::loadFromFile() {

    Json::Value projectJson;
    Json::Reader reader;
    std::ifstream fileStream;
    fileStream.open(m_projectFilePath.toStdString().c_str());
    bool ok = reader.parse(fileStream, projectJson);
    Q_ASSERT(ok);
    if (ok) {
        m_projectFilePath = QString::fromStdString(projectJson["projectFilePath"].asString());
        qDebug() << "Loading project.\nProject File Path: " << m_projectFilePath;
    }
}

bool Project::save() {

    Json::Value projectJson;
    projectJson["projectFilePath"] = Json::Value(m_projectFilePath.toStdString());

    Json::StyledWriter writer;
    QString jsonString = QString::fromStdString(writer.write(projectJson));
    qDebug() << "Saving project. JSON: " << jsonString;

    QFile file(m_projectFilePath);
    bool ok = file.open(QIODevice::WriteOnly);
    Q_ASSERT(ok);
    if(ok) {
        QTextStream stream(&file);
        stream << jsonString;
        stream.flush();
    }
    return true;
}
