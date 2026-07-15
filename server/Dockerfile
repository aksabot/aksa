# Aksa compile server. Build from the repo root:
#   docker build -f server/Dockerfile -t aksa-server .
#   docker run -p 8765:8765 aksa-server
# The compile step itself needs no network; abuse is bounded by the server's
# body caps, queue, and hard timeout.

FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends gcc make && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY Makefile ./
COPY core core
RUN make aksa

FROM oven/bun:1
RUN apt-get update && apt-get install -y --no-install-recommends curl python3 ca-certificates && rm -rf /var/lib/apt/lists/* \
    && useradd -m aksa
USER aksa
ENV PATH="/home/aksa/bin:$PATH"
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/home/aksa/bin sh
# Installed as the aksa user so ~/.arduino15 stays user-owned.
RUN arduino-cli config init \
    && arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json \
    && arduino-cli core update-index \
    && arduino-cli core install esp32:esp32

WORKDIR /home/aksa/app
COPY --chown=aksa --from=build /src/aksa ./aksa
COPY --chown=aksa core core
COPY --chown=aksa runtime-device runtime-device
COPY --chown=aksa locales locales
COPY --chown=aksa server server
# Warm the core's object cache so the first real request compiles in seconds,
# not minutes.
RUN mkdir -p /tmp/warm/warm && printf 'void setup(){} void loop(){}\n' > /tmp/warm/warm/warm.ino \
    && arduino-cli compile --fqbn esp32:esp32:esp32 /tmp/warm/warm && rm -rf /tmp/warm

EXPOSE 8765
CMD ["bun", "server/compile.js"]
