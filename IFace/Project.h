#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>

#include "lib_json/json/json.h"

#include "ProjectFile.h"

class Project {

	Json::Value m_projectJson;
    QString m_projectFilePath;
    QList<ProjectFile *> m_projectFiles;

    void initEmptyProject();
    void loadFromFile();
	void saveToFile();

	bool isFileAlreadyInProject(ProjectFile *projectFile);
	ProjectFile *makeProjectFile(Json::Value projectFileJson);
	Json::Value makeProjectFileJson(ProjectFile *projectFile);

public:
    Project(const QString &path);
    ~Project();

	QString projectName();

    void saveProject();
	void saveProjectFile(ProjectFile *projectFile);
	void saveUpdatedStructure();

    void addProjectFile(ProjectFile *projectFile);
    void removeProjectFile(ProjectFile *projectFile);

	QString path() const { return m_projectFilePath; }
	QList<ProjectFile *> projectFiles() const { return m_projectFiles; }
};

#endif // PROJECT_H
