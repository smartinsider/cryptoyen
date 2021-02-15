CryptoYen Coin
=====================

Setup
---------------------
[CryptoYen Coin](https://cryptoyen_modern.icu/wallet) is the original YEN client and it builds the backbone of the network. However, it downloads and stores the entire history of YEN transactions; depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more. Thankfully you only have to do this once.

Running
---------------------
The following are some helpful notes on how to run YEN on your native platform.

### Unix

Unpack the files into a directory and run:

- bin/32/cryptoyen_modern-qt (GUI, 32-bit) or bin/32/cryptoyen_modernd (headless, 32-bit)
- bin/64/cryptoyen_modern-qt (GUI, 64-bit) or bin/64/cryptoyen_modernd (headless, 64-bit)

### Windows

Unpack the files into a directory, and then run cryptoyen_modern-qt.exe.

### OSX

Drag YEN-Qt to your applications folder, and then run YEN-Qt.

### Need Help?

* See the documentation at the [YEN Wiki](https://en.bitcoin.it/wiki/Main_Page) ***TODO***
for help and more information.
* Ask for help on [BitcoinTalk](https://bitcointalk.org/index.php?topic=1262920.0) or on the [YEN Forum](http://forum.cryptoyen_modern.icu/).
* Join our Discord server [Discord Server](https://discord.cryptoyen_modern.icu)
* Join one of our Slack groups [YEN Slack Groups](https://cryptoyen_modern.icu/slack-logins/) (Main community is replaced by Discord).

Building
---------------------
The following are developer notes on how to build YEN on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [OSX Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The CryptoYen repo's [root README](https://github.com/smartinsider/cryptoyen_modern/blob/master/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Multiwallet Qt Development](multiwallet-qt.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://dev.visucore.com/bitcoin/doxygen/) ***TODO***
- [Translation Process](translation_process.md)
- [Unit Tests](unit-tests.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Dnsseed Policy](dnsseed-policy.md)

### Resources

* Discuss on the [BitcoinTalk](https://bitcointalk.org/index.php?topic=1262920.0) or the [YEN](http://forum.cryptoyen_modern.icu/) forum.
* Join the [YEN-Dev](https://cryptoyen_modern-dev.slack.com/) Slack group ([Sign-Up](https://cryptoyen_modern-dev.herokuapp.com/)).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)

License
---------------------
Distributed under the [MIT/X11 software license](http://www.opensource.org/licenses/mit-license.php).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.
