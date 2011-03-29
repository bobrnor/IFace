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
			bool ok = saveProject();
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
		Json::Value files = projectJson["projectFilesList"];
		if (files.isArray() && files.size() > 0) {		  
			for (int i = 0; i < files.size(); ++i) {
				std::string stdPath = files[i]["path"].asString();
				QString filePath = QString::fromStdString(stdPath);
				SProjectFile projectFile(new ProjectFile(filePath));
				m_projectFiles.append(projectFile);
			}
		}
	}
}

bool Project::saveProject() {

	Json::Value projectJson;
	projectJson["projectFilePath"] = Json::Value(m_projectFilePath.toStdString());

	Json::Value projectFilesListJson(Json::arrayValue);
	foreach (SProjectFile file, m_projectFiles) {
		Json::Value projectFileJson;
		projectFileJson["path"] = Json::Value(file->path().toStdString());

		projectFilesListJson.append(projectFileJson);
	}

	projectJson["projectFilesList"] = projectFilesListJson;

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

void Project::addProjectFile(SProjectFile projectFile) {

	if (projectFile != NULL && !isFileAlreadyInProject(projectFile.data())) {
		m_projectFiles.append(projectFile);
	}
}

void Project::removeProjectFile(SProjectFile projectFile) {

	int index = m_projectFiles.indexOf(projectFile);
	if (index > 0) {
		m_projectFiles.removeAt(index);
	}
}

bool Project::isFileAlreadyInProject(ProjectFile *projectFile) {

	bool result = false;
	foreach (SProjectFile file, m_projectFiles) {
		if (file->path() == projectFile->path()) {
			result = true;
			break;
		}
	}
	return result;
}
