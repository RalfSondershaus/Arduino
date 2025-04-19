FROM ubuntu:24.10
WORKDIR /app

# Install git and other dependencies
RUN apt-get update && \
    apt-get install -y git

# Install make
RUN apt-get install -y make

# Install AVR GCC
RUN apt-get install -y gcc-avr avr-libc

# Authorize SSH Host
# RUN mkdir -p /root/.ssh && \
#     chmod 0700 /root/.ssh && \
#     ssh-keyscan [gitlab.com|github.com] > /root/.ssh/known_hosts

# Add the keys and set permissions
# COPY ./ssh/id_rsa /root/.ssh/id_rsa
# RUN chmod 600 /root/.ssh/id_rsa

# Clone your project
# RUN git clone git@[gitlab.com|github.com]:username/repository.git
RUN git clone https://github.com/RalfSondershaus/Arduino arduino

WORKDIR /app/arduino

# Make it
# CMD ["make", "-s", "-f", "./Build/make/Makefile.gmk", "ARG_PROJECT=App/BlinkSample", "ARG_TARGET_ECU=mega", "ARG_COMPILER=avr_gcc", "ARG_BSW=arduino", "ARG_BSW_TESTFRAMEWORK=none", "all"]
CMD ["./Build/build_with_pull.sh", "App/BlinkSample", "mega", "avr_gcc", "arduino", "none", "all"]
# CMD ["git", "pull"]

