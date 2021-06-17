From ubuntu:20.04
RUN apt update; apt install curl tar make libgcrypt-dev gcc srecord zip python3 gzip unzip -y
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/bin sh
RUN curl -sSLo mklittlefs.tar.gz https://github.com/earlephilhower/mklittlefs/releases/download/3.0.0/x86_64-linux-gnu-mklittlefs-295fe9b.tar.gz && tar xfz mklittlefs.tar.gz -C /bin/ --strip-components=1 && rm mklittlefs.tar.gz
RUN arduino-cli config init && arduino-cli config add board_manager.additional_urls http://arduino.esp8266.com/stable/package_esp8266com_index.json
# https://esp8266.github.io/stable/package_esp8266com_index.json
RUN arduino-cli config set library.enable_unsafe_install true
RUN arduino-cli core install esp8266:esp8266 
RUN arduino-cli core upgrade 
RUN arduino-cli lib upgrade 
COPY Ps4-wifi-http_nosd /Ps4-wifi-http
COPY mk8266.sh /
ENTRYPOINT ["/bin/bash","mk8266.sh"]
