#include <cstddef>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>

using Byte = uint8_t;
using Bytes = std::queue<Byte>;

class Producer {
public:
    explicit Producer(Bytes *sink);
    void Simulate(const int begin_count=1,
                  const int middle_count=3,
                  const int end_count=1);

private:
    Bytes *sink_;
};

enum /*class*/ ByteType { Begin = 0x01, Middle = 0x42, End = 0x80 };

Producer::Producer(Bytes *sink)
    : sink_(sink) {}

void Producer::Simulate(const int begin_count,
                        const int middle_count,
                        const int end_count) {
    for (auto i = 0; i < begin_count; ++i) {
        sink_->push(ByteType::Begin);
    }
    for (auto i = 0; i < middle_count; ++i) {
        sink_->push(ByteType::Middle);
    }
    for (auto i = 0; i < end_count; ++i) {
        sink_->push(ByteType::End);
    }
}


using Token = std::vector<Byte>;
using Tokens = std::queue<Token>;


class Consumer {
public:
    explicit Consumer(Bytes *source, Tokens *sink);
    bool AreBytesReady() const;
    void ConsumeReadyBytes();

private:
    bool Lookahead(const ByteType expected) const;
    Byte Consume();

    Bytes *source_;
    std::unique_ptr<Token> token_;
    Tokens *sink_;
};

Consumer::Consumer(Bytes *source, Tokens *sink)
    : source_(source)
    , sink_(sink) {}

bool Consumer::AreBytesReady() const {
    return !source_->empty();
}

void Consumer::ConsumeReadyBytes() {
    while (!source_->empty()) {
        if (!token_) {
            token_.reset(new Token);
            if (!Lookahead(ByteType::Begin)) {
                throw std::runtime_error("expected begin byte, got something else");
            }
            auto byte = Consume();
            token_->push_back(byte);
        } else if (Lookahead(ByteType::Middle)) {
            auto byte = Consume();
            token_->push_back(byte);
        }
        else if (Lookahead(ByteType::End)) {
            auto byte = Consume();
            token_->push_back(byte);

            sink_->push(*(token_.get()));
            token_.reset(nullptr);
        } else {
            throw std::runtime_error("unexpected byte");
        }
    }
}

bool Consumer::Lookahead(const ByteType expected) const {
    auto byte = source_->front();
    return expected == byte;
}

Byte Consumer::Consume() {
    auto byte = source_->front();
    source_->pop();
    return byte;
}

int main(int, char*[]) {
    Bytes bytes;
    Producer producer(&bytes);
    producer.Simulate();

    Tokens tokens;
    Consumer consumer(&bytes, &tokens);
    if (consumer.AreBytesReady()) {
        consumer.ConsumeReadyBytes();
    }
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        for (auto byte : token) {
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<uint32_t>(byte) << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
