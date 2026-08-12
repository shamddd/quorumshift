# Multi-stage build for C++20 Distributed Storage Simulator
FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    cmake \
    ninja-build \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --target dss-cli dss-bench

FROM debian:bookworm-slim AS runner

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    ca-certificates \
    python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/dss-cli /app/bin/dss-cli
COPY --from=builder /app/build/dss-bench /app/bin/dss-bench
COPY scripts /app/scripts
COPY python /app/python

ENV PATH="/app/bin:${PATH}"

ENTRYPOINT ["dss-cli"]
CMD ["demo"]
