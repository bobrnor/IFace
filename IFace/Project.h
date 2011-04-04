#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>

#include "lib_json/json/json.h"

#include "ProjectFile.h"

class Project {

	Json::Value m_projectJson;
    QString m_projectFilePath;
    QList<SProjectFile> m_projectFiles;

    void initEmptyProject();
    void loadFromFile();
	void saveToFile();

	bool isFileAlreadyInProject(ProjectFile *projectFile);
	SProjectFile makeProjectFile(Json::Value projectFileJson);
	Json::Value makeProjectFileJson(SProjectFile projectFile);

public:
    Project(const QString &path);
    ~Project();

    void saveProject();
	void saveProjectFile(SProjectFile projectFile);
	void saveUpdatedStructure();

    void addProjectFile(SProjectFile projectFile);
    void removeProjectFile(SProjectFile projectFile);

	QString path() const { return m_projectFilePath; }
	QList<SProjectFile> projectFiles() const { return m_projectFiles; }
};

#endif // PROJECT_H
