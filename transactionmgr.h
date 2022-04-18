#ifndef TRANSACTIONMGR_H
#define TRANSACTIONMGR_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QReadWriteLock>

#include "dbmanagement.h"

typedef enum _request_type{
    req_unknown = 0,
    req_sign_in_frist,
    req_sign_in_returning,
    req_sign_out,
}REQUEST_TYPE;


class RequestSignIn
{
public:
    RequestSignIn(){};
    ~RequestSignIn(){};
public:
    QImage  m_image;
    Person  m_person;
    int     m_nStaffId;
    QString m_strTime;
};

class RequestSignOut
{
public:
    RequestSignOut(){};
    ~RequestSignOut(){};
public:
    int     m_nVisitorId;
    int     m_nPersonId;
    int     m_nSessionId;
    QString m_strTime;
};


typedef struct _request_signin{
    QImage*  _image;
    Person*  _person;
}REQUEST_SIGN_IN;


typedef struct transaction_req{
    REQUEST_TYPE _type;
    void*        _data;
}TRANSACTTION_REQUEST;



class TransactionMgr : public QThread
{
    Q_OBJECT
public:
    explicit TransactionMgr(QObject *parent = 0);
signals:
    void sendRequest();
    void sendRequestCheckedOutList();
public slots:
    void onSendRequest();
    void receiveSignInReply();
    void receiveSignOutReply();
    void receiveCheckedOutListReply();
    void onSendCheckedOutListReq();
public:
    void scheduleSigninFirstUser(Person* pPerson, QImage& img, int nStaffId);
    void scheduleSigninReturningUser(Person* pPerson, QImage& img, int nStaffId);
    void scheduleSignOut(int nVisitorId, int nPersonId, int sessionId);
    void sendSignInRequeest();
    void sendSignOutRequeest();
protected:
    void  _checkQueueForFirstUser(int nPersonId, int nVisitorId, int sessionId);

    bool  _getRequest(TRANSACTTION_REQUEST* out);
    void _pushRequest(TRANSACTTION_REQUEST* out);

    QList<TRANSACTTION_REQUEST> m_requests;
    QReadWriteLock              m_requestsLock;

    TRANSACTTION_REQUEST        m_currentReq;
    bool                        m_bValidCurrentReq;
private:
    bool                        m_bBusy;

    QNetworkAccessManager*  m_netMgr;
    QNetworkReply*          m_replySignIn;
    QNetworkReply*          m_replySignOut;
    QNetworkReply*          m_replyChekedOutList;
    void                    _closeReplySignIn();
    void                    _closeReplySignOut();
    void                    _closeReplyCheckedOutList();
    void  run() override ;
};

#endif // TRANSACTIONMGR_H
