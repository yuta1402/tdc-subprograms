# TODO
- [x] tdc-frozen-bit-analyzerの実装
- [x] tdc-sc-benchmarkの実装
- [x] tdc-sc-ber-simulatorの実装
- [ ] tdc-scl-ber-simulatorの実装
- [ ] tdc-scl-crc-ber-simulatorの実装
- [x] tdc-raw-ber-simulatorの実装
- [ ] channel::TDCの最大ドリフト値を削除
- [x] ldpc+markerの実装
- [x] SIR計算の実装
- [x] DriftTransitionProbをpcstdcからtdcsに移動
- [x] CyclicPriorProbの移植
- [x] LikelihoodCalculatorの実装
- [x] utl::MarkerHandlerの実装
- [x] lcs::BPDecoderの移植
- [x] lcstdc::BPMarkerDecoderの実装
- [x] lcstdc::BPMarkerEncoderの実装

## SC Decoderの最適化
- [ ] level0の計算結果を保存して再利用
- [x] daとdbの差を利用した枝狩り
- [x] DriftTransitionProbで確率が0の場合を除外したリストを作成
- [ ] 再帰計算をやめる

# Experiments
- Channel Polarization
    - [ ] v=1e-0でn=11..15
    - [ ] v=1e-1でn=11..15
- BER
    - [ ] raw, ldpc+marker, scl (n=12?)
    - [ ] L=1,2,4,8でscl (n=12?)
