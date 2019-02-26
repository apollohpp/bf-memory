FROM ubuntu:16.04 as builder

RUN apt-get update && apt-get install -y \
    g++ \
    make

COPY /cmake /cmake
COPY /bf-memory /bf-memory
WORKDIR /cmake

RUN make


FROM ubuntu:16.04

RUN mkdir /work
WORKDIR /work

COPY --from=builder /cmake/test.out ./test.out
CMD ["./test.out"]