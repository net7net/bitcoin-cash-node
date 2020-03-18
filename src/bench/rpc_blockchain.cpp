// Copyright (c) 2016-2019 The Bitcoin Core developers
// Copyright (c) 2020 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bench/bench.h>
#include <bench/data.h>

#include <chainparams.h>
#include <validation.h>
#include <streams.h>
#include <consensus/validation.h>
#include <rpc/blockchain.h>
#include <rpc/protocol.h>

#include <univalue.h>

static void BlockToJsonVerbose(benchmark::State& state) {
    SelectParams(CBaseChainParams::MAIN);

    CDataStream stream(benchmark::data::block413567, SER_NETWORK, PROTOCOL_VERSION);
    char a = '\0';
    stream.write(&a, 1); // Prevent compaction

    CBlock block;
    stream >> block;

    CBlockIndex blockindex;
    const auto blockHash = block.GetHash();
    blockindex.phashBlock = &blockHash;
    blockindex.nBits = 403014710;

    while (state.KeepRunning()) {
        (void)JSONRPCReply(
            blockToJSON(block, &blockindex, &blockindex, /*verbose*/ true),
            UniValue(UniValue::VNULL),
            UniValue("benchmark"));
    }
}

static void JsonReadWrite1MBBlock(benchmark::State& state) {
    SelectParams(CBaseChainParams::MAIN);

    CDataStream stream(benchmark::data::block413567, SER_NETWORK, PROTOCOL_VERSION);
    char a = '\0';
    stream.write(&a, 1); // Prevent compaction

    CBlock block;
    stream >> block;

    CBlockIndex blockindex;
    const auto blockHash = block.GetHash();
    blockindex.phashBlock = &blockHash;
    blockindex.nBits = 403014710;
    const auto blockuv = blockToJSON(block, &blockindex, &blockindex, /*verbose*/ true);

    UniValue uv;
    while (state.KeepRunning()) {
        // write it out to str and read it back again to test reading & writing of UniValue
        if (!uv.read(blockuv.write(4/* pretty indent 4 spaces */)))
            throw std::runtime_error("UniValue lib failed to parse its own generated string.");
    }
}

BENCHMARK(BlockToJsonVerbose, 10);
BENCHMARK(JsonReadWrite1MBBlock, 7);