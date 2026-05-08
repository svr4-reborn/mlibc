#ifndef _SYS_IOCCOM_H
#define _SYS_IOCCOM_H

/* SVR4 ioctls actually are not randomly chosen integers. This file provides helpers for them. 
 * Not sure how to do this in a original way, so I just copied the definitions from the source.
 */
#define	IOCPARM_MASK	0xff		/* parameters must be < 256 bytes */
#define	IOC_VOID	0x20000000	/* no parameters */
#define	IOC_OUT		0x40000000	/* copy out parameters */
#define	IOC_IN		0x80000000	/* copy in parameters */
#define	IOC_INOUT	(IOC_IN|IOC_OUT)
/* the 0x20000000 is so we can distinguish new ioctl's from old */
#define	_IO(x,y)	(IOC_VOID|(x<<8)|y)
#define	_IOR(x,y,t)	(IOC_OUT|((((int)sizeof(t))&IOCPARM_MASK)<<16)|(x<<8)|y)
#define	_IORN(x,y,t)	(IOC_OUT|(((t)&IOCPARM_MASK)<<16)|(x<<8)|y)
#define	_IOW(x,y,t)	(IOC_IN|((((int)sizeof(t))&IOCPARM_MASK)<<16)|(x<<8)|y)
#define	_IOWN(x,y,t)	(IOC_IN|(((t)&IOCPARM_MASK)<<16)|(x<<8)|y)
/* this should be _IORW, but stdio got there first */
#define	_IOWR(x,y,t)	(IOC_INOUT|((((int)sizeof(t))&IOCPARM_MASK)<<16)|(x<<8)|y)


#endif /* _SYS_IOCCOM_H */