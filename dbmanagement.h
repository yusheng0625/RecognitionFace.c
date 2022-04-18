#ifndef DBMANAGEMENT_H
#define DBMANAGEMENT_H

#include <QDebug>
#include <QApplication>
#include <QString>
#include <QDir>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QReadWriteLock>

#define FEATURE_LEN  256

class Person{
public:
    Person();
    ~Person();
    Person(int id, int visitorId, QString strFirst, QString strLast, QString strCompany, QString strEmail, QString strPhone, float* feature, int nlen, QString filePath, QString strType,QString filePath2,QString filePath3,float* feature2,float* feature3);
    void Copy(Person& person);
    void updateNewImage(QImage newImg, float* feature);
public:
    int m_nId;
    int m_nVisitorId;
    QString m_strFirst;
    QString m_strLast;
    QString m_strCompany;
    QString m_strEmail;
    QString m_strPhone;
    QString m_strType;
    float   m_fFeature[FEATURE_LEN];
    float   m_fFeature2[FEATURE_LEN];
    float   m_fFeature3[FEATURE_LEN];
    QString m_strFilePath;
    QString m_strFilePath2;
    QString m_strFilePath3;
};


class StaffInfo{
public:
    StaffInfo();
    StaffInfo(int id, int staffId, QString strName, QString strPhone, QString strEmail, QString lastUpdated);
    ~StaffInfo();

public:
    int m_nId;
    int m_nStaffId;
    QString m_strName;
    QString m_strPhone;
    QString m_strEmail;
    QString m_lastUpdated;
};


class DBManagement{
public:
    DBManagement();
    ~DBManagement();
public:
    static DBManagement* getInstance();
    static void deleteInstance();

    void CreateDatabase();

    //user table
    bool InsertPerson(Person& person);
    bool UpdateVisitorId(Person person);
    bool UpdatePerson(Person person);
    bool UpdatePersonImages(Person person);


    int GetSimilarId(float* feature, int nLen, float fThreshold, float& flSimilarity, QString& strfirstName, QString& strLastName);
    Person* GetPerson(int nId);
    void DeletePerson(int nId);

    //history table
    bool InsertCheck(int nPersonId, int nTime, int nStaffId, int nSessionId);
    void UpdateCheck(int nPersonId, int sessionId, int nStaffId);
    bool IsExist(int nPersonId, int& nId, int& sessionId, int& staffId);
    void DeleteCheck(int nId);
    void DeleteCheckBySessionID(int nSessionId);

    //staff table
    void InsertStaff(StaffInfo info);
    void GetStaffInfo();
public:
    QSqlDatabase m_UserTable;
    QSqlDatabase m_HistoryTable;
    QSqlDatabase m_StaffTable;

private:
    QReadWriteLock m_lockUserTB;
    QReadWriteLock m_lockHistoryTB;
public:
    void lockUserTB(){m_lockUserTB.lockForWrite();}
    void lockHistoryTB(){m_lockHistoryTB.lockForWrite();}

    void unlockUserTB(){m_lockUserTB.unlock();}
    void unlockHistoryTB(){m_lockHistoryTB.unlock();}
};

extern DBManagement* g_dbManager;

#endif // DBMANAGEMENT_H
