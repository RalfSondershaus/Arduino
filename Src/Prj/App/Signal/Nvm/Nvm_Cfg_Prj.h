/**
 * @file Nvm_Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief NVM declaration for Signal.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * See <https://www.gnu.org/licenses/>.
 */

#ifndef NVM_DEF_OBJ_INIT_signals
#define NVM_DEF_OBJ_INIT_signals                        \
    {                                                   \
      /* aspects */                                     \
      {                                                 \
          { 0b00011000, 0b00000000 } /* 0 (HP0) */      \
        , { 0b00000100, 0b00000000 } /* 1 (HP1) */      \
        , { 0b00000110, 0b00000000 } /* 2 (HP2) */      \
        , { 0b00011001, 0b00000000 } /* 3 (HP3) */      \
        , { 0b00011111, 0b00000000 } /* 4 (ALL) */      \
      },                                                \
      /* targets */                                     \
      {                                                 \
        /* 0 */                                         \
        {                                               \
          { { cfg_output_type::kOnboard, 13 } /* red */ \
          , { cfg_output_type::kOnboard, 12 } /* red */ \
          , { cfg_output_type::kOnboard, 11 } /* grn */ \
          , { cfg_output_type::kOnboard, 10 } /* yel */ \
          , { cfg_output_type::kOnboard,  9 } /* wht */ \
          }                                             \
        },                                              \
        /* 1 */                                         \
        {                                               \
          { { cfg_output_type::kOnboard, 8 }            \
          , { cfg_output_type::kOnboard, 7 }            \            
          , { cfg_output_type::kOnboard, 6 }            \
          , { cfg_output_type::kOnboard, 5 }            \
          , { cfg_output_type::kOnboard, 4 }            \
          }                                             \
        },                                              \
        /* 2 */                                         \
        {                                               \
          { { cfg_output_type::kOnboard, 13 }           \
          , { cfg_output_type::kOnboard, 12 }           \
          , { cfg_output_type::kOnboard, 11 }           \
          , { cfg_output_type::kOnboard, 10 }           \
          , { cfg_output_type::kOnboard,  9 }           \
          }                                             \
        },                                              \
        /* 3 */                                         \
        {                                               \
          { { cfg_output_type::kOnboard, 13 }           \
          , { cfg_output_type::kOnboard, 12 }           \
          , { cfg_output_type::kOnboard, 11 }           \
          , { cfg_output_type::kOnboard, 10 }           \
          , { cfg_output_type::kOnboard,  9 }           \
          }                                             \
        },                                              \
        /* 4 */                                         \
        {                                               \
          { { cfg_output_type::kOnboard, 13 }           \
          , { cfg_output_type::kOnboard, 12 }           \
          , { cfg_output_type::kOnboard, 11 }           \
          , { cfg_output_type::kOnboard, 10 }           \
          , { cfg_output_type::kOnboard,  9 }           \
          }                                             \
        },                                              \
        /* 5 */                                         \
        {                                               \
          { { cfg_output_type::kOnboard, 13 }           \
          , { cfg_output_type::kOnboard, 12 }           \
          , { cfg_output_type::kOnboard, 11 }           \
          , { cfg_output_type::kOnboard, 10 }           \
          , { cfg_output_type::kOnboard,  9 }           \
          }                                             \
        }                                               \
      }
#endif // NVM_DEF_OBJ_INIT_signals


NVM_DEF_START

NVM_DEF_OBJ_START
NVM_DEF_OBJ(cod_signal_array, signals, NVM_DEF_OBJ_INIT_signals)
NVM_DEF_OBJ_END

NVM_DEF_END
