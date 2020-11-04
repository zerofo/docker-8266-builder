# docker-8266-builder

## 目前run.sh 仅适用于 有docker的 linux。 windows 需要自己写 run脚本



8266's code modify from https://github.com/zerofo/ESP8266-WEB-FTP-DNS-Basico 

## 修改config.txt 修改对应参数
```
WIFISSID="PS4-672_zerofo"; #wifi名称;
WIFIPass=""; #wifi密码;
IP="9,9,9,9"; #IP地址;
binname="ps4672-zerofo"; #输出名称
UserOffline="ok" #用户指南支持离线缓存 并断网
```


# 执行
```
# bash run.sh 离线包文件夹路径名称(index.html 所在)
# 例如 index.html 在上层目录 ../zerofo.github.io/index.html
bash run.sh ../zerofo.github.io/;
```
