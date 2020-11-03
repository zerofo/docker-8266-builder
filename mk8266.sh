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
for f in `ls /mk8266/*.manifest`;
do
    cacheF=$f;
done;

echo ${cacheF};
cd /mk8266;
rm .git .github -rf;
echo "CACHE MANIFEST" > ${cacheF};
files=(`find ./ -mindepth 1 -type f |grep -v README.md|grep -v github|grep -v cache.manifest`)
for f in ${files[@]};
do
    echo $f >> ${cacheF};
    if [ ${#f} -gt 30 ]; then
        echo "$f 目录加文件名超过了30。需要手动修改文件名和index.html 对应位置，否则无法正常生成bin";
        exit;
    fi;
    if [[ ! $f =~ ".css" || ! $f =~ ".gz" ]];then
        if [[ $f =~ "index" ]];then 
            sed -i "s/<head>/<head>\n    <script>if (location.pathname != '\/' && location.pathname != '\/index.html') location\.href=location\.origin;<\/script>/g" /mk8266/index.html;
        fi;
        gzip $f -f;
    fi;
done;
echo "FALLBACK:" >> ${cacheF};
echo ". index.html" >> ${cacheF};
source /config.txt;
cd /Ps4-wifi-http;
sed -i "s/.*char\*\ WIFISSID.*/\ \ \ \ \ char\*\ WIFISSID\ =\ \"${WIFISSID}\"\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
sed -i "s/.*char\*\ WIFIPass.*/\ \ \ \ \ char\*\ WIFIPass\ =\ \"${WIFIPass}\"\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
sed -i "s/.*IPAddress\ IP\ =\ IPAddress.*/\ \ \ \ IPAddress\ IP\ =\ IPAddress(${IP})\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2:xtal=160,baud=115200,eesz=4M3M Ps4-wifi-http --output-dir=./firmware 
mkspiffs -c /mk8266 -p 256 -b 8192 -s 0x2FA000 ./firmware/data.bin
srec_cat -output /output/${binname}_${packuptime}.bin -binary ./firmware/Ps4-wifi-http.ino.bin -binary  -fill 0xFF 0x0 0x00100000 ./firmware/data.bin -binary -offset 0x00100000;
if [[ ! -f "/output/${binname}_${packuptime}.bin" ]];then
echo "编译出错";
fi;
md5sum /output/${binname}_${packuptime}.bin > /output/${binname}_${packuptime}.md5;

zip /output/${binname}_${packuptime}.zip /output/${binname}_${packuptime}.bin /output/${binname}_${packuptime}.md5;
rm /output/${binname}_${packuptime}.bin /output/${binname}_${packuptime}.md5;
rm /mk8266/* -rf;
