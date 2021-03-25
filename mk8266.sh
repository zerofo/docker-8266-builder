#!/bin/bash
packuptime=`date '+%Y-%m-%d_%H%M%S'`
if [[ ! -f /config.txt ]]; then
    echo "没有找到config.txt";
    exit;
fi;
if [[ ! -d /mk8266 || -z `ls /mk8266` ]]; then
    echo "没有找到 需要打包的项目";
    exit;
fi;
for f in `find /mk8266/ -type f -name *.manifest`;
do
    cacheF=$f;
done;

if [[ -z $cacheF ]]; then
    echo "该网站没有找到manifest格式的离线缓存文件";
    echo "";
    echo "请 修改对应的 cache/appcache 后缀的文件";
    echo "";
    echo " 与之对应的 html 文件 第一二行 文件名";
    echo "";
    echo " 修改后从试";
    echo "";
    echo " 该源码包  离线缓存不符合， 请选择，否则10秒后退出：";
    echo "";
    echo "强制自动替换(可能无法使用,适合使用cache/appcache格式缓存的离线包) 请输入(I)"
    echo "";
    echo "强制忽略(可能无法使用,适合默认没有离线缓存，不检测离线缓存的离线包) 请输入(Y)"
    read -t 10 -n 1 Icache
    echo "";
    if [[ $Icache == "Y" || $Icache == "y" ]]
    then 
        echo "强制继续";
    elif [[ $Icache == "i" || $Icache == "I" ]]; then
        for f in `find /mk8266/ -type f -name *cache`;
        do
            echo "强制自动替换";
            ext=${f##*.};
            cacheName=${f##*/};
            echo $cacheName;
            echo "";
            mv $f ${f%.*}.manifest;
            for fh in `find /mk8266/ -type f -name *.html`;
            do
                sed -i "s/${cacheName}/${cacheName%.*}\.manifest/g" $fh;
            done;
        done;
    else
        echo "退出";
        exit;
    fi;
fi

blockList="";
WIFISSID="PS4-672_zerofo";
WIFIPass="";
IP="9,9,9,9";
source /config.txt;
echo "config:"
cat /config.txt;
ls /mk8266;
cd /mk8266;
rm .git .github -rf;
files=(`find ./ -mindepth 1 -type f |grep -v README.md|grep -v github|grep -v cache.manifest`)
for f in ${files[@]};
do
    if [[ ! $f =~ ".css" || ! $f =~ ".gz" || ! $f =~ ".manifest" ]];then
        if [[ $compress == "y" ]];then
            if [[ $blockList != "" ]];then

                blockArray=(${blockList//,/ });
                comp=1;
                for var in ${blockArray[@]}
                do
                   if [[ ${f##*/} == $var || ${f##*.} == "\.${var}" ]]; then
                      comp=0;
                      break;
                   fi;
                done;
                if [[ $comp -eq 1 ]]; then
                   gzip $f -f;
                fi;
            else   
                gzip $f -f;
            fi;
        fi;
    fi;
done;
cd /Ps4-wifi-http;
sed -i "s/.*char\*\ WIFISSID.*/\ \ \ \ \ char\*\ WIFISSID\ =\ \"${WIFISSID}\"\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
sed -i "s/.*char\*\ WIFIPass.*/\ \ \ \ \ char\*\ WIFIPass\ =\ \"${WIFIPass}\"\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
sed -i "s/.*IPAddress\ IP\ =\ IPAddress.*/\ \ \ \ IPAddress\ IP\ =\ IPAddress(${IP})\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2:xtal=160,baud=115200,eesz=4M3M Ps4-wifi-http --output-dir=./firmware 
mklittlefs -c /mk8266 -p 256 -b 8192 -s 0x2FA000 ./firmware/data.bin
srec_cat -output /output/${binname}_${packuptime}.bin -binary ./firmware/Ps4-wifi-http.ino.bin -binary  -fill 0xFF 0x0 0x00100000 ./firmware/data.bin -binary -offset 0x00100000;
if [[ ! -f "/output/${binname}_${packuptime}.bin" ]];then
echo "编译出错";
fi;
md5sum /output/${binname}_${packuptime}.bin > /output/${binname}_${packuptime}.md5;

zip /output/${binname}_${packuptime}.zip /output/${binname}_${packuptime}.bin /output/${binname}_${packuptime}.md5;
rm /output/${binname}_${packuptime}.bin /output/${binname}_${packuptime}.md5;
rm /mk8266/* -rf;
