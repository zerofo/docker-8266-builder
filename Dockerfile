From ubuntu:20.04
RUN apt update; apt install curl tar make libgcrypt-dev gcc srecord zip python3 gzip -y
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/bin sh
RUN curl -sSLo mklittlefs.tar.gz https://github.com/earlephilhower/mklittlefs/releases/download/3.0.0/x86_64-linux-gnu-mklittlefs-295fe9b.tar.gz && tar xfz mklittlefs.tar.gz -C /bin/ --strip-components=1 && rm mklittlefs.tar.gz
RUN arduino-cli core install esp8266:esp8266 --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json 
RUN arduino-cli core upgrade --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json 
RUN arduino-cli lib upgrade --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json 
RUN arduino-cli lib install SdFat --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json
COPY Ps4-wifi-http /Ps4-wifi-http
COPY mk8266.sh /
ENTRYPOINT ["/bin/bash","mk8266.sh"]
