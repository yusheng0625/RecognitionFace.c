#include "global.h"
#include "FaceApi.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>

DBManagement* g_dbManager = NULL;

Person::Person()
{
    m_nId = 0;
    m_nVisitorId = 0;
    m_strFirst = "";
    m_strLast = "";
    m_strCompany = "";
    m_strEmail = "";
    m_strPhone = "";
    m_strFilePath = "";
    m_strFilePath2 = "";
    m_strFilePath3 = "";

    memset(m_fFeature, 0 , FEATURE_LEN * sizeof(float));
    memset(m_fFeature2, 0 , FEATURE_LEN * sizeof(float));
    memset(m_fFeature3, 0 , FEATURE_LEN * sizeof(float));
}
Person::~Person()
{
}

Person::Person(int id, int nVisitorId, QString strFirst, QString strLast, QString strCompany, QString strEmail, QString strPhone, float *feature, int nlen, QString filePath, QString strType,QString filePath2,QString filePath3,float* feature2,float* feature3)
{
    m_nId = id;
    m_nVisitorId = nVisitorId;
    m_strFirst = strFirst;
    m_strLast = strLast;
    m_strCompany = strCompany;
    m_strEmail = strEmail;
    m_strPhone = strPhone;

    memcpy(m_fFeature, feature, nlen * sizeof(float));

    if(feature2)
        memcpy(m_fFeature2, feature2, nlen * sizeof(float));
    if(feature3)
        memcpy(m_fFeature3, feature3, nlen * sizeof(float));

    m_strFilePath = filePath;
    m_strType = strType;
    m_strFilePath2 = filePath2;
    m_strFilePath3 = filePath3;
}
void Person::Copy(Person& person)
{
    m_nId = person.m_nId;
    m_nVisitorId = person.m_nVisitorId;
    m_strFirst = person.m_strFirst;
    m_strLast = person.m_strLast;
    m_strCompany = person.m_strCompany;
    m_strEmail = person.m_strEmail;
    m_strPhone = person.m_strPhone;

    memcpy(m_fFeature, person.m_fFeature, FEATURE_LEN * sizeof(float));
    memcpy(m_fFeature2, person.m_fFeature2, FEATURE_LEN * sizeof(float));
    memcpy(m_fFeature3, person.m_fFeature3, FEATURE_LEN * sizeof(float));

    m_strFilePath =  person.m_strFilePath;
    m_strFilePath2 = person.m_strFilePath2;
    m_strFilePath3 = person.m_strFilePath3;
    m_strType = person.m_strType;
}


void Person::updateNewImage(QImage newImg, float* feature)
{
    QList<QImage*> images;
    QList<float*>  features;

    QImage imgage0, imgage1, imgage2;
    float  freatureBuff0[FEATURE_LEN];
    float  freatureBuff1[FEATURE_LEN];
    float  freatureBuff2[FEATURE_LEN];

    if( DecryptImage(g_imagepath  + "/" + m_strFilePath, imgage0) && m_fFeature[0] !=0 && m_fFeature[1] !=0)
    {
        images.push_back(&imgage0);
        memcpy(freatureBuff0, m_fFeature, FEATURE_LEN * sizeof(float));
        features.push_back(freatureBuff0);
    }
    if(DecryptImage(g_imagepath  + "/" + m_strFilePath2, imgage1) && m_fFeature2[0] !=0 && m_fFeature2[1] !=0)
    {
        images.push_back(&imgage1);
        memcpy(freatureBuff1, m_fFeature2, FEATURE_LEN * sizeof(float));
        features.push_back(freatureBuff1);
    }
    if(DecryptImage(g_imagepath  + "/" + m_strFilePath3, imgage2) && m_fFeature3[0] !=0 && m_fFeature3[1] !=0)
    {
        images.push_back(&imgage2);
        memcpy(freatureBuff2, m_fFeature3, FEATURE_LEN * sizeof(float));
        features.push_back(freatureBuff2);
    }
    images.push_back(&newImg);
    features.push_back(feature);

    if(images.size() > 3)
    {
        images.erase(images.begin());
        features.erase(features.begin());
    }

    //delete all images
    QFile::remove(m_strFilePath);   QFile::remove(g_imagepath  + "/" + m_strFilePath);
    QFile::remove(m_strFilePath2);   QFile::remove(g_imagepath  + "/" + m_strFilePath2);
    QFile::remove(m_strFilePath3);   QFile::remove(g_imagepath  + "/" + m_strFilePath3);
    m_strFilePath.clear();
    m_strFilePath2.clear();
    m_strFilePath3.clear();
    memset(m_fFeature, 0 ,FEATURE_LEN* sizeof(float));
    memset(m_fFeature2, 0 ,FEATURE_LEN* sizeof(float));
    memset(m_fFeature3, 0 ,FEATURE_LEN* sizeof(float));

    QString strfile;
    //save all images
    for(int i=0; i<images.size(); i++)
    {
        strfile = m_strFirst + "_" + m_strLast + "_" + QString::number(i + 1) + ".jpg";
        EncryptImage(*images[i], g_imagepath + "/" + strfile);

        //images[i]->save(g_imagepath + "/" + strfile);
        switch(i)
        {
            case 0:
                m_strFilePath = strfile;
                memcpy(m_fFeature, features[i] ,FEATURE_LEN* sizeof(float));
            break;
            case 1:
                m_strFilePath2 = strfile;
                memcpy(m_fFeature2, features[i] ,FEATURE_LEN* sizeof(float));
            break;
            case 2:
                m_strFilePath3 = strfile;
                memcpy(m_fFeature3, features[i] ,FEATURE_LEN* sizeof(float));
            break;
        }
    }


    images.clear();
    features.clear();
}


