/**
 * @file DecoderCfg.h
 * 
 * @author Ralf Sondershaus
 * 
 * @brief DCC Decoder configuration definitions.
 *
 * @copyright Copyright (c) 2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: MIT
 */


#ifndef DCC_DECODERCFG_H
#define DCC_DECODERCFG_H
#include <Std_Types.h>

#define OPT_DCC_DECODER_DEBUG_ON       1
#define OPT_DCC_DECODER_DEBUG_OFF      0

/** Select option for DCC decoder debug */
#define CFG_DCC_DECODER_DEBUG          OPT_DCC_DECODER_DEBUG_OFF

#define CFG_DCC_DECODER_FIFO_SIZE     5  ///< Number of packets stored in FIFO
#endif // DCC_DECODERCFG_H