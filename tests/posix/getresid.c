#include <assert.h>
#include <unistd.h>

int main(void) {
	uid_t ruid;
	uid_t euid;
	uid_t suid;
	gid_t rgid;
	gid_t egid;
	gid_t sgid;

	assert(!getresuid(&ruid, &euid, &suid));
	assert(!getresgid(&rgid, &egid, &sgid));

	assert(ruid == getuid());
	assert(euid == geteuid());
	assert(rgid == getgid());
	assert(egid == getegid());
	assert(suid == ruid || suid == euid);
	assert(sgid == rgid || sgid == egid);

	return 0;
}