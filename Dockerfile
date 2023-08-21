from alpine:3.18 as builder

WORKDIR /usr/src/tom_scott

RUN apk update
RUN apk upgrade
RUN apk add git
RUN apk add cmake
RUN apk add ninja
RUN apk add clang build-base llvm-static llvm-dev clang-static clang-dev

RUN apk add openssl-dev

COPY ./CMakeLists.txt .
COPY ./src ./src

RUN cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DDOCKER_BUILD=1
RUN cd build && ninja

from alpine:3.18

WORKDIR /usr/src/tom_scott

RUN apk update
RUN apk upgrade
RUN apk add build-base llvm-static clang-static

COPY --from=builder /usr/src/tom_scott/build/tom-scott ./
COPY --from=builder /usr/src/tom_scott/build/libdpp.* /usr/local/lib/


CMD ./tom-scott

