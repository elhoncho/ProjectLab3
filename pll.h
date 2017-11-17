void PLL_Open(int16_t maxClkDevide, int16_t maxLoopDevide, int8_t referenceFreqMHz);
void PLL_Close();
void PLL_Read();
void PLL_Write(int freqMHz, int freqDecimal);
void PLL_ManualFrequency(int freqDevider);
void PLL_ManualClock(int clockDevider);