StaffInfo::StaffInfo(){
    m_nId = 0;
    m_nStaffId = 0;
}

StaffInfo::StaffInfo(int id, int staffId, QString strName, QString strPhone, QString strEmail, QString lastUpdated)
{
    m_nId = id;
    m_nStaffId = staffId;
    m_strName = strName;
    m_strPhone = strPhone;
    m_strEmail = strEmail;
    m_lastUpdated = lastUpdated;
}

StaffInfo::~StaffInfo(){

}


DBManagement::DBManagement()
{
}

DBManagement::~DBManagement()
{
}

DBManagement* DBManagement::getInstance()
{
    if(!g_dbManager)
        g_dbManager = new DBManagement();

    return g_dbManager;
}

void DBManagement::deleteInstance()
{
    if(g_dbManager)
    {
        delete g_dbManager;
        g_dbManager = NULL;
    }
}

void DBManagement::CreateDatabase()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    QString configDir = pGlobal->m_strCurrentDir;
    QDir qDir;

    if (! qDir.exists(configDir))
        qDir.mkpath(configDir);

    // Connects with the databases for usertable
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();

    if (m_UserTable.isOpen())
    {
        QSqlQuery query(m_UserTable);
        query.exec("create table usertable (id integer not null primary key autoincrement, visitorid integer, firstname varchar(30), lastname varchar(30), companyname varchar(30), emailadress varchar(30), phonenumber varchar(30), feature blob, filepath varchar(30), usertype varchar(30), filepath2 varchar(30), filepath3 varchar(30), feature2 blob, feature3 blob )");
        query.exec("ALTER TABLE usertable ADD COLUMN filepath2 char(30)");
        query.exec("ALTER TABLE usertable ADD COLUMN filepath3 char(30)");
        query.exec("ALTER TABLE usertable ADD COLUMN feature2 blob");
        query.exec("ALTER TABLE usertable ADD COLUMN feature3 blob");
        m_UserTable.close();
    }

    // Connects with the databases for historytable
    m_HistoryTable = QSqlDatabase::addDatabase("QSQLITE");
    m_HistoryTable.setDatabaseName(configDir + "/historytable.db");
    m_HistoryTable.open();

    if (m_HistoryTable.isOpen())
    {
        QSqlQuery query(m_HistoryTable);
        query.exec("create table historytable (id integer not null primary key autoincrement, personid integer, time integer, staff_id integer, session_id integer)");
        m_HistoryTable.close();
    }

    // Connects with the databases for historytable
    QFile::remove(configDir + "/stafftable.db");
    m_StaffTable = QSqlDatabase::addDatabase("QSQLITE");
    m_StaffTable.setDatabaseName(configDir + "/stafftable.db");
    m_StaffTable.open();

    if (m_StaffTable.isOpen())
    {
        pGlobal->log("creating staff table");
        QSqlQuery query(m_StaffTable);
        query.exec("create table stafftable (id integer not null primary key autoincrement, staff_id integer, staff_name varchar(128), phone_number varchar(128), email varchar(128), last_updated varchar(128))");
        m_StaffTable.close();
    }
}

