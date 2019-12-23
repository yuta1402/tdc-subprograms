# TODO
- [x] tdc-frozen-bit-analyzerの実装
- [x] tdc-sc-benchmarkの実装
- [ ] level0の計算結果を保存して再利用
- [ ] daとdbの差を利用した枝狩り
- [x] tdc-sc-ber-simulatorの実装
- [ ] tdc-scl-ber-simulatorの実装
- [ ] tdc-scl-crc-ber-simulatorの実装
- [x] tdc-raw-ber-simulatorの実装
- [ ] channel::TDCの最大ドリフト値を削除
- [ ] ldpc+markerの実装
- [x] SIR計算の実装
- [x] DriftTransitionProbをpcstdcからtdcsに移動
- [x] CyclicPriorProbの移植
- [x] LikelihoodCalculatorの実装
- [x] utl::MarkerHandlerの実装
- [ ] utl::CodewordBERSimulatorの実装
- [ ] lcs::LDPCRandomEncoderの実装
- [ ] lcs::BPDecoderの移植
- [ ] lcstdc::BPMarkerDecoderの実装
- [ ] lcstdc::BPMarkerEncoderの実装

## LDPC+Markerの構成
- lcstdc
    - BPDecoder
    - LikelihoodCalculator
    - DriftTransitionProb
    - CyclicPriorProb

### 問題点
- utl::BERSimulatorはEncoderが必要なので生成行列が必要になる
    - 長い符号長の検査行列から生成行列を生成するのは時間がかかる
    - ランダムな情報語を用いるBERSimulatorではなく、ランダムな符号語を用いるBERSimulatorを作れば良さそう
