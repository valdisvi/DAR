/*********************************************************************/
// dar - disk archive - a backup/restoration program
// Copyright (C) 2002-2052 Denis Corbin
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// to contact the author : http://dar.linux.free.fr/email.html
/*********************************************************************/

    /// \file macro_tools.hpp
    /// \brief macroscopic tools for libdar internals
    /// \ingroup Private

#ifndef MACRO_TOOLS_HPP
#define MACRO_TOOLS_HPP

#include "../my_config.h"

extern "C"
{
#if HAVE_LIMITS_H
#include <limits.h>
#endif
}
#include <string>

#include "catalogue.hpp"
#include "compressor.hpp"
#include "infinint.hpp"
#include "header_version.hpp"
#include "generic_file.hpp"
#include "scrambler.hpp"
#include "crypto.hpp"
#include "escape.hpp"
#include "pile.hpp"
#include "entrepot.hpp"

    /// \addtogroup Private
    /// @{


#define BUFFER_SIZE 102400
#ifdef SSIZE_MAX
#if SSIZE_MAX < BUFFER_SIZE
#undef BUFFER_SIZE
#define BUFFER_SIZE SSIZE_MAX
#endif
#endif

#define GLOBAL_ELASTIC_BUFFER_SIZE 10240

namespace libdar
{

    extern const archive_version macro_tools_supported_version;
    extern const std::string LIBDAR_STACK_LABEL_UNCOMPRESSED;
    extern const std::string LIBDAR_STACK_LABEL_CLEAR;
    extern const std::string LIBDAR_STACK_LABEL_UNCYPHERED;
    extern const std::string LIBDAR_STACK_LABEL_LEVEL1;

    extern void macro_tools_open_archive(user_interaction & dialog,     //< for user interaction
					 memory_pool *pool,             //< whether memory_pool allocation has to be performed
					 const entrepot &where,         //< slices location
                                         const std::string &basename,   //< slice basename
					 const infinint & min_digits,   //< minimum digits for the slice number
                                         const std::string &extension,  //< slice extensions
					 crypto_algo crypto,            //< encryption algorithm
                                         const secu_string &pass,       //< pass key for crypto/scrambling
					 U_32 crypto_size,              //< crypto block size
					 pile & stack,                  //< the stack of generic_file resulting of the archive openning
                                         header_version &ver,           //< header read from raw data
                                         const std::string &input_pipe, //< named pipe for input when basename is "-" (dar_slave)
                                         const std::string &output_pipe,       //< named pipe for output when basename is "-" (dar_slave)
                                         const std::string & execute,          //< command to execute between slices
					 infinint & second_terminateur_offset, //< where to start looking for the second terminateur (set to zero if there is only one terminateur).
					 bool lax,  // whether we skip&warn the usual verifications
					 bool sequential_read, // whether to use the escape sequence (if present) to get archive contents and proceed to sequential reading
					 bool info_details); // be or not verbose about the archive openning
        // all allocated objects (ret1, ret2, scram), must be deleted when no more needed by the caller of this routine

    extern catalogue *macro_tools_get_derivated_catalogue_from(user_interaction & dialog,
							       memory_pool *pool,
							       pile & data_stack,  // where to get the files and EA from
							       pile & cata_stack,  // where to get the catalogue from
							       const header_version & ver, // version format as defined in the header of the archive to read
							       bool info_details, // verbose display (throught user_interaction)
							       infinint &cat_size, // return size of archive in file (not in memory !)
							       const infinint & second_terminateur_offset, // location of the second terminateur (zero if none exist)
							        bool lax_mode);         // whether to do relaxed checkings

    extern catalogue *macro_tools_get_catalogue_from(user_interaction & dialog,
						     memory_pool *pool,
						     pile & stack,  // raw data access object
						     const header_version & ver, // version format as defined in the header of the archive to read
                                                     bool info_details, // verbose display (throught user_interaction)
                                                     infinint &cat_size, // return size of archive in file (not in memory !)
						     const infinint & second_terminateur_offset,
						     bool lax_mode);

    extern catalogue *macro_tools_lax_search_catalogue(user_interaction & dialog,
						       memory_pool *pool,
						       pile & stack,
						       const archive_version & edition,
						       compression compr_algo,
						       bool info_details,
						       bool even_partial_catalogues,
						       const label & layer1_data_name);

	// the beginning of the catalogue.

    extern infinint macro_tools_get_terminator_start(generic_file & f, const archive_version & reading_ver);

	/// build layers for a new archive
	///
	/// \param[in]  dialog for user interaction
	/// \param[out] layers the resulting stack of generic_file layers ready for use
	/// \param[out] ver the archive "header" to be dropped at end of archive
	/// \param[in]  pool memory pool to use for memory allocation (NULL for no pool usage)
	/// \param[in]  sauv_path_t where to create the archive
	/// \param[in]  filename archive base name
	/// \param[in]  extension archive extension
	/// \param[in]  allow_over whether to allow slice overwriting
	/// \param[in]  warn_over whether to warn before overwriting
	/// \param[in]  info_details whether to display detailed information
	/// \param[in]  pause how many slices to wait before pausing (0 to never wait)
	/// \param[in]  algo compression algorithm
	/// \param[in]  compression_level compression level
	/// \param[in]  file_size size of the slices
	/// \param[in]  first_file_size size of the first slice
	/// \param[in]  execute command to execute after each slice creation
	/// \param[in]  crypto cipher algorithm to use
	/// \param[in]  pass password/passphrase to use for encryption
	/// \param[in]  empty dry-run execution (null_file at bottom of the stack)
	/// \param[in]  slice_permission permission to set the slices to
	/// \param[in]  add_marks_for_sequential_reading whether to add an escape layer in the stack
	/// \param[in]  user_comment user comment to add into the slice header/trailer
	/// \param[in]  hash algorithm to use for slices hashing
	/// \param[in]  slice_min_digits minimum number of digits slice number must have
	/// \param[out] data_name label generated by the bottom layer of the created stack (= layers object).
    extern void macro_tools_create_layers(user_interaction & dialog,
					  pile & layers,
					  header_version & ver,
					  memory_pool *pool,
					  const entrepot & sauv_path_t,
					  const std::string & filename,
					  const std::string & extension,
					  bool allow_over,
					  bool warn_over,
					  bool info_details,
					  const infinint & pause,
					  compression algo,
					  U_I compression_level,
					  const infinint & file_size,
					  const infinint & first_file_size,
					  const std::string & execute,
					  crypto_algo crypto,
					  const secu_string & pass,
					  U_32 crypto_size,
					  bool empty,
					  const std::string & slice_permission,
					  bool add_marks_for_sequential_reading,
					  const std::string & user_comment,
					  hash_algo hash,
					  const infinint & slice_min_digits,
					  label & data_name);

	/// dumps the catalogue and close all the archive layers to terminate the archive
	///
	/// \param[in] dialog for user interaction
	/// \param[in] layers the archive layers to close
	/// \param[in] ver the archive "header" to be dropped at end of archive
	/// \param[in] cat the catalogue to dump in the layer before closing the archive
	/// \param[in] info_details whether to display detailed information
	/// \param[in] crypto cipher algorithm used in "layers"
	/// \param[in] algo compression algorithm used
	/// \param[in] empty dry-run execution (null_file at bottom of the stack)
    void macro_tools_close_layers(user_interaction & dialog,
				  pile & layers,
				  const header_version & ver,
				  const catalogue & cat,
				  bool info_details,
				  crypto_algo crypto,
				  compression algo,
				  bool empty);

} // end of namespace

	/// @}

#endif
