#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/mntent.h>
#include <sys/mnttab.h>

namespace {

char *skip_ws(char *s) {
	while(*s == ' ' || *s == '\t')
		++s;
	return s;
}

char *scan_token_end(char *s) {
	while(*s && *s != ' ' && *s != '\t' && *s != '\n')
		++s;
	return s;
}

bool field_matches(const char *candidate, const char *reference) {
	if(!reference)
		return true;
	if(!candidate)
		return false;
	return !strcmp(candidate, reference);
}

char *find_option(char *options, const char *opt) {
	if(!options || !opt)
		return nullptr;

	size_t opt_len = strlen(opt);
	for(char *current = options; *current;) {
		char *token_end = current;
		while(*token_end && *token_end != ',')
			++token_end;
		if(static_cast<size_t>(token_end - current) == opt_len
				&& !strncmp(current, opt, opt_len))
			return current;
		if(!*token_end)
			break;
		current = token_end + 1;
	}

	return nullptr;
}

} // namespace

int getmntent(FILE *stream, struct mnttab *entry) {
	if(!stream || !entry) {
		errno = EINVAL;
		return MNT_TOOFEW;
	}

	static char line[MNT_LINE_MAX];

	while(true) {
		if(!fgets(line, sizeof(line), stream))
			return feof(stream) ? -1 : MNT_TOOLONG;

		size_t len = strlen(line);
		if(!len)
			continue;
		if(line[len - 1] != '\n') {
			int ch;
			while((ch = fgetc(stream)) != '\n' && ch != EOF)
				;
			return MNT_TOOLONG;
		}

		char *cursor = skip_ws(line);
		if(*cursor == '#' || *cursor == '\n' || *cursor == '\0')
			continue;

		char *fields[5];
		for(int i = 0; i < 5; ++i) {
			cursor = skip_ws(cursor);
			if(*cursor == '\n' || *cursor == '\0')
				return MNT_TOOFEW;
			fields[i] = cursor;
			cursor = scan_token_end(cursor);
			if(*cursor)
				*cursor++ = '\0';
		}

		cursor = skip_ws(cursor);
		if(*cursor != '\n' && *cursor != '\0')
			return MNT_TOOMANY;

		entry->mnt_special = fields[0];
		entry->mnt_mountp = fields[1];
		entry->mnt_fstype = fields[2];
		entry->mnt_mntopts = fields[3];
		entry->mnt_time = fields[4];
		return 0;
	}
}

int getmntany(FILE *stream, struct mnttab *entry, struct mnttab *reference) {
	if(!stream || !entry || !reference) {
		errno = EINVAL;
		return MNT_TOOFEW;
	}

	while(true) {
		int ret = getmntent(stream, entry);
		if(ret)
			return ret;

		if(field_matches(entry->mnt_special, reference->mnt_special)
				&& field_matches(entry->mnt_mountp, reference->mnt_mountp)
				&& field_matches(entry->mnt_fstype, reference->mnt_fstype)
				&& field_matches(entry->mnt_mntopts, reference->mnt_mntopts)
				&& field_matches(entry->mnt_time, reference->mnt_time))
			return 0;
	}
}

char *hasmntopt(struct mnttab *mnt, const char *opt) {
	if(!mnt)
		return nullptr;
	return find_option(mnt->mnt_mntopts, opt);
}