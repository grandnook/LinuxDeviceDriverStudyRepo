
# 環境の準備
カーネルの機能にアクセスするためのヘッダ等が必要
ヘッダ一式とビルド用Makefileがここ(`/usr/src/linux-headers-4.9.41-v7+/`)にインストールされる

シンボリックリンクがここ(`/lib/modules/4.9.41-v7+/build`)に作成される

なお，4.9.41-v7+については，`uname -r`で取得できる

```bash
$ sudo apt-get install raspberrypi-kernel-headers
```

<pre>
パッケージリストを読み込んでいます... 完了
依存関係ツリーを作成しています                
状態情報を読み取っています... 完了
以下のパッケージが新たにインストールされます:
  raspberrypi-kernel-headers
アップグレード: 0 個、新規インストール: 1 個、削除: 0 個、保留: 161 個。
16.7 MB のアーカイブを取得する必要があります。
この操作後に追加で 109 MB のディスク容量が消費されます。
取得:1 http://archive.raspberrypi.org/debian stretch/main armhf raspberrypi-kernel-headers armhf 1.20190819~stretch-1 [16.7 MB]
16.7 MB を 4秒 で取得しました (3,737 kB/s)             
以前に未選択のパッケージ raspberrypi-kernel-headers を選択しています。
(データベースを読み込んでいます ... 現在 122663 個のファイルとディレクトリがインストールされています。)
.../raspberrypi-kernel-headers_1.20190819~stretch-1_armhf.deb を展開する準備をしています ...
raspberrypi-kernel-headers (1.20190819~stretch-1) を展開しています...
raspberrypi-kernel-headers (1.20190819~stretch-1) を設定しています ...
</pre>

# make
```bash
$ make
```
<pre>
make -C /lib/modules/4.19.66-v7+/build M=/home/pi/work_device_driver/01_01 modules
make[1]: ディレクトリ '/usr/src/linux-headers-4.19.66-v7+' に入ります
  CC [M]  /home/pi/work_device_driver/01_01/test.o
  Building modules, stage 2.
  MODPOST 1 modules
WARNING: modpost: missing MODULE_LICENSE() in /home/pi/work_device_driver/01_01/test.o
see include/linux/module.h for more information
  CC      /home/pi/work_device_driver/01_01/test.mod.o
  LD [M]  /home/pi/work_device_driver/01_01/test.ko
make[1]: ディレクトリ '/usr/src/linux-headers-4.19.66-v7+' から出ます
</pre>

# 実行
```bash
$ sudo insmod test.ko
$ sudo rmmod test.ko
$ dmesg
```
<pre>
[2131630.263067] Hello my module
[2131652.234242] Bye bye my module
</pre>