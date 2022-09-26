/*
Basic app to test the Rpmsg communication with the remote M4 core.
*/

#include <uapi/linux/rpmsg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>



/**
 * struct rpmsg_endpoint_info - endpoint info representation
 * @name: name of service
 * @src: local address. To set to RPMSG_ADDR_ANY if not used.
 * @dst: destination address. To set to RPMSG_ADDR_ANY if not used.
 */
struct rpmsg_endpoint_info {
	char name[32];
	uint32_t src;
	uint32_t dst;
};

/*
 * argv[1] : "/dev/rpmsg_ctrl<X>", where <X> is the instance number of the device
 * argv[2] : a name for the device
 * argv[3] : local endpoint address 
 * argv[4] : remote endpoint address to communicate with
 */
int createEndPoint(int argc, char **argv)
{
	struct rpmsg_endpoint_info ept;
	int ret;
	int fd;
	char *endptr;

	if (argc != 5)
		fprintf(stderr, "not enough args, exiting...\n");
		exit(1);
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "failed to open %s\n", argv[1]);

	strncpy(ept.name, argv[2], sizeof(ept.name));
	ept.name[sizeof(ept.name)-1] = '\0';

	ept.src = strtoul(argv[3], &endptr, 10);

	if (*endptr || ept.src <= 0)
		fprintf(stderr, "conversion failed at src...\n");
		exit(1);

	ept.dst = strtoul(argv[4], &endptr, 10);

	if (*endptr  || ept.src <= 0)
		fprintf(stderr, "conversion failed at dest...\n");
		exit(1);

	ret = ioctl(fd, RPMSG_CREATE_EPT_IOCTL, &ept);
	if (ret < 0) {
		fprintf(stderr, "failed to create endpoint");
		exit(1);
	}

	close(fd);
	return 0;
}


int main(int argc, char *argv[])
{
	
    return createEndPoint(argc, argv);
}