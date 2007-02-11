/*
* Description: find a file in a branch
*
* License: BSD-style license
*
*
* Author: Radek Podgorny <radek@podgorny.cz>
*
*
*/

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdbool.h>

#include "unionfs.h"
#include "cache.h"
#include "opts.h"
#include "general.h"


/**
 * If path exists, return the root number that has path. Also create a cache entry.
 * TODO: We can still stat() fname_~HIDDEN, though these are hidden by readdir()
 *       and should mainly be for internal usage, only.
*/
int findroot(const char *path) {
	int i = -1;
	
	if (uopt.cache_enabled) cache_lookup(path);

	if (i != -1) return i;

	// create a new cache entry, if path exists
	bool hidden = false;
	for (i = 0; i < uopt.nroots; i++) {
		char p[PATHLEN_MAX];
		snprintf(p, PATHLEN_MAX, "%s%s", uopt.roots[i].path, path);

		struct stat stbuf;
		int res = lstat(p, &stbuf);

		if (res == 0 && !hidden) {
			if (uopt.cache_enabled) cache_save(path, i);
			return i;
		} else if (hidden) {
			/* the file is hidden in this root, we also ignore it
			* in all roots below this level */
			return -1;
		}
		// check check for a hide file
		hidden = file_hidden(p);
	}

	return -1;
}

/**
 * Try to find root when we cut the last path element
 */
int findroot_cutlast(const char *path) {
	char* ri = rindex(path, '/'); // this char should always be found
	int len = ri - path;

	char p[PATHLEN_MAX];
	strncpy(p, path, len);
	p[len] = '\0';

	return findroot(p);
}

/**
 * Get a writable root in from our branch list, but only above root_ro.
 */
int wroot_from_list(int root_ro)
{
	int i;
	
	// take the first read-write root available
	for (i = 0; i < root_ro; i++)
		if (uopt.roots[i].rw) 
			return i; // found it it.

	return -1;
}