/**
 * @file Nvm_Cfg_Mac.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Helper file to provide #defines for configuration.
 *
 * Intentionally without header guards because this file is included several times in a row.
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

#ifndef NVM_DEF_START
#define NVM_DEF_START       namespace nvm {
#endif

#ifndef NVM_DEF_END
#define NVM_DEF_END         } // namespace nvm
#endif

#ifdef NVM_DEF_OBJ_START
#undef NVM_DEF_OBJ_START
#endif
#ifdef NVM_DEF_OBJ
#undef NVM_DEF_OBJ
#endif
#ifdef NVM_DEF_OBJ_END
#undef NVM_DEF_OBJ_END
#endif

// ----------------------------------------------------------------------
/// type definition
// ----------------------------------------------------------------------
#ifdef NVM_DEF_MODE_TYPE
#define NVM_DEF_OBJ_START                 typedef struct {           
#define NVM_DEF_OBJ_END                   } cod _type;
#define NVM_DEF_OBJ(type, obj, init)      type obj;
#endif

#ifdef NVM_DEF_MODE_OBJ
#define NVM_DEF_OBJ_START                 calib_type calib = {           
#define NVM_DEF_OBJ_END                   };
#define NVM_DEF_OBJ(type, obj, init)      init,
#endif
