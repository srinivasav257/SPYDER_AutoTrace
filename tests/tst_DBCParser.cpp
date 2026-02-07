/**
 * @file tst_DBCParser.cpp
 * @brief Unit tests for DBCParser — parsing, encoding, decoding, index.
 *
 * Uses inline DBC content strings so no external files are needed.
 */

#include <gtest/gtest.h>
#include "DBCParser.h"
#include <cstring>

using namespace DBCManager;

// ============================================================================
// Minimal DBC content for testing
// ============================================================================

static const char* MINIMAL_DBC = R"(
VERSION "1.0"

NS_ :

BS_:

BU_: ECU1 Tester

BO_ 256 EngineData: 8 ECU1
 SG_ EngineSpeed : 0|16@1+ (0.25,0) [0|16383.75] "rpm" Tester
 SG_ EngineTemp : 16|8@1- (1,-40) [-40|215] "degC" Tester

BO_ 512 TransmissionData: 8 ECU1
 SG_ GearPosition : 0|4@1+ (1,0) [0|15] "" Tester
 SG_ TorquePercent : 8|8@1+ (0.4,0) [0|100] "%" Tester

CM_ BO_ 256 "Engine data message containing speed and temperature";
CM_ SG_ 256 EngineSpeed "Engine speed in rpm";
CM_ BU_ ECU1 "Main engine control unit";

VAL_ 512 GearPosition 0 "Park" 1 "Reverse" 2 "Neutral" 3 "Drive" ;
)";

// ============================================================================
// Parse basics
// ============================================================================

TEST(DBCParser, ParseMinimalDBC_NoErrors)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    EXPECT_FALSE(parser.hasErrors())
        << "Parser errors: " << (parser.errors().isEmpty() ? "" : parser.errors()[0].message.toStdString());
    EXPECT_EQ(db.version, "1.0");
}

TEST(DBCParser, ParseMinimalDBC_Nodes)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    EXPECT_EQ(db.nodes.size(), 2);
    EXPECT_EQ(db.nodes[0].name, "ECU1");
    EXPECT_EQ(db.nodes[1].name, "Tester");
}

TEST(DBCParser, ParseMinimalDBC_Messages)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    EXPECT_EQ(db.messages.size(), 2);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);
    EXPECT_EQ(eng->name, "EngineData");
    EXPECT_EQ(eng->dlc, 8u);
    EXPECT_EQ(eng->sender, "ECU1");
    EXPECT_EQ(eng->signalList.size(), 2);
}

TEST(DBCParser, ParseMinimalDBC_Signals)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);

    const DBCSignal* speed = eng->signal("EngineSpeed");
    ASSERT_NE(speed, nullptr);
    EXPECT_EQ(speed->startBit, 0u);
    EXPECT_EQ(speed->bitLength, 16u);
    EXPECT_EQ(speed->byteOrder, ByteOrder::LittleEndian);
    EXPECT_EQ(speed->valueType, ValueType::Unsigned);
    EXPECT_DOUBLE_EQ(speed->factor, 0.25);
    EXPECT_DOUBLE_EQ(speed->offset, 0.0);
    EXPECT_EQ(speed->unit, "rpm");

    const DBCSignal* temp = eng->signal("EngineTemp");
    ASSERT_NE(temp, nullptr);
    EXPECT_EQ(temp->bitLength, 8u);
    EXPECT_EQ(temp->valueType, ValueType::Signed);
    EXPECT_DOUBLE_EQ(temp->offset, -40.0);
}

// ============================================================================
// Comments parsed correctly (requires working index during parse)
// ============================================================================

TEST(DBCParser, CommentsAttachedCorrectly)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);
    EXPECT_EQ(eng->comment, "Engine data message containing speed and temperature");

    const DBCSignal* speed = eng->signal("EngineSpeed");
    ASSERT_NE(speed, nullptr);
    EXPECT_EQ(speed->comment, "Engine speed in rpm");

    // Node comment
    bool found = false;
    for (const auto& node : db.nodes) {
        if (node.name == "ECU1") {
            EXPECT_EQ(node.comment, "Main engine control unit");
            found = true;
        }
    }
    EXPECT_TRUE(found) << "ECU1 node not found";
}

// ============================================================================
// Value descriptions (requires working index during parse)
// ============================================================================

TEST(DBCParser, ValueDescriptionsAttached)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* trans = db.messageById(512);
    ASSERT_NE(trans, nullptr);

    const DBCSignal* gear = trans->signal("GearPosition");
    ASSERT_NE(gear, nullptr);

    EXPECT_EQ(gear->valueDescriptions.size(), 4);
    EXPECT_EQ(gear->valueDescriptions.value(0), "Park");
    EXPECT_EQ(gear->valueDescriptions.value(1), "Reverse");
    EXPECT_EQ(gear->valueDescriptions.value(2), "Neutral");
    EXPECT_EQ(gear->valueDescriptions.value(3), "Drive");
}

