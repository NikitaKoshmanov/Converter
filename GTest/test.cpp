#include "pch.h"
#include "..\Testing\Converter.cpp"

class ConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
        source = new MockSource();
        sink = new MockSink();
        converter = new Converter(source, sink);
    }

    void TearDown() override {
        delete converter;
        delete sink;
        delete source;
    }

    MockSource* source;
    MockSink* sink;
    Converter* converter;
};

TEST(ConverterTest, TestConversion) {
    EXPECT_CALL(*source, readByte())
        .WillOnce(::testing::Return(0b00000001)) // type = 0b00, data = 0b000001
        .WillOnce(::testing::Return(0b01000010)) // type = 0b01, data = 0b000010
        .WillOnce(::testing::Return(0b10000100)); // type = 0b10, data = 0b000100

    std::string expected1 = "Unsigned integer : 1";
    std::string expected2 = "Signed integer : 2";
    std::string expected3 = "Letter: e";

    EXPECT_CALL(*sink, writeString(expected1));
    EXPECT_CALL(*sink, writeString(expected2));
    EXPECT_CALL(*sink, writeString(expected3));

    converter->start();

    EXPECT_EQ(converter->readByte(), 0b00000001);
    EXPECT_EQ(converter->readByte(), 0b01000010);
    EXPECT_EQ(converter->readByte(), 0b10000100);

    converter->stop();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}