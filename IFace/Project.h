#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

class Project {

    QString m_projectFilePath;
    // list of files

    void initEmptyProject();
    void loadFromFile();

public:
    Project(const QString &path);
    ~Project();

    bool save();
};

#endif // PROJECT_H
