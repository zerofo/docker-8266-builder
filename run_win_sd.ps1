$path = Split-Path -Parent $MyInvocation.MyCommand.Definition;
docker run -it --rm --name budil8266 -v ${path}:/output -v ${path}\config.txt:/config.txt -v ${path}\${args}:/mk8266  zerofo/build_ps4_8266:sd
