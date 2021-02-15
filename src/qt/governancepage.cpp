// Copyright (c) 2018 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "governancepage.h"
#include "ui_governancepage.h"

#include "activemasternode.h"
#include "clientmodel.h"
#include "masternode-sync.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "utilmoneystr.h"
#include "walletmodel.h"
#include "askpassphrasedialog.h"


#include "ui_interface.h"

#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QToolButton>
#include <QDesktopServices>

#include <QPoint>
#include <QSettings>
#include <QSignalMapper>
#include <QUrl>


GovernancePage::GovernancePage(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::GovernancePage),
                                                  clientModel(0),
                                                  walletModel(0)
{

    //QWidget.setCursor (self, QCursor)
    //.setCursor(QCursor(QtCore.Qt.PointingHandCursor))
    //QPushButton->setCursor
    //setCursor(Qt::PointingHandCursor);

    ui->setupUi(this);


    //LEFT

    pushButtonWebsite = new QPushButton("Project website", this);
    pushButtonWebsite->setMinimumSize(200, 8);
    pushButtonWebsite->setToolTip(tr("https://cryptoyen.icu"));
    ui->verticalLayoutPromo->addWidget(pushButtonWebsite);

    /*
    pushSupport = new QPushButton("Support", this);
    pushSupport->setMinimumSize(200, 8);
    ui->verticalLayoutPromo->addWidget(pushSupport);
    */


    pushButtonExplorer = new QPushButton("Official block explorer", this);
    pushButtonExplorer->setMinimumSize(200, 8);
    pushButtonExplorer->setToolTip(tr("https://live.cryptoyen.icu"));
    ui->verticalLayoutPromo->addWidget(pushButtonExplorer);


    pushButtonGit = new QPushButton("Source code on GitHub", this);
    pushButtonGit->setMinimumSize(200, 8);
    pushButtonGit->setToolTip(tr("https://github.com/smartinsider/cryptoyen"));
    ui->verticalLayoutPromo->addWidget(pushButtonGit);

    /*
    pushForum = new QPushButton("Official forum", this);
    pushForum->setMinimumSize(200, 8);
    pushForum->setToolTip(tr("https://forum.cryptoyen.icu"));
    ui->verticalLayoutPromo->addWidget(pushForum);
    */

    connect(pushButtonWebsite, SIGNAL(clicked()), this, SLOT(openWebsiteUrl()));
    connect(pushButtonGit, SIGNAL(clicked()), this, SLOT(openGitUrl()));
    connect(pushButtonExplorer, SIGNAL(clicked()), this, SLOT(openExplorer()));
    // connect(pushForum, SIGNAL(clicked()), this, SLOT(openForum()));    
    // connect(pushSupport, SIGNAL(clicked()), this, SLOT(openSupport()));



    //RIGHT
    Discord = new QPushButton("Discord", this);
    Discord->setMinimumSize(200, 8);
    ui->verticalLayoutPromo->addWidget(Discord);

    Telegram = new QPushButton("Telegram", this);
    Telegram->setMinimumSize(200, 8);
    ui->verticalLayoutPromoRight->addWidget(Telegram);


    VK = new QPushButton("VK", this);
    VK->setMinimumSize(200, 8);
    ui->verticalLayoutPromoRight->addWidget(VK);

    YouTube = new QPushButton("YouTube", this);
    YouTube->setMinimumSize(200, 8);
    ui->verticalLayoutPromoRight->addWidget(YouTube);

    Twitter = new QPushButton("Twitter", this);
    Twitter->setMinimumSize(200, 8);
    ui->verticalLayoutPromoRight->addWidget(Twitter);





    connect(Discord, SIGNAL(clicked()), this, SLOT(openDiscord()));
    connect(Telegram, SIGNAL(clicked()), this, SLOT(openTelegram()));
    connect(Twitter, SIGNAL(clicked()), this, SLOT(openTwitter()));
    connect(YouTube, SIGNAL(clicked()), this, SLOT(openYouTube()));
    connect(VK, SIGNAL(clicked()), this, SLOT(openVK()));



    pushBuyCoin = new QPushButton("ABOUT CRYPTOYEN", this);
    pushBuyCoin->setMinimumSize(200, 8);
    ui->horizontalLayout_Header->addWidget(pushBuyCoin);
    connect(pushBuyCoin, SIGNAL(clicked()), this, SLOT(buyCoin()));


//verticalLayoutPromoRight

    /*
    Twitter +
	Facebook +
	LinkedIn
	YouTube +
	Telegram +
	Discord +
	Bitcoin Talk +
	Github +
	Reddit +
	Forum +
	News Blog +
	*/

}


void GovernancePage::openGitUrl()
{
    QDesktopServices::openUrl (QUrl("https://github.com/smartinsider/cryptoyen"));
}

void GovernancePage::openWebsiteUrl()
{
    QDesktopServices::openUrl (QUrl("https://cryptoyen.icu"));
}

void GovernancePage::openExplorer()
{
    QDesktopServices::openUrl (QUrl("https://live.cryptoyen.icu"));
}

void GovernancePage::openForum()
{
    QDesktopServices::openUrl (QUrl("https://forum.cryptoyen.icu"));
}

void GovernancePage::openDiscord()
{
    QDesktopServices::openUrl (QUrl("https://discord.gg/ScbmPjUE3v"));
}

void GovernancePage::openTelegram()
{
    QDesktopServices::openUrl (QUrl("https://t.me/cryptoyencoin"));
}

void GovernancePage::openTwitter()
{
    QDesktopServices::openUrl (QUrl("https://twitter.com/cryptoyencoin"));
}

void GovernancePage::openQuora()
{
    QDesktopServices::openUrl (QUrl("https://quora.cryptoyen.icu"));
}

void GovernancePage::openFacebook()
{
    QDesktopServices::openUrl (QUrl("https://facebook.cryptoyen.icu"));
}

void GovernancePage::openYouTube()
{
    QDesktopServices::openUrl (QUrl("https://www.youtube.com/channel/UCAtDlBL8_r66vt1Jd__bJMQ"));
}

void GovernancePage::openReddit()
{
    QDesktopServices::openUrl (QUrl("https://reddit.cryptoyen.icu"));
}

void GovernancePage::openNewsBlog()
{
    QDesktopServices::openUrl (QUrl("https://blog.cryptoyen.icu"));
}

void GovernancePage::openLinkedIn()
{
    QDesktopServices::openUrl (QUrl("https://linkedin.cryptoyen.icu"));
}

void GovernancePage::openSupport()
{
    QDesktopServices::openUrl (QUrl("https://support.cryptoyen.icu"));
}

void GovernancePage::openVK()
{
    QDesktopServices::openUrl (QUrl("https://vk.com/cryptoyen"));
}

void GovernancePage::buyCoin()
{
    QDesktopServices::openUrl (QUrl("https://cryptoyen.icu/"));
}

GovernancePage::~GovernancePage()
{
    delete ui;
}

void GovernancePage::setClientModel(ClientModel* model)
{
    this->clientModel = model;
}

void GovernancePage::setWalletModel(WalletModel* model)
{
    this->walletModel = model;
}

void GovernancePage::lockUpdating(bool lock)
{
    fLockUpdating = lock;
}
