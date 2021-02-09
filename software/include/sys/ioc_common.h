
#ifndef _KERNEL_IOC_NUMS_H
#define _KERNEL_IOC_NUMS_H

#define _IOCPARM_MASK	0x1FFF
#define _IOC_VOID	0x20000000
#define _IOCTYPE_MASK	0xFFFF
#define _IOC_IN		0x40000000
#define _IOC_OUT	0x80000000
#define _IOC_INOUT	(_IOC_IN | _IOC_OUT)

#define _IO(x,y)	((x << 8) | y | _IOC_VOID)
#define _IOR(x,y,t)	((x << 8) | y | ((sizeof(t) & _IOCPARM_MASK) << 16) | _IOC_OUT)
#define _IOW(x,y,t)	((x << 8) | y | ((sizeof(t) & _IOCPARM_MASK) << 16) | _IOC_IN)
#define _IORW(x,y,t)	((x << 8) | y | ((sizeof(t) & _IOCPARM_MASK) << 16) | _IOC_INOUT)

#define _IO_NUM(req)	((req) & 0xFF)
#define _IO_TYPE(req)	(((req) >> 8) & 0xFF)
#define _IO_PARAM(req)	(((req) >> 16) & _IOCPARM_MASK)

#endif