// ============================================================================
// Index / lookup
// ============================================================================

TEST(DBCParser, MessageByIdLookup)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    EXPECT_NE(db.messageById(256), nullptr);
    EXPECT_NE(db.messageById(512), nullptr);
    EXPECT_EQ(db.messageById(999), nullptr);
}

TEST(DBCParser, MessageByNameLookup)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    EXPECT_NE(db.messageByName("EngineData"), nullptr);
    EXPECT_NE(db.messageByName("TransmissionData"), nullptr);
    EXPECT_EQ(db.messageByName("NonExistent"), nullptr);
}

TEST(DBCParser, MessageIds)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    auto ids = db.messageIds();
    EXPECT_EQ(ids.size(), 2);
    EXPECT_TRUE(ids.contains(256));
    EXPECT_TRUE(ids.contains(512));
}

TEST(DBCParser, RebuildIndex)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    // Mutate and rebuild
    db.messages.append(DBCMessage{0x7FF, "NewMsg", 8, "ECU1"});
    db.buildIndex();

    EXPECT_NE(db.messageById(0x7FF), nullptr);
    EXPECT_EQ(db.messages.size(), 3);
}

// ============================================================================
// Encode / Decode round-trip
// ============================================================================

TEST(DBCParser, SignalDecodeEncode_RoundTrip)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);
    const DBCSignal* speed = eng->signal("EngineSpeed");
    ASSERT_NE(speed, nullptr);

    // Encode 3000.0 rpm  →  raw = 3000 / 0.25 = 12000
    uint8_t data[8] = {};
    speed->encode(3000.0, data, 8);

    // Decode back
    double decoded = speed->decode(data, 8);
    EXPECT_NEAR(decoded, 3000.0, 0.01);
}

TEST(DBCParser, SignedSignal_DecodeEncode)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);
    const DBCSignal* temp = eng->signal("EngineTemp");
    ASSERT_NE(temp, nullptr);

    // -10°C  →  raw = (-10 - (-40)) / 1 = 30
    uint8_t data[8] = {};
    temp->encode(-10.0, data, 8);
    double decoded = temp->decode(data, 8);
    EXPECT_NEAR(decoded, -10.0, 0.01);
}

TEST(DBCParser, DecodeAll)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);

    // Manually set EngineSpeed raw = 4000  → physical = 1000 rpm
    // EngineTemp raw = 70  → physical = 70 - 40 = 30 degC
    uint8_t data[8] = {};
    std::memset(data, 0, 8);
    eng->signal("EngineSpeed")->encode(1000.0, data, 8);
    eng->signal("EngineTemp")->encode(30.0, data, 8);

    auto values = eng->decodeAll(data, 8);
    EXPECT_NEAR(values["EngineSpeed"], 1000.0, 0.01);
    EXPECT_NEAR(values["EngineTemp"], 30.0, 0.01);
}

TEST(DBCParser, ValueToString_WithDescription)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* trans = db.messageById(512);
    ASSERT_NE(trans, nullptr);
    const DBCSignal* gear = trans->signal("GearPosition");
    ASSERT_NE(gear, nullptr);

    // Physical 0.0 → raw 0 → "Park"
    EXPECT_EQ(gear->valueToString(0.0), "Park");
    EXPECT_EQ(gear->valueToString(3.0), "Drive");
}

// ============================================================================
// Extended ID
// ============================================================================

TEST(DBCParser, ExtendedId)
{
    const char* dbc = R"(
VERSION ""
BU_:
BO_ 2147484160 ExtMsg: 8 Vector__XXX
 SG_ Sig1 : 0|8@1+ (1,0) [0|255] "" Vector__XXX
)";

    DBCParser parser;
    DBCDatabase db = parser.parseString(dbc);
    EXPECT_FALSE(parser.hasErrors());

    // 2147484160 = 0x80000400 → extended, actual ID = 0x400
    ASSERT_EQ(db.messages.size(), 1);
    EXPECT_TRUE(db.messages[0].isExtended);
    EXPECT_EQ(db.messages[0].id, 0x400u);
}

// ============================================================================
// Empty / invalid input
// ============================================================================

TEST(DBCParser, EmptyInput)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString("");
    EXPECT_TRUE(db.isEmpty());
    EXPECT_FALSE(parser.hasErrors());
}

TEST(DBCParser, DisplayString)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);

    const DBCMessage* eng = db.messageById(256);
    ASSERT_NE(eng, nullptr);
    QString display = eng->displayString();
    EXPECT_TRUE(display.contains("100")) << "256 = 0x100, display: " << display.toStdString();
    EXPECT_TRUE(display.contains("EngineData"));
}

TEST(DBCParser, TotalSignalCount)
{
    DBCParser parser;
    DBCDatabase db = parser.parseString(MINIMAL_DBC);
    EXPECT_EQ(db.totalSignalCount(), 4); // 2 + 2
}
