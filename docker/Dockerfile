FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Installiamo solo lo stretto necessario
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    g++ \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Build: se fallisce qui, l'action si ferma subito e ti dice perché
RUN mkdir -p build && cd build && cmake .. && make

# Esecuzione
CMD ["./build/unit_tests"]