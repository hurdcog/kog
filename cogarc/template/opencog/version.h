/**
 * version.h - Version information for @COMPONENT_NAME_CAMEL@
 *
 * Copyright (C) @YEAR@ @AUTHOR@
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_@COMPONENT_NAME_UPPER@_VERSION_H
#define _OPENCOG_@COMPONENT_NAME_UPPER@_VERSION_H

// Semantic versioning: MAJOR.MINOR.PATCH
// See https://semver.org/
#define @COMPONENT_NAME_UPPER@_VERSION_STRING "@VERSION@"

// Individual version components
#define @COMPONENT_NAME_UPPER@_MAJOR_VERSION @VERSION_MAJOR@
#define @COMPONENT_NAME_UPPER@_MINOR_VERSION @VERSION_MINOR@
#define @COMPONENT_NAME_UPPER@_PATCH_VERSION @VERSION_PATCH@

// Combined version as integer (for comparison)
// Format: (MAJOR * 10000) + (MINOR * 100) + PATCH
#define @COMPONENT_NAME_UPPER@_VERSION \
	((@COMPONENT_NAME_UPPER@_MAJOR_VERSION * 10000) + \
	 (@COMPONENT_NAME_UPPER@_MINOR_VERSION * 100) + \
	 @COMPONENT_NAME_UPPER@_PATCH_VERSION)

#endif // _OPENCOG_@COMPONENT_NAME_UPPER@_VERSION_H
