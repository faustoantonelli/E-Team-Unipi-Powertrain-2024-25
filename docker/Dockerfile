FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y g++ --no-install-recommends
COPY tester.cpp /build/tester.cpp
RUN g++ -O3 /build/tester.cpp -o /engine

FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ valgrind python3 octave chktex cppcheck \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /engine /usr/local/bin/engine
RUN chmod +x /usr/local/bin/engine

WORKDIR /github/workspace
ENTRYPOINT ["engine"]
# IMPORTANTE PER GITHUB ACTIONS:
# GitHub monta il codice del repo in /github/workspace
WORKDIR /github/workspace

ENTRYPOINT ["engine"]