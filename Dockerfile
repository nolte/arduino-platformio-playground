FROM python:2.7-wheezy

WORKDIR /iotsrc

RUN python -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"

# PubSubClient
# Json Lib

# IR lib

ENTRYPOINT ["/usr/local/bin/platformio","ci"]