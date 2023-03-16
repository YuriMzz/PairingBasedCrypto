FROM contiker/contiki-ng

# relic needs cmake
RUN sudo apt-get autoremove && \
sudo apt-get update && \
sudo apt-get -y install cmake && \
sudo apt-get -y install gdb-multiarch && \
sudo apt-get -y install m4 && \
sudo ln -s $(which python3) /usr/bin/python