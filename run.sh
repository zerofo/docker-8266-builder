#!/bin/bash
rm -rf ./tmp_data;
cp -r $1 ./tmp_data;
docker run -it --rm --name budil8266 -v $(pwd)/:/output -v $(pwd)/config.txt:/config.txt -v $(pwd)/tmp_data/:/mk8266  zerofo/build_ps4_8266

