# docker-8266-builder

8266's code modify from https://github.com/zerofo/ESP8266-WEB-FTP-DNS-Basico 

## 修改config.txt 修改对应参数
```
WIFISSID="ps4-meun_by_zerofo"; #wifi名称;
WIFIPass=""; #wifi密码;
IP="9,9,9,9"; #IP地址;
binname="ps4-zerofo"; #输出名称
compress="n" #进行gzip 压缩 y压缩，输入其他则不压缩
blockList=".htm, index.html" #标记不压缩格式、文件名 以逗号区分
```

# win10 执行 (需要安装  docker)
```
# run_win.ps1 离线包文件夹路径名称(index.html 所在)
# 例如 index.html 在上层目录 ../zerofo.github.io/index.html
.\run_win.ps1 ../zerofo.github.io/;
```

# linux 执行 (需要安装  docker)
```
# bash run.sh 离线包文件夹路径名称(index.html 所在)
# 例如 index.html 在上层目录 ../zerofo.github.io/index.html
bash run.sh ../zerofo.github.io/;
```
