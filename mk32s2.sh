#!/bin/bash
packuptime=`date '+%Y-%m-%d_%H%M%S'`
if [[ ! -f /config.txt ]]; then
    echo "";
    echo -e "\033[43;31m ############################# \033[0m";
    echo -e "\033[43;31m #     没有找到config.txt    # \033[0m";
    echo -e "\033[43;31m ############################# \033[0m";
    echo "";
    exit;
fi;
if [[ ! -d /mk32 || -z `ls /mk32` ]]; then
    echo "";
    echo -e "\033[43;31m ############################# \033[0m";
    echo -e "\033[43;31m #  没有找到 需要打包的项目  # \033[0m";
    echo -e "\033[43;31m ############################# \033[0m";
    echo "";
    exit;
fi;
for f in `find /mk32/ -type f -name *.manifest`;
do
    cacheF=$f;
done;

if [[ -z $cacheF ]]; then
    echo "";
    echo -e "\033[43;31m ########################################## \033[0m";
    echo -e "\033[43;31m #该网站没有找到manifest格式的离线缓存文件# \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m # 请修改对应的 cache/appcache 后缀的文件 # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #  与之对应的 html 文件 第一二行 文件名  # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #              修改后重试                # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #        该源码包  离线缓存不符合，      # \033[0m";
    echo -e "\033[43;31m #         请选择，否则10秒后退出：       # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #      ****强制自动替换  请输入(I)       # \033[0m"; 
    echo -e "\033[43;31m # (可能无效,适合cache/appcache格式的缓存)# \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #      ****强制忽略 请输入(Y)            # \033[0m";
    echo -e "\033[43;31m # (可能无效,适合没有离线缓存/不检测缓存) # \033[0m";
    echo -e "\033[43;31m ########################################## \033[0m";
    echo "";
    read -t 10 -n 1 Icache
    echo "";
    if [[ $Icache == "Y" || $Icache == "y" ]]
    then 
        echo "";
        echo -e "\033[43;31m ########################################## \033[0m";
        echo -e "\033[43;31m #               强制忽略                 # \033[0m";
        echo -e "\033[43;31m ########################################## \033[0m";
        echo "";
    elif [[ $Icache == "i" || $Icache == "I" ]]; then
        for f in `find /mk32/ -type f -name *cache`;
        do
            echo "";
            echo -e "\033[43;31m ########################################## \033[0m";
            echo -e "\033[43;31m #              强制自动替换              # \033[0m";
            echo -e "\033[43;31m ########################################## \033[0m";
            echo "";
            ext=${f##*.};
            cacheName=${f##*/};
            echo $cacheName;
            echo "";
            mv $f ${f%.*}.manifest;
            for fh in `find /mk32/ -type f -name *.html`;
            do
                sed -i "s/${cacheName}/${cacheName%.*}\.manifest/g" $fh;
            done;
        done;
    else
        echo "";
        echo -e "\033[43;31m ########################################## \033[0m";
        echo -e "\033[43;31m #                 退出                   # \033[0m";
        echo -e "\033[43;31m ########################################## \033[0m";
        echo "";
        exit;
    fi;
fi

blockList="";
WIFISSID="PS4-672_zerofo";
WIFIPass="";
IP="9,9,9,9";
source /config.txt;
echo -e "\033[43;31m ###   请检查 你的设置是否被读取生效   #### \033[0m";
echo "";
echo "config:"
echo "";
echo -e "\033[43;31m ### 如相关设置不正确请检查 config.txt #### \033[0m";
echo "";
cat /config.txt;
echo "";
echo -e "\033[43;31m ####     请核对离线包内容是否正确    ##### \033[0m";
echo "";
ls /mk32;
echo "";
echo -e "\033[43;31m ########################################## \033[0m";
echo "";
sed -i 's#//esp32s2-log1-##' /mk32/common/kexploit.js
sed -i 's#//esp32s2-log2-##' /mk32/common/kexploit.js
sed -i 's#//esp32s2-log3-##' /mk32/common/kexploit.js
cd /mk32;
rm .git .github -rf;
files=(`find ./ -mindepth 1 -type f |grep -v README.md|grep -v github|grep -v cache.manifest|grep -v .img`)
for f in ${files[@]};
do
    if [[ ( ! $f =~ ".git" ) && ( ! $f =~ ".css" ) && ( ! $f =~ ".manifest" ) ]]; then
        if [[ $compress == "y" ]];then
            if [[ ( ! ${blockList//,/ } =~ ${f##*/} ) || ( ! ${blockList//,/ } =~ ${f##*.} ) ]];then
            gzip -f $f;
            fi;
        else
            if [[ ( ${compress_List//,/ } =~ ${f##*/} ) ]];then
            gzip -f $f;
            fi;

        fi;
    fi;
done;
cd /;
sed -i "s/.*char\*\ WIFISSID.*/\ \ \ \ \ char\*\ WIFISSID\ =\ \"${WIFISSID}\"\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
sed -i "s/.*char\*\ WIFIPass.*/\ \ \ \ \ char\*\ WIFIPass\ =\ \"${WIFIPass}\"\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
sed -i "s/.*IPAddress\ IP\ =\ IPAddress.*/\ \ \ \ IPAddress\ IP\ =\ IPAddress(${IP})\;/" /Ps4-wifi-http/Ps4-wifi-http.ino;
grep WIFISSID /Ps4-wifi-http/Ps4-wifi-http.ino
echo "compile"
arduino-cli compile --fqbn esp32:esp32:esp32s2:CPUFreq=240 Ps4-wifi-http --output-dir=./firmware 
echo "built"
mklittlefs -c /mk32 -p 256 -b 4096 -s 0x2F0000 ./firmware/data.bin
echo "mkfs"
if [ $? -ne 0 ]; then
    echo "";
    echo -e "\033[43;31m ########################################## \033[0m";
    echo -e "\033[43;31m #            离线包体积超标              # \033[0m";
    echo -e "\033[43;31m #        请确认 confit.txt中:            # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #           compress=\"y\";                # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #           如还是不行则 去除:           # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #          blockArray=\"xxxxxx\"           # \033[0m";
    echo -e "\033[43;31m #                                        # \033[0m";
    echo -e "\033[43;31m #            此行后再试。                # \033[0m";
    echo -e "\033[43;31m #       仍然失败则，8266无法装下此包     # \033[0m";
    echo -e "\033[43;31m ########################################## \033[0m";
    echo "";
    exit;
fi;
#srec_cat -output /output/${binname}_${packuptime}.bin -binary  bootloader_qio_80m.bin -binary -offset 0x1000 -fill 0xff 0x0000 0x8000 Ps4-wifi-http.ino.partitions.bin -binary -offset 0x8000 -fill 0xff 0x8000 0x10000 ./firmware/Ps4-wifi-http.ino.bin -binary -offset 0x10000 0x11000 data.bin -binary -offset 0x10000 -fill 0xff 0x3F0000 ;
#srec_cat -output /output/${binname}_${packuptime}.bin -binary  /esp32_base/bootloader_qio_80m.bin -binary -offset 0x1000 -fill 0xff 0x0000 0x8000 /esp32_base/partition.bin -binary -offset 0x8000 -fill 0xff 0x8000 0x10000 ./firmware/Ps4-wifi-http.ino.bin -binary -offset 0x10000 -fill 0xff 0x10000 0x100000 ./firmware/data.bin -binary -offset 0x100000

#-binary -offset 0x1000 -fill 0xff 0x1000 0x8000 ./firmware/Ps4-wifi-http.ino.partitions.bin \
#-binary -generate 0x0000 0x1000 -constant 0xff /esp32_base/bootloader_qio_80m.bin \
#-binary -offset 0x1000 -fill 0xff 0x1000 0x8000 /esp32_base/partition.bin \
#./firmware/Ps4-wifi-http.ino.bootloader.bin \
srec_cat -output /output/${binname}_${packuptime}.bin \
-binary -generate 0x0000 0x1000 -constant 0xff /esp32_base/esp.ino.bootloader.bin \
-binary -offset 0x1000 -fill 0xff 0x1000 0x8000 /esp32_base/esp.ino.partitions.bin \
-binary -offset 0x8000 -fill 0xff 0x8000 0x10000 ./firmware/Ps4-wifi-http.ino.bin \
-binary -offset 0x10000 -fill 0xff 0x10000 0x110000  ./firmware/data.bin \
-binary -offset 0x110000 -fill 0xff 0x110000 0x3F0000;
echo "output"
if [[ ! -f "/output/${binname}_${packuptime}.bin" ]];then
echo "编译出错";
fi;
md5sum /output/${binname}_${packuptime}.bin > /output/${binname}_${packuptime}.md5;

zip /output/${binname}_${packuptime}.zip /output/${binname}_${packuptime}.bin /output/${binname}_${packuptime}.md5;
rm /output/${binname}_${packuptime}.bin /output/${binname}_${packuptime}.md5;
rm /mk32/* -rf;
