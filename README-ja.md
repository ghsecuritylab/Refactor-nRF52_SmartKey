# nRF52_SmartKey


## 概要
 
nRF52840を使用して作成した、スマートキー。

スマホとnRF52840をBLE通信させることで、解錠・施錠が可能。

ブロックチェーン技術を用いて、許可された第三者のみに鍵を譲渡することができる。


## デモ

// ToDo
動作している様子がGIFで挿入される予定。


## 準備

1. Goをインストール

2. ArduinoIDEをインストール

バージョン: [ArduinoIDE 1.8.10](https://www.arduino.cc/en/main/software)

3. Adafruit Feather nRF52840 Expressを使用。

nRF52840が搭載されているAdafruit社製のボードならば、ピン配置を適宜変更すれば、ソースコードをそのまま利用可能。


## インストール

1. このリポジトリをクローン。

```
git clone --recursive git@github.com:NaotoFujihiro/nRF52_SmartKey.git
```

2. ArduinoIDEの『スケッチブックの保存場所』を下記のように変更します。

`
/[YOUR_INSTALLED_PATH]/nRF52_SmartKey/nRF52_Arduino
`

## 使い方

* `nRF5_SDK_15.3.0_59ac345` 内のソースコードをコンパイルする場合

コンパイラを[こちら](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)からダウンロードします。

バージョンは、Makefileにあるように、 `7-2018-q2-update` を選択します。

MACをお使いの方は、Symlinkを貼ります。

```
sudo ln -s ~/[YOUR_PATH_TO_COMPILER]/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc /usr/local/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc
```

これで、コンパイルと実行ができるようになりました。

`Cryptography library` に関しては、[こちら](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.2.0%2Flib_crypto.html)をご覧ください。

```例(nRF5_SDK_15.3.0_59ac345/examples/crypto/nrf_crypto/ecdsa/pca10056/main.c をコンパイルして実行する場合)
// ベースディレクトリに移動。
$ cd /[YOUR_DOWNLOADED_PATH]/nRF52_SmartKey/nRF5_SDK_15.3.0_59ac345/

// 暗号化で使用するmicro-eccをインストールして、コンパイル。
$ cd external/micro-ecc/
$ git clone git@github.com:kmackay/micro-ecc.git
$ cd nrf52nf_armgcc/armgcc/
$ make

// 今回、実行したいmain.cをコンパイル。
$ cd ../../../../examples/crypto/nrf_crypto/ecdsa/pca10056/blank/armgcc/
$ make

// nRF52840に書き込む。
$ nrfjprog --eraseall
$ nrfjprog --chiperase --program _build/nrf52840_xxaa.hex -f NRF52 -r
```

JLinkRTTViewer のソフトウェアを使用すると、実行結果を確認できます。

[log](docs/JLinkRTTViewer-crypto_ecdsa.png)

JLinkRTTViewerの入手は[こちら](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack)から。


## ライセンス

[LICENSE](./LICENSE)




 