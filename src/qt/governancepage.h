// Copyright (c) 2018-2021 @smartinsider
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_GOVERNANCEPAGE_H
#define BITCOIN_QT_GOVERNANCEPAGE_H

#include "masternode.h"
#include "platformstyle.h"
#include "sync.h"
#include "util.h"

#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include <QWidget>

#include <QKeyEvent>
#include <QAction>

class ClientModel;
class WalletModel;

namespace Ui
{
    class GovernancePage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class GovernancePage : public QWidget
{
    Q_OBJECT

public:
    explicit GovernancePage(QWidget* parent = 0);
    ~GovernancePage();

    void setClientModel(ClientModel* clientModel);
    void setWalletModel(WalletModel* walletModel);
    void lockUpdating(bool lock);

private:
    QMenu* contextMenu;
    int64_t nTimeFilterUpdated;
    bool fFilterUpdated;
    bool fLockUpdating;
    QPushButton* pushButtonGit;
    QPushButton* pushButtonWebsite;
    QPushButton* pushButtonExplorer;
    QPushButton* pushForum;
    QPushButton* Discord;
    QPushButton* VK;
    QPushButton* Telegram;
    QPushButton* Twitter;
    QPushButton* Quora;
    QPushButton* Facebook;
    QPushButton* YouTube;
    QPushButton* Reddit;
    QPushButton* NewsBlog;
    QPushButton* LinkedIn;  
    QPushButton* pushSupport;  
    QPushButton* pushBuyCoin;  


      

private slots:
    void openGitUrl();
    void openWebsiteUrl();
    void openExplorer();
    void openForum();
    void openDiscord();
    void openTelegram();
    void openTwitter();
    void openQuora();
    void openFacebook();
    void openYouTube();
    void openReddit();
    void openNewsBlog();
    void openLinkedIn();
    void openSupport();
    void openVK();
    void buyCoin();




Q_SIGNALS:

private:
    QTimer* timer;
    Ui::GovernancePage* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;
    QString strCurrentFilter;

private Q_SLOTS:
    //void on_UpdateButton_clicked();
};

#endif // BITCOIN_QT_GOVERNANCEPAGE_H
