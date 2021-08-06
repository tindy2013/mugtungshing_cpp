FROM alpine:3.12

WORKDIR /bot
COPY * ./
RUN apk add --virtual .build_deps build-base linux-headers gcc cmake boost-dev curl-dev zlib-dev openssl-dev rapidjson-dev git && \
    git clone https://github.com/reo7sp/tgbot-cpp --depth=1 && \
	cd tgbot-cpp && \
	cmake -DCMAKE_BUILD_TYPE=Release . && \
	make install -j2 && \
	cd .. && \
	c++ -o bot main.cpp -lTgBot -lcurl -lssl -lcrypto -lz -std=c++2a -O3 && \
	apk add curl zlib openssl libstdc++ && \
	apk del .build_deps && \
	rm -rf /var/cache

ENTRYPOINT ./bot