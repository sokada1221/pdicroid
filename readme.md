# 環境構築

## 動作環境

- Windowsを前提

## NDKのビルド

- AndroidStudioでNDKをinstallする
  - Tools - SDK Managerにて、
    - SDK ToolsタブでNDKを選択
      - 右下の"Show Package Details"をチェックして使用するNDK versionを選択
      - 検証済みversion
        - 29.0.1420685
        - 21.0.6113669
      - 使用するNDK versionに応じてMakefileのコマンドラインオプションを変更する
        - target build:のところを参照

- MSYS2, MinGWなど、make, cpコマンドが実行できる環境を用意する
  - MSYS2を使用する場合
    - https://www.msys2.org/ より installerをdownload/install

- 環境変数
  - ndk-build.cmdへのpathを設定する
  - 通常 C:\Users\<user>\AppData\Local\Android\Sdk\ndk\<installed version> にインストールされる
  - 次のいずれか。
    - ndk-build.cmdへのpathを環境変数PATHに設定
    - NDK_PATH=ndk-build.cmdへのpathを記述

## 確認済みversion

ndk-build.cmd --version
GNU Make 3.82.90, 4.4.1