bool DBManagement::InsertPerson(Person& person)
{
    bool res;
    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();

    if (!m_UserTable.isOpen()) return false;

    QSqlQuery query(m_UserTable);

    query.prepare("insert into usertable(visitorid, firstname, lastname, companyname, emailadress, phonenumber, feature, filepath, usertype) values(:visitorid, :firstname, :lastname, :companyname, :emailadress, :phonenumber, :feature, :filepath, :usertype )");
    query.bindValue(":visitorid", person.m_nVisitorId);
    query.bindValue(":firstname", person.m_strFirst);
    query.bindValue(":lastname", person.m_strLast);
    query.bindValue(":companyname", person.m_strCompany);
    query.bindValue(":emailadress", person.m_strEmail);
    query.bindValue(":phonenumber", person.m_strPhone);
    QByteArray feature((char*)person.m_fFeature, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature", feature);
    query.bindValue(":filepath", person.m_strFilePath);
    query.bindValue(":usertype", person.m_strType);
    res = query.exec();
    person.m_nId = query.lastInsertId().toInt();
    m_UserTable.close();

    return res;
}

void DBManagement::InsertStaff(StaffInfo info)
{
    bool bExist = false;
    QString configDir = qApp->applicationDirPath();
    m_StaffTable = QSqlDatabase::addDatabase("QSQLITE");
    m_StaffTable.setDatabaseName(configDir + "/stafftable.db");
    m_StaffTable.open();
    if (!m_StaffTable.isOpen())return;

    QSqlQuery query(m_StaffTable);

    query.prepare("select * from stafftable where staff_id=:id");
    query.bindValue(":id", info.m_nStaffId);
    query.exec();
    while(query.next())
    {
        bExist = true;
        break;
    }

    if(bExist) //update
    {
        query.prepare("update stafftable set staff_id=:staffid, staff_name=:staffname, phone_number=:phonenumber, email=:email, last_updated=:lastupdated where staff_id=:staffid");
        query.bindValue(":staffid", info.m_nStaffId);
        query.bindValue(":staffname", info.m_strName);
        query.bindValue(":phonenumber", info.m_strPhone);
        query.bindValue(":email", info.m_strEmail);
        query.bindValue(":lastupdated", info.m_lastUpdated);
        query.exec();
    }else{ //insert
        query.prepare("insert into stafftable(staff_id, staff_name, phone_number, email, last_updated) values(:staffid, :staffname, :phonenumber, :email, :lastupdated)");
        query.bindValue(":staffid", info.m_nStaffId);
        query.bindValue(":staffname", info.m_strName);
        query.bindValue(":phonenumber", info.m_strPhone);
        query.bindValue(":email", info.m_strEmail);
        query.bindValue(":lastupdated", info.m_lastUpdated);
        query.exec();
    }
    m_StaffTable.close();

}

void DBManagement::GetStaffInfo()
{
    QString configDir = qApp->applicationDirPath();
    m_StaffTable = QSqlDatabase::addDatabase("QSQLITE");
    m_StaffTable.setDatabaseName(configDir + "/stafftable.db");
    m_StaffTable.open();
    if (!m_StaffTable.isOpen())return;

    QSqlQuery query(m_StaffTable);
    query.prepare("select * from stafftable");
    if(!query.exec())
    {
        m_StaffTable.close();
    }
    while(query.next())
    {
        int id = query.value(0).toInt();
        int staffId = query.value(1).toInt();
        QString staffName = query.value(2).toString();
        QString phoneNo = query.value(3).toString();
        QString email = query.value(4).toString();
        QString lastupdated = query.value(5).toString();
        g_staffList.push_back(StaffInfo(id, staffId, staffName, phoneNo, email, lastupdated));
    }
    m_StaffTable.close();

}

bool DBManagement::UpdateVisitorId(Person person)
{
    bool res;
    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();
    if (!m_UserTable.isOpen())return false;

    QSqlQuery query(m_UserTable);

    query.prepare("update usertable set visitor_id=:visitorid where id=:id");
    query.bindValue(":visitorid", person.m_nVisitorId);
    query.bindValue(":id", person.m_nId);
    res = query.exec();
    m_UserTable.close();
    return res;
}


bool DBManagement::UpdatePerson(Person person)
{
    GlobalData* pGlobal = GlobalData::getInstance();

    bool res;
    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();
    if (!m_UserTable.isOpen()) return false;

    QSqlQuery query(m_UserTable);

    query.prepare("update usertable set firstname=:firstname, lastname=:lastname, companyname=:companyname, emailadress=:emailadress, phonenumber=:phonenumber, feature=:feature, filepath=:filepath, usertype=:usertype, visitorid=:visitorid, filepath2=:filepath2, filepath3=:filepath3, feature2=:feature, feature3=:feature3 where id=:id");
    query.bindValue(":firstname", person.m_strFirst);
    query.bindValue(":lastname", person.m_strLast);
    query.bindValue(":companyname", person.m_strCompany);
    query.bindValue(":emailadress", person.m_strEmail);
    query.bindValue(":phonenumber", person.m_strPhone);

    QByteArray feature((char*)person.m_fFeature, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature", feature);

    QByteArray feature2;
    if(pGlobal->isValidImageUserImage(person.m_strFilePath2))
        feature2.setRawData((char*)person.m_fFeature2, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature2", feature2);

    QByteArray feature3;
    if(pGlobal->isValidImageUserImage(person.m_strFilePath3))
        feature3.setRawData((char*)person.m_fFeature3, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature3", feature3);

    query.bindValue(":filepath", person.m_strFilePath);
    query.bindValue(":filepath2", person.m_strFilePath2);
    query.bindValue(":filepath3", person.m_strFilePath3);

    query.bindValue(":usertype", person.m_strType);
    query.bindValue(":visitorid", person.m_nVisitorId);
    query.bindValue(":id", person.m_nId);
    res = query.exec();
    m_UserTable.close();
    return res;
}

bool DBManagement::UpdatePersonImages(Person person)
{
    GlobalData* pGlobal = GlobalData::getInstance();
    bool res = false;
    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();
    if (!m_UserTable.isOpen())return false;

    QSqlQuery query(m_UserTable);

    query.prepare("update usertable set filepath=:filepath, filepath2=:filepath2, filepath3=:filepath3, feature=:feature, feature2=:feature2, feature3=:feature3 where id=:id");

    QByteArray feature((char*)person.m_fFeature, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature", feature);

    QByteArray feature2;
    if(pGlobal->isValidImageUserImage(person.m_strFilePath2))
        feature2.setRawData((char*)person.m_fFeature2, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature2", feature2);

    QByteArray feature3;
    if(pGlobal->isValidImageUserImage(person.m_strFilePath3))
        feature3.setRawData((char*)person.m_fFeature3, FEATURE_LEN * sizeof(float));
    query.bindValue(":feature3", feature3);

    query.bindValue(":filepath", person.m_strFilePath);
    query.bindValue(":filepath2", person.m_strFilePath2);
    query.bindValue(":filepath3", person.m_strFilePath3);
    query.bindValue(":id", person.m_nId);
    res = query.exec();
    m_UserTable.close();
    return res;
}

int DBManagement::GetSimilarId(float* feature, int nLen,  float fThreshold, float& flSimilarity, QString& strfirstName, QString& strLastName)
{
    GlobalData* pGlobal = GlobalData::getInstance();

    int nMaxId = 0;
    float maxSimilarity = 0;
    QString userNameFirst;
    QString userNameLast;

    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();
    if (!m_UserTable.isOpen()) return nMaxId;

    QSqlQuery query(m_UserTable);
    query.prepare("select id, feature, firstname, lastname, feature2, feature3, filepath2, filepath3 from usertable");
    if(!query.exec())
    {
        m_UserTable.close();
        return nMaxId;
    }

    double similarities[3];
    double maxSimilarities[3];

    while(query.next())
    {
        for(int i=0; i<3; i++)similarities[i] = 0;

        int id = query.value(0).toInt();

        QByteArray ba = query.value(1).toByteArray();

        //compare 3 features.
        double similarity = 0;
        if(ba.length() == FEATURE_LEN * sizeof(float))
        {
            Match_templates(feature, nLen, (float*)ba.data(), nLen, similarity);
            similarities[0] = similarity;
            //pGlobal->log("image1 compare match=" + QString::number(similarities[0]));
        }


        QByteArray ba2 = query.value(4).toByteArray();
        if(pGlobal->isValidImageUserImage(query.value(6).toString()) && ba2.length() == FEATURE_LEN * sizeof(float))
        {
            Match_templates(feature, nLen, (float*)ba2.data(), nLen, similarity);
            similarities[1] = similarity;
            //pGlobal->log("image2 compare match=" + QString::number(similarities[1]));
        }


        QByteArray ba3 = query.value(5).toByteArray();
        if(pGlobal->isValidImageUserImage(query.value(7).toString()) && ba3.length() == FEATURE_LEN * sizeof(float))
        {
            Match_templates(feature, nLen, (float*)ba3.data(), nLen, similarity);
            similarities[2] = similarity;
            //pGlobal->log("image3 compare match=" + QString::number(similarities[2]));
        }

        similarity = 0;
        //select max
        for(int i=0; i<3; i++)
        {
            if(similarities[i] > similarity)
                similarity = similarities[i];
        }


        if(similarity > maxSimilarity )
        {
            maxSimilarity = similarity;
            nMaxId = id;
            userNameFirst = query.value(2).toString();
            userNameLast = query.value(3).toString();

            maxSimilarities[0] = similarities[0];
            maxSimilarities[1] = similarities[1];
            maxSimilarities[2] = similarities[2];
        }
    }
    m_UserTable.close();

    flSimilarity = maxSimilarity;
    strfirstName = userNameFirst;
    strLastName = userNameLast;

    if(nMaxId > 0)
    {
        //pGlobal->log("matching=>" + strfirstName + " " + strLastName + " max=" + QString::number(maxSimilarity));
        //pGlobal->log("similar_0 = " + QString::number(maxSimilarities[0]));
        //pGlobal->log("similar_1 = " + QString::number(maxSimilarities[1]));
        //pGlobal->log("similar_2 = " + QString::number(maxSimilarities[2]));
    }

    return nMaxId;
}

Person* DBManagement::GetPerson(int nId)
{
    float feature1[FEATURE_LEN];
    float feature2[FEATURE_LEN];
    float feature3[FEATURE_LEN];

    Person* person = NULL;
    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();
    if (!m_UserTable.isOpen())return person;

    QSqlQuery query(m_UserTable);
    query.prepare("select * from usertable where id=:id");
    query.bindValue(":id", nId);
    if(!query.exec())
    {
        m_UserTable.close();
        return NULL;
    }
    while(query.next())
    {
        memset(feature1, 0, FEATURE_LEN * sizeof(float));
        memset(feature2, 0, FEATURE_LEN * sizeof(float));
        memset(feature3, 0, FEATURE_LEN * sizeof(float));

        int nId = query.value(0).toInt();
        int nVisitorId = query.value(1).toInt();
        QString strFirst = query.value(2).toString();
        QString strLast = query.value(3).toString();
        QString strCompany = query.value(4).toString();
        QString strEmail = query.value(5).toString();
        QString strPhone = query.value(6).toString();
        QByteArray ba = query.value(7).toByteArray();
        memcpy(feature1, ba.data(), ba.size());

        QString strFilePath = query.value(8).toString();
        QString strType = query.value(9).toString();

        QString strFilePath2 = query.value(10).toString();
        QString strFilePath3 = query.value(11).toString();

        QByteArray ba2 = query.value(12).toByteArray();
        memcpy(feature2, ba2.data(), ba2.size());

        QByteArray ba3 = query.value(13).toByteArray();
        memcpy(feature3, ba3.data(), ba3.size());


        person = new Person(nId, nVisitorId, strFirst, strLast, strCompany, strEmail, strPhone, feature1, ba.size() / 4, strFilePath,strType, strFilePath2, strFilePath3, feature2, feature3);
        break;
    }
    m_UserTable.close();
    return person;
}


void DBManagement::DeletePerson(int nId)
{
    QString configDir = qApp->applicationDirPath();
    m_UserTable = QSqlDatabase::addDatabase("QSQLITE");
    m_UserTable.setDatabaseName(configDir + "/usertable.db");
    m_UserTable.open();
    if (!m_UserTable.isOpen()) return;

    QSqlQuery query(m_UserTable);
    query.prepare("delete from usertable where id=:id");
    query.bindValue(":id", nId);
    query.exec();
    m_UserTable.close();
}
bool DBManagement::InsertCheck(int nPersonId, int nTime, int nStaffId, int nSessionId)
{
    bool res;
    QString configDir = qApp->applicationDirPath();
    m_HistoryTable = QSqlDatabase::addDatabase("QSQLITE");
    m_HistoryTable.setDatabaseName(configDir + "/historytable.db");
    m_HistoryTable.open();
    if (!m_HistoryTable.isOpen())return false;

    QSqlQuery query(m_HistoryTable);

    query.prepare("insert into historytable(personid, time, staff_id, session_id) values(:id, :time, :staff_id, :sessionid)");
    query.bindValue(":id", nPersonId);
    query.bindValue(":time", nTime);
    query.bindValue(":staff_id", nStaffId);
    query.bindValue(":sessionid", nSessionId);
    res = query.exec();
    m_HistoryTable.close();
    return res;
}

void DBManagement::UpdateCheck(int nPersonId, int sessionId, int nStaffId)
{
    bool res = false;
    QString configDir = qApp->applicationDirPath();
    m_HistoryTable = QSqlDatabase::addDatabase("QSQLITE");
    m_HistoryTable.setDatabaseName(configDir + "/historytable.db");
    m_HistoryTable.open();
    if (!m_HistoryTable.isOpen())return;

    QSqlQuery query(m_HistoryTable);

    query.prepare("update historytable set session_id=:session_id, staff_id=:staff_id where personid=:personid");
    query.bindValue(":session_id", sessionId);
    query.bindValue(":staff_id", nStaffId);
    query.bindValue(":personid", nPersonId);
    query.exec();
    m_HistoryTable.close();
}


bool DBManagement::IsExist(int nPersonId, int& nId, int& sessionId, int& staffId)
{
    bool res = false;
    QString configDir = qApp->applicationDirPath();
    m_HistoryTable = QSqlDatabase::addDatabase("QSQLITE");
    m_HistoryTable.setDatabaseName(configDir + "/historytable.db");
    m_HistoryTable.open();
    if (!m_HistoryTable.isOpen())return false;

    QSqlQuery query(m_HistoryTable);
    query.prepare("select id, session_id, staff_id from historytable where personid=:id");
    query.bindValue(":id", nPersonId);
    query.exec();
    while(query.next())
    {
        nId = query.value(0).toInt();
        sessionId = query.value(1).toInt();
        staffId = query.value(2).toInt();
        res = true;
        break;
    }
    m_HistoryTable.close();
    return res;
}

void DBManagement::DeleteCheck(int nId)
{
    QString configDir = qApp->applicationDirPath();
    m_HistoryTable = QSqlDatabase::addDatabase("QSQLITE");
    m_HistoryTable.setDatabaseName(configDir + "/historytable.db");
    m_HistoryTable.open();
    if (!m_HistoryTable.isOpen()) return ;

    QSqlQuery query(m_HistoryTable);

    query.prepare("delete from historytable where personid=:id");
    query.bindValue(":id", nId);
    query.exec();
    m_HistoryTable.close();
}
void DBManagement::DeleteCheckBySessionID(int nSessionId)
{
    QString configDir = qApp->applicationDirPath();
    m_HistoryTable = QSqlDatabase::addDatabase("QSQLITE");
    m_HistoryTable.setDatabaseName(configDir + "/historytable.db");
    m_HistoryTable.open();
    if (!m_HistoryTable.isOpen()) return;

    QSqlQuery query(m_HistoryTable);

    query.prepare("delete from historytable where session_id=:session_id");
    query.bindValue(":session_id", nSessionId);
    query.exec();
    m_HistoryTable.close();
}

