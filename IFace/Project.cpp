#include "Project.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <fstream>

Project::Project(const QString &path) {

	if (path.length() > 0) {
		m_projectFilePath = path;
		if (QFile::exists(path)) {
			loadFromFile();
		}
		else {
			initEmptyProject();
			saveProject();
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

	Json::Reader reader;
	std::ifstream fileStream;
	fileStream.open(m_projectFilePath.toStdString().c_str());
	bool ok = reader.parse(fileStream, m_projectJson);
	Q_ASSERT(ok);
	if (ok) {
		m_projectFilePath = QString::fromStdString(m_projectJson["projectFilePath"].asString());
		Json::Value files = m_projectJson["projectFilesList"];
		if (files.isArray() && files.size() > 0) {		  
			for (int i = 0; i < files.size(); ++i) {
				SProjectFile projectFile = makeProjectFile(files[i]);
				m_projectFiles.append(projectFile);
			}
		}
	}
}

void Project::saveToFile() {

	Json::StyledWriter writer;
	QString jsonString = QString::fromStdString(writer.write(m_projectJson));
	qDebug() << "Saving project. JSON: " << jsonString;

	QFile file(m_projectFilePath);
	bool ok = file.open(QIODevice::WriteOnly);
	Q_ASSERT(ok);
	if(ok) {
		QTextStream stream(&file);
		stream << jsonString;
		stream.flush();
	}
}

void Project::saveProject() {

	foreach (SProjectFile projectFile, m_projectFiles) {
		projectFile->save();
	}

	m_projectJson["projectFilePath"] = Json::Value(m_projectFilePath.toStdString());

	Json::Value projectFilesListJson(Json::arrayValue);
	foreach (SProjectFile file, m_projectFiles) {
		Json::Value projectFileJson = makeProjectFileJson(file);
		projectFilesListJson.append(projectFileJson);
	}

	m_projectJson["projectFilesList"] = projectFilesListJson;

	saveToFile();
}

void Project::saveProjectFile(SProjectFile projectFile) {

	projectFile->save();

	Json::Value projectFileJson = makeProjectFileJson(projectFile);
	Json::Value oldFileListJson = m_projectJson["projectFilesList"];
	Json::Value newFileListJson;
	bool changed = false;

	if (oldFileListJson.isArray()) {
		for (int i = 0; i < oldFileListJson.size(); ++i) {
			QString path(oldFileListJson[i]["path"].asString().c_str());
			if (path != projectFile->path()) {
				newFileListJson.append(oldFileListJson[i]);
			}
			else {
				newFileListJson.append(projectFileJson);
				changed = true;
			}
		}
		if (!changed) {
			newFileListJson.append(projectFileJson);
		}
	}
	else {
		newFileListJson.append(projectFileJson);
	}

	m_projectJson["projectFilesList"] = newFileListJson;

	saveToFile();
}

SProjectFile Project::makeProjectFile(Json::Value projectFileJson) {

	std::string stdPath = projectFileJson["path"].asString();
	QString filePath = QString::fromStdString(stdPath);
	SProjectFile projectFile(new ProjectFile(filePath));

	Json::Value breakPointsJson = projectFileJson["breakPoints"];
	if (breakPointsJson.isArray()) {
		for (int i = 0; i < breakPointsJson.size(); ++i) {
			int line = breakPointsJson[i].asInt();
			projectFile->addBreakPoint(line);
		}
	}

	Json::Value commentsJson = projectFileJson["comments"];
	if (commentsJson.isArray()) {
		for (int i = 0; i < commentsJson.size(); ++i) {
			Json::Value commentJson = commentsJson[i];
			int line = commentJson["line"].asInt();
			QString text(commentJson["text"].asString().c_str());
			projectFile->addOrReplaceComment(line, text);
		}
	}

	return projectFile;
}

Json::Value Project::makeProjectFileJson(SProjectFile projectFile) {

	Json::Value projectFileJson;
	projectFileJson["path"] = Json::Value(projectFile->path().toStdString());

	Json::Value projectFileBreakPointsListJson(Json::arrayValue);
	foreach (int line, projectFile->breakPoints().values()) {
		Json::Value lineJson(line);
		projectFileBreakPointsListJson.append(lineJson);
	}

	projectFileJson["breakPoints"] = projectFileBreakPointsListJson;

	Json::Value projectFileCommentsListJson(Json::arrayValue);
	QMap<int, QString> comments = projectFile->comments();
	foreach (int key, comments.keys()) {
		Json::Value comment;
		comment["line"] = Json::Value(key);
		comment["text"] = Json::Value(comments.value(key).toStdString());
		projectFileCommentsListJson.append(comment);
	}

	projectFileJson["comments"] = projectFileCommentsListJson;

	return projectFileJson;
}

void Project::saveUpdatedStructure() {

	Json::Value oldFileListJson = m_projectJson["projectFilesList"];
	Json::Value newFileListJson;
	if (oldFileListJson.isArray()) {
		for (int i = 0; i < oldFileListJson.size(); ++i) {
			newFileListJson.append(oldFileListJson[i]);
		}
	}

	m_projectJson["projectFilesList"] = newFileListJson;

	saveToFile();
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
